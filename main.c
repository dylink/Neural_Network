#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "list.h"

#define SIZE 150
#define SIZE_T 60
#define DATA_SIZE 4

typedef struct Fleur Fleur;
struct Fleur {
  double * vect;
  char * type;
};

typedef struct Neurone Neurone;
struct Neurone {
  double * vect;
  int categorie;
};

void fillVect(Fleur * all){
  for(int i = 0; i<SIZE; i++){
    all[i].vect = malloc(DATA_SIZE * sizeof(double));
  }
  FILE *file = NULL;
  file = fopen("iris.data", "r");
  double data[DATA_SIZE];
  char *type = malloc(30 * sizeof(char));
  int i = 0;
  if(file != NULL){
    while(fscanf(file, "%lf, %lf, %lf, %lf, %s", &data[0], &data[1], &data[2], &data[3], type) != EOF){
      for(int j = 0; j<DATA_SIZE; j++){
        all[i].vect[j] = data[j];
      }
      all[i].type = strdup(type);
      i++;
    }
    fclose(file);
    return;
  }
  printf("can't open file \n");
  exit(1);
}


void fleurNorme(Fleur * all){
  for(int i = 0; i<SIZE;i++){
    double som = 0;
    for(int j = 0; j<DATA_SIZE; j++){
      som += pow(all[i].vect[j],2);
    }
    for(int j = 0; j<DATA_SIZE;j++){
      all[i].vect[j] /= sqrt(som);
    }
  }
}


double moyenneCol(Fleur * all, int col){
  double moyenne = 0;
  for(int i = 0; i<SIZE; i++){
    moyenne += all[i].vect[col];
  }
  return moyenne/SIZE;
}


void fillVect2(Fleur * all, Neurone * neur){
  for(int i = 0; i<SIZE; i++){
    neur[i].vect = malloc(DATA_SIZE * sizeof(double));
  }
  for(int i = 0; i<SIZE_T;i++){
    neur[i].categorie = 0;
    for(int j = 0; j<DATA_SIZE; j++){
      double range = (0.025 - (-0.05));
      double div = RAND_MAX / range;
      double r = (-0.05) + (rand() / div);
      neur[i].vect[j] = moyenneCol(all, j) + r;
    }
  }
}


void shuffle(int * tab){
  for(int i = 0; i<SIZE; i++){
    tab[i] = i;
  }
  for(int i = 0; i<SIZE; i++){
    int div = RAND_MAX / SIZE-1;
    int r = rand()/div;
    int temp = tab[i];
    tab[i] = tab[r];
    tab[r] = temp;
  }
}


void findIJ(int data, int nbl, int nbc, int ij[2]){
  ij[0] = 0; ij[1] = 0;
  for(int i = 0; i<nbl; i++){
    if(i*nbc < data){
      ij[0] = i;
    }
  }
  ij[1] = data - ij[0]*nbc;
}


int findFloor(int ij[2], int limit){
  int a = 0, x = ij[0], y = ij[1];
  for(int i = 0; i<limit; i++){
    for(int j = 0; j<limit; j++){
      if(i == 0 && j!= 0){
        if(y+j < 10 ) a++;
        if(y-j >= 0 ) a++;
      }
      else if(j == 0 && i!= 0){
        if(x+i < 6 ) a++;
        if(x-i >= 0 ) a++;
      }
      if(i == 0 && j == 0) continue;
      else if(i > 0 && j > 0){
        if(x+i < 6 && y+j < 10) a++;
        if(x-i >= 0 && y+j < 10) a++;
        if(x+i < 6 && y-j >=0) a++;
        if(x-i >=0 && y-j >=0) a++;
      }
    }
  }
  if(a<30){limit++; return findFloor(ij, limit);}
  else return limit-1;
}

double findLimit(){
  double a = 0;
  for(int i = 8; i<SIZE_T/2; i+=8){
    a++;
  }
  return a;
}

void diffuseNeur(double* all, double* neur, double a){
  for(int i =0; i<DATA_SIZE; i++){
    neur[i] = neur[i] + a * (all[i] - neur[i]);
  }
}


void diffuse(double * all, Neurone * neur, int ij[2], int voisin, double a){
  int x = ij[0], y=ij[1];
  for(int i = 0; i <= voisin; i++){
    for(int j = 0; j<=voisin; j++){
      if(i == 0 && j != 0){
        if(y+j < 10 ){
          //printf("%d && %d\n", x, y+j);
          diffuseNeur(all, neur[x * 10 + (y+j)].vect, a);
         }
        if(y-j >= 0 ){
          //printf("%d && %d\n", x, y-j);
          diffuseNeur(all, neur[x * 10 + (y-j)].vect, a);}
      }
      else if(j == 0 && i != 0){
        if(x+i < 6 ){
          //printf("%d && %d\n", x+i, y);
          diffuseNeur(all, neur[(x+i) * 10 + y].vect, a);}
        if(x-i >= 0 ){
          //printf("%d && %d\n", x-i, y);
          diffuseNeur(all, neur[(x-i) * 10 + y].vect, a);}
      }
      if(i == 0 && j == 0) continue;
      else if(i > 0 && j > 0){
        if(x+i < 6 && y+j < 10){
          //printf("%d && %d\n", x+i, y+j);
          diffuseNeur(all, neur[(x+i) * 10 + (y+j)].vect, a);}
        if(x-i >= 0 && y+j < 10){
          //printf("%d && %d\n", x-i, y+j);
          diffuseNeur(all, neur[(x-i) * 10 + (y+j)].vect, a);}
        if(x+i < 6 && y-j >=0){
          //printf("%d && %d\n", x+i, y-j);
          diffuseNeur(all, neur[(x+i) * 10 + (y-j)].vect, a);}
        if(x-i >=0 && y-j >=0){
          //printf("%d && %d\n", x-i, y-j);
          diffuseNeur(all, neur[(x-i) * 10 + (y-j)].vect, a);}
      }
    }
  }
}

void distEuc(Fleur * all, Neurone * neur, int * tab){
  List *list = NULL;
  double a = 0.8;
  double voisin = findLimit();
  double a2 = a, t_total = 500;
  double division = t_total / voisin;
  int ij[2], phase = 1;
  for(double g = 0; g<t_total; g++){
    if(g>division && voisin > 1){ voisin--;  division+=division;}
    if(g==499 && phase == 1){ voisin = 1; t_total = 1500; a = 0.08; a2 = a; g = 0; t_total = 1500; phase = 2;}
    if(g==1499) return;
    shuffle(tab);
    a2 = a * (1-(g/t_total));
    for(int i = 0; i<SIZE; i++){
      int indexGagnant = 0;
      double bestDist = 9999;
      for(int x = 0; x < SIZE_T; x++){
        double som = 0;
        for(int j = 0; j < DATA_SIZE; j++){
          som += pow(all[tab[i]].vect[j] - neur[x].vect[j],2);
        }
        som = sqrt(som);
        if(som <= bestDist){
          if(som == bestDist){
            list_append(list, x);
          }
          else{
            free(list);
            list = NULL;
            list = list_append(list, x);
          }
          bestDist = som;
        }
      }
      indexGagnant = list->data;
      if(list_length(list) > 1){
        int div = RAND_MAX / list_length(list);
        int r = (rand() / div);
        indexGagnant = list_getDataByIndex(list, r);
      }
      findIJ(indexGagnant, 6, 10, ij);
      diffuse(all[tab[i]].vect, neur, ij, voisin, a2);
    }
  }
  free(list);
}


void etiquetage(Fleur * all, Neurone * neur){
  for(int i = 0; i<SIZE; i++){
    int indexGagnant = 0;
    double bestDist = 9999;
      for(int x = 0; x < SIZE_T; x++){
      double som = 0;
      for(int j = 0; j < DATA_SIZE; j++){
        som += pow(all[i].vect[j] - neur[x].vect[j],2);
      }
      som = sqrt(som);
      if(som < bestDist){
        bestDist = som;
        indexGagnant = x;
      }
    }
    if(i >= 0 && i < 50) neur[indexGagnant].categorie = 1;
    else if(i >=50 && i < 100) neur[indexGagnant].categorie = 2;
    else if(i >=100 && i < 150) neur[indexGagnant].categorie = 3;
  }
}

void printNetwork(Neurone * neur){
  for(int i = 0; i<6; i++){
    for(int j = 0; j<10; j++){
      if(!neur[i*10+j].categorie) printf("%d", neur[i*10+j].categorie);
      else if(neur[i*10+j].categorie == 1) printf("\033[44m%d\033[0m", neur[i*10+j].categorie);
      else if(neur[i*10+j].categorie == 2) printf("\033[41m%d\033[0m", neur[i*10+j].categorie);
      else if(neur[i*10+j].categorie == 3) printf("\033[42m%d\033[0m", neur[i*10+j].categorie);
    }
    printf("\n");
  }
}


int main(){
  srand(time(NULL));
  Fleur * all = malloc(SIZE * sizeof(struct Fleur));
  fillVect(all);
  fleurNorme(all);
  Neurone * neur = malloc(SIZE_T * sizeof(struct Neurone));
  fillVect2(all, neur);
  int *tab = malloc(SIZE * sizeof(int));
  distEuc(all, neur, tab);
  etiquetage(all, neur);
  printNetwork(neur);

  free(all);
  free(neur);
  free(tab);

  return 0;
}
