//
//  Created by Gianluca Regis on 29/05/2019.
//  Copyright © 2019 Gianluca Regis. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define INPUT 1000
#define HASH_DIMENSION 26
#define NAME 333



//********************************************************************************************* Strutture


// Iterations list (one for each entity)
struct listaInter{
    short rel;
    bool dest;
    struct treeEnt *altraEntNode;
    struct listaCont *cont;
    struct listaInter *otherInter;
    struct listaInter *prevPtr;
    struct listaInter *nextPtr;
};

// Create some aliases
typedef struct listaInter ListaInter;
typedef ListaInter *ListaInterPtr;




// Entities tree structure (each one linked to Hash arrays)
struct treeEnt{
    unsigned short id;
    char idEnt[NAME];
    ListaInterPtr interazioniOrig[HASH_DIMENSION];
    ListaInterPtr interazioniDest[HASH_DIMENSION];
    struct treeEnt *pPtr;
    struct treeEnt *leftPtr;
    struct treeEnt *rightPtr;
};

// Create some aliases
typedef struct treeEnt TreeEnt;
typedef TreeEnt *TreeEntPtr;




// Counters list (one for each relation)
struct listaCont{
    unsigned short contatore_rel;
    TreeEntPtr destNode;
    struct listaCont *prevPtr;
    struct listaCont *nextPtr;
};

// Create some aliases
typedef struct listaCont ListaCont;
typedef ListaCont *ListaContPtr;




// Relations list
struct listaRel{
    unsigned short idRel;
    char relazione[NAME];
    bool delent;
    ListaContPtr contatori;
    struct listaRel *prevPtr;
    struct listaRel *nextPtr;
};

// Create some aliases
typedef struct listaRel ListaRel;
typedef ListaRel *ListaRelPtr;


//********************************************************************************************* Variabili globali
unsigned short cont=0;
unsigned short contRel=0;
TreeEntPtr Hash[HASH_DIMENSION];
ListaRelPtr relRoot;


//********************************************************************************************* Prototipi funzioni
void addent( char string[] );
void delent( char string[] );
void addrel( char string[] );
void delrel( char string[] );
void report( void );
void hashInsert( char idEnt[] );
void hashDelete( TreeEntPtr entNode );
TreeEntPtr hashSearch(unsigned int key,  char idEnt[] );
void treeEntInsert( TreeEntPtr EntRoot, unsigned short key, char nome[] );
void treeEntDelete( TreeEntPtr *EntRoot,TreeEntPtr treeNode );
void sistemaInterazioni( ListaInterPtr interazioniOrig[], ListaInterPtr interazioniDest[], TreeEntPtr entNode );
TreeEntPtr treeEntSuccessor( TreeEntPtr treeNode );
TreeEntPtr treeEntMinimum( TreeEntPtr treeNode );
TreeEntPtr treeEntSearch( TreeEntPtr node, char idEnt[] );
void listaInterInsert( TreeEntPtr destNode, ListaContPtr *contRoot, TreeEntPtr altraEnt, unsigned short rel );
ListaInterPtr listaInterAdd( ListaInterPtr *interRoot, ListaInterPtr altraInter, ListaContPtr contNode, TreeEntPtr altraEnt, unsigned short rel );
void listaInterCrea( TreeEntPtr destNode, ListaContPtr *contNode, TreeEntPtr origNode, unsigned short rel );
void listaInterDelete( TreeEntPtr destNode, TreeEntPtr origNode, ListaRelPtr relNode );
void altraInterDelete(ListaInterPtr *interRoot, ListaInterPtr interNode );
void interDelent( TreeEntPtr entNode );
ListaInterPtr altraEntInterDelete( ListaInterPtr interNode );
void ripristinaSnap(void);
ListaRelPtr listaRelInsert( char rel[] );
ListaContPtr listaContInsert( ListaContPtr *contRoot, TreeEntPtr entNode );
void listaContDelete( ListaContPtr *contRoot, unsigned short dest );
void listaContDecrementa( ListaContPtr *contRoot, ListaContPtr contNode );
void listaContSnap(ListaContPtr *contRoot, ListaContPtr contNode );
ListaRelPtr listaRelSearch ( char rel[] );
bool listaRelDelete( ListaRelPtr relNode );
ListaContPtr swap(ListaContPtr primoPtr, ListaContPtr secondoPtr );





//************************************************************************************************* MAIN
int main(){
    
    // Initialize to NULL the relations list
    relRoot = NULL;
    
    // Initialize to NULL the hash array
    unsigned int i;
    
    for(i=0;i<HASH_DIMENSION;i++){
        Hash[i] = NULL;
    }
    
    // Read input
    char input[INPUT];
    char s;
    unsigned long contRiga = 0;
        
    while(i != 1){
        
        s = *fgets(input, INPUT, stdin);
        s++;
        contRiga++;
        if(input[0] == 'a'){
            if(input[3] == 'e'){
                addent( input );
            }else{
                addrel( input );
            }
        }else if(input[0] == 'd'){
            if(input[3] == 'e'){
                delent( input );
            }else{
                delrel( input );
            }
        }else if(input[0] == 'r'){
            report();
        }else if(input[0] == 'e'){
            i = 1;
        }else {
            printf("Errore\n");
        }
        
    }
    
    
    return 0;
}


//************************************************  FUNCTIONS

// Start monitoring an entity (adding entity to entities hash structure)
void addent( char string[] ){
    unsigned int i=8, j=0;
    char idEnt[NAME];
    
    while (string[i] != '"'){
        idEnt[j] = string[i];
        i++;
        j++;
    }
    idEnt[j] = '\0';
    hashInsert( idEnt );
    
}



// Stop monitoring an entity (delete entity from entities hash structure)
void delent( char string[] ){
    unsigned int i=8, j=0;
    char idEnt[NAME];
    
    while (string[i] != '"'){
        idEnt[j] = string[i];
        i++;
        j++;
    }
    idEnt[j] = '\0';
    TreeEntPtr entNode;
    entNode = hashSearch( idEnt[0] % HASH_DIMENSION,  idEnt );
    if( entNode!=NULL ){
        interDelent( entNode );
        hashDelete( entNode );
        ripristinaSnap();
    }
}




// Adds a new relationship between two entities
void addrel( char string[] ){
    unsigned int i=8, j=0;
    char idOrig[NAME];
    char idDest[NAME];
    char rel[NAME];
    TreeEntPtr origNode;
    TreeEntPtr destNode;
    
    
    while( string[i] != '"' ){
        idOrig[j] = string[i];
        i++;
        j++;
    }
    idOrig[j] = '\0';
    i = i+3;
    j = 0;
    
    while( string[i] != '"' ){
        idDest[j] = string[i];
        i++;
        j++;
    }
    idDest[j] = '\0';
    i = i+3;
    j = 0;
    
    while ( string[i] != '"' ){
        rel[j] = string[i];
        i++;
        j++;
    }
    rel[j] = '\0';
    
    origNode = hashSearch( idOrig[0] % HASH_DIMENSION, idOrig );
    if( origNode!=NULL ){
        destNode = hashSearch( idDest[0] % HASH_DIMENSION, idDest );
        if( destNode!=NULL ){
            ListaRelPtr relNode;
            relNode = listaRelSearch( rel );
            if(relNode != NULL ){
                listaInterInsert(destNode, &relNode->contatori, origNode, relNode->idRel);
            }else{
                relNode = listaRelInsert( rel );
                listaInterCrea(destNode, &relNode->contatori, origNode, relNode->idRel);
            }
        }
    }
    
}




// Delete a relationship between two entities
void delrel( char string[] ){
    
    unsigned int i=8, j=0;
    char idOrig[NAME];
    char idDest[NAME];
    char rel[NAME];
    TreeEntPtr origNode;
    TreeEntPtr destNode;
    
    
    while( string[i] != '"' ){
        idOrig[j] = string[i];
        i++;
        j++;
    }
    idOrig[j] = '\0';
    i = i+3;
    j = 0;
    
    while( string[i] != '"' ){
        idDest[j] = string[i];
        i++;
        j++;
    }
    idDest[j] = '\0';
    i = i+3;
    j = 0;
    
    while ( string[i] != '"' ){
        rel[j] = string[i];
        i++;
        j++;
    }
    rel[j] = '\0';
    
    origNode = hashSearch( idOrig[0] % HASH_DIMENSION , idOrig);
    if( origNode!=NULL ){
        destNode = hashSearch( idDest[0] % HASH_DIMENSION, idDest );
        if( destNode!=NULL ){
            ListaRelPtr relNode;
            relNode = listaRelSearch( rel );
            if(relNode != NULL ){
                // Check if a relationship between two entities already exists. If so, delete it
                listaInterDelete(destNode, origNode, relNode);
                
            }
        }
    }
}




void report(void){
    
    if( relRoot==NULL ){
        printf("none");
    }else{
        ListaRelPtr relNode;
        ListaContPtr contNode;
        relNode = relRoot;
        while( relNode!=NULL ){
            fputs("\"", stdout);
            fputs(relNode->relazione, stdout);
            fputs("\" ", stdout);
            contNode = relNode->contatori;
            fputs("\"", stdout);
            fputs( (contNode->destNode)->idEnt, stdout);  ;
            fputs("\" ", stdout);
            while( contNode->nextPtr!=NULL ){
                if( (contNode->nextPtr)->contatore_rel==contNode->contatore_rel){
                    contNode = contNode->nextPtr;
                    fputs("\"", stdout);
                    fputs( (contNode->destNode)->idEnt, stdout);
                    fputs("\" ", stdout);
                }else{
                    break;
                }
            }
            if( relNode->nextPtr!=NULL ){
                printf("%d; ", contNode->contatore_rel);
            }else{
                printf("%d;", contNode->contatore_rel);
            }
            relNode = relNode->nextPtr;
        }
    }
    
    printf("\n");
    
}



void hashInsert( char idEnt[] ){
    
    short i;
    TreeEntPtr node;
    node = hashSearch( idEnt[0] % HASH_DIMENSION, idEnt);
    if( node == NULL ){
        unsigned int key = cont;
        cont++;
        if( Hash[ idEnt[0] % HASH_DIMENSION ] == NULL){
            TreeEntPtr newNode;
            newNode = malloc( sizeof(TreeEnt) ); // Create the node to insert
            if( newNode != NULL ){ // If the new node as been created...
                newNode->id = key;
                strcpy(newNode->idEnt, idEnt);
                for( i=0;i<HASH_DIMENSION;i++ ){
                    newNode->interazioniDest[i] = NULL;
                    newNode->interazioniOrig[i] = NULL;
                }
                newNode->pPtr = NULL;
                newNode->rightPtr = NULL;
                newNode->leftPtr = NULL;
                Hash[ idEnt[0] % HASH_DIMENSION ] = newNode;
                
                
            }
        }else{
            treeEntInsert( Hash[ idEnt[0] % HASH_DIMENSION ] , key, idEnt );
        
        }
    }
}




void hashDelete( TreeEntPtr entNode ){
    
    unsigned int init;
    init = (entNode->idEnt[0]) % HASH_DIMENSION;
    if( entNode != NULL ){
        if( (Hash[init] )->pPtr == NULL && (Hash[init])->leftPtr == NULL && (Hash[init])->rightPtr == NULL ){
            free( Hash[init] );
            Hash[init] = NULL;
        }else{
            treeEntDelete( &Hash[init], entNode );
        }
    }
}




// Return searched entity's node giving the name
TreeEntPtr hashSearch( unsigned int key, char idEnt[] ){
    
    if( Hash[key] == NULL ){
        return NULL;
    }
    else{
        return( treeEntSearch( Hash[key], idEnt ) );
    }
}




// Insert a new node in the tree ordering it by name
void treeEntInsert( TreeEntPtr EntRoot, unsigned short key, char nome[] ){
    
    TreeEntPtr newNode; // Point to next node
    TreeEntPtr prevNode; // Point to previous node
    TreeEntPtr currNode; // Point to current node
    int res;
    
    newNode = malloc( sizeof(TreeEnt) ); // Create the node to insert
    
    if( newNode != NULL ){ // If the new node has been created...
        
        newNode->id = key; // Assign key to new node
        strcpy(newNode->idEnt, nome); // Assign entity's name to new node
        
        // Initialize new node's pointers
        for(res=0;res<HASH_DIMENSION;res++){
            newNode->interazioniDest[res] = NULL;
            newNode->interazioniOrig[res] = NULL;
        }
        newNode->pPtr = NULL;
        newNode->leftPtr = NULL;
        newNode->rightPtr = NULL;
        
        prevNode = NULL; // Initialize prevNode
        currNode = EntRoot; // Initialize currNode with radix address
        
        while( currNode != NULL ){
            
            // Go to next node through correct tree side
            prevNode = currNode;
            res = strcmp( newNode->idEnt, currNode->idEnt );
            if( res < 0 ){
                currNode = currNode->leftPtr;
            }else{ 
                currNode = currNode->rightPtr;
            }
            
        }
        
        newNode->pPtr = prevNode; //Salvo il riferimento al padre del nuovo nodo
        if( prevNode == NULL ){ //Se l'albero è vuoto...
            EntRoot = newNode; //...il nuovo nodo diventa la radice...
        }else {
            res = strcmp( newNode->idEnt, prevNode->idEnt );
            if( res < 0 ){ //altrimenti scelgo il lato corretto
                prevNode->leftPtr = newNode;
            }else{
                prevNode->rightPtr = newNode;
            }
        }
    }
    
}




//Elimino un nodo dall'albero usando le funzioni di appoggio treeSuccessor e treeMinimum
void treeEntDelete(TreeEntPtr *EntRoot, TreeEntPtr treeNode){
    
    TreeEntPtr eliminaNodo;
    TreeEntPtr sostiituisciNodo;
    
    if( (treeNode->leftPtr == NULL) || (treeNode->rightPtr == NULL) ){
        eliminaNodo = treeNode;
    }else{
        eliminaNodo = treeEntSuccessor(treeNode);
    }
    if( eliminaNodo->leftPtr != NULL ){
        sostiituisciNodo = eliminaNodo->leftPtr;
    }else{
        sostiituisciNodo = eliminaNodo->rightPtr;
    }
    if( sostiituisciNodo != NULL ){
        sostiituisciNodo->pPtr = eliminaNodo->pPtr;
    }
    if( eliminaNodo->pPtr == NULL ){
        *EntRoot = sostiituisciNodo;
    }else if( eliminaNodo == (eliminaNodo->pPtr)->leftPtr ){
        (eliminaNodo->pPtr)->leftPtr = sostiituisciNodo;
    }else{
        (eliminaNodo->pPtr)->rightPtr = sostiituisciNodo;
    }
    if( eliminaNodo != treeNode ){
        treeNode->id = eliminaNodo->id;
        strcpy(treeNode->idEnt, eliminaNodo->idEnt);
        for(int i=0;i<HASH_DIMENSION;i++){
            treeNode->interazioniOrig[i] = eliminaNodo->interazioniOrig[i];
            treeNode->interazioniDest[i] = eliminaNodo->interazioniDest[i];
        }
        sistemaInterazioni( treeNode->interazioniOrig, treeNode->interazioniDest, treeNode);
    }
    free( eliminaNodo );
}




void sistemaInterazioni( ListaInterPtr interazioniOrig[], ListaInterPtr interazioniDest[], TreeEntPtr entNode ){
    ListaInterPtr interOrigNode;
    ListaInterPtr interDestNode;
    for(int i=0;i<HASH_DIMENSION;i++){
        
        //Sistemo le interazioni origine
        interOrigNode = interazioniOrig[i];
        while( interOrigNode!=NULL ){
            (interOrigNode->otherInter)->altraEntNode = entNode;
            if( interOrigNode->dest ){
                (interOrigNode->cont)->destNode = entNode;
            }
            interOrigNode = interOrigNode->nextPtr;
        }
        
        //Sistemo le interazioni destinazione
        interDestNode = interazioniDest[i];
        while( interDestNode!=NULL ){
            (interDestNode->otherInter)->altraEntNode = entNode;
            if( interDestNode->dest ){
                (interDestNode->cont)->destNode = entNode;
            }
            interDestNode = interDestNode->nextPtr;
        }
    }
    
}




//Trova  il successore del nodo dato
TreeEntPtr treeEntSuccessor(TreeEntPtr treeNode){
    
    TreeEntPtr yNode;
    
    if( treeNode->rightPtr != NULL ){
        return treeEntMinimum(treeNode->rightPtr);
    }
    
    yNode = treeNode->pPtr;
    while( (yNode != NULL) && (treeNode == yNode->rightPtr) ){
        treeNode = yNode;
        yNode = yNode->pPtr;
    }
    
    return yNode;
}




//Trova il minimo nell'albero che ha come raadice il nodo passato
TreeEnt* treeEntMinimum(TreeEntPtr treeNode){
    while( treeNode->leftPtr != NULL ){
        treeNode = treeNode->leftPtr;
    }
    return treeNode;
}




//Cerca e restituisce un nodo sapendo l'idEnt
TreeEntPtr treeEntSearch( TreeEntPtr node, char idEnt[] ){
    
    if( node == NULL ){
        return NULL;
    }else{
        int res;
        res = strcmp(idEnt, node->idEnt);
        if( res == 0 ){
            return node;
        }else if( res < 0 ){
            return treeEntSearch(node->leftPtr, idEnt);
        }else{
            return treeEntSearch(node->rightPtr, idEnt);
        }
    }
}




//Controlla se esiste già l'interazione. Se esiste restituisce 1, altrimenti 0 e la aggiunge
void listaInterInsert( TreeEntPtr destNode, ListaContPtr *contRoot, TreeEntPtr altraEnt, unsigned short rel ){
    ListaInterPtr prevPtr;
    ListaInterPtr currPtr;
    prevPtr = NULL;
    short init = (altraEnt->idEnt[0]) % HASH_DIMENSION;
    currPtr = destNode->interazioniDest[init];
    
    while( (currPtr!=NULL) ){
        if( (currPtr->rel != rel) || (currPtr->dest != true) || ( (currPtr->altraEntNode)->id) != (altraEnt)->id ){
            prevPtr = currPtr;
            currPtr = currPtr->nextPtr;
        }else{
            break;
        }
        
    }
    if( currPtr == NULL ){
        ListaInterPtr newNode;
        newNode = malloc( sizeof(ListaInter) );
        
        if( newNode!=NULL ){
        
            ListaContPtr contNode;
            ListaInterPtr altraInter;
            //Inizializzo il nuovo nodo
            newNode->rel = rel;
            newNode->dest = true;
            newNode->prevPtr = NULL;
            newNode->nextPtr = NULL;
            newNode->altraEntNode = altraEnt;
            
            if(prevPtr == NULL){
                destNode->interazioniDest[init] = newNode;
            }else{
                newNode->prevPtr = prevPtr;
                prevPtr->nextPtr = newNode;
                
            }
            contNode = listaContInsert( contRoot, destNode);
            newNode->cont = contNode;
            
            altraInter = listaInterAdd( &altraEnt->interazioniOrig[ (destNode->idEnt[0]) % HASH_DIMENSION ], newNode, contNode, destNode, rel );
            /*if( (altraEnt->interazioni)->nextPtr!=NULL ){
                ((altraEnt->interazioni)->nextPtr)->prevPtr = (altraEnt->interazioni);
                }*/
            newNode->otherInter = altraInter;
        }
    }
}




//Aggiunge un istanza di relazione
ListaInterPtr listaInterAdd( ListaInterPtr *interRoot, ListaInterPtr altraInter, ListaContPtr contNode, TreeEntPtr altraEnt, unsigned short rel ){
    ListaInterPtr newNode;
    newNode = malloc( sizeof(ListaInter) );
    if( newNode!=NULL ){
        
        //Inizializzo il nuovo nodo
        newNode->rel = rel;
        newNode->dest = false;
        newNode->prevPtr = NULL;
        newNode->otherInter = altraInter;
        newNode->altraEntNode = altraEnt;
        newNode->cont = contNode;
        if( (*interRoot)!=NULL ){
            newNode->nextPtr  = *interRoot;
            (*interRoot)->prevPtr = newNode;
        }else{
            newNode->nextPtr = NULL;
        }
        
        *interRoot = newNode;
        return newNode;
    }else{
        return NULL;
    }
}




void listaInterCrea( TreeEntPtr destNode, ListaContPtr *contNode, TreeEntPtr origNode, unsigned short rel ){
    
    ListaContPtr newCont;
    newCont = malloc( sizeof( ListaCont ) );
    if( newCont!=NULL ){
        ListaInterPtr newInterDest;
        newInterDest = malloc( sizeof( ListaInter ) );
        if( newInterDest!=NULL ){
            ListaInterPtr newInterOrig;
            newInterOrig = malloc( sizeof( ListaInter ) );
            if( newInterOrig!=NULL ){
                //Inizializzo e attacco il contatore
                newCont->contatore_rel = 1;
                newCont->destNode = destNode;
                newCont->prevPtr = NULL;
                if( (*contNode)!=NULL ){
                    (*contNode)->prevPtr = newCont;
                    newCont->nextPtr = (*contNode);
                }else{
                    newCont->nextPtr = NULL;
                }
                
                *contNode = newCont;
                
                //Inizializzo e attacco l'interazione per il destinatario
                short init = (origNode->idEnt[0]) % HASH_DIMENSION;
                newInterDest->altraEntNode = origNode;
                newInterDest->cont = newCont;
                newInterDest->dest = true;
                newInterDest->prevPtr = NULL;
                if( destNode->interazioniDest[init]!=NULL ){
                    newInterDest->nextPtr = destNode->interazioniDest[init];
                    (destNode->interazioniDest[init])->prevPtr = newInterDest;
                }else{
                    newInterDest->nextPtr = NULL;
                }
                newInterDest->rel = rel;
                newInterDest->otherInter = newInterOrig;
                destNode->interazioniDest[init] = newInterDest;
                
                //Inizializzo e attacco l'interazione per l'origine
                init = (destNode->idEnt[0]) % HASH_DIMENSION;
                newInterOrig->altraEntNode = destNode;
                newInterOrig->cont = newCont;
                newInterOrig->dest = false;
                newInterOrig->prevPtr = NULL;
                if( origNode->interazioniOrig[init]!=NULL ){
                    newInterOrig->nextPtr = origNode->interazioniOrig[init];
                    (origNode->interazioniOrig[init])->prevPtr = newInterOrig;
                }else{
                    newInterOrig->nextPtr = NULL;
                }
                newInterOrig->rel = rel;
                newInterOrig->otherInter = newInterDest;
                
                origNode->interazioniOrig[init] = newInterOrig;
            }
        }
    }
    
}




//Controllo se esiste già l'interazione. Se esiste la elimina, decrementa (o elimina) il contatore ed elimina l'interazione per l'altra entità
void listaInterDelete( TreeEntPtr destNode, TreeEntPtr origNode, ListaRelPtr relNode ){
    ListaInterPtr prevPtr;
    ListaInterPtr currPtr;
    prevPtr = NULL;
    currPtr = destNode->interazioniDest[ (origNode->idEnt[0]) % HASH_DIMENSION ];
    while( (currPtr!=NULL) ){
        if( (currPtr->rel != relNode->idRel) || ((currPtr->altraEntNode)->id != origNode->id) || !(currPtr->dest) ){
            prevPtr = currPtr;
            currPtr = currPtr->nextPtr;
        }else{
            break;
        }
    }
    if( currPtr != NULL ){
        
        //Elimino il blocco
        if( (prevPtr==NULL) && (currPtr->nextPtr==NULL) ){//Radice e NULL finale
            destNode->interazioniDest[ (origNode->idEnt[0]) % HASH_DIMENSION ] = NULL;
        }else if( (prevPtr==NULL) && (currPtr->nextPtr!=NULL) ){//Radice
            destNode->interazioniDest[ (origNode->idEnt[0]) % HASH_DIMENSION ] = currPtr->nextPtr;
            (currPtr->nextPtr)->prevPtr = NULL;
        }else if( (prevPtr!=NULL) && (currPtr->nextPtr==NULL) ){//NULL finale
            prevPtr->nextPtr = NULL;
        }else{//Centrale
            ListaInterPtr currNextPtr;
            currNextPtr = currPtr->nextPtr;
            currNextPtr->prevPtr = prevPtr;
            prevPtr->nextPtr = currNextPtr;
        }
        listaContDecrementa(&(relNode->contatori), currPtr->cont );
        listaRelDelete(relNode);
        altraInterDelete(&(origNode->interazioniOrig[ (destNode->idEnt[0]) %HASH_DIMENSION ]), currPtr->otherInter);
        free(currPtr);
        
    }
}



void altraInterDelete(ListaInterPtr *interRoot, ListaInterPtr interNode ){
    ListaInterPtr prevPtr;
    prevPtr = interNode->prevPtr;
    //Elimino il blocco
    if( (prevPtr==NULL) && (interNode->nextPtr==NULL) ){//Radice e NULL finale
        *interRoot = NULL;
    }else if( (prevPtr==NULL) && (interNode->nextPtr!=NULL) ){//Radice
        *interRoot = interNode->nextPtr;
        (interNode->nextPtr)->prevPtr = NULL;
    }else if( (prevPtr!=NULL) && (interNode->nextPtr==NULL) ){//NULL finale
        prevPtr->nextPtr = NULL;
    }else{//Centrale
        ListaInterPtr currNextPtr;
        currNextPtr = interNode->nextPtr;
        currNextPtr->prevPtr = prevPtr;
        prevPtr->nextPtr = currNextPtr;
    }
    free(interNode);
}




//Scorri le relazioni dell'entità ed elimina le loro interazioni con questa entità. Ogni volta decrementa i relativi contatori.
void interDelent( TreeEntPtr entNode ){
    ListaRelPtr relNode;
    ListaInterPtr interNode;
    
    for( short i=0;i<HASH_DIMENSION;i++ ){
        interNode = entNode->interazioniDest[i];
        
        //Scorro la lista delle interazioniDest e chiamo le relative funzioni di appoggio
        while( interNode!=NULL ){
            relNode = relRoot;
            while ( relNode!=NULL ){
                if( relNode->idRel!= interNode->rel ){
                    relNode = relNode->nextPtr;
                }else{
                    break;
                }
                
            }
            short init = (entNode->idEnt[0]) % HASH_DIMENSION;
            if( relNode!=NULL){
                if( !(relNode->delent) ){
                    listaContSnap(&(relNode->contatori), interNode->cont);
                    bool del;
                    del = listaRelDelete(relNode);
                    if( !del ){
                        relNode->delent = true;
                    }
                }
                
                ListaInterPtr tmpPtr;
                if( ((interNode->altraEntNode)->interazioniOrig[init] == NULL) || (((interNode->altraEntNode)->interazioniOrig[init])->nextPtr == NULL) ){
                    (interNode->altraEntNode)->interazioniOrig[init] = NULL;
                }else{
                    tmpPtr = altraEntInterDelete( interNode->otherInter );
                    if( tmpPtr!= NULL ){
                        (interNode->altraEntNode)->interazioniOrig[init] = tmpPtr;
                    }
                }
                tmpPtr = interNode;
                interNode = interNode->nextPtr;
                free(tmpPtr);
            }else{
                ListaInterPtr tmpPtr;
                if( ((interNode->altraEntNode)->interazioniOrig[ (entNode->idEnt[0]) % HASH_DIMENSION ] == NULL) || (((interNode->altraEntNode)->interazioniOrig[ (entNode->idEnt[0]) % HASH_DIMENSION ])->nextPtr == NULL) ){
                    (interNode->altraEntNode)->interazioniOrig[ (entNode->idEnt[0]) % HASH_DIMENSION ] = NULL;
                }else{
                    tmpPtr = altraEntInterDelete( interNode->otherInter );
                    if( tmpPtr!= NULL ){
                        (interNode->altraEntNode)->interazioniOrig[ (entNode->idEnt[0]) % HASH_DIMENSION ] = tmpPtr;
                    }
                }
                tmpPtr = interNode;
                interNode = interNode->nextPtr;
                free(tmpPtr);
            }
        }
    }
    
   
    for( short i=0;i<HASH_DIMENSION;i++ ){
        interNode = entNode->interazioniOrig[i];

        //Scorro la lista delle interazioniOrig e chiamo le relative funzioni di appoggio
        while( interNode!=NULL ){
            relNode = relRoot;
            while ( relNode!=NULL ){
                if( relNode->idRel!= interNode->rel ){
                    relNode = relNode->nextPtr;
                }else{
                    break;
                }
            
            }
            if( relNode!=NULL){
                
                listaContDecrementa(&(relNode->contatori), interNode->cont);
                listaRelDelete(relNode);
                
                ListaInterPtr tmpPtr;
                if( ((interNode->altraEntNode)->interazioniDest[ (entNode->idEnt[0]) % HASH_DIMENSION ] == NULL) || (((interNode->altraEntNode)->interazioniDest[ (entNode->idEnt[0]) % HASH_DIMENSION ])->nextPtr == NULL) ){
                    (interNode->altraEntNode)->interazioniDest[ (entNode->idEnt[0]) % HASH_DIMENSION ] = NULL;
                }else{
                    tmpPtr = altraEntInterDelete( interNode->otherInter );
                    if( tmpPtr!= NULL ){
                        (interNode->altraEntNode)->interazioniDest[ (entNode->idEnt[0]) % HASH_DIMENSION ] = tmpPtr;
                    }
                }
                tmpPtr = interNode;
                interNode = interNode->nextPtr;
                free(tmpPtr);
            }else{
                ListaInterPtr tmpPtr;
                if( ((interNode->altraEntNode)->interazioniOrig[ (entNode->idEnt[0]) % HASH_DIMENSION ] == NULL) || (((interNode->altraEntNode)->interazioniOrig[ (entNode->idEnt[0]) % HASH_DIMENSION ])->nextPtr == NULL) ){
                    (interNode->altraEntNode)->interazioniOrig[ (entNode->idEnt[0]) % HASH_DIMENSION ] = NULL;
                }else{
                    tmpPtr = altraEntInterDelete( interNode->otherInter );
                    if( tmpPtr!= NULL ){
                        (interNode->altraEntNode)->interazioniOrig[ (entNode->idEnt[0]) % HASH_DIMENSION ] = tmpPtr;
                    }
                }
                tmpPtr = interNode;
                interNode = interNode->nextPtr;
                free(tmpPtr);
            }
        }
    }
}



//Elimina l'interazione dell'altra entità per delent
ListaInterPtr altraEntInterDelete(ListaInterPtr interNode ){
    ListaInterPtr prevPtr = interNode->prevPtr;
    ListaInterPtr currPtr = interNode;
    
    //Elimino il blocco
    if( (prevPtr==NULL) && (currPtr->nextPtr==NULL) ){//Radice e NULL finale
        free(currPtr);
        return NULL;
    }else if( (prevPtr==NULL) && (currPtr->nextPtr!=NULL) ){//Radice
        (currPtr->nextPtr)->prevPtr = NULL;
        prevPtr = currPtr->nextPtr;
        free(currPtr);
        return prevPtr;
    }else if( (prevPtr!=NULL) && (currPtr->nextPtr==NULL) ){//NULL finale
        prevPtr->nextPtr = NULL;
        free(currPtr);
        return NULL;
    }else{//Centrale
        ListaInterPtr currNextPtr;
        currNextPtr = currPtr->nextPtr;
        currNextPtr->prevPtr = prevPtr;
        prevPtr->nextPtr = currNextPtr;
        free(currPtr);
        return NULL;
    }
    
}




//Fa come Hulk in Endgame
void ripristinaSnap(void){
    ListaRelPtr relNode;
    relNode = relRoot;
    while( relNode!=NULL ){
        relNode->delent = false;
        relNode = relNode->nextPtr;
    }
}




//Inserisci una relazione
ListaRelPtr listaRelInsert( char rel[] ){
    ListaRelPtr newNode;
    
    newNode = malloc( sizeof(ListaRel) );
    if( newNode!=NULL ){
        ListaRelPtr prevPtr;
        ListaRelPtr currPtr;
        strcpy(newNode->relazione, rel);
        newNode->idRel = contRel;
        contRel++;
        newNode->contatori = NULL;
        newNode->delent = false;
        newNode->prevPtr = NULL;
        newNode->nextPtr = NULL;
        prevPtr = NULL;
        currPtr = relRoot;
        int res;
        while( (currPtr!=NULL) ){//Scorri per l'ordine alfabetico
            res = strcmp(currPtr->relazione, rel);
            if(res<0){
                prevPtr = currPtr;
                currPtr = currPtr->nextPtr;
            }else{
                break;
            }
        }
        if( currPtr!=NULL ){
            if( prevPtr!=NULL ){
                prevPtr->nextPtr = newNode;
                newNode->prevPtr = prevPtr;
                newNode->nextPtr = currPtr;
                currPtr->prevPtr = newNode;
            }else{
                relRoot = newNode;
                newNode->nextPtr = currPtr;
                currPtr->prevPtr = newNode;
            }
            
            return newNode;
        }else{
            if( prevPtr == NULL ){
                relRoot = newNode;
                return relRoot;
            }else{
                prevPtr->nextPtr = newNode;
                newNode->prevPtr = prevPtr;
                return newNode;
            }
            
        }
        
    }else{
        return NULL;
    }
}




//Incrementa (o inizializza) il contatore per il destinatario
ListaContPtr listaContInsert( ListaContPtr *contRoot, TreeEntPtr entNode ){
    if( (*contRoot) == NULL ){
        ListaContPtr newNode;
        newNode = malloc( sizeof(ListaCont) );
        newNode->contatore_rel = 1;
        newNode->destNode = entNode;
        newNode->prevPtr = NULL;
        newNode->nextPtr = NULL;
        *contRoot = newNode;
        
        return newNode;
    }else{
        ListaContPtr prevPtr;
        ListaContPtr currPtr;
        prevPtr = NULL;
        currPtr = (*contRoot);
        while( (currPtr != NULL) && ((currPtr->destNode)->id!=entNode->id) ){
            prevPtr = currPtr;
            currPtr = currPtr->nextPtr;
        }
        if( currPtr == NULL ){
            ListaContPtr newNode;
            newNode = malloc( sizeof(ListaCont) );
            
            if( newNode!=NULL ){
                
                //Inizializzo il nuovo nodo
                newNode->contatore_rel = 1;
                newNode->destNode = entNode;
                newNode->prevPtr = NULL;
                newNode->nextPtr = NULL;
                
                if(prevPtr == NULL){
                    *contRoot = newNode;
                }else{
                    prevPtr->nextPtr = newNode;
                    newNode->prevPtr = prevPtr;
                    int res;
                    while ( (newNode->prevPtr!=NULL) ) {
                        if((newNode->prevPtr)->contatore_rel<=newNode->contatore_rel){
                            res = strcmp( ((newNode->destNode)->idEnt), (((newNode->prevPtr)->destNode)->idEnt) );
                            if( res < 0){
                                ListaContPtr newRoot;
                                newRoot = swap(newNode->prevPtr, newNode);
                                if( newRoot!=NULL ){
                                    *contRoot = newRoot;
                                }
                            }else{
                                break;
                            }
                        }else{
                            break;
                        }
                    }
                }
            }
            return newNode;
        }else{
            currPtr->contatore_rel++;
            int res;
            while( (currPtr->prevPtr!=NULL) && (currPtr->contatore_rel>(currPtr->prevPtr)->contatore_rel ) ){
                ListaContPtr newRoot;
                newRoot = swap(currPtr->prevPtr, currPtr);
                if( newRoot!=NULL ){
                    *contRoot = newRoot;
                }
            }
            while ( (currPtr->prevPtr!=NULL) ) {
                if((currPtr->prevPtr)->contatore_rel<=currPtr->contatore_rel){
                    res = strcmp((currPtr->destNode)->idEnt, ((currPtr->prevPtr)->destNode)->idEnt);
                    if( res < 0 ){
                        ListaContPtr newRoot;
                        newRoot = swap(currPtr->prevPtr, currPtr);
                        if( newRoot!=NULL ){
                            *contRoot = newRoot;
                        }
                    }else{
                        break;
                    }
                }else{
                    break;
                }
                
            }
            return currPtr;
        }
    }
}




//Decrementa (o elimina) il contatore per il destinatario
void listaContDelete( ListaContPtr *contRoot, unsigned short dest ){
    if( (*contRoot) != NULL ){
        ListaContPtr prevPtr;
        ListaContPtr currPtr;
        prevPtr = NULL;
        currPtr = *contRoot;
        while( (currPtr != NULL) && ((currPtr->destNode)->id!=dest) ){
            prevPtr = currPtr;
            currPtr = currPtr->nextPtr;
        }
        if( currPtr != NULL ){
            currPtr->contatore_rel--;
            if( currPtr->contatore_rel == 0 ){
                
                //Elimino il blocco
                if( (prevPtr==NULL) && (currPtr->nextPtr==NULL) ){//Radice e NULL finale
                    *contRoot = NULL;
                }else if( (prevPtr==NULL) && (currPtr->nextPtr!=NULL) ){//Radice
                    *contRoot = currPtr->nextPtr;
                    (currPtr->nextPtr)->prevPtr = NULL;
                }else if( (prevPtr!=NULL) && (currPtr->nextPtr==NULL) ){//NULL finale
                    prevPtr->nextPtr = NULL;
                }else{//Centrale
                    ListaContPtr currNextPtr;
                    currNextPtr = currPtr->nextPtr;
                    currNextPtr->prevPtr = prevPtr;
                    prevPtr->nextPtr = currNextPtr;
                }
                
                free(currPtr);
                
            }else{
                while( (currPtr->nextPtr!=NULL) && (currPtr->contatore_rel<(currPtr->nextPtr)->contatore_rel) ){
                    ListaContPtr newRoot;
                    newRoot = swap(currPtr, currPtr->nextPtr);
                    if( newRoot!=NULL ){
                        *contRoot = newRoot;
                    }
                }while( (currPtr->nextPtr!=NULL) && ( strcmp(((currPtr->destNode)->idEnt), ((currPtr->nextPtr)->destNode)->idEnt)>0 ) && (currPtr->contatore_rel==(currPtr->nextPtr)->contatore_rel) ){
                    ListaContPtr newRoot;
                    newRoot = swap(currPtr, currPtr->nextPtr );
                    if( newRoot!=NULL ){
                        *contRoot = newRoot;
                    }
                }
            }
        }
    }
}




void listaContDecrementa( ListaContPtr *contRoot, ListaContPtr contNode ){
    if( contNode->contatore_rel==1 ){
        ListaContPtr prevPtr;
        prevPtr = contNode->prevPtr;
        //Elimino il blocco
        if( (prevPtr==NULL) && (contNode->nextPtr==NULL) ){//Radice e NULL finale
            *contRoot = NULL;
        }else if( (prevPtr==NULL) && (contNode->nextPtr!=NULL) ){//Radice
            *contRoot = contNode->nextPtr;
            (contNode->nextPtr)->prevPtr = NULL;
        }else if( (prevPtr!=NULL) && (contNode->nextPtr==NULL) ){//NULL finale
            prevPtr->nextPtr = NULL;
        }else{//Centrale
            ListaContPtr currNextPtr;
            currNextPtr = contNode->nextPtr;
            currNextPtr->prevPtr = prevPtr;
            prevPtr->nextPtr = currNextPtr;
        }
        
        free(contNode);
    }else{
        contNode->contatore_rel--;
        while( (contNode->nextPtr!=NULL) && (contNode->contatore_rel<(contNode->nextPtr)->contatore_rel) ){
            ListaContPtr newRoot;
            newRoot = swap(contNode, contNode->nextPtr);
            if( newRoot!=NULL ){
                *contRoot = newRoot;
            }
        }while( (contNode->nextPtr!=NULL) && ( strcmp(((contNode->destNode)->idEnt), ((contNode->nextPtr)->destNode)->idEnt)>0 ) && (contNode->contatore_rel==(contNode->nextPtr)->contatore_rel) ){
            ListaContPtr newRoot;
            newRoot = swap(contNode, contNode->nextPtr );
            if( newRoot!=NULL ){
                *contRoot = newRoot;
            }
        }
    }
}




void listaContSnap(ListaContPtr *contRoot, ListaContPtr contNode ){
    ListaContPtr prevPtr;
    ListaContPtr currPtr;
    prevPtr = contNode->prevPtr;
    currPtr = contNode;
    //Elimino il blocco
    if( (prevPtr==NULL) && (currPtr->nextPtr==NULL) ){//Radice e NULL finale
        *contRoot = NULL;
    }else if( (prevPtr==NULL) && (currPtr->nextPtr!=NULL) ){//Radice
        *contRoot = currPtr->nextPtr;
        (currPtr->nextPtr)->prevPtr = NULL;
    }else if( (prevPtr!=NULL) && (currPtr->nextPtr==NULL) ){//NULL finale
        prevPtr->nextPtr = NULL;
    }else{//Centrale
        ListaContPtr currNextPtr;
        currNextPtr = currPtr->nextPtr;
        currNextPtr->prevPtr = prevPtr;
        prevPtr->nextPtr = currNextPtr;
    }
    
    free(currPtr);
}




//Cerca se esiste già il tipo di relazione. Se esiste lo ritorna, altrimenti ritorna NULL
ListaRelPtr listaRelSearch ( char rel[] ){
    if( relRoot==NULL || strcmp( relRoot->relazione, rel)==0 ){
        return relRoot;
    }else{
        ListaRelPtr currPtr;
        currPtr = relRoot;
        while( currPtr != NULL ){
            if( strcmp(currPtr->relazione, rel)< 0 ){
                currPtr = currPtr->nextPtr;
            }else{
                break;
            }
        }
        if( currPtr==NULL ){
            return NULL;
        }else if( strcmp(currPtr->relazione, rel)==0 ){
            return currPtr;
        }else{
            return NULL;
        }
    }
}




//Elimino un tipo di relazione se non ha più contatori
bool listaRelDelete( ListaRelPtr relNode ){
    if( relNode->contatori==NULL ){
        ListaRelPtr prevPtr;
        ListaRelPtr currPtr;
        prevPtr = relNode->prevPtr;
        currPtr = relNode;
        //Elimino il blocco
        if( (prevPtr==NULL) && (currPtr->nextPtr==NULL) ){//Radice e NULL finale
            relRoot = NULL;
        }else if( (prevPtr==NULL) && (currPtr->nextPtr!=NULL) ){//Radice
            relRoot = currPtr->nextPtr;
            (currPtr->nextPtr)->prevPtr = NULL;
        }else if( (prevPtr!=NULL) && (currPtr->nextPtr==NULL) ){//NULL finale
            prevPtr->nextPtr = NULL;
        }else{//Centrale
            ListaRelPtr currNextPtr;
            currNextPtr = currPtr->nextPtr;
            currNextPtr->prevPtr = prevPtr;
            prevPtr->nextPtr = currNextPtr;
        }
        
        free(currPtr);
        return true;
    }else{
        return false;
    }
}




//Scambia i due nodi
ListaContPtr swap(ListaContPtr primoPtr, ListaContPtr secondoPtr ){
    if( ((primoPtr->prevPtr == NULL) && (secondoPtr->nextPtr == NULL)) ){//Radice e NULL finale
        secondoPtr->prevPtr = NULL;
        secondoPtr->nextPtr = primoPtr;
        primoPtr->prevPtr = secondoPtr;
        primoPtr->nextPtr = NULL;
        return secondoPtr;
    }else if( ((primoPtr->prevPtr == NULL) && (secondoPtr)->nextPtr != NULL) ){//Radice
        ListaContPtr secondoNextPtr;
        secondoNextPtr = secondoPtr->nextPtr;
        
        secondoPtr->prevPtr = NULL;
        secondoPtr->nextPtr = primoPtr;
        primoPtr->prevPtr = secondoPtr;
        primoPtr->nextPtr = secondoNextPtr;
        secondoNextPtr->prevPtr = primoPtr;
        return secondoPtr;
    }else if( ((primoPtr->prevPtr != NULL) && (secondoPtr->nextPtr == NULL)) ){//NULL finale
        ListaContPtr primoPrevPtr;
        primoPrevPtr = primoPtr->prevPtr;
        
        primoPrevPtr->nextPtr = secondoPtr;
        secondoPtr->prevPtr = primoPrevPtr;
        secondoPtr->nextPtr = primoPtr;
        primoPtr->prevPtr = secondoPtr;
        primoPtr->nextPtr = NULL;
        return NULL;
    }else{//Centrale
        ListaContPtr primoPrevPtr;
        ListaContPtr secondoNextPtr;
        primoPrevPtr = primoPtr->prevPtr;
        secondoNextPtr = secondoPtr->nextPtr;
        
        primoPrevPtr->nextPtr = secondoPtr;
        secondoPtr->prevPtr = primoPrevPtr;
        secondoPtr->nextPtr = primoPtr;
        primoPtr->prevPtr = secondoPtr;
        primoPtr->nextPtr = secondoNextPtr;
        secondoNextPtr->prevPtr = primoPtr;
        return NULL;
    }
}
