#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Liste Liste;
struct Liste
{
    int nombre;
    Liste *suivant;
};

int tailleListe(Liste * element){
  int taille = 0;
  while(element){
    element = element->suivant;
    taille++;
  }
  return taille;
}

Liste *listInit(int nb){
  Liste *listeChaine = malloc(sizeof(Liste));
  listeChaine->nombre = nb;
  listeChaine->suivant = NULL;
  return listeChaine;
}

Liste *insertListe(Liste *element, int nb){
  Liste **next = &element;
    while(*next){
      next = &(*next)->suivant;
    }
    *next = listInit(nb);
    if(*next)
      return element;
    return NULL;

}

double Randomdouble(double a, double b) {
    double diff = b - a;
    double random = RAND_MAX / diff;
    double r = rand() / random;
    return r+a;
}

int IndexListeChaine(Liste * element){
    int i=0,taille=0, neuroneGagnant;
    taille = tailleListe(element);
    if(taille>1){
      neuroneGagnant = Randomdouble((double)0,(double)taille);
      while(element){
        if(i==neuroneGagnant){
          return element->nombre;
        }
        i++;
        element=element->suivant;
      }
    }
    return element->nombre;
}

void affiche(Liste *element){
  Liste *next = element;
  while(next->suivant !=NULL){

    printf("%d \n",next->nombre);
    next = next->suivant;
  }
  printf("%d \n",next->nombre);
}
//--------------------------------------



typedef struct Donnee Donnee;
struct Donnee {
  double* donnee;
  char * nom;
  int taille;
  int tailledonnee;
};

typedef struct Neurone Neurone;
struct Neurone {
  double * vect;
  int taille;
  int type;
};

double min(double a, double b){
  if(a<=b)
   return a;
 return b;
}

void moyenne(Donnee *v, double * resultat ){
  int j,i;
  for(j=0; j<v[0].tailledonnee; j++){
    double res=0.0;
    for(i=0; i<v[0].taille; i++){
      res += v[i].donnee[j];
    }
    resultat[j] = res/v[0].taille;
  }
}

void affichee(Donnee * f){
  for(int i=0; i<150; i++){
    for(int j=0; j<4; j++){
      printf("%f ",f[i].donnee[j]);
    }
      printf("\n");
  }
}

void norme(Donnee *v){
  int i,j,k;
  for(i=0; i<v[0].taille; i++){
    double s=0;
    for(j=0; j<v[0].tailledonnee; j++){
      s += pow(v[i].donnee[j],2);
    }
    for(k=0; k<v[0].tailledonnee; k++){
      v[i].donnee[k] /= sqrt( s );
    }
  }
}

Donnee * readData(Donnee *data){
  FILE *file = fopen("iris.data","r");
  ssize_t ligne;
  size_t len = 0;
  char * line = NULL;
  int tailledonnee=0, nbLine=0;

  while((ligne = getline(&line, &len, file)) != -1){
    nbLine++;//taille du tableau de toutes les données
    if(nbLine==1){
      for(int i=0; i<(int)ligne; i++){
        if(line[i] == ','){tailledonnee++;}
      }
    }
  }

  data = malloc(nbLine * sizeof(struct Donnee));
  for(int j=0; j<nbLine; j++){
    data[j].donnee = malloc(tailledonnee * sizeof(double));
  }
  data[0].taille = nbLine;
  data[0].tailledonnee = tailledonnee;

  //remplit les données dans le vecteur
  fseek(file, 0, SEEK_SET);
  char * donneeLu, *endptr;
  int dat=0, it=0;
  while((ligne = getline(&line, &len, file)) != -1){
    it=0;
    donneeLu = strtok(line, ",");
    while(donneeLu != NULL){
      data[dat].donnee[it] = strtod(donneeLu, &endptr);
      if(it==4){ data[dat].nom = donneeLu;}
      donneeLu = strtok(NULL,",");
      it++;
    }
    dat++;
  }
  //affichee(data);
  fclose(file);
  return data;
}

 void fillVector(Neurone *neurone, double *moyenneArr, Donnee *data){
   for(int i=0; i<neurone[0].taille; i++){
     neurone[i].vect = malloc(data[0].tailledonnee * sizeof(double));
      neurone[i].type=0;
   }
   for(int i=0; i<neurone[0].taille; i++){
     for(int j=0; j<data[0].tailledonnee; j++){
       double k = Randomdouble(-0.05,0.025);
       neurone[i].vect[j] = moyenneArr[j] + k;
     }
   }
 }


 void shuffle(int * randArr, int nb){
  for(int i=0; i<nb ;i++){
     int r = Randomdouble(0,nb);
     int temp = randArr[i];
     randArr[i]=randArr[r];
     randArr[r]=temp;
   }
 }

////////-------------FROM HERE
 int * trouverCoordonnee(int nb, int nblig, int nbcol, int * coord){
   int i;
   for(i=0; i<nblig; i++){
     if(i*nbcol < nb){
       coord[0]=i;
     }
   }
   coord[1] = nb - coord[0]*nbcol;
   return coord;
 }

 int nbEtage(int max){
   int nb=8,i;
   for(i=0; nb<max/2; i++){
     nb += (8*i);
   }
   return i;
 }

 void diffusion(Neurone *neurone, double * v, int neuroneGagnant, int taille, double alpha,int etage){
    //printf("neur %d \n",neuroneGagnant);
    int coord[2] , x,y;
    trouverCoordonnee(neuroneGagnant, 10, 6, coord);
    int x0 = coord[0]-etage<0?0:coord[0]-etage;
    int y0 = coord[1]-etage<0?0:coord[1]-etage;
    int x1 = coord[0]+etage>10?10:coord[0]+etage;
    int y1 = coord[1]+etage>6?6:coord[1]+etage;
    for( x = x0; x<x1; x++){
      for( y = y0; y<y1; y++){
        //on applique sur chaque neurone la formule
        printf("%d && %d\n", x, y);
        for(int k=0; k<taille; k++){
          neurone[x*6 + y].vect[k] = neurone[x*6 + y].vect[k] + alpha * (v[k] - neurone[x*6 + y].vect[k]);
        }
      }
    }
   return;
 }

 void distanceEucli(int *randArr, Donnee *data, Neurone *neurone){
   int m,i,j,k,nbtour=2000,tour = nbtour/4,etage;
   etage = nbEtage(neurone[0].taille);;
   double etape = tour/etage;
   double alpha=0.8;
   double alphas = alpha,temp=alphas/tour;
   for(int i=0; i<data[0].taille; i++){
     randArr[i] = i;
   }
   Liste * bestList = NULL;
   for(m=0; m<nbtour+1; m++){
     shuffle(randArr,data[0].taille);
     if(m>etape && etage>1){etage--; etape+=etape;}
     if(m<tour){alphas-=temp;}
     if(m == tour){alpha/=10; temp=alpha/(nbtour-tour); alphas = alpha;}
     if(m>=tour){alphas-=temp;}
     //----------------
     printf("ALPHA %f\n", alphas);
      for(i=0; i<data[0].taille ;i++){
         bestList = NULL;
         double bestDist = 9999;
         for(j=0; j<neurone[0].taille; j++){
           double dist=0;
           for(k=0; k<data[0].tailledonnee; k++){
             dist += pow(data[randArr[i]].donnee[k] - neurone[j].vect[k] ,2);
           }
           dist = sqrt(dist);
           if(dist<bestDist){
             bestDist=dist;
             bestList = NULL;
             bestList=insertListe(bestList,j);
           }
           else if(dist == bestDist){
             bestList=insertListe(bestList,j);
           }
         }
         diffusion(neurone, data[randArr[i]].donnee, 21, data[0].tailledonnee, alphas, etage);
         return;
      }
    }

 }

 void etiquetage(Donnee *data, Neurone *neurone){
   int bestIndex = 0;
   for(int i=0; i<data[0].taille ;i++){
      double bestDist = 9999;
      for(int j=0; j<neurone[0].taille; j++){
        double dist=0;
        for(int k=0; k<data[0].tailledonnee; k++){
          dist += pow(data[i].donnee[k] - neurone[j].vect[k] ,2);
        }
        dist = sqrt(dist);
        if(dist<bestDist){
          bestDist=dist;
          bestIndex = j;
        }
      }
     if(i<50){neurone[bestIndex].type = 1;}
     else if(i >= 50 && i<100 ){neurone[bestIndex].type = 2;}
     else if(i >= 100 && i<150){neurone[bestIndex].type = 3;}
   }
 }

void afficheNeuronne(Neurone * neurone){
  for(int i=0; i<10; i++){
    for(int j=0; j<6; j++){
      printf("%d",neurone[i*6+j].type);
    }
    printf("\n");
  }
}

int calculTailleNeurone(int nb){
  int a,b;
  nb = sqrt(nb)*5;
  a = (nb/10) *10;
  b = a + 10;
  return (nb-a > b-nb)? b : a;
}

int main(int argc, char * argv[]){
  srand(time(NULL));

  Donnee *v=NULL ;
  v = readData(v);

  int tailleneurone = calculTailleNeurone(v[0].taille);
  double *moyenneArr = malloc(v[0].tailledonnee * sizeof(double));
  Neurone * neurone = malloc(tailleneurone * sizeof(struct Neurone));
  neurone[0].taille = tailleneurone;
  int *randArr = malloc(v[0].taille * sizeof(int));

//affichee(v);

  norme(v);
  moyenne(v, moyenneArr);
  fillVector(neurone, moyenneArr, v);

  //shuffle(randArr,v[0].taille);
  distanceEucli(randArr, v, neurone);
  etiquetage(v, neurone);
  afficheNeuronne(neurone);
  free(neurone);
  free(v);





  return 0;
}
