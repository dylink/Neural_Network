#define  _GNU_SOURCE
#include <stdio.h>
#define  _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <sys/types.h>
#include "list.h"

typedef struct Config Config;
struct Config {
  double ecartType[2];
  int tailleMatrice[2];
  int iter;
  double alpha;
};

typedef struct Vector Vector;
struct Vector {
  double * value;
  int capacity;
};

typedef struct Data Data;
struct Data {
  int nbVect;
  Vector * vect;
};

typedef struct Neurone Neurone;
struct Neurone {
  int nbVect;
  Vector * vect;
  int * categorie;
};

//Initialisation de la structure des paramètres
void initConfig(Config * config){
  FILE *file = NULL;
  file = fopen("config.txt", "r");
  if(file != NULL){
    int mode = 0;
    ssize_t read;
    size_t len = 0;
    char * line = NULL, *endptr;
    int l = 0;
    while((read = getline(&line, &len, file) != -1)){
      if(strncmp(line, "Matrice:", read) == 0){ mode = 1; l = 0;}
      else if(strncmp(line, "EcartType:", read) == 0){ mode = 2; l = 0;}
      else if(strncmp(line, "NbIteration:", read) == 0){ mode = 3; l = 0;}
      else if(strncmp(line, "Alpha:", read) == 0){ mode = 4; l = 0;}
      else{
        if(mode == 1){
          config->tailleMatrice[l] = strtol(line, &endptr, 10);
        }
        if(mode == 2){
          config->ecartType[l] = strtod(line, &endptr);
        }
        if(mode == 3){
          config->iter = strtol(line, &endptr, 10);
        }
        if(mode == 4){
          config->alpha = strtod(line, &endptr);
        }
        l++;
      }
    }
    fclose(file);
    return;
  }
  printf("can't open file \n");
  exit(1);
}

void initData(Data * all){
  int i;
  FILE *file = NULL;
  file = fopen("iris.data", "r");
  int nbData = 0, nbVect = 0;
  if(file != NULL){
    ssize_t read;
    size_t len = 0;
    char * line = NULL;
    while((read = getline(&line, &len, file)) != -1){
      if(nbVect == 0){
        for(i = 0; i<(int)read; i++){
          if(line[i] == ',') nbData++;
        }
      }
      nbVect++;
    }
    all->vect = malloc(nbVect * sizeof(struct Vector));
    all->nbVect = nbVect;
    for(i = 0; i<nbVect; i++){
      all->vect[i].value = malloc(nbData * sizeof(double));
      all->vect[i].capacity = nbData;
    }
    fclose(file);
    return;
  }
  printf("can't open file \n");
  exit(1);
}

//Remplis le vecteur de données
void fillVect(Data * all){
  initData(all);
  FILE *file = NULL;
  file = fopen("iris.data", "r");
  int nbVect = 0;
  if(file != NULL){
    ssize_t read;
    size_t len = 0;
    char * line = NULL;
    int categorie = 0;
    while((read = getline(&line, &len, file)) != -1){
      char * token, *endptr;
      token = strtok(line, ",");
      int nbData = 0, max = all->vect[nbVect].capacity;
      while(token != NULL){
        if(nbData < max){
          all->vect[nbVect].value[nbData] = strtod(token, &endptr);
        }
        /*else{
          //printf("%s && %s && %d\n", temp, token, nbVect);
          if(strncmp(token, temp, read)){
            categorie++;
            all->type[nbVect] = categorie;
            //printf("Type %d\n", all->type[nbVect]);
          }
        }*/
        token = strtok(NULL, ",");
        nbData++;
      }
      nbVect++;
    }
    fclose(file);
    return;
  }
  printf("can't open file \n");
  exit(1);
}

//Normalise le vecteur de données
void fleurNorme(Data * all){
  int i, j;
  int maxV = all->nbVect;
  int maxD = all->vect[0].capacity;
  for(i = 0; i<maxV;i++){
    double som = 0;
    for(j = 0; j<maxD; j++){
      som += pow(all->vect[i].value[j],2);
    }
    double racine = sqrt(som);
    for(j = 0; j<maxD;j++){
      all->vect[i].value[j] /= racine;
    }
  }
}

//Retourne la moyenne d'une colonne
double moyenneCol(Data * all, int col){
  int i;
  double moyenne = 0;
  int max = all->nbVect;
  for(i = 0; i<max; i++){
    moyenne += all->vect[i].value[col];
  }
  return moyenne/max;
}

//Initialise le réseau de neurones
void initNeurone(Neurone * neur, int size, int nbData){
  int i;
  neur->vect = malloc(size * sizeof(struct Vector));
  neur->categorie = malloc(size * sizeof(int));
  neur->nbVect = size;
  for(i = 0; i<size; i++){
    neur->vect[i].value = malloc(nbData * sizeof(double));
    neur->vect[i].capacity = nbData;
  }
}

//Remplis le réseau de neurones
void fillVect2(Data * all, Neurone * neur, Config * config){
  int i, j;
  double nb = 5 * sqrt(all->nbVect);
  nb /= 10; nb = floor(nb); nb *= 10;
  int nbData = all->vect[0].capacity;
  initNeurone(neur, (int)nb, nbData);
  int maxNeur = neur->nbVect;

  for(i = 0; i<maxNeur;i++){
    neur->categorie[i] = 0;
    for(j = 0; j<nbData; j++){
      double range = (config->ecartType[1] - config->ecartType[0]);
      double div = RAND_MAX / range;
      double r = config->ecartType[0] + (rand() / div);
      neur->vect[i].value[j] = moyenneCol(all, j) + r;
    }
  }
}

//Initialise le tableau de parcours
void initShuffle(int * tab, int max){
  int i;
  for(i = 0; i<max; i++){
    tab[i] = i;
  }
}

//Mélange le tableau de parcours
void shuffle(int * tab, int max){
  int i;
  for(i = 0; i<max; i++){
    int div = RAND_MAX / max-1;
    int r = rand()/div;
    int temp = tab[i];
    tab[i] = tab[r];
    tab[r] = temp;
  }
}

//Trouve les coordonnées d'un index
void findIJ(int data, int nbl, int nbc, int ij[2]){
  int i;
  ij[0] = 0; ij[1] = 0;
  for(i = 0; i<nbl; i++){
    if(i*nbc <= data){
      ij[0] = i;
    }
  }
  ij[1] = data - ij[0]*nbc;
}

//Trouve le rayon d'action de la diffusion
double findLimit(int max){
  int i;
  double a = 0;
  int nbvoisin = 8;
  for(i = 0; nbvoisin<max/2; i++){
    nbvoisin += i*8;
    a++;
  }
  return a;
}

//Diffuse l'information aux voisins
void diffuse(double * all, Neurone * neur, int ij[2], int voisin, double a, int x0, int y0){
  int i, j, x;
  int i0 = ij[0] - voisin, i1 = ij[0] + voisin, j0 = ij[1] - voisin, j1 = ij[1] + voisin;
  int max = neur->vect[0].capacity;
  if(i0 < 0) i0 = 0;
  if(j0 < 0) j0 = 0;
  if(i1 > x0-1) i1 = x0-1;
  if(j1 > y0-1) j1 = y0-1;
  for(i = i0; i <= i1; i++){
    for(j = j0; j<= j1;j++){
      //printf("%d && %d && %d\n", i, j, i * y0 + j);
      for(x =0; x<max; x++){
        neur->vect[i * y0 + j].value[x] = neur->vect[i * y0 + j].value[x] + a * (all[x] - neur->vect[i * y0 + j].value[x]);
      }
    }
  }
}

//Fonction d'apprentissage
void distEuc(Data * all, Neurone * neur, Config * config, int * tab){
  int g, i, j, x;
  List *list = NULL;
  double a = config->alpha;
  double a2 = a;
  int t_total = config->iter/4, maxAll = all->nbVect, maxNeur = neur->nbVect, nbData = all->vect[0].capacity, x0 = config->tailleMatrice[0], y0 = config->tailleMatrice[1], first_total = t_total-1, last_total = (t_total+= config->iter/2)-1;
  double voisin = findLimit(maxNeur);
  double division = t_total / voisin;
  int ij[2], phase = 1;
  for(g = 0; g<t_total; g++){
    if(g>division && voisin > 1){ voisin--;  division+=division;}
    if(g==first_total && phase == 1){ voisin = 1; t_total += config->iter/2; a /= 10; a2 = a; g = 0; phase = 2;}
    if(g==last_total) return;
    shuffle(tab, maxAll);
    a2 = a * (1-((double)g/(double)t_total));
    for(i = 0; i<maxAll; i++){
      int indexGagnant = 0;
      double bestDist = 2;
      for(x = 0; x < maxNeur; x++){
        double som = 0;
        for(j = 0; j < nbData; j++){
          som += pow(all->vect[tab[i]].value[j] - neur->vect[x].value[j],2);
        }
        som = sqrt(som);
        if(som <= bestDist){
          if(som == bestDist) list_append(list, x);
          else{
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
      findIJ(indexGagnant, x0, y0, ij);
      //ij[0] = 3; ij[1] = 3;
      int b, n;
      n = (indexGagnant / 10);
      b = (indexGagnant % 10);
      //printf("%d && %d / %d && %d\n", ij[0], ij[1], n, b);
      diffuse(all->vect[tab[i]].value, neur, ij, voisin, a2, x0, y0);
    }
  }
  free(list);
}

//Etiquetage des neurones
void etiquetage(Data * all, Neurone * neur, int * tab){
  int i, j, x;
  int maxAll = all->nbVect, maxNeur = neur->nbVect, nbData = all->vect[0].capacity;
  shuffle(tab, maxAll);
  for(i = 0; i<maxAll; i++){
    int indexGagnant = 0;
    double bestDist = 2;
      for(x = 0; x < maxNeur; x++){
      double som = 0;
      for(j = 0; j < nbData; j++){
        som += pow(all->vect[tab[i]].value[j] - neur->vect[x].value[j],2);
      }
      som = sqrt(som);
      if(som < bestDist){
        bestDist = som;
        indexGagnant = x;
      }
    }
    if(tab[i] >= 0 && tab[i] < 50) neur->categorie[indexGagnant] = 1;
    if(tab[i] >= 50 && tab[i] < 100) neur->categorie[indexGagnant] = 2;
    if(tab[i] >= 100 && tab[i] < 150) neur->categorie[indexGagnant] = 3;
  }
}

//Affiche le réseau de neurones
void printNetwork(Neurone * neur, Config * config){
  int i, j;
  int x = config->tailleMatrice[0], y = config->tailleMatrice[1];
  for(i = 0; i<x; i++){
    for(j = 0; j<y; j++){
      if(!neur->categorie[i*y+j]) printf("%d", neur->categorie[i*y+j]);
      else if(neur->categorie[i*y+j] == 1) printf("\033[44m%d\033[0m", neur->categorie[i*y+j]);
      else if(neur->categorie[i*y+j] == 2) printf("\033[41m%d\033[0m", neur->categorie[i*y+j]);
      else if(neur->categorie[i*y+j] == 3) printf("\033[42m%d\033[0m", neur->categorie[i*y+j]);
    }
    printf("\n");
  }
}

//Libère les espaces alloués
void freeData(Data * data){
  int i;
  for(i = 0; i<data->nbVect; i++){
    free(data->vect[i].value);
  }
  free(data->vect);
}

void freeNeurone(Neurone * neur){
  int i;
  for(i = 0; i<neur->nbVect; i++){
    free(neur->vect[i].value);
  }
  free(neur->vect);
  free(neur->categorie);
}


int main(){

  srand(time(NULL));
  Config config;
  Data all;
  Neurone neur;
  initConfig(&config);
  fillVect(&all);
  fleurNorme(&all);
  fillVect2(&all, &neur, &config);
  int *tab = malloc(all.nbVect * sizeof(int));
  initShuffle(tab, all.nbVect);
  distEuc(&all, &neur, &config, tab);
  etiquetage(&all, &neur, tab);
  printNetwork(&neur, &config);
  freeData(&all);
  freeNeurone(&neur);
  free(tab);

  return 0;
}
