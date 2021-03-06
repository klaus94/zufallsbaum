#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


typedef struct listElement* ptrList;
typedef struct listElement
{
	int value;
	ptrList next;	
} listElement;

// Schnittstelle: Struktur zum Speichern der Kanten:
typedef struct edgeListElement* ptr_edgeList;
typedef struct edgeListElement
{
	int start;
	int end;
	ptr_edgeList next;
} edgeListElement;

// Ergebnis-Baum ///////////////////////
typedef struct node *NPtr;
typedef struct edge *EPtr;

typedef struct node{						// representiert alle Elemente des Baumes
	int value;								
	int count; 								// Anzahl der Vorgänger
	EPtr edges;								// Liste mit allen Kanten zu Nachfolgern (siehe [struct edge])
	NPtr next;								// nächstes Element im Baum
} node;

typedef struct edge{						// representiert alle FolgeKanten eines Knotens
	NPtr id;								// mit id
	EPtr next;								// mit nächster Kante
} edge;

void outEdges(EPtr ptr_head){
	if (ptr_head == NULL) return;

	if (ptr_head->id != NULL)
		printf("%d, ", ptr_head->id->value);

	if (ptr_head != NULL)
		outEdges(ptr_head->next);
}

int findLow(int code[], int start, int restrictedList[]){
	int iOK;
	int n = sizeof(code) + 2;
	for (int i = 0; i < n; i++ ){
		iOK = 1;
		// i in restrictedList?
		// i in code[start bis ende] ?
		// nein --> gefunden
		for (int j = 0; j < sizeof(restrictedList); j++){
			if (i == restrictedList[j])
				iOK = 0;
		}
		for (int j = start; j < sizeof(code); j++){
			if (i == code[j])
				iOK = 0;
		}
		if (iOK == 1){
			return i;
		}
	}
	return -1;
}

void makeHelpArray(int code[], int* helpArray){
	int i = 0;
	int n = sizeof(code);

	while (n > 0){
		helpArray[i] = findLow(code, i, helpArray);
		n--;
		i++;
	}
}

ptr_edgeList makeEdgeList(int code[], int help[]){
	// Part 1: Kante jeweils zwischen einem Code-und einem helpList-element
	ptr_edgeList newElement, lastElement;
	ptr_edgeList ptr_head = (ptr_edgeList) malloc(sizeof(edgeListElement));
	ptr_head->next = NULL;
	ptr_head->start = code[0];
	ptr_head->end = help[0];
	lastElement = ptr_head;

	for (int i = 1; i < sizeof(code); i++){
		newElement = (ptr_edgeList) malloc(sizeof(edgeListElement));
		newElement->next = NULL;
		newElement->start = code[i];
		newElement->end = help[i];

		lastElement->next = newElement;
		lastElement = newElement;
	}

	// Part 2: letzte Kante finden:
	int n = (int)sizeof(code);
	int entryNumber = 0;				// gibt an, ob gefundene Zahl: start oder end der Kante ist				

	int iOK;
	for (int i = 0; i < n+2; i++){		// Größe des Alphabetes ist um 2  größer, als die Länge des Prüfercodes
		iOK = 1;
		for (int j = 0; j < n; j++){	// loop durch help[]
			if (i == help[j])
				iOK = 0;
		}
		if (iOK == 1){
			if (entryNumber == 0){
				newElement = (ptr_edgeList) malloc(sizeof(edgeListElement));
				newElement->next = NULL;
				newElement->start = i;
				lastElement->next = newElement;
				entryNumber++;
			}
			if (entryNumber == 1){
				newElement->end = i;
			}
		}
	}

	return ptr_head;
}

NPtr findEntry(NPtr ptr_head, int val){
	if (ptr_head == NULL) return NULL;
	while (ptr_head->value != val){
		ptr_head = ptr_head->next;
	}
	return ptr_head;
}

// Ausgabe des Baumes
void out(NPtr ptr_tree, int stufe){
	if (ptr_tree == NULL) return;
	printf("Stufe: %d: %d\n", stufe, ptr_tree->value);
	EPtr tmpOutEdges = ptr_tree->edges;
	while (tmpOutEdges->id != NULL){
		out(tmpOutEdges->id, stufe + 1);
		tmpOutEdges = tmpOutEdges->next;
	}

	//printf("Wurzel: %d\n", ptr_tree->value);
}

void newEdge(NPtr* startNode, NPtr* endNode){
	EPtr newEdge = (EPtr) malloc(sizeof(edge));
	newEdge->next = NULL;
	newEdge->id = (*endNode);

	EPtr lastEdgeOfStartNode = (*startNode)->edges;
	while (lastEdgeOfStartNode != NULL){
		lastEdgeOfStartNode = lastEdgeOfStartNode->next;
	}

	lastEdgeOfStartNode = newEdge;
}

// MAIN ////////////////////////////////
int main(){
	// Code per Zufall erstellen
	time_t t;
	time(&t);
	srand((unsigned int)t);
	int codeArray[8];
	for (int i = 0; i < 8; i++){
		codeArray[i] = rand() % 10;
	}																			// code = [x,x,x,x,x,x,x,x]
	
	// Hilfsarray erstellen:
	int n = sizeof(codeArray)/sizeof(int);
	int helpArray[n];
	makeHelpArray(codeArray, helpArray);										// help = [y,y,y,y,y,y,y,y]


	// Liste mit allen Kanten erstellen:
	ptr_edgeList allEdges = makeEdgeList(codeArray, helpArray);					// allEdges = {{x,y}, {x,y}, ...}

	
	//	Ausgabe der Liste aller Kanten:
	ptr_edgeList tmpOutEdges = allEdges;
	while (tmpOutEdges != NULL){
		printf("start: %d end: %d\n", tmpOutEdges->start, tmpOutEdges->end);
		tmpOutEdges = tmpOutEdges->next;
	}

	// Graphen erstellen
	NPtr newTree, lastTree;
	NPtr ptr_graph = (NPtr) malloc(sizeof(node));
	ptr_graph->next = NULL;
	ptr_graph->edges = NULL;
	lastTree = ptr_graph;

	for (int i = 1; i < n+2; i++){												// Liste aller Knoten mit den Werten des Alphabetes
		newTree = (NPtr) malloc(sizeof(node));									// [0]->[1]->[2]-> ...
		newTree->edges = (EPtr) malloc(sizeof(edge));
		newTree->value = i;
		newTree->count = 0;
		newTree->next = NULL;
		lastTree->next = newTree;
		lastTree = newTree;
	}

	// jedem Knoten seine Kanten zuordnen
	NPtr ptr_throughNodes;
	EPtr lastEdge; //newEdge, 
	ptr_edgeList tempEdgeList = allEdges;

	ptr_throughNodes = ptr_graph;

	ptr_throughNodes->edges = (EPtr) malloc(sizeof(edge));
	ptr_throughNodes->edges->next = NULL;
	lastEdge = ptr_throughNodes->edges;


	EPtr tempEdge;
	for (int i = 0; i < n+1; i++){
		NPtr startNode = findEntry(ptr_graph, tempEdgeList->start);				// Start und End-Knoten bestimmen
		NPtr endNode = findEntry(ptr_graph, tempEdgeList->end);
		tempEdge = (EPtr) malloc(sizeof(edge));
		tempEdge->id = endNode;
		tempEdge->next = startNode->edges;
		startNode->edges = tempEdge;
		endNode->count = endNode->count + 1;

		tempEdgeList = tempEdgeList->next;
	}


	//Wurzel finden --> ptr_tree zeigt auf Wurzel:
	NPtr ptrTmp = ptr_graph;
	NPtr ptr_tree;										// Pointer, der auf die Wurzel des Baumes zeigt
	while (ptrTmp != NULL){
		if (ptrTmp->count == 0){
			ptr_tree = ptrTmp;							// Baum-Zeiger umbiegen
			break;
		}
		ptrTmp = ptrTmp->next;
	}


	// Testen, ob ein richtiger Baum entstanden ist
	printf("out:\n");
	out(ptr_tree, 0);


	free(ptr_graph);
	ptr_graph = NULL;
	
	return 0;
}