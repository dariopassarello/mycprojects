#include<stdio.h>
#include<stdlib.h>

#define DIM 20
#define EPSILON 1e-12

typedef struct
{
    float riga[DIM]; 
    int numColonne; //Numero elementi riga ("Colonne")
}
riga_t;

typedef struct
{
    float matrix[DIM][DIM]; //matrix[INDICE RIGA][INDICE COLONNA]
    int numRighe;           
    int numColonne;
}
matrice_t;

matrice_t MEG(matrice_t matrice);
matrice_t trovaSottoMatrice(matrice_t matrice);
riga_t estraiRiga(matrice_t matrice,int numRiga);
matrice_t aggiungiRiga(matrice_t matrice,riga_t riga);
int posPivot(matrice_t matrice, int riga);
matrice_t ordinaPerPosPivot(matrice_t matrice);
matrice_t leggiDaFile(char *path,int righe,int colonne);
void stampaMatrice(matrice_t matrice);
float absol(float val);


matrice_t MEG(matrice_t matrice)
{
    matrice_t sottoMatrice;
    riga_t primaRiga;
    int i,j;
    float coeff;
    if(matrice.numRighe < 2)
    {
        return matrice;
    }
    else
    {
        matrice = ordinaPerPosPivot(matrice);
        stampaMatrice(matrice);
        for(i = 1; i < matrice.numRighe; i++)
        {
            if(posPivot(matrice,0) < matrice.numColonne && posPivot(matrice,0) == posPivot(matrice,i))  //Funziona anche senza la seconda condizione
            {
                printf("%d",posPivot(matrice,0));
                coeff = -1*(matrice.matrix[i][posPivot(matrice,0)]/matrice.matrix[0][posPivot(matrice,0)]);            
                for(j = posPivot(matrice,0); j < matrice.numColonne; j++)
                {
                    matrice.matrix[i][j] += matrice.matrix[0][j]*coeff;
                }
            }
        }
        stampaMatrice(matrice);
        sottoMatrice = trovaSottoMatrice(matrice);
        primaRiga = estraiRiga(matrice,0);
        return aggiungiRiga(MEG(sottoMatrice),primaRiga);
    }
}

matrice_t trovaSottoMatrice(matrice_t matrice)
{
    matrice_t sottoMatrice;
    int i,j;
    sottoMatrice.numColonne = matrice.numColonne;
    sottoMatrice.numRighe = matrice.numRighe - 1;

    for(i = 1; i < matrice.numRighe; i++)
    {
        for(j = 0; j < matrice.numColonne; j++)
        {
            sottoMatrice.matrix[i - 1][j] = matrice.matrix[i][j]; 
        }
    }
    return sottoMatrice;
}

riga_t estraiRiga(matrice_t matrice,int numRiga)
{
    int i;
    riga_t riga;
    riga.numColonne = matrice.numColonne;
    for(i = 0; i < riga.numColonne; i++)
    {
        riga.riga[i] = matrice.matrix[numRiga][i];
    }
    return riga;
}

matrice_t aggiungiRiga(matrice_t matrice,riga_t riga)
{
    matrice_t orlata;
    int i,j;
    orlata.numColonne = matrice.numColonne;
    orlata.numRighe = matrice.numRighe + 1;
    for(j = 0; j < matrice.numColonne; j++)
    {
        orlata.matrix[0][j] = riga.riga[j];
    }
    for(i = 1; i < orlata.numRighe; i++)
    {
        for(j = 0; j < orlata.numColonne; j++)
        {
            orlata.matrix[i][j] = matrice.matrix[i - 1][j];
        }
    }
    return orlata;
}

int posPivot(matrice_t matrice, int riga)
{
    int i;
    for(i = 0;i < matrice.numColonne && absol(matrice.matrix[riga][i]) < EPSILON;i++);
    return i;

}

matrice_t ordinaPerPosPivot(matrice_t matrice)
{
    int i,j,k,pos1,pos2;
    float rigaTemp[DIM];
    for(i = 0; i < matrice.numRighe - 1; i++)
    {
        for(j = i + 1; j < matrice.numRighe;j++)
        {
            pos1 = posPivot(matrice,i);
            pos2 = posPivot(matrice,j);
            //printf("\n%d %d",pos1,pos2);
            if(pos1 > pos2)
            {
                //printf("swap %d %d",i,j);
                for(k = 0; k < matrice.numColonne;k++)
                {
                    rigaTemp[k] = matrice.matrix[i][k];
                }
                for(k = 0; k < matrice.numColonne;k++)
                {
                    matrice.matrix[i][k] = matrice.matrix[j][k];
                }
                for(k = 0; k < matrice.numColonne;k++)
                {
                    matrice.matrix[j][k] = rigaTemp[k];
                }
            }
        }
    }
    return matrice;
}

matrice_t leggiDaFile(char *path,int righe,int colonne)
{
    matrice_t matrice;
    FILE *input;
    int i,j;
    float val;
    input = fopen(path,"r");
    for(i = 0; i < righe; i++)
    {
        for(j = 0; j < colonne; j++)
        {
            fscanf(input,"%f",&(matrice.matrix[i][j]));
        }
    }
    fclose(input);
    matrice.numColonne = colonne;
    matrice.numRighe = righe;
    return matrice;
}

void stampaMatrice(matrice_t matrice)
{
    int i,j;
    for(i = 0; i < matrice.numRighe; i++)
    {
        printf("\n");
        for(j = 0; j < matrice.numColonne; j++)
        {
            printf("\t%.2f\t",matrice.matrix[i][j]);
        }
    }
    printf("\n\n");

}

float absol(float val)
{
    if(val > 0)
    {
        return val;
    }
    else
    {
        return -val;
    }
}

int main()
{
    matrice_t matrice;
    matrice = leggiDaFile("meg.txt",4,4);
    stampaMatrice(matrice);
    for(int i = 0; i < matrice.numRighe; i++)
    {
        printf("\n%d",posPivot(matrice,i));
    }
    matrice = MEG(matrice);
    stampaMatrice(matrice);
}