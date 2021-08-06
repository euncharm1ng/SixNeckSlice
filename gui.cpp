#include<stdio.h>
 
void draw_check01(int column, int row);
 
int main(void)
{
    printf("바둑판\n");
    int row = 19, column = 19;
    draw_check01(column, row);
 
    return 0;
}
 
void draw_check01(int c, int r)
{
    int i, j;
    unsigned char a=0xa6;
    unsigned char b[12];
 
    for(i=1;i<12;i++)
        b[i]=0xa0+i;
    printf("%c%c", a, b[3]);
 
    for(i=0;i<c-1;i++)
        printf("%c%c", a, b[8]);
    printf("%c%c", a, b[4]);
    printf("\n");
   
    for(i=0;i<r-1;i++)
    {
        printf("%c%c", a, b[7]);
        for(j=0;j<c-1;j++)
            printf("%c%c", a, b[11]);
        printf("%c%c", a, b[9]);
        printf("\n");
    }
    printf("%c%c", a, b[6]);
    for(i=0;i<c-1;i++)
        printf("%c%c", a, b[10]);
    printf("%c%c", a, b[5]);
    printf("\n");
}
/*
void GUI::printBoard() {
#if DEBUG
    printf("print board\n");
#endif
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 19; j++) {
            if(this->board[i][j] == 1) printf("○");
            else if (this->board[i][j] == 2) printf("●");
            else printf("＃");
            printf("%s", C_NRML);
        }
        printf("\n");
    }
#if DEBUG
    printf("end of print board\n");
#endif
}
*/