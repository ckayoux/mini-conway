#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

void afficherSurface(char plat[30][30]);
bool yesno_question(const char *chaine);
char convertir_minuscules(char c);
void saisir_coord(char plat[30][30]);
void remplissage_auto(char plat[30][30]);
unsigned char getNbVoisins(char plat[30][30],char xini, char yini);
bool actualiser(char plat[30][30], int *nb_en_vie);

int main()
{
    srand(time(NULL));
    //Création de la surface
    char surf[30][30];
    int g,dg;
    bool encoreenvie;
    bool finprematuree;
    int nbindiv;
    bool lancer;
    bool automatique = false;
    lancer = yesno_question("Voulez-vous lancer une simulation ?");
    while(lancer) {
        finprematuree=false;
        nbindiv=0;
        putchar('\a');
        //initialisation de toutes les cases à 'vide'
        for(int i = 0 ; i<30 ; i++){
            for(int j = 0; j<30 ; j++) {
                surf[i][j]=0;
            }
        }

        g = 1;
        //Generation des individus presents a la generation 1
        automatique = yesno_question("Voulez-vous que les individus soient generes automatiquement ?");
        if(!automatique){
            puts("\nSaisir a la suite les coordonnees des individus presents initialement au format :");
            puts("\t\"x , y\" (avec x et y entiers naturels compris entre 1 et 30 inclus).\n\tEntrer les coordonnees d'une case contenant deja un individu le supprimera.\n(Entrer 's' pour mettre fin a la saisie.)");
            saisir_coord(surf);
        }
        else {
            remplissage_auto(surf);
        }

        printf("\n______________________\nGENERATION 1\n\n");
        afficherSurface(surf);
        encoreenvie=false;
        for(int i=0 ; i<30 ; i++){
            for(int j=0; j<30 ; j++){
                if(surf[i][j]!=0){
                    encoreenvie=true;
                    break;
                }
            }
        }
        while(encoreenvie){

            printf("Entrez le nombre de generations dont avance la simulation (entier superieur ou egal a 1) ou 0 pour y mettre fin : ");
            dg=-1;
            do {
                scanf("%d",&dg);
                if(dg<0) {
                    printf("Saisie incorrecte, veuillez reessayer : ");
                    fflush(stdin);
                }
            }while(dg < 0);
            if(dg==0){
                    finprematuree=true;
                    break;
            }
            for(int d = 1; d<=dg ; d++){
                encoreenvie=actualiser(surf,&nbindiv);
                g++;
                if(!encoreenvie){
                    break;
                }
            }
            printf("\n______________________\nGENERATION %2d\n\n",g);
            afficherSurface(surf);
            if(nbindiv==1) {
                printf("La generation compte 1 individu.\n");
            }
            else if(nbindiv>1){
                printf("La generation compte %d individus.\n",nbindiv);
            }

        };
        if(nbindiv==0&&!finprematuree){
                printf("L'espece a survecu jusqu'a la generation %d.\n_______________________\nFIN DE LA SIMULATION\n",g-1);
        }

        putchar('\n');
        lancer = yesno_question("Voulez-vous lancer une nouvelle simulation ?");
    }

    puts("Au revoir !");
    return 0;
}


void afficherSurface(char plat[30][30]){
    for(int i=0 ; i<30 ; i++) {
        for(int j=0 ; j<30 ; j++) {
            if (plat[i][j]==0) {
                printf("   ");
            }
            else {
                printf(" x ");
            }

            if(j!=29) { //on met des séparations partout sauf première et dernière colonne
                putchar('|');
            }
        }
        putchar('\n');
        if(i!=29) {
            for(int k=0 ; k<119 ; k++){
                putchar(196); //le caractère 196 est une ligne horizontale à mi-hauteur
            }
        }
        putchar('\n');
    }
}

unsigned char getNbVoisins(char plat[30][30],char xini, char yini){ //fonction comptant le nombre de voisins d'une case
    unsigned char nbv=0;
    char x,y; //coordonnees qu'on utilisera pour se deplacer autour d'une case
    x=xini-1; //On se place à la case en haut à gauche de la case d'intéret. Si elle n'existe pas, nous sauterons l'étape de vérification/comptage
    for(int i = 0 ; i<3 ; i++){
        y=yini-1; //on initialise/reinitialise y a la colonne precedant la case
        for(int j = 0 ; j<3 ; j++){
            //on commence par verifier que la case existe...
            if((x>=0 && x<30) && (y>=0 && y<30) && !(x==xini && y==yini)){//...en regardant si elle appartient bien à la surface. On sautera aussi la case dont on compte les voisins.
                //on regarde ensuite s'il y a un individu sur la case voisine de coordonnées (x,y) :
                if(plat[x][y]!=0){
                    nbv++; //si la case est non-vide, le nombre de voisins est incremente
               }
            }
            y++; //on incremente ensuite y (pour le faire varier entre yini-1 et yini+1)
        }
        x++;//puis on incremente x une fois tous les y verifies pour une ligne (pour le faire varier entre xini-1 et xini+1)
    }
    return nbv;
}
bool actualiser(char plat[30][30],int *nb_en_vie){
    int nb=0;
    char plat_temp[30][30];
    bool ilenreste = false;
    //on ne peut pas effectuer les modifications en direct au risque de modifier le voisinage d'une case encore non traitee. On copie donc le tableau pour en creer un temporaire
    for(int i=0 ; i<30 ; i++){
        for(int j=0 ; j<30 ; j++){
            plat_temp[i][j]=plat[i][j];
        }
    }

    unsigned char nbv;
    for(char x=0 ; x<30 ; x++){
        for(char y=0 ; y<30 ; y++){
            nbv = getNbVoisins(plat_temp,x,y);
            //en fonction du nombre de voisins, on actualise la case du plateau temporaire.
            if(nbv==3){//la case possède 3 voisins : elle donne naissance a un individu
                plat_temp[x][y]='x';
                nb++;
            }
            else if(nbv==2){//la case possède 2 voisins : si un individu s'y trouve, il reste en vie.
                if(plat_temp[x][y]!=0){
                        plat_temp[x][y]='x';
                        nb++;
                }
            }
            else {//La case possède 0,1 ou 4,5,6,7,8 voisins : dans tous ces cas, l'individu meurt.
                plat_temp[x][y]=0;
            }
        }
    }
    for(int i=0 ; i<30 ; i++){//on copie ensuite la surface temporaire dans la surface permanente.
        for(int j=0 ; j<30 ; j++){
            plat[i][j]=plat_temp[i][j];
        }
    }
    if(nb>0) ilenreste=true;
    *nb_en_vie=nb;
    return ilenreste; //ilenreste vaut false si et seulement s'il ne reste plus aucun individu.
}
//FONCTIONS POUR L'INTERACTION AVEC L'UTILISATEUR
void remplissage_auto(char plat[30][30]){
    int nb_indiv;
    unsigned short int x,y;
    char pileouface = 0;
    bool dispo;
    printf("Saisir le nombre d'individus a generer (entier positif compris entre 0 et 900) : ");
    do {
        scanf("%d",&nb_indiv);
        if(nb_indiv>900|| nb_indiv<0) {
            printf("Saisie incorrecte. Veuillez reessayer : ");
        }
    }while(nb_indiv>900|| nb_indiv<0);
    unsigned short int tab_dejapris[nb_indiv][2];
    for(int i=0 ; i < nb_indiv ; i++){
       x = rand()%30;
       y = rand()%30;
       dispo = false;
       while(!dispo){ //on regarde si on a deja cree un individu a la case generee.
           dispo = true;
           for(int k=0 ; k < i ; k++) {
             if (x==tab_dejapris[k][0]){
                if (y==tab_dejapris[k][1]){
                    dispo = false;
                    pileouface=rand()%2; //on va incrementer soit x soit y, au hasard, jusqu'à trouver une case libre.
                    if(pileouface==0){
                        x=(x+1)%30;
                    }
                    else {
                        y=(y+1)%30;
                    }
                }
             }
           }

       }
       tab_dejapris[i][0]=x;
       tab_dejapris[i][1]=y;
       plat[x][y]='x';
    }
    return;
}
void saisir_coord(char plat[30][30]){
    int x;
    int y;
    char saisie[15];
    putchar('\n');
    bool saisiecorrecte;
    char c;
    unsigned short int i;
    while(1) { //on demande la saisie des coordonnees tant que l'utilisateur n'a pas saisi 's'.
        i=0;
        do {
            c=getchar();
            if(c=='s'||c=='S') return; //si l'utilisateur saisit le caractère 's', fin de la saisie des coordonnees.
            saisie[i]=c;
            i++;
        } while(c!='\n'&&i<15);
        saisie[i-1]='\0';
        fflush(stdin);

        saisiecorrecte = false;
        x=0;
        y=0;
        do {
            sscanf(saisie,"%d , %d",&x,&y); //on utilise sscanf car on regarde d'abord toute la saisie pour voir s'il y a un s.
            if((x>30||x<1)||(y>30)||(y<1)){
                puts("Saisie incorrecte, veuillez reessayer : ");
                break; //retour à la boucle while(1)
            }
            else {
                saisiecorrecte=true;
            }
        }while(!saisiecorrecte);
        x--;y--; //les indices du tableau commencent à 0 et non 1.
        if(plat[x][y]==0){
                plat[x][y] = 'x'; //ajout d'un individu à la case demandee s'il n'y en avait aucun.
        }
        else {
                plat[x][y] = 0; //pour supprimer l'individu si l'utilisateur saisit de nouveau les mêmes coordonnées.
        }
    }
    return;
}

bool yesno_question(const char *chaine) { //cette fonction permet d'obtenir une reponse positive ou negative de l'utilisateur.
    printf(chaine);
    printf(" (o/n) :\n");
    char yesno;
    while(1) {
        scanf(" %c",&yesno);
        yesno=convertir_minuscules(yesno);//rend la saisie insensible à la casse
        switch(yesno) {
            case 'o':
            fflush(stdin); //usage de fflush après scanf, au cas où l'utilisateur ait entré plus que prévu et remplisse le prochain scanf.
            return true;

            case 'n':
            fflush(stdin);
            return false;

            default:
            puts("Entree non valide. Veuillez entrer uniquement \"o\" ou \"n\".");
        }
        fflush(stdin);
    }
}
char convertir_minuscules(char c) { //cette fonction permet de convertir une réponse en minuscules au cas où l'utilisateur l'aurait entrée en majuscules
    if(c>='A' && c<='Z')
        return c+32;
    else
        return c;
}
