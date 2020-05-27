#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>

typedef struct Node {
    unsigned  long int val;
    int height;
    struct Node** next;
} Node;

typedef struct SkipList {
    int max_height;
    Node* start;
} SkipList;

int getHeight(int max_h) {
    int h = 1;
    while (h < max_h && rand() < RAND_MAX / 2) h++;
    return h;
}

Node* newNode(unsigned long int val, int level) {
    Node* n = malloc(sizeof(Node));
    n->val = val;
    n->height = level;
    n->next = malloc( level * sizeof(Node*));
    return n;
}

void deleteNode(Node* n) {
    if(n){
        free(n->next);
        free(n);
    }
}

SkipList* newSkipList(int max_height) {
    SkipList* list = malloc(sizeof(SkipList));
    list->max_height = 1;
    Node* start = malloc(sizeof(Node));
    list->start = start;
    start->next = malloc(max_height*sizeof(Node*));
    start->height = max_height;
    start->val = LONG_MAX;

    for(int i=0; i<max_height; i++) {
        start -> next[i] = list->start;
    }

    return list;
}

void deleteSkipList(SkipList* list) {
    Node* nast = list ->start;
    Node* tmp = NULL;
    while(tmp!=list->start){
        tmp = nast;
        nast = tmp ->next[0];
        for(int i = list->max_height -1;i>=0;i--){
            tmp -> next[i] = NULL;
        }
        deleteNode(tmp);
    }
    free(list);
}

Node* find(SkipList* list, unsigned long int val) {
    Node* tmp = list->start;
    for(int i =list->max_height- 1;i>=0;i--){
        while(tmp->next[i]->val < val) {
            tmp = tmp->next[i];
        }
    }
    if(tmp->next[0]->val == val){
        return tmp->next[0];
    }
    return NULL;
}

void insert(SkipList* list, unsigned long int val, int max_h) {

    Node* tmp = list->start;
    Node *update[max_h];

    for (int i = list->max_height-1; i >= 0; i--) {
        while(tmp->next[i]->val < val) {
            tmp = tmp->next[i];
        }
        update[i] = tmp;
    }

    int level = getHeight(max_h);

    if(list->max_height < level){
        for(int i = list->max_height; i < level; i++){
            update[i] = list->start;
        }
        list->max_height = level;
    }

    Node* n = newNode(val, level);

    for(int i = 0; i < level; i++){
        n -> next[i] = update[i] -> next[i];
        update[i] -> next[i] = n;
    }
}


void removeFromList(SkipList* list, unsigned long int val) {
    int i;
    Node *update[list->max_height];
    Node *tmp = list->start;
    for (i = list->max_height -1; i >= 0; i--) {
        while (tmp->next[i]->val < val)
            tmp = tmp->next[i];
        update[i] = tmp;
    }

    tmp = tmp->next[0];
    if (tmp->val == val) {
        for (i = 0; i < list->max_height; i++) {
            if (update[i]->next[i] != tmp)
                break;
            update[i]->next[i] = tmp->next[i];
        }
        deleteNode(tmp);
    }
    while(list->max_height > 1 && list->start->next[list->max_height] == list->start){
        list->max_height --;
    }
}
void jakaWysokosc(SkipList* list, int max_h){
    int* wysokosc = calloc(max_h, sizeof(int));
    Node* tmp = list->start;
    tmp = tmp->next[0];
    wysokosc[tmp->height - 1 ]++;

    while(tmp->next[0]!= list->start){
        tmp = tmp->next[0];
        wysokosc[tmp->height - 1 ]++;
    }
    for(int i=0;i<max_h;i++){
        printf("Elementow wysokosci %d jest %d\n",i+1,wysokosc[i]);
    }
}

int main(int argc, char** argv) {
    srand(time(NULL));
    for(unsigned long int i = 10000; i<=300000;i+=10000){
        unsigned long int ile_danych = i;
        unsigned long int ile = 3*ile_danych;
        int Z = 100;

        unsigned long int* insertions;
        unsigned long int* removes;
        unsigned long int* finds;
        int h = (int)log(ile);
        unsigned long int a=ile_danych, r=ile_danych, f=ile_danych;

        FILE * pHandle = fopen("dane.txt","w+");
        FILE * zapis = fopen("wyniki.txt", "w+");
        for(unsigned long int i=0; i<ile;i++){
            fprintf (pHandle, "%lu\n", rand());
        }

        for (int i=0; i<Z; i++) {

            SkipList* list = newSkipList(h);

            removes = calloc(r, sizeof(unsigned long int));
            finds = calloc(f, sizeof(unsigned long int));
            insertions = calloc(a, sizeof(unsigned long int));
            for(size_t k = 0; k < a; ++k) {
                fscanf(pHandle, "%lu", &insertions[k]);
            }
            for(size_t k = 0; k < r; ++k) {
                fscanf(pHandle, "%lu", &removes[k]);
            }
            for(size_t k = 0; k < f; ++k) {
                fscanf(pHandle, "%lu", &finds[k]);
            }

            clock_t start_t, end_t, total_t;
            start_t = clock();
            for(int j=0; j<a; j++) {
                insert(list, insertions[j], h);
            }

            end_t = clock();
            total_t = (end_t - start_t)*1000/ CLOCKS_PER_SEC;

            //jakaWysokosc(list,h);

            fprintf (zapis, "%lu\n", total_t);
           // printf("%d. Total insert time: %lu ms\n", i+1, total_t  );

            start_t = clock();
            for(int j=0; j<r; j++) {
                removeFromList(list, removes[j]);
            }
            end_t = clock();
            total_t = (end_t - start_t)*1000/ CLOCKS_PER_SEC;
            fprintf (zapis, "%lu\n", total_t);
            //printf("Total remove time: %lu ms\n", total_t  );

            start_t = clock();
            for(int j=0; j<f; j++) {
                Node* n = find(list, finds[j]);
            }
            end_t = clock();
            total_t = (end_t - start_t)*1000/ CLOCKS_PER_SEC;
            fprintf (zapis, "%lu\n", total_t);
            //printf("Total finding time: %lu ms\n", total_t  );


            deleteSkipList(list);


            free(insertions);
            free(removes);
            free(finds);
        }
        fclose (zapis);
        fclose(pHandle);

        double wynik1 = 0;
        double wynik2 = 0;
        double wynik3 = 0;
        int s, b, c;
        FILE * plik = fopen("wyniki.txt","r");
        for(int i=0;i<100;i++){
            fscanf(plik,"%d", &s);
            wynik1 +=s;
            fscanf(plik,"%d", &b);
            wynik2 +=b;
            fscanf(plik,"%d", &c);
            wynik3 +=c;
        }
        FILE * czasy_insert = fopen("czasInsert.txt","a");
        FILE * czasy_remove = fopen("czasRemove.txt","a");
        FILE * czasy_find = fopen("czasFind.txt","a");
        wynik1 = wynik1 /100.0;
        wynik2 = wynik2 /100.0;
        wynik3 = wynik3 /100.0;
        fprintf (czasy_insert, "%f\n",wynik1);
        fprintf (czasy_remove, "%f\n",wynik2);
        fprintf (czasy_find, "%f\n",wynik3);

        fclose (plik);
        fclose(czasy_insert);
        fclose(czasy_find);
        fclose(czasy_remove);
    }}
