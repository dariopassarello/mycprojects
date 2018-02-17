
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define SQUADRA_LUNG 30
#define SQUADRE_MAX 100
#define STRINGA_LUNG (SQUADRA_LUNG*3 + 50)
#define PUNTI_VINTA 3
#define PUNTI_PAREGGIATA 1
#define PUNTI_PERSA 0
#define ERROR -1
#define STRINGA_PUNTI 5
#define NON_DISP "N"
typedef enum {false,true} bool;

typedef struct
{
    char nome[SQUADRA_LUNG];
}
squadra_t;

typedef struct
{
    squadra_t elencoSquadre[SQUADRE_MAX];
    int numeroSquadre;
} 
girone_t;

typedef struct
{
    squadra_t squadra;
    int vinte;
    int pareggiate;
    int perse;
    int golFatti;
    int golSubiti;
}
classifica_t;


void stampaGirone(girone_t girone);
girone_t leggiGirone(FILE *input);
void stampaFileBeger(girone_t girone,FILE *output);
girone_t ruotaGirone(girone_t girone,int posti);
int trovaSquadra(girone_t girone,squadra_t squadra);
classifica_t *calcolaClassifica(girone_t girone,FILE *partite);
void stampaClassifica(girone_t girone,classifica_t *classifica,FILE *partite,FILE *output);
bool squadreUguali(squadra_t a,squadra_t b);

bool squadreUguali(squadra_t a,squadra_t b)
{
    bool uguali = true;
    int i = 0;
    while(uguali == true && a.nome[i] != '\0' && b.nome != '\0')
    {
        if(a.nome[i] != b.nome[i])
        {
            uguali = false;
        }
        else
        {
            i++;
        }
    }
    if(a.nome[i] != b.nome[i])
    {
        uguali = false;
    }
    return uguali;
}

girone_t leggiGirone(FILE *input)
{
    girone_t girone;
    squadra_t squadraCorr;
    char carLetto;
    int i;
    rewind(input);
    girone.numeroSquadre = 0;
    i = 0;
    carLetto = fgetc(input);
    while(carLetto != EOF && girone.numeroSquadre < SQUADRE_MAX)
    {
        //printf("%c",carLetto);
        if(carLetto != '\n')
        {
            girone.elencoSquadre[girone.numeroSquadre].nome[i] = carLetto;
            i++;
        }
        else
        {
            //printf("%s",girone.elencoSquadre[girone.numeroSquadre].nome);

            girone.numeroSquadre++;
            //printf("%d",girone.numeroSquadre);
            i = 0;
        }
   
        carLetto = fgetc(input);

    }

    girone.numeroSquadre++;
    return girone;
}

//Applica l'algoritmo di Beger creando le giornate e gli abbinamenti
void stampaFileBeger(girone_t girone,FILE *output)
{
    int giornata,i;
    girone_t gironeCorr = girone;
    for(giornata = 1; giornata < (girone.numeroSquadre + girone.numeroSquadre % 2); giornata++)
    {
        gironeCorr = ruotaGirone(girone,giornata - 1);
        stampaGirone(gironeCorr);
        i = 0;
        fprintf(output,"GIORNATA %d\n",giornata);
        while(i < (girone.numeroSquadre - (girone.numeroSquadre % 2)))
        {
            
            fprintf(output,"%s - %s [%s] - [%s]\n",gironeCorr.elencoSquadre[i].nome,gironeCorr.elencoSquadre[i+1].nome,NON_DISP,NON_DISP);
            i = i + 2;
        }
        if(girone.numeroSquadre % 2 == 1)
        {
            fprintf(output,"RIPOSO: %s\n",gironeCorr.elencoSquadre[i].nome);
        }
    }
}

void stampaGirone(girone_t girone)
{
    printf("\nGIRONE: ");
    for(int i = 0; i < girone.numeroSquadre; i++)
    {
        printf("\n%d.%s",i+1,girone.elencoSquadre[i].nome);
    }
}

girone_t ruotaGirone(girone_t girone,int posti)
{
    int i,k, nuovaPos;
    girone_t nuovoGirone, gironeTemp;
    nuovoGirone.numeroSquadre = girone.numeroSquadre;
    nuovoGirone.elencoSquadre[0] = girone.elencoSquadre[0];
    for(i = 1; i < girone.numeroSquadre; i++)
    {
        nuovaPos = i - posti;
        if(nuovaPos <= 0)
        {
            nuovaPos = girone.numeroSquadre - 1 + nuovaPos;
        }
        nuovoGirone.elencoSquadre[nuovaPos] = girone.elencoSquadre[i];
    }
    if(posti == girone.numeroSquadre - 1 && girone.numeroSquadre % 2 == 1)
    {
        gironeTemp.numeroSquadre = nuovoGirone.numeroSquadre;
        for(i = girone.numeroSquadre - 1; i >= 0; i--)
        {
            gironeTemp.elencoSquadre[i] = nuovoGirone.elencoSquadre[girone.numeroSquadre - i -1];
        }
        return gironeTemp;
    }

    return nuovoGirone;
}

int trovaSquadra(girone_t girone,squadra_t squadra)
{
    int i;
    bool trovato = false;
    for(i = 0; i < girone.numeroSquadre && trovato == false; i++)
    {
        if(squadreUguali(girone.elencoSquadre[i],squadra))
        {
            trovato = true;
        }
    }
    if(trovato == false)
    {
        return ERROR;
    }
    else
    {
        return (i-1);
    }
    
}

classifica_t *calcolaClassifica(girone_t girone,FILE *partite)
{
    char *stringa = malloc(STRINGA_LUNG*sizeof(char)); //Contiene l'ultima riga letta
    char *stringaPunti1 = malloc(STRINGA_PUNTI*sizeof(char)); //Contiene il punteggio di del giocatore "in casa"
    char *stringaPunti2 = malloc(STRINGA_PUNTI*sizeof(char)); //Contiene il punteggio di del giocatore "ospite"
    int giornata;  //Contatore giornate
    int i,     
        j,    
        linea,   //Contiene il numero di linea letto (PER GLI ERRORI)
        esito,   //Esito lettura riga (Deve essere uguale a 4 altrimenti la lettura è fallita)
        punti1,  //Punteggio convertito del giocatore in casa
        punti2,  //Punteggio convertito del giocatore ospite
        indice1, //Contiene la posizone della squadra nell'array del girone della squadra in casa
        indice2, //Contiene la posizone della squadra nell'array del girone della squadra ospite
        esitoConv1,   //Contiene l'esito della conversione da stringa in int del punteggio della squadra
        esitoConv2;   //Contiene l'esito della covnersione da stringa in int del punteggio della squadra
    squadra_t squadra1, squadra2; //Squadra in casa e ospite
    classifica_t *classifica;  //Classifica(ARRAY)

    classifica = malloc(sizeof(classifica_t)*SQUADRE_MAX); //Alloca memoria per classifica
    rewind(partite); //Resetta la posizione del cursore del file di lettura delle partite
    linea = 0;  //Inizializza la linea
    //Inizializza la classifica 
    for(i = 0; i < girone.numeroSquadre; i++)
    {

        classifica[i].squadra = girone.elencoSquadre[i]; //Inserisce le squadre in classifica
        classifica[i].vinte = 0;
        classifica[i].pareggiate = 0;
        classifica[i].perse = 0;
        classifica[i].golFatti = 0;
        classifica[i].golSubiti = 0;

    }
    /*
     * Ciclo che controlla le partite contenute nel file *partite. Se n è il numero delle squadre n - 1 è il numero delle giornate se n è pari,.
     * Altrimenti il numero delle giornate è n
     */
    for(giornata = 1; giornata < (girone.numeroSquadre + (girone.numeroSquadre % 2)); giornata++)
    {
        stringa = fgets(stringa,STRINGA_LUNG,partite); //Legge la prima riga contenente il numero della giornata
        linea++;
        printf("\n");
        if(stringa == NULL)
        {
            printf("\nERRORE FORMATO FILE, HEDER GIORNATA LINEA %d",linea);
            return NULL;
        }
        //Ciclo che legge riga per 
        for(i = 0;i < girone.numeroSquadre / 2;i++)
        {
            stringa = fgets(stringa,STRINGA_LUNG,partite); //Legge l'i-esima partita della giornata
            if(stringa == NULL)
            {
                printf("\nERRORE FORMATO FILE, LINEA %d",linea);
                return NULL;
            }
            linea++;
            //Conversione stringa letta nei dati squadra1,squadra2,punti1,punti2
            esito = sscanf(stringa,"%s - %s [%[^][]] - [%[^][]]",squadra1.nome,squadra2.nome,stringaPunti1,stringaPunti2);
            if(esito != 4)
            {
                printf("\nERRORE FORMATO FILE (COPIATURA PARTITA), LINEA %d",linea);
                return NULL ;
            }
            indice1 = trovaSquadra(girone,squadra1); //Trova l'indice delle squadre
            indice2 = trovaSquadra(girone,squadra2);
            //printf("\nCasa: %s, Fuoricasa: %s",classifica[indice1].squadra.nome,classifica[indice2].squadra.nome);
            if(indice1 == ERROR)
            {
                printf("\nERRORE NOME SQUADRA 1, LINEA %d",linea);
                return NULL;
            }
            if(indice2 == ERROR)
            {
                printf("\nERRORE NOME SQUADRA 2, LINEA %d",linea);
                return NULL;
            }
            esitoConv1 = sscanf(stringaPunti1,"%d",&punti1); //Converte (se possibile) la stringa in punteggio
            esitoConv2 = sscanf(stringaPunti2,"%d",&punti2); //Se la conversione fallisce salta la partita (probabilmente non è stata disputata)
            if(esitoConv1 != 1 || esitoConv2 != 1)
            {
                printf("\nPARTITA NON DISPUTATA, LINEA %d. SKIPPATA",linea);
            }
            else
            {
                //Segno esito partite nella classifica
                if(punti1 > punti2)
                {
                    printf("\n%s vince contro %s, %d - %d",classifica[indice1].squadra.nome,classifica[indice2].squadra.nome,punti1,punti2);
                    classifica[indice1].vinte++;
                    classifica[indice2].perse++;
                }
                if(punti1 == punti2)
                {
                    printf("\n%s pareggia con %s, %d - %d",classifica[indice2].squadra.nome,classifica[indice1].squadra.nome,punti1,punti2);
                    classifica[indice1].pareggiate++;
                    classifica[indice2].pareggiate++;
                }
                if(punti2 > punti1)
                {
                    printf("\n%s vince contro %s, %d - %d",classifica[indice2].squadra.nome,classifica[indice1].squadra.nome,punti1,punti2);
                    classifica[indice1].perse++;
                    classifica[indice2].vinte++;
                }
                classifica[indice1].golFatti += punti1;
                classifica[indice1].golSubiti += punti2;
                classifica[indice2].golFatti += punti2;
                classifica[indice2].golSubiti += punti1;
            }
        }
        if(girone.numeroSquadre % 2 == 1)
        {
            //Se il numero di squadre è dispari ignora la riga che segna il "RIPOSO"
            stringa = fgets(stringa,STRINGA_LUNG,partite);
            linea++;
            if(stringa == NULL)
            {
                printf("\nERRORE FORMATO FILE, LINEA %d",linea);
                return NULL;
            }
        }
        
    }
    return classifica;
}

classifica_t *ordinaClassifica(girone_t girone,classifica_t *classifica)
{
    int i,j,punti1,punti2,dfg1,dfg2;
    bool scambio;
    classifica_t temp;
    for(i = 0; i < girone.numeroSquadre - 1; i++)
    {
        for(j = i + 1; j < girone.numeroSquadre; j++)
        {
            punti1 = (classifica[i].vinte * PUNTI_VINTA) + (classifica[i].pareggiate * PUNTI_PAREGGIATA) + (classifica[i].perse * PUNTI_PERSA);
            punti2 = (classifica[j].vinte * PUNTI_VINTA) + (classifica[j].pareggiate * PUNTI_PAREGGIATA) + (classifica[j].perse * PUNTI_PERSA);
            dfg1 = classifica[i].golFatti - classifica[i].golSubiti;
            dfg2 = classifica[j].golFatti - classifica[j].golSubiti;
            if((punti1 < punti2) || ((punti1 == punti2)&&(dfg1 < dfg2)))
            {
                temp = classifica[i];
                classifica[i] = classifica[j];
                classifica[j] = temp;
            }
        }
    }
    return classifica;
}



void stampaClassifica(girone_t girone,classifica_t *classifica,FILE *partite,FILE *output)
{
    int i,punti,dfg,gioc;
    classifica = calcolaClassifica(girone,partite);
    classifica = ordinaClassifica(girone,classifica);
    printf("\nCLASSIFICA");
    for(i = 0; i < girone.numeroSquadre; i++)
    {
        punti = (classifica[i].vinte * PUNTI_VINTA) + (classifica[i].pareggiate * PUNTI_PAREGGIATA) + (classifica[i].perse * PUNTI_PERSA);
        gioc = classifica[i].vinte + classifica[i].pareggiate + classifica[i].perse;
        dfg = classifica[i].golFatti - classifica[i].golSubiti;
        fprintf(output,"\n%d.%s\tGIOC=%d\tPT=%d\tDIFF=%d",i + 1,classifica[i].squadra.nome,gioc,punti,dfg);
    }
}

int main(int argc,char *argv[])
{
    girone_t girone;
    classifica_t classifica[SQUADRE_MAX];
    FILE *squadre, *partite, *output;
    //partite = fopen("partite.txt","w");
    printf("PROGRAMMA DI GESTIONE TORNEI");
    if(argc == 1)
    {
        printf("\nSCHERMATA DI AIUTO\n");
    }
    if(argc > 2)
    {
        if(strcmp(argv[1],"genera") == 0)
        {
            printf("\nGENERAZIONE PARTITE...");
            squadre = fopen(argv[2],"r");
            if(squadre == NULL)
            {
                printf("\nERRORE DI I/O, SQUADRE");
                return 1;
            }
            if(argc == 3)
            {
                printf("\nVISUALIZZAZIONE CALENDARIO. N = NON DISPUTATA. PER MODIFICARE PARTITE INSERIRE FILE OUTPUT\n");
                partite = stdout;
            }
            else
            {
                partite = fopen(argv[3],"w");
                if(partite == NULL)
                {
                    printf("\nERRORE DI I/O\n");
                    return 1;
                }
                printf("\nCALENDARIO SALVATO IN %s. ORA E' POSSIBILE INSERIRE I RISULTATI, SOSTITUENDOLI ALLE 'N' \n");
            }
            girone = leggiGirone(squadre);
            stampaFileBeger(girone,partite);
            fclose(squadre);
            if(argc != 3)
            {
                fclose(partite);
            }
        }
        else if(strcmp(argv[1],"genera"))
        {
            squadre = fopen(argv[2],"r");
            if(squadre == NULL)
            {
                printf("\nERRORE DI I/O, SQUADRE\n");
                return 1;
            }
            girone = leggiGirone(squadre);
            partite = fopen(argv[3],"r");
            if(partite == NULL)
            {
                printf("\nERRORE DI I/O, PARTITE\n");
                return 1;
            }
            if(argc == 4)
            {
                output = stdout;
            }
            else
            {
                output = fopen(argv[4],"w");
                if(output == NULL)
                {
                    printf("\nERRORE DI I/O, OUTPUT\n");
                    return 1;
                }
            }
            stampaClassifica(girone,classifica,partite,output);
        }
    }

}