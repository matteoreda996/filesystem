/*
	La struttura che ho creato è costituita da semplici array di struct, che corrospondono a tabelle hash di varie dimensioni.
	La prima cella di ogni array viene utilizzata come cella di appoggio, nella quale è memorizzata la posizione dell'ultimo figlio creato,
	la dimensione della tabella e il puntatore al padre, e non può essere utilizzata per salvare una risorsa.
	Per quanto riguarda l'algoritmo di ricerca, ho fatto una semplice ricerca in post-order, partendo quiindi dall'ultimo elemento più a destra, 
	analizzo i singoli nodi dell'albero.
	Le funzioni di create_dir, create, write, read, delete e delete_r, la parte di codice dove si va a cercare la risorsa su cui lavorare è molto simile,
	se non uguale.
*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define NAMELEN 255				// lunghezza massima di un nome
#define TREELEN 255				// altezza massima dell'albero

typedef struct el {
	char			*nome;						// nome della risorsa
	int 			conta_figli;				// tiene in memoria il numero di figli
	int				bit_file;					// bit riconoscimento di una risorsa, se file->bit=1, se dir->bit=0
	int				dim;						// nella prima cella questo valore indica la dimensione del vettore, nelle risorse corrisponde alla chiave
	char			*contenuto_file;			// contenuto di un file
	char	       	*path;						// percorso della risorsa
	int		        last_pos;					// nella prima cella, indica la posizione dell'ultima risorsa creata, nelle risorse corrisponde alla propria posizione
	struct			el *dad;					// puntatore al padre
	struct          el *son;					// puntatore al figlio
	struct          el *dx;						// puntatore al fratello destro
	struct          el *sx;						// puntatore al fratello sinistro
} Risorsa;

typedef Risorsa 	*Ris;

void 	create_dir				( Risorsa *root , int *nodi_root);			// variabili: root=radice, nodi_root=nodi radice
void 	create					( Risorsa *root , int *nodi_root);			// variabili: root=radice, nodi_root=nodi radice
void	write					( Risorsa *root);							// variabili: root=radice
void	read					( Risorsa *root);							// variabili: root=radice
void	delete					( Risorsa *root, int *nodi_root);			// variabili: root=radice, nodi_root=nodi radice
void	delete_r				( Risorsa *root, int *nodi_root);			// variabili: root=radice, nodi_root=nodi radice
void   	find                    (Risorsa *root);							// variabili: root=radice
int 	hash_function		    ( int key, int j, int c);					// variabili: key=chiave, j=k(contatore di collisioni), c=punt->dim(domensione dell'array/tabella) 
Ris 	aggiorna             	( Risorsa  *punt);							// variabili: root=radice

// funzione principale
int main(){

	char 	comando[12];							// comando da standard input
	int  	nodi_radice = 0;						// numero di nodi iniziali occupati della radice
    Risorsa  *radice;								// array di risorse, primo nodo
	radice=calloc( 2050, sizeof( Risorsa ));		// creazione della radice
	radice->dx=NULL;								// inizializzazione puntatori e valori
	radice->dim=2050;
    radice->last_pos=0;
    // ricezione del comando
    if ( fscanf ( stdin, "%s", comando ) != 1 ){
    	return -1;
    }

    // analisi del comando ricevuto
	while( strcmp ( comando , "exit" ) != 0 ){

		// comando di creazione directory
		if( strcmp ( comando, "create_dir" ) ==0 ){
			create_dir( radice, &nodi_radice);
		}

		// comando creazione file
		if( strcmp ( comando, "create" ) ==0 ){
			create( radice, &nodi_radice );
		}

		// comando write (scrivi nel file)
		if( strcmp ( comando, "write" ) ==0 ){
			write( radice );
		}

		//comando read (stampa contenuto di un file)
		if( strcmp( comando, "read" ) == 0){
			read( radice );
		}

		//comando cancela una risorsa
		if( strcmp( comando, "delete" ) == 0){
			delete( radice, &nodi_radice );
		}

        //comando cancella una cartella
        if( strcmp( comando, "delete_r") == 0){
            delete_r( radice, &nodi_radice );
        }

        //comando cerca risorsa
        if( strcmp( comando, "find") == 0){
            find( radice );
        }

		// re-input comando
		if( fscanf(stdin, "%s", comando) != 1 ){
			return -1;
		}
	}
	return 0;
}

// funzione di hash
/* 
	calcola la posizione all'interno dell'array/tabella. Tale posizione viene calcolata moltiplicando 
	la chiave per alcune variabili, e modulata con il valore c. Se la posizione è uguale a zero, si calcola un altra posizione.
	Il valore c è sempre uguale alla dimensione dell'array inserito - 1.
*/
int hash_function( int key , int j, int c ){			

    int pos=0;
    int q=5381;
    int p=2589;
    c=c-1;
    pos=(((( key >> 5 )+key )+q )*j )%c;
	if( pos == 0){										// se la posizione trovata è uguale a zero, bisogna calcolare un altra posizione
		pos=((((key >> 5 )+key )+p )*j )%c;
	}
	return pos;
}

// funzione aggiorna
/*
	Questa funzione crea un array più lungo, e inserisce nelle nuove posizioni tutte le risorse del vecchio array, che è più piccolo.
	Ritorna l'array nuovo.
*/
Ris aggiorna(Risorsa *punt){
    Risorsa		*p;				// puntatori di appoggio
    Risorsa  	*p2;
    Risorsa  	*p3;
    Risorsa  	*p4;
    int         i;				// contatore di elementi da re-inserire
    int         key;			// chiave di ogni risorsa
    int         k;				// contatore di collisioni
    int         lung;			// variabile di appogio per allocare nuovi spazi in memoria
    int 	    pos;			// posizione nel nuovo array

    p2=calloc(2050,sizeof(Risorsa));											// allocazione di un nuovo array più lungo
    i=0;																		// inizializzazione variabili
    p2->dim=2050;
    p=p2;
    p3=punt;
    while(i<24){																// si salvano nel nuovo array tutti i fratelli partendo dal primo
        p3=p3->dx;
        key=p3->dim;
        k=1;
        pos=hash_function(key, k, 2050 );										// calcolo dell nuova posiione
       while( (p2 + pos)->nome !=NULL ){
                 k++;
                 pos=hash_function(key, k, 2050);
		}
        (p->dx)=(p2+pos);														// vengono copiati tutti i dati di ogni singola risorsa nel nuovo array						
        (p2+pos)->sx=p;
        lung=strlen(p3->nome);
        (p2+pos)->nome=malloc((lung+1)*sizeof(char));
        strcpy((p2+pos)->nome, (p3->nome));
        lung=strlen(p3->path);
        (p2+pos)->path=malloc((lung+1)*sizeof(char));
        strcpy((p2+pos)->path, (p3->path));
        if(p3->bit_file==1){
            lung=strlen(p3->contenuto_file);
            (p2+pos)->contenuto_file=malloc((lung+1)*sizeof(char));
            strcpy((p2+pos)->contenuto_file, p3->contenuto_file);
        }
        (p2+pos)->conta_figli=p3->conta_figli;
        (p2+pos)->bit_file=p3->bit_file;
        (p2+pos)->last_pos=pos;
        if(p3->son!=NULL){
            (p2+pos)->son=p3->son;
            p4=p3->son;
            p4->dad=(p2+pos);
        }
        p=(p2+pos);
        i++;
    }
    p2->last_pos=pos;
    free(punt);																	// deallocazione del vecchio array
    return p2;																	// ritorna il nuovo array
}

void create_dir ( Risorsa *root , int *nodi_root){

	char  		    sostegno[NAMELEN];			// array di appoggio per calcolare la chiave
    char  		    percorso[65280];			// path della directory (65279 = NAMELEN * TREELEN)
	char   		    *arrayf[NAMELEN];			// array di puntatori ai nomi delle directory
	char   		    *tokenf;					// appoggio per strtoken
	int 		    i;							// indice numero elementi analizzati
	int 		    j;							// indice di elemento da analizzare
	int 		    k;							// contatore collisioni
	int			    c;							// variabile di appoggio
	int			    lung_path;					// lunghezza dell'array contenente il path
	int 		    lung_nome;					// lunghezza dell'array contenente il nome dell risorsa
	int 		    pos;						// posizione all'interno della hash table
	int 		    chiave;						// chiave della risorsa
	Risorsa 	    *punt;						// punta alla prima cella di ogni array
    Risorsa 	    *punt2;						// puntatori di appoggio
    Risorsa         *punt3;

	punt = root;											// punt punta alla prima cella della radice

	// input del percorso
	if( fscanf (stdin, "%s", percorso ) != 1 ) {
        printf( "no\n" );
		return;
	}

	lung_path=strlen(percorso);								// calcolo lunghezza percorso
	char        percorso2[lung_path+1];						// array di supporto per memorizzazione percorso
	strcpy(percorso2,percorso);

	// controlo lunghezza percorso
    if(lung_path>65280){									
        printf("no\n");
        return;
    }

	// riempimento dell'arrayf con le directory: in ordine crescente vengono elencate la root e le subdirectory
	tokenf = strtok ( percorso, "/" );
	i = 0;
	while( tokenf != NULL ){
		
		// controllo altezza albero
        if(i==TREELEN){
            printf("no\n");
            for( j = 0; j < i; j++ ){
                free(arrayf[j]);
            }
            return;
        }
        lung_nome=strlen(tokenf);
		
		// controllo lunghezza nome
        if(lung_nome>NAMELEN){
            printf("no\n");
            return;
        }
        arrayf[i] = malloc((lung_nome+1) * sizeof(char));
		strcpy (arrayf[i], tokenf);
		i++;
		tokenf = strtok( NULL, "/" );
	}
	// inserimento, secondo struttura ad albero (quindi nella corretta hash table) del nome della directory
	for( j=0 ; j < i ; j++ ){
		// inizio creazione directory
		chiave = 0;
		lung_nome = strlen( arrayf[j] );

		// copia il nome della directory in essere nell'array sostegno
		strcpy( sostegno, arrayf[j] );

		// calcolo della chiave di hash
		for( k = 0; k < lung_nome; k++ ){														// calcola la chiave da inserire nella hash function per trovare la posizione
			chiave = chiave + sostegno[k];														// la chiave viene calcolata sommando il valore ascii di ogni carattere della stringa.
		}
        k=1;
        pos=hash_function(chiave, k, punt->dim);

        // recupero della hash table di riferimento ed inserimento della directory nella hash table
		if( j < i-1 ){																			// divisione in due casi: then-->analisi dei nodi non foglia, else-->creazione foglia(caso in cui j=i-1)
			while( (punt + pos)->nome !=NULL || (punt+pos)->nome == NULL ){
				// verifica numero massimo collisioni di hash table
				if( k == (punt->dim)-1 ){
					break;
				}
				if( (punt+pos)->nome != NULL ){
					// Verifica nomi uguali
					if( strcmp((punt + pos)->nome, arrayf[j])==0 && (punt+pos)->bit_file == 0  ){
						break;
					}
					else{
						// se nome diverso, calcolare nuova posizione, incremento contatore collisioni di 1
						k++;
						pos=hash_function(chiave, k, punt->dim);
					}
				}
				else{
					// se nome=null, calcolare nuova posizione, incremento contatore collisioni di 1
					k++;
					pos=hash_function(chiave, k, punt->dim);
				}
			}
            if( k == (punt->dim)-1 ){
					printf("no\n");
					break;
				}
            // verifica ultimo nodo dell'albero con il penultimo nodo dell'input
			if( j == i - 2 ){													// analisi del penutlimo nodo
				punt2 = punt+pos;

				if( (punt + pos)->son == NULL ){								// se non ha alcun figlio, si crea un nuovo array
					(punt+pos)->son = calloc(48 , sizeof( Risorsa ));
					(punt + pos)->conta_figli = 1;								// inizializzazione variabili
                    punt=(punt+pos)->son;
					punt->dx=NULL;
                    punt->dim=48;												// indica la dimensione dell'array
                    punt->last_pos=0;
                }
				else{
					if( ( punt + pos )->conta_figli >= 1024 ){					//se il penultimo nodo ha più di 1024 figli, allora si interrmpe tutto
                        printf( "no\n" );
						break;
					}
                    else{
                    	c=((punt+pos)->son)->dim;								// memorizzo in variabile di supporto, le dimensioni dell'array ( per rendere un po più ordinato)
                    	if( ((punt+pos)->conta_figli) == c/2 ){					// se l'array figlio è mezzo pieno, si chiama la funzione aggiorna
                    		(punt+pos)->son = aggiorna((punt+pos)->son);
                    		punt3=(punt+pos)->son;
                    		punt3->dad=(punt+pos);								// memorizzazione del padre del nuovo array
                    	}
                        (punt+pos)->conta_figli++;								// si incrementa il contatore di 1, e si punta al figlio
                        punt=(punt+pos)->son;
                    }
				}
			}
			else{																// se il figlio del penultino nodo non esiste, si interrompe tutto
				if( ( punt + pos )->son == NULL ){
					printf( "no\n" );
					break;
				}
				else{
					punt = ( punt + pos )->son;									// altrimenti si passa all'array figlio
				}
			}
		}

		// inserimento di nuovi nodi della struttura/array (se esiste la cartella padre, oppure parte di root)
		else{
			// verifico se sono nella radice
			if( i == 1 ){
				// Verifica numero elementi < 1024
				if( *nodi_root < 1024 ){
					punt2 = NULL;
					*(nodi_root) = *(nodi_root) + 1; 							// incremento contatore elementi della radice
				}
				else{
					printf( "no\n" );
					break;
				}
			}

			while( (punt + pos)->nome !=NULL ){
				// verifica numero massimo collisioni di array/hash table
				if( k == (punt->dim)-1 ){												// se il numero di collisioni è pari alla dimensione dell'array -1
					break;																// si interrompe tutto
				}

				// Verifica nomi uguali
				if( strcmp((punt + pos)->nome, arrayf[j]) == 0 ) {
					printf("no\n");
					for( j = 0; j < i; j++ ){
						free(arrayf[j]);
					}
					return;
				}
				else{
					// se nome diverso, calcolare nuova posizione, incremento contatore collisioni di 1
					k++;
					pos=hash_function(chiave, k, punt->dim);
				}
			}
			if(k==(punt->dim)-1){
				printf("no\n");
				break;
			}

			// inserimento del nuovo elemento nella posizione/nodo corretto
            if(punt->dx==NULL){
                punt->dx=(punt+pos);
                punt->dad=punt2;														// in punt2 è memorizzato l'indirizzo del padre
            }
			else{
				(punt+(punt->last_pos))->dx=(punt+pos);									// il vecchio fratello punta a quello nuovo
			}
			(punt + pos)->nome = malloc((lung_nome+1) * sizeof (char));					// inizializzazione degli elementi della nuova risorsa
			strcpy((punt + pos)->nome, arrayf[j]);
			(punt + pos)->bit_file = 0;
			(punt + pos)->son = NULL;
			(punt+pos)->dx=NULL;
            (punt + pos)->conta_figli = 0;
			(punt + pos)->last_pos=pos;
            (punt + pos)->dim=chiave;
			(punt + pos)->sx=punt+(punt->last_pos);
			(punt + pos)->path=malloc((lung_path+1) * sizeof (char));
			strcpy((punt+pos)->path, percorso2);
			punt->last_pos=pos;
            printf("ok\n");
        }
	}																					// fine ciclio for

    for( j = 0; j < i; j++ ){
    	free(arrayf[j]);																// deallocazione dell'array contenente il percorso
    }
    return;
}

void create ( Risorsa *root , int *nodi_root){

	char  		sostegno[NAMELEN];			// valori uguali a funzione create_dir
    char  		percorso[65280];
    char   		*arrayf[NAMELEN];			
    char   		*tokenf;
	int 		i;
	int 		j;
	int 		k;			
	int			c;
	int			lung_path;
	int 		lung_nome;
	int 		pos;
	int 		chiave;
	Risorsa 	*punt;
	Risorsa 	*punt2;
	Risorsa     *punt3;

	punt = root;

	// input del percorso
	if( fscanf (stdin, "%s", percorso ) != 1 ) {
        printf( "no\n" );
		return;
	}

	lung_path=strlen(percorso);
	char        percorso2[lung_path+1];
	strcpy(percorso2,percorso);
	
	// controllo lunghezza percorso
    if(lung_path>65280){
        printf("no\n");
        return;
    }

	// riempimento dell'arrayf con le directory
	tokenf = strtok ( percorso, "/" );
	i = 0;
		while( tokenf != NULL ){
		// controllo altezza albero
        if(i==TREELEN){
            printf("no\n");
            return;
        }
        lung_nome=strlen(tokenf);
		// controllo lunghezza nome
        if(lung_nome>NAMELEN){
            printf("no\n");
            return;
        }
        arrayf[i] = malloc((lung_nome+1) * sizeof(char));
		strcpy (arrayf[i], tokenf);
		i++;
		tokenf = strtok( NULL, "/" );
	}

	// inserimento, secondo struttura ad albero
	for( j=0 ; j < i ; j++ ){
		// inizio creazione directory
		chiave = 0;
		lung_nome = strlen( arrayf[j] );

		strcpy( sostegno, arrayf[j] );

		// calcolo della chiave della risorsa
		for( k = 0; k < lung_nome; k++ ){
			chiave = chiave + sostegno[k];
		}
        k=1;
        pos=hash_function(chiave, k, punt->dim);

        // recupero della hash table di riferimento ed inserimento della directory nella hash table
		if( j < i-1 ){           															// divisione in due casi
			while( (punt + pos)->nome !=NULL || (punt+pos)->nome == NULL ){
				// verifica numero massimo collisioni di hash table
				if( k == (punt->dim)-1 ){
                    printf("no\n");
                    return;
				}
				if( (punt+pos)->nome != NULL ){
					// Verifica nomi uguali
					if( strcmp((punt + pos)->nome, arrayf[j])==0 && (punt+pos)->bit_file == 0  ){
						break;
					}
					else{
						// se nome diverso, incremento contatore collisioni di 1
						k++;
						pos=hash_function(chiave, k, punt->dim);
					}
				}
				else{
					// se nome è null, incremento contatore collisioni di 1
					k++;
					pos=hash_function(chiave, k, punt->dim);
				}
			}
			// verifica ultima nodo dell'albero con il penultimo nodo dell'input
			if( j == i - 2 ){
				punt2 = punt+pos;

				if( (punt + pos)->son == NULL ){
					(punt+pos)->son = calloc(48 , sizeof( Risorsa ));
					(punt + pos)->conta_figli = 1;
					punt=(punt+pos)->son;
					punt->dx=NULL;
					punt->dim=48;
                    punt->last_pos=0;
				}
				else{
					if( ( punt + pos )->conta_figli >= 1024 ){		// se il penultimo nodo ha più di 1024 figli si interrmpe tutto
						printf( "no\n" );
						break;
					}
					else{
						c=((punt+pos)->son)->dim;
						if(((punt+pos)->conta_figli)== c/2){
							(punt+pos)->son=aggiorna((punt+pos)->son);
							punt3=(punt+pos)->son;
                    		punt3->dad=(punt+pos);
						}

						(punt+pos)->conta_figli++;						// incrementa il contatore di 1
						punt=(punt+pos)->son;							// si punta al figlio
					}

				}

			}
			else{
				if( ( punt + pos )->son == NULL ){						//controllo esistenza figlio
					printf( "no\n" );
					break;
				}
				else{
					punt = ( punt + pos )->son;
				}
			}
		}

		// inserimento di nuovi nodi della struttura (se esiste la cartella padre, oppure parte di root)
		else{
			// verifico se sono nella radice
			if( i == 1 ){
				// Verifica numero elementi < 1024
				if( *nodi_root < 1024 ){
					punt2 = NULL;
					c=punt->dim;
					if( *nodi_root == c/2){
						punt=aggiorna(punt);
					}
					*(nodi_root) = *(nodi_root) + 1;
				}
				else{
					printf( "no\n");
					break;
				}
			}

			while( (punt + pos)->nome !=NULL ){
				// verifica numero massimo collisioni di hash table
				if( k == (punt->dim)-1 ){
					break;
				}

				// Verifica nomi uguali
				if( strcmp((punt + pos)->nome, arrayf[j])==0 ) {
					printf("no\n");
					for( j = 0; j < i; j++ ){
						free(arrayf[j]);
					}
					return;
				}
				else{
					// se nome diverso, calcolare nuova posizione, incremento contatore collisioni di 1
					k++;
					pos=hash_function(chiave, k, punt->dim);
				}
			}
			if( k == (punt->dim)-1){
				printf("no\n");
				break;
			}

			// inserimento del nuovo elemento nel nodo corretto
			if(punt->dx==NULL){
				punt->dx=(punt+pos);
				punt->dad=punt2;
			}
			else{
				(punt+(punt->last_pos))->dx=(punt+pos);
			}
			(punt + pos)->nome = malloc((lung_nome+1) * sizeof (char));				// inizializzazione degli elementi della nuova risorsa
			strcpy((punt + pos)->nome, arrayf[j]);
			(punt + pos)->bit_file = 1;
			(punt + pos)->son = NULL;
			(punt + pos)->dx=NULL;
			(punt + pos)->conta_figli = 0;
			(punt + pos)->last_pos=pos;
			(punt + pos)->dim=chiave;
			(punt + pos)->sx=punt+(punt->last_pos);
			(punt + pos)->path=malloc((lung_path+1) * sizeof (char));
			strcpy((punt+pos)->path, percorso2);
			(punt + pos)->contenuto_file=malloc(sizeof (char));
			strcpy((punt+pos)->contenuto_file, "\0");
			punt->last_pos=pos;
            printf("ok\n");
        }																			// fine ciclio for
	}
    for( j = 0; j < i; j++ ){
    	free(arrayf[j]);
    }
    return;
}

void write ( Risorsa *root){

	char  		sostegno[NAMELEN];			// array di appoggio per calcolo chiave
    char  		percorso[65280];			// path della directory (65279 = NAMELEN * TREELEN)
	char   		*arrayf[NAMELEN];			// array di puntatori ai nomi delle directory
	char		contenuto[1000];			// aray di appoggio in cui viene inserito il contenuto
    char   		*tokenf;					// appoggio per strtoken
	int 		i;							// indice numero elementi analizzati
	int 		j;							// indice 
	int 		k;							// contatore tentativi hash table
	int			l;							// indice cella dell'array contenuto
	int 		lung_nome;					// lunghezza dell'array contenente il nome dell risorsa
	int			lung_contenuto;				// lunghezza dell'array contenente il contenuto
	int         lung_path;					// lunghezza dell'array contenente il percorso
    int 		pos;						// posizione all'interno della hash table
	int 		chiave;						// chiave
	Risorsa 	*punt;						// punatore alla prima cella di ogni array

	punt = root;			// punt punta alla prima cella della radice

	// input del percorso
	if( fscanf (stdin, "%s", percorso ) != 1 ) {
        printf( "no\n" );
		return;
	}

    lung_path=strlen(percorso);
	
	//controllo lunghezza percorso
    if(lung_path>65280){
        printf("no\n");
        return;
    }
	
	// inserimento del contenuto nell'array omonimo
	if( fgets (contenuto, 1000, stdin ) == NULL ) {
		printf( "no\n" );
		return;
	}

	// riempimento dell'arrayf con le directory: in ordine crescente vengono elencate la root e le subdirectory
	tokenf = strtok ( percorso, "/" );
	i = 0;
	while( tokenf != NULL ){
		
		// controllo altezz albero
        if(i==TREELEN){
            printf("no\n");
            return;
        }
        lung_nome=strlen(tokenf);
		
		//controllo lunghezza nome
        if(lung_nome>NAMELEN){
            printf("no\n");
            return;
        }
        arrayf[i] = malloc((lung_nome+1) * sizeof(char));
		strcpy (arrayf[i], tokenf);
		i++;
		tokenf = strtok( NULL, "/" );
	}

	// inserimento contenuto
	for( j=0 ; j < i ; j++ ){

		chiave = 0;
		lung_nome = strlen( arrayf[j] );

		// copia il nome della directory in essere nell'array sostegno
		strcpy( sostegno, arrayf[j] );

		// calcolo della chiave di hash
		for( k = 0; k < lung_nome; k++ ){
			chiave = chiave + sostegno[k];
		}
        k=1;
        pos=hash_function(chiave, k, punt->dim);

        // recupero della hash table/array di riferimento e inserimento del contnuto del file 
		if( j < i-1 ){																						//suddivisione in due casi
			while( (punt + pos)->nome !=NULL || (punt+pos)->nome == NULL ){
				// verifica numero massimo collisioni di hash table
				if( k == (punt->dim)-1 ){
					printf("no\n");
					return;
				}
				if( (punt+pos)->nome != NULL ){
					// Verifica nomi uguali
					if( strcmp((punt + pos)->nome, arrayf[j])==0 && (punt+pos)->bit_file == 0  ) {
						break;
					}
					else{
						// se nome diverso, calcolare nuova posizione, incremento contatore collisioni di 1
						k++;
						pos=hash_function(chiave, k, punt->dim);
					}
				}
				else{
					// se nome=null, calcolare nuova posizione, incremento contatore collisioni di 1
					k++;
					pos=hash_function(chiave, k, punt->dim);
				}
			}
			// verifica esistenza del figlio
			if( ( punt + pos )->son == NULL ){
				printf( "no\n" );
				break;
			}
			else{
				punt = ( punt + pos )->son;				// passaggio al figlio
			}
		}

		else{
			// Verifica dell'esistenza di un elemento nella posizione punt+pos
			while( (punt + pos)->nome !=NULL || (punt+pos)->nome == NULL ){
				// verifica numero massimo collisioni di hash table
				if( k == (punt->dim)-1 ){
					break;
				}
				if((punt+pos)->nome != NULL ){
					// Verifica nomi uguali
					if( strcmp((punt + pos)->nome, arrayf[j])==0 && (punt + pos)->bit_file == 1 ) {
						break;
					}
					else{
						k++;
						pos=hash_function(chiave, k, punt->dim);
					}
				}
				else{
					// se nome diverso, calcolare nuova posizione, incremento contatore collisioni di 1
					k++;
					pos=hash_function(chiave, k, punt->dim);
				}
			}
			if( k == (punt->dim)-1 ){
				printf("no\n");
				break;
			}
			// salva il contenuto nel file
			lung_contenuto=strlen(contenuto);
			(punt+pos)->contenuto_file=realloc((punt+pos)->contenuto_file, (lung_contenuto-2) * sizeof(char));
			for( l = 2; l <= lung_contenuto-3; l++ ){						// salva solo quello che sta in mezzo alle virgolette
					((punt+pos)->contenuto_file)[l-2]=contenuto[l];
			}
            (punt+pos)->contenuto_file[l-1]='\0';
            (punt+pos)->contenuto_file[l-2]='\0';
			printf("ok %d\n", lung_contenuto-4);
			break;
		}
	}																		//fine ciclio for

    for( j = 0; j < i; j++ ){
    	free(arrayf[j]);													// deallocazione array con percorso
    }
    return;
}

void read (Risorsa *root){

	char  		sostegno[NAMELEN];			// variabili uguai a funzione write
	char  		percorso[65280];
	char   		*arrayf[NAMELEN];
	char   		*tokenf;
	int 		i;
	int 		j;
	int 		k;
	int 		lung_nome;
	int         lung_path;
    int 		pos;
	int 		chiave;
	Risorsa 	*punt;

	punt = root;

	// input del percorso
	if( fscanf (stdin, "%s", percorso ) != 1 ) {
		printf( "no\n" );
		return;
	}
    lung_path=strlen(percorso);
	
	//controllo lunghezza percorso
    if(lung_path>65280){
        printf("no\n");
        return;
    }

	// riempimento dell'arrayf con le directory: in ordine crescente vengono elencate la root e le subdirectory
	tokenf = strtok ( percorso, "/" );
	i = 0;
		while( tokenf != NULL ){
			
		// controllo altezza albero
        if(i==TREELEN){
            printf("no\n");
            return;
        }
        lung_nome=strlen(tokenf);
		// controllo lunghezza nome
        if(lung_nome>NAMELEN){
            printf("no\n");
            return;
        }
        arrayf[i] = malloc((lung_nome+1) * sizeof(char));
		strcpy (arrayf[i], tokenf);
		i++;
		tokenf = strtok( NULL, "/" );
	}

	// ricerca risorsa e stampa contenuto
	for( j=0 ; j < i ; j++ ){

		chiave = 0;
		lung_nome = strlen( arrayf[j] );

		// copia il nome della directory in essere nell'array sostegno
		strcpy( sostegno, arrayf[j] );

		// calcolo della chiave di hash
		for( k = 0; k < lung_nome; k++ ){
			chiave = chiave + sostegno[k];
		}
		k=1;
		pos=hash_function(chiave, k, punt->dim);

		// recupero della hash table di riferimento e stampa contenuto del file
		if( j < i-1 ){																// divisione in due casi
			while( (punt + pos)->nome !=NULL || (punt+pos)->nome == NULL ){
				// verifica numero massimo collisioni di hash table
				if( k == (punt->dim)-1  ){
					printf("no\n");
					return;
				}
				if( (punt+pos)->nome != NULL ){
					// Verifica nomi uguali
					if( strcmp((punt + pos)->nome, arrayf[j])==0 && (punt+pos)->bit_file == 0  ) {
						break;
					}
					else{
						// se nome diverso, calcolare nuova posizione, incremento contatore collisioni di 1
						k++;
						pos=hash_function(chiave, k, punt->dim);
					}
				}
				else{
					// se nome=null, calcolare nuova posizione, incremento contatore collisioni di 1
					k++;
					pos=hash_function(chiave, k, punt->dim);
				}
			}
			// verifica esistenza del figlio
			if( ( punt + pos )->son == NULL ){
				printf( "no\n" );
				break;
			}
			else{
				punt = ( punt + pos )->son;			// si punta al figlio
			}
		}
		
		// ricerca della risorsa con contenuto da stampare
		else{
			// Verifica dell'esistenza di un elemento nella posizione punt+pos
			while( (punt + pos)->nome !=NULL || (punt+pos)->nome == NULL ){
				// verifica numero massimo collisioni di hash table
				if( k == (punt->dim)-1 ){
					break;
				}
				if((punt+pos)->nome != NULL ){
					// Verifica nomi uguali
					if( strcmp((punt + pos)->nome, arrayf[j])==0 && (punt + pos)->bit_file == 1 ) {
						break;
					}
					else{
						k++;
						pos=hash_function(chiave, k, punt->dim);
					}
				}
				else{
					// se nome diverso, calcolare nuova posizione, incremento contatore collisioni di 1
					k++;
					pos=hash_function(chiave, k, punt->dim);
				}
			}
			if( k == (punt->dim)-1 ){
				printf("no\n");
				break;
			}

			// stampa il contenuto del file
			printf("contenuto %s\n", (punt+pos)->contenuto_file);
			break;
		}
	}

	for( j = 0; j < i; j++ ){
		free(arrayf[j]);
	}
	return;
}

void delete (Risorsa *root, int *nodi_root){

	char  		sostegno[NAMELEN];			// array di appoggio per calcolo chiave
	char  		percorso[65280];			// path della directory (65279 = NAMELEN * TREELEN)
	char   		*arrayf[NAMELEN];			// array di puntatori ai nomi delle directory
	char   		*tokenf;					// appoggio per strtoken
	int 		i;							// indice numero elementi analizzati
	int 		j;							// indice di elemento da analizzare
	int 		k;							// contatore collisioni
	int 		lung_nome;					// lunghezza dell'array contenente il nome dell risorsa
	int         lung_path;					// lunghezza dell'array contenente il path
    int 		pos;						// posizione all'interno della hash table
	int 		chiave;						// chaive della risorsa
	Risorsa 	*punt;						// punta alla prima cella di ogni array
	Risorsa		*punt1;						// puntatori di appoggio
	Risorsa		*punt2;
	Risorsa		*punt3;

	punt = root;									// punt punta alla prima cella della radice

	// input del percorso
	if( fscanf (stdin, "%s", percorso ) != 1 ) {
		printf( "no\n" );
		return;
	}
    lung_path=strlen(percorso);
	
	// controllo lunghezza percorso
    if(lung_path>65280){
        printf("no\n");
        return;
    }

	// riempimento dell'arrayf con le directory: in ordine crescente vengono elencate la root e le subdirectory
	tokenf = strtok ( percorso, "/" );
	i = 0;
	while( tokenf != NULL ){
	// controllo altezza albero
		if(i==TREELEN){
			printf("no\n");
			return;
		}
		lung_nome=strlen(tokenf);
		
		// controllo lunghezza nome
		if(lung_nome>NAMELEN){
			printf("no\n");
		return;
		}
		arrayf[i] = malloc((lung_nome+1) * sizeof(char));
		strcpy (arrayf[i], tokenf);
		i++;
		tokenf = strtok( NULL, "/" );
	}

	// ricerca della risorsa da cancellare
	for( j=0 ; j < i ; j++ ){

		chiave = 0;
		lung_nome = strlen( arrayf[j] );

		// copia il nome della directory in essere nell'array sostegno
		strcpy( sostegno, arrayf[j] );

		// calcolo della chiave di hash
		for( k = 0; k < lung_nome; k++ ){
			chiave = chiave + sostegno[k];
		}
		k=1;
		pos=hash_function(chiave, k, punt->dim);

		// recupero della hash table di riferimento ed cancellazione del file
		if( j < i-1 ){																//divisione in due casi
			while( (punt + pos)->nome !=NULL || (punt+pos)->nome == NULL ){
				// verifica numero massimo collisioni di hash table
				if( k == (punt->dim)-1 ){
					printf("no\n");
					return;
				}
				if( (punt+pos)->nome != NULL ){
					// Verifica nomi uguali
					if( strcmp((punt + pos)->nome, arrayf[j])==0 && (punt+pos)->bit_file == 0  ) {
						break;
					}
					else{
						// se nome diverso, calcolare nuova posizione, incremento contatore collisioni di 1
						k++;
						pos=hash_function(chiave, k, punt->dim);
					}
				}
				else{
					// se nome uguale a null, calcolare nuova posizione, incremento contatore collisioni di 1
					k++;
					pos=hash_function(chiave, k, punt->dim);
				}
			}
			// verifica esistenza del figlio
			if( ( punt + pos )->son == NULL ){
				printf( "no\n" );
				break;
			}
			else{
				if( j == i-2 ){
					punt3=(punt+pos);				// salvataggio del puntatore della cartella contenente la risorsa da cancellare
				}
				punt = ( punt + pos )->son;			// passaggio al figlio se esiste
			}
		}

		// cancellazione di una risorsa
		else{
			// Verifica dell'esistenza di un elemento nella posizione punt+pos
			while( (punt + pos)->nome != NULL || (punt+pos)->nome == NULL ){
				// verifica numero massimo collisioni di hash table
				if( k== (punt->dim)-1 ){
					break;
				}
				if((punt+pos)->nome != NULL ){
					// Verifica nomi uguali
					if( strcmp((punt + pos)->nome, arrayf[j])==0 ) {
						break;
					}
					else{
						k++;
						pos=hash_function(chiave, k, punt->dim);
					}
				}
				else{
					// se nome diverso, calcolare nuova posizione, incremento contatore collisioni di 1
					k++;
					pos=hash_function(chiave, k, punt->dim);
				}
			}
			if( k == (punt->dim)-1 ){
				printf("no\n");
				break;
			}
			if( (punt+pos)->son != NULL ){								// verifica dell'esistenza del figlio
				printf("no\n");											// se il figlio esiste, tale risorsa non si può cancellare e si interompe tutto
				break;
			}
			punt1=(punt+pos)->sx;										// salvataggio dell'indirizzo del fratello di sinistra in punt1
			if( (punt+pos)->dx == NULL){								// controllo esistenza fratello destro della risorsa da cancellare
				punt2=NULL;
				if( (punt+pos)->sx == punt ){							// controllo se fratello sinistro è la prima cella dell'array
 					punt->last_pos=0;									// assegnamento valori alla prima cella dell'array
					punt->dx=punt2;
				}
				else{													// se esiste fratello sinistra
					punt->last_pos=punt1->last_pos;						// aggiornamento della posizione dell'ultimo fratello esistente			
					punt1->dx=punt2;									// il fratello sinitro, ora punta a null alla sua destra
				}
			}
			else{														// se la risorsa da cancellare ha sia un fratello a dx sia a sx
				punt2=(punt+pos)->dx;									// si collegano i primi fratelli subito a sx e dx della risorsa da cancellare
				punt1->dx=punt2;										// collegamento dei fratelli tramite puntatori
				punt2->sx=punt1;

			}
			free((punt+pos)->nome);										// cancellazione di tutti i dati dela risorsa da cancellare
			(punt+pos)->nome=NULL;
			free((punt+pos)->contenuto_file);
			(punt+pos)->contenuto_file=NULL;
			(punt+pos)->dad=NULL;
			(punt+pos)->son=NULL;
			(punt+pos)->dx=NULL;
			(punt+pos)->sx=NULL;
			free((punt+pos)->path);
			(punt+pos)->path=NULL;
			(punt+pos)->bit_file=-1;
			(punt+pos)->last_pos=-1;
			if( i == 1){
				*nodi_root=*nodi_root-1;								// contatore figli della radice viene decrementato
			}
			else{
				(punt3->conta_figli) = (punt3->conta_figli)-1;			// contatore figli del padre viene decrementato
                // verifica esistenza di figli del padre
				if(punt3->conta_figli==0){
                    free(punt);
                    punt3->son=NULL;
				}
			}
			printf("ok\n");
			break;
		}
	}																	//fine ciclio for

	for( j = 0; j < i; j++ ){
		free(arrayf[j]);
	}
	return;
}




void delete_r (Risorsa *root, int *nodi_root){

	char  		sostegno[NAMELEN];			// array di appoggio per calcolo chiave
	char  		percorso[65280];			// path della directory (65279 = NAMELEN * TREELEN)
	char   		*arrayf[NAMELEN];			// array di puntatori ai nomi delle directory
	char   		*tokenf;					// appoggio per strtoken
	int 		i;							// indice numero elementi analizzati
	int 		j;							// indice elemento da analizzare
	int 		k;							// contatore collisioni
	int 		lung_nome;					// lunghezza dell'array contenente il nome dell risorsa
	int         lung_path;					// lunghezza dell'array contenente il percorso
	int 		pos;						// posizione all'interno della hash table
	int 		chiave;						// chiave
	Risorsa 	*punt;						// puntaote a prima cella di ogni array
	Risorsa		*p;							// puntatori di appoggio
	Risorsa		*p1;
	Risorsa		*punt1;
	Risorsa		*punt2;
	Risorsa		*punt3;

	punt = root;			// punt punta alla prima cella della radice

	// input del percorso
	if( fscanf (stdin, "%s", percorso ) != 1 ) {
		printf( "no\n" );
		return;
	}
    lung_path=strlen(percorso);
	
	// controllo lunghezza percorso
    if(lung_path>65280){
        printf("no\n");
        return;
    }

	// riempimento dell'arrayf con le directory: in ordine crescente vengono elencate la root e le subdirectory
	tokenf = strtok ( percorso, "/" );
	i = 0;
		while( tokenf != NULL ){
        // controllo altezza albero
		if(i==TREELEN){
            printf("no\n");
            return;
        }
        lung_nome=strlen(tokenf);
		// controllo lunghezza nome
        if(lung_nome>NAMELEN){
            printf("no\n");
            return;
        }
        arrayf[i] = malloc((lung_nome+1) * sizeof(char));
		strcpy (arrayf[i], tokenf);
		i++;
		tokenf = strtok( NULL, "/" );
	}

	// ricerca e cancellazione della directory
	for( j=0 ; j < i ; j++ ){

		chiave = 0;
		lung_nome = strlen( arrayf[j] );

		// copia il nome della directory in essere nell'array sostegno
		strcpy( sostegno, arrayf[j] );

		// calcolo della chiave di hash
		for( k = 0; k < lung_nome; k++ ){
			chiave = chiave + sostegno[k];
		}
		k=1;
		pos=hash_function(chiave, k, punt->dim);

		// recupero della hash table di riferimento e cancellazione della directory
		if( j < i-1 ){
			while( (punt + pos)->nome !=NULL || (punt+pos)->nome == NULL ){
				// verifica numero massimo collisioni di hash table
				if( k == (punt->dim)-1 ){
					printf("no\n");
					return;
				}
				if( (punt+pos)->nome != NULL ){
					// Verifica nomi uguali
					if( strcmp((punt + pos)->nome, arrayf[j])==0 && (punt+pos)->bit_file == 0  ) {
						break;
					}
					else{
						// se nome diverso, calcolare nuova posizione, incremento contatore collisioni di 1
						k++;
						pos=hash_function(chiave, k, punt->dim);
					}
				}
				else{
					// se nome=null, calcolare nuova posizione, incremento contatore collisioni di 1
					k++;
					pos=hash_function(chiave, k, punt->dim);
				}
			}
			// controllo esistenza del figlio
			if( ( punt + pos )->son == NULL ){
				printf( "no\n" );
				break;
			}
			else{
				if( j == i-2 ){
					punt3=(punt+pos);
				}
				punt = ( punt + pos )->son;			//passaggio al figlio
			}
		}

		// cancellazione della directory
		else{
			// Verifica dell'esistenza di un elemento nella posizione punt+pos
			while( (punt + pos)->nome != NULL || (punt+pos)->nome == NULL ){
				// verifica numero massimo collisioni di hash table
				if( k== (punt->dim)-1 ){
					break;
				}
				if((punt+pos)->nome != NULL ){
					// Verifica nomi uguali
					if( strcmp((punt + pos)->nome, arrayf[j])==0 ) {
						break;
					}
					else{
						// se nome diverso, calcolare nuova posizione, incremento contatore collisioni di 1
						k++;
						pos=hash_function(chiave, k, punt->dim);
					}
				}
				else{
					// se nome=null, calcolare nuova posizione, incremento contatore collisioni di 1
					k++;
                    pos=hash_function(chiave, k, punt->dim);
				}
			}
			if( k == (punt->dim)-1 ){
				printf("no\n");
				break;
			}

			if( (punt+pos)->son == NULL){						// caso in cui la directry e una folgia dell'albero (non ha figli)
				punt1=(punt+pos)->sx;							// algoritmo uguale a funzione delete
				if( (punt+pos)->dx == NULL){
					punt2=NULL;
					if( (punt+pos)->sx == punt ){
						punt->last_pos=0;
						punt->dx=punt2;
					}
					else{
						punt->last_pos=punt1->last_pos;
						punt1->dx=punt2;
					}
				}
				else{
					punt2=(punt+pos)->dx;
					punt1->dx=punt2;
					punt2->sx=punt1;

				}
				free((punt+pos)->nome);
				(punt+pos)->nome=NULL;
				free((punt+pos)->contenuto_file);
				(punt+pos)->contenuto_file=NULL;
				(punt+pos)->dad=NULL;
				(punt+pos)->son=NULL;
				(punt+pos)->dx=NULL;
				(punt+pos)->sx=NULL;
				free((punt+pos)->path);
				(punt+pos)->path=NULL;
				(punt+pos)->bit_file=-1;
				(punt+pos)->last_pos=-1;
				(punt+pos)->dim=-1;
			}
			else{												// caso in cui esiste il figlio della directory da cancellare
				p=(punt+pos)->son;								// punt+pos è la cartella da cancellare, p è di supporto
				p=p+(p->last_pos);								// pasiamo direttamente all'ultimo figlio della hash table
				while(p!=(punt+pos)){							// finchè p è diverso dalla directory da cancellare continua a ciclare
					if(p->dx==NULL){							// se il figlio destro non esiste
						if(p->sx==NULL){						// se il figlio sinistro non esiste
							p1=p;								// stiamo puntando alla prima cella della tabella
							p=p->dad;							// p punta al padre
							p->son=NULL;
							free(p1);							// viene cancellata la tabella/array
							p1=NULL;
						}
						else{									// non esiste il fratello destro, ma esiste il sinistro
							if((p->son)!=NULL){					// se il figlio esiste
								p=p->son;						// si punta al figlio (prima cella dell'array figlio)
								p=p+(p->last_pos);				// si punta all'ultimo figlio creato dell'array puntato
							}
							else{								// se non esiste il figlio
								p1=p;							// p1=p punta alla risorsa da cacellare
								p=p->sx;						// p punta al fratello a sinistra
								free(p1->nome);					// tramite p1 si cancellano tutti gli elementi della cella/risorsa
								p1->nome=NULL;
								free(p1->contenuto_file);
								p1->contenuto_file=NULL;
								p1->dad=NULL;
								p1->son=NULL;
								p1->dx=NULL;
								p1->sx=NULL;
								free(p1->path);
								p1->path=NULL;
								p1->bit_file=-1;
								p1->last_pos=-1;
								p1->dim=-1;
								p->dx=NULL;
							}
						}
					}
				}
				punt1=(punt+pos)->sx;							// quando si arriva alla directory da dove si è partiti, si deve cancellare
				if( (punt+pos)->dx == NULL){					// l'algoritmo è uguale alla funzione delete
					punt2=NULL;
					if( (punt+pos)->sx == punt ){
						punt->last_pos=0;
						punt->dx=punt2;
					}
					else{
						punt->last_pos=punt1->last_pos;
						punt1->dx=punt2;
					}
				}
				else{
					punt2=(punt+pos)->dx;
					punt1->dx=punt2;
					punt2->sx=punt1;

				}
				free((punt+pos)->nome);
				(punt+pos)->nome=NULL;
				free((punt+pos)->contenuto_file);
				(punt+pos)->contenuto_file=NULL;
				(punt+pos)->dad=NULL;
				(punt+pos)->son=NULL;
				(punt+pos)->dx=NULL;
				(punt+pos)->sx=NULL;
				free((punt+pos)->path);
				(punt+pos)->path=NULL;
				(punt+pos)->bit_file=-1;
				(punt+pos)->last_pos=-1;
				(punt+pos)->dim=-1;
			}
			if( i == 1){
				*nodi_root=*nodi_root-1;
			}
			else{
				(punt3->conta_figli) = (punt3->conta_figli)-1;
			}
			printf("ok\n");
			break;

		}
	}
	for( j = 0; j < i; j++ ){
		free(arrayf[j]);												// libera l'array del percorso
	}
	return;
}

void     find(Risorsa *root){
    char        name_res[NAMELEN];			// array contenente il nome della risosra da cercare
    char        *array[10000];				// array contnente tutti i path delle risorse trovate
    char        *name;						// array di appoggio per confronto nome
    char		sostegno[100];				// array di apppggio per salvare il path
    int         i;							// contatore di elementi trovati
    int			j;							// variabile di appogggio
	int			k;							// variabile di appoggio
    int         lung_name;					// lunghezza dell'array contenente il nome
    int         bit;						// variabile di appoggio, bit di "stop"
    Risorsa     *punt;						// puntatore alle risorse
    Risorsa    	*p1;						// puntatore di appoggio

    punt=root;			// punt punta alla prima cella della radice

    i=0;				// inizializzazione delle variabili
    bit=0;
	
	// inserimento del nome
    if( fscanf (stdin, "%s", name_res ) != 1 ) {
		printf( "no\n" );
		return;
	}

    lung_name=strlen(name_res);
    name = malloc(( lung_name+1 ) * sizeof( char ));
    strcpy(name, name_res);										// salvo il nome dentro il puntatore name

    punt=punt+(punt->last_pos);									// punt punta all'ultima risorsa creata della radice

    while(punt != root ){										// finchè punt è diverso dal puntatore della prima cella della radice, ovvero root
			if(punt->sx==NULL){									// se il figlio sinistro non esiste stiamo puntando alla prima cella dell'array
				punt=punt->dad;									// punto al padre
				bit=1;											// segnalo il passaggio al padre con un bit si segnalazione
			}
			else{												// a sinistra esisste un fratello
				if((punt->son)!=NULL && bit == 0){				// se esistono figli e il bit di segnalazione è uguale a zero
					punt=punt->son;								// punto al figlio (prima cella dell'array
					punt=punt+(punt->last_pos);					// punto all'ultimo fratello creato
				}
				else{											// se non esiste il figlio o se il bit di segnalazione ha valore 1
					p1=punt;									// p1=punt p1 punta alla risorsa da analizzare
					punt=punt->sx;								// punt punta al fratello sinistro
					if( strcmp(name, p1->nome)==0){				// se il nome della risorsa è uguale a quello cercato
						array[i]=malloc(100*sizeof(char));		// alloco spazio in memoria per memorizare il path nell'array di path	
						strcpy(array[i],p1->path);				// copio il path nella cella dell'array di path
						i=i+1;									// incremento il contatore di nomi uguali trovati 
					}
				}
				bit=0;											// il bit di segnalazione assume valore zero
			}
		}
    if(i==0){													// se non si sono trovate risorse stampa no
    	printf("no\n");
    }
    else{														// altrimenti metti in ordine lessicografico i path
    	for(j=1; j<i; j++){										// algoritmo usato è l'insertion sort
    		strcpy(sostegno, array[j]);
    		k=j-1;
    		while( k>=0 && strcmp(array[k],sostegno)>0){
    			strcpy(array[k+1],array[k]);
    			k=k-1;
    		}
    		strcpy(array[k+1], sostegno);
    	}
    	for(k=0;k<i;k++){										// stampa i path in ordine
    		printf("ok %s\n",array[k]);
    	}

    }
    for(j=0; j<i; j++){
      free(array[j]);											// dealloca l'array di path
    }
    return;
}

/*
	  powered by 
	Matteo Redaelli
	 matr. 844344
*/
	