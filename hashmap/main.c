#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INITIAL_CAPACITY 5

typedef struct Node {
    int *keys;
    int *values;
    int size;
    int capacity;
} Node;

typedef struct HashMap {
    int bucket_count;
    Node** buckets;
    bool flag;
    int total_entries;
} HashMap;

float calculate_load_factor(int buckets, int total_entries){
    return total_entries/buckets;
}

int hash(HashMap* hm, int key) {
    return key % hm->bucket_count;
}

HashMap* rehash(HashMap* hm){

    hm -> flag = 1;
    HashMap *new_hm = create_map(hm -> bucket_count * 2);

    for(int i = 0 ; i < hm -> bucket_count ; ++i){
        Node* curr = hm -> buckets[i];
        for(int j = 0 ; j < curr -> size ; ++j)
            put(new_hm, curr -> keys[j], curr -> values[j]);
    }
    
    return new_hm;
}

Node* create_node() {
    Node* node = malloc(sizeof(Node));
    node->capacity = INITIAL_CAPACITY;
    node->size = 0;
    node->keys = malloc(sizeof(int) * INITIAL_CAPACITY);
    node->values = malloc(sizeof(int) * INITIAL_CAPACITY);
    return node;
}

HashMap* create_map(int bucket_count) {
    HashMap* map = malloc(sizeof(HashMap));
    map -> flag = false;
    map -> total_entries = 0;
    map->bucket_count = bucket_count;
    map->buckets = calloc(bucket_count, sizeof(Node*));
    return map;
}

void put(HashMap* hm, int key, int value){
    if(hm -> flag)
        return;

    if(calculate_load_factor(hm -> total_entries , hm -> bucket_count) > 1.0)
        hm = rehash(hm);

    int idx = hash(hm, key);
    
    if (hm->buckets[idx] == NULL) {
        hm->buckets[idx] = create_node();
    }
    
    Node* curr = hm->buckets[idx];
    hm -> total_entries++;

    printf("pushed %d and %d into bucket %d\n", key, value, idx);
    
    for(int i = 0; i < curr->size; i++) {
        if(curr->keys[i] == key) {
            curr->values[i] = value;  
            return;
        }
    }
    
    if (curr->size >= curr->capacity) {
        curr->capacity *= 2;
        curr->keys = realloc(curr->keys, sizeof(int) * curr->capacity);
        curr->values = realloc(curr->values, sizeof(int) * curr->capacity);
    }
    
    curr->keys[curr->size] = key;
    curr->values[curr->size] = value;
    curr->size++;
}

int get(HashMap* hm, int key) {
    if(hm -> flag)
        return -1;

    int idx = hash(hm, key);
    
    Node* curr = hm->buckets[idx];
    
    if(curr == NULL || curr->size == 0)
        return -1;
    
    for(int i = 0; i < curr->size; i++) {
        if(curr->keys[i] == key)
            return curr->values[i];
    }
    return -1;
}

int main() {
    HashMap* hm = create_map(10);
    
    put(hm, 69, 420);
    printf("got: %d\n", get(hm, 69));
    
    put(hm, 79, 100); 
    printf("got 79: %d\n", get(hm, 79));
    printf("got 69 again: %d\n", get(hm, 69));
    
    return 0;
}