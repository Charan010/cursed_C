#include <stdio.h>

#define N 69

int parent[N];
int rank[N];
int weights[N];


void make_set(int n){
    for(int i = 0 ; i < n ; ++i){
        parent[i] = i;
        rank[i] = 0;
        weights[i] = 0;   
    }
}


int find(int x){
    if(parent[x]!=x){
        int actual_p = parent[x];
        parent[x] = find(parent[x]);
        weights[x] += weights[actual_p];
    }
    return parent[x];
}


void union_set(int x, int y, int w){
    int rootX = find(x);
    int rootY = find(y);
    
    //if both belong to same set or parent then why union.
    if(rootX == rootY)
        return;
    
    //basically when trying to union two different sets, try to attach smaller set to larger set to avoid having huge tree.
    if(rank[rootX] < rank[rootY]){
        parent[rootX] = rootY;
        weights[rootX] = weights[y] - weights[x] + w;
    } else {
        parent[rootY] = rootX;
        weights[rootY] = weights[x] + w - weights[y]; 
        if(rank[rootX] == rank[rootY])
            rank[rootX]++;
    }
}

int main(){

    int n = 10;
    make_set(n);

    union_set(0, 2,2);
    union_set(4,2, 3);
    union_set(3, 1,5);
    union_set(9,1,4);
    union_set(0, 1, 6);

    printf("Parent array: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", parent[i]);
    }
    printf("\n");

    printf("parent of node %d is %d and weight is %d\n",9, find(9),weights[9]);

    return 0;

}