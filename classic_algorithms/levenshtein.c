#include <stdio.h>
#include <string.h>

/*

levenshtein distance is basically how many edits that is insert,delete or updates does it 
take to convert from one string to other.

we have two pointers i , j for two strings.
if two characters doesnt match then check for
i, j-1 - insert from B current char to A.
i-1, j - delete A string character.
i-1,j-1 - replace with B character.

*/

int dp[100][100];
char ops[100][100];

int min(int a, int b , int c){
    int min = a;
    if(b < min)
        min = b;
    if(min > c)
        min = c;

    return min;
}

int levenshtein(char A[], char B[]){

    int n = strlen(A);
    int m = strlen(B);

    for(int i = 0 ; i <= n ; ++i)
        dp[i][0] = i;
    for(int i = 0 ; i <= m ; ++i)
        dp[0][i] = i;
    
    
    for(int i = 1 ; i <= n ; ++i){
        for(int j = 1; j <= m ; ++j){
            if(A[i-1] == B[j-1])
                dp[i][j] = dp[i-1][j-1];
            else{
                dp[i][j] = 1 + min(dp[i-1][j], dp[i-1][j-1], dp[i][j-1]);
            }
        }
    }
    
    return dp[n][m];
}

int main(){

char A[] = "book";
char B[] = "back";

printf("edit distance is %d\n", levenshtein(A, B));

return 0;

}