#include "include/ex3.h"

void decode(char* cmd){
    //TODO
    int  len = strlen(cmd);
    int sq = sqrt(len);
    //check if aqrt is whole
    if (sq == (sqrt(len))) {
        // sq = (int)sq;
        //printf("The number has no decimal component.\n");
        char decodedString[MAX_CHARS];
        int i = 0;
        for (int col = 0; col < sq; col++){
            for (int row = 0; row < sq; row++){
                decodedString[i] = cmd[col +(sq * row)];
                i++;
            }
        }
        printf("%s\n", decodedString);
        memset(cmd, 0, MAX_CHARS);  
        memset(decodedString, 0, MAX_CHARS);
    } else {
        //return INVALID ;
        printf("%s\n", "INVALID");
    }
    
}

int main(void){
    // Tests Variables
    char cmd[MAX_CHARS];

    //Test 1
    strcpy(cmd, "WE OUE OUT LNGSAO H RWDN!");
    decode(cmd); // Expected: WE ARE LOW ON DOUGHNUTS !
    memset(cmd,0,MAX_CHARS);

    //Test 2
    strcpy(cmd, "S EFEM FNOCEDROE");
    decode(cmd); // Expected: SEND MORE COFFEE
    memset(cmd,0,MAX_CHARS);

    //Test 3
    strcpy(cmd, "CSERULES");
    decode(cmd); // Expected: INVALID
    memset(cmd,0,MAX_CHARS);

    return 0;
}