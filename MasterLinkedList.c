/*****************************************************************************************
*			This program was written by Emre YнанT and it is an example of a BOW.		 *
*		It simply navigates within 3 specific folders "econ, health, magazin".			 *
*		Next it takes the terms in text files in these folders as input.				 *
*		And then insert them to a MLL (master linked list)								 *
******************************************************************************************/

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

struct downNode{
	char docID[8];
	int totalTimeofTerm;	//Total number of times this term occurs in this document
	int lengthDoc;			//Length of the document (total number of words in the document)
	char categoryName[8];
	struct downNode *nextDown;
};
typedef struct downNode downNode;

struct node{
	char termName[26];
	int totalDocNum;	// Total number of documents this term occur
	struct node *nextTerm;
	downNode *downTerm;
};
typedef struct node node;

struct stopWords{		// this struct for the stop words/general words
	char termName[26];
	struct stopWords *nextWord;
};
typedef struct stopWords stopWords;

struct discWord{		// these following two structs for the discriminant words
	char termName[26];
	struct discWord *nextDiscWord;
};
typedef struct discWord discWord;

struct discCat{
	char categoryName[8];
	struct discCat *nextCat;
	discWord *downDiscWord;
};
typedef struct discCat discCat;

char *TurkishCharsFunc(char aTerm[]);
int totalDoc(char termName[]);
int isAscending(char arr[], char arr2[]);
int insert(node ** header, char wordToAdd[], char pathForDown[], int lengthofDoc, char category[]);
int insertDown(node *header, char whichDoc[], int lengthOfDoc, char category[]);
int stopWordsFunc(downNode *hdrDown);
int insertStopWord(stopWords ** header, char wordToAdd[]);
char *discWordFunc(downNode *hdrDown);
int insertCat(discCat **header, char category[]);

int main(void){
	discCat *hdrCat, *pCat, *iterCat, *iterCat2;
	hdrCat = NULL;
	iterCat = NULL;
	discWord *hdrWord;
	hdrWord = NULL;
	
	stopWords *hdrStop, *pStop;
	hdrStop = NULL;
	
	node *hdr, *iter, *iter2;
	hdr = NULL;
	iter = NULL;
	iter2 = NULL;
	
	int counter = 0;
	int i = 0;
	char aTerm[26];
	int c;
	int d;
	char path[17];
	char category[8];
	int lengthOfDoc = 0;
	FILE *fp1;
	FILE *fp2;
	
	struct dirent *drPtr;  // Pointer for directory entry 
 	DIR *director;
 	
    for(i = 0; i < 3; i++){
		if(i == 0)		//Firstly, the code will enter the econ folder. Then, the others
		    director = opendir("econ"); // opendir() returns a pointer of DIR type.  
		if(i == 1)
			director = opendir("health");
		if(i == 2)
			director = opendir("magazin");

    	if (director == NULL){  // opendir returns NULL if couldn't open directory
       		printf("Could not open current directory" ); 
       		return 0; 
   		}
		while ((drPtr = readdir(director))!= NULL){
			if(counter == 2) {		// This if statement to ignore "." and ".." 
				if(i == 0){		// if 'i' is equal to 0, then we work on the econ folder
					strcpy(category,"econ");
					strcpy(path,"econ\\");
				}
				if(i == 1){
					strcpy(category,"health");
					strcpy(path,"health\\");
				}
				if(i == 2){
					strcpy(category,"magazin");
					strcpy(path,"magazin\\");
				}
				strcat(path,drPtr->d_name);
		    	lengthOfDoc = 0;
    			fp2 = fopen(path,"r");
    			do {				// This do-while loop to calculate the length of the text file
    				d = fscanf(fp2,"%s",aTerm);
    				lengthOfDoc++;
   				} while (d != EOF);
   				lengthOfDoc--;//I wrote this line because I checked the condition in the loop after increment of lengthOfDoc variable
				fp1 = fopen(path,"r");
    			do {	//This do-while loop take an term as input from the file and insert it to the linked list
    				c = fscanf(fp1,"%s",aTerm);
   					strcpy(aTerm,TurkishCharsFunc(aTerm));	//this function editing Turkish characters
    				if (c != EOF)
    					insert(&hdr, aTerm, drPtr->d_name, lengthOfDoc, category);
   				} while (c != EOF);
    			counter--;
			}
    		counter++;
		}
		fclose(fp1);
    	closedir(director);
		counter = 0;
	}
	iter = hdr;
	iter2 = iter;
	
	int stopWordCounter = 0;
	while(iter != NULL && stopWordCounter != 5){	//this loop determines the stop words
		if(stopWordsFunc(iter->downTerm) == 3){//this function returns 3 if input which parameter of the function is exist in all categories
			insertStopWord(&hdrStop,iter->termName);
			stopWordCounter++;			// it is for first 5 words
		}
		iter = iter->nextTerm;
	}
	i = 1;printf("\n\n\n\t  +++++++StopWords+++++++\n\n");
	while(hdrStop != NULL){									// this loop prints stop words
		printf("\t\tTerm-%d: %s\n",i,hdrStop->termName);
		hdrStop = hdrStop->nextWord;
		i++;
	}
									//these following lines(157-170) are for discriminating category
	hdrCat = (discCat*)malloc(sizeof(discCat));
	hdrCat->nextCat = NULL;
	hdrCat->downDiscWord = NULL;
	strcpy(hdrCat->categoryName,"econ");
	for(i = 0; i < 2; i++){				// this loop creates a linked list for categories of discriminant words
		pCat = (discCat*)malloc(sizeof(discCat));
		if(i == 0){
			hdrCat->nextCat = pCat;
			hdrCat->nextCat->nextCat = NULL;
			hdrCat->nextCat->downDiscWord = NULL;
			strcpy(hdrCat->nextCat->categoryName,"health");
		}
		else if(i == 1){
			hdrCat->nextCat->nextCat = pCat;
			hdrCat->nextCat->nextCat->nextCat = NULL;
			hdrCat->nextCat->nextCat->downDiscWord = NULL;
			strcpy(hdrCat->nextCat->nextCat->categoryName,"magazin");
		}
	}
	int discWordCounterEcon = 0;
	int discWordCounterHealth = 0;
	int discWordCounterMagazin = 0;
	int discWordCounterTotal = 0;
	while(iter2 != NULL && discWordCounterTotal != 15){			//this loop determines the discriminant words
			
		if(strcmp(discWordFunc(iter2->downTerm), "econ") == 0 && discWordCounterEcon != 5){	//	if return of the discWordFunc() is "econ" then we see 
			insertDiscWord(hdrCat, iter2->termName);										//the term which parameter of the func is only exist in econ category
			discWordCounterEcon++;															// and the number of 5 is to limit only 5 words
		}
		else if(strcmp(discWordFunc(iter2->downTerm), "health") == 0 && discWordCounterHealth != 5){
			insertDiscWord(hdrCat->nextCat, iter2->termName);
			discWordCounterHealth++;
		}
		else if(strcmp(discWordFunc(iter2->downTerm), "magazin") == 0 && discWordCounterMagazin != 5){
			insertDiscWord(hdrCat->nextCat->nextCat, iter2->termName);
			discWordCounterMagazin++;
		}
		
		discWordCounterTotal = discWordCounterMagazin + discWordCounterEcon+ discWordCounterHealth;	// whenever the total is 15, then the loop ends.
		iter2 = iter2->nextTerm;
	}i = 0;
	iterCat = hdrCat;
	iterCat2 = iterCat;
	printf("\n\n\n\t+++++++Discriminating Words+++++++\n\n");
	while(hdrCat != NULL){												// this loop prints category of the discriminating words
		if(i == 0)
			printf("%-20s", hdrCat->categoryName);
		if(i != 0)
			printf("%s%-20s","|",hdrCat->categoryName);
		i++;
		hdrCat = hdrCat->nextCat;
	}printf("\n-------------------------------------------------\n");
	i = 0;
	while(iterCat->downDiscWord != NULL){								// this loop prints the discriminating words
		while(iterCat != NULL){
			if(i == 0)
				printf("%-20s",iterCat->downDiscWord->termName);
			if(i != 0)
				printf("%s%-20s", "|", iterCat->downDiscWord->termName);
			iterCat = iterCat->nextCat;
			i++;
		}printf("\n");
		i = 0;
		iterCat = iterCat2;
		while(iterCat != NULL){
			iterCat->downDiscWord = iterCat->downDiscWord->nextDiscWord;
			iterCat = iterCat->nextCat;
		}
		iterCat = iterCat2;
	}printf("\n\n\n       +++++++Words in The Linked List+++++++\n\n");
	while(hdr != NULL){														// this loop prints the words in the linked list
		printf("%-15s	",hdr->termName);
		/*i = 0;
		while(hdr->downTerm != NULL){										// this loop to print all of content of the MLL
			if(i == 0)
				printf("%d	%d	%s	%s	%d\n",hdr->downTerm->totalTimeofTerm,hdr->totalDocNum, hdr->downTerm->categoryName, hdr->downTerm->docID, hdr->downTerm->lengthDoc);
			if(i != 0)
				printf("		%d	%d	%s	%s	%d\n",hdr->downTerm->totalTimeofTerm,hdr->totalDocNum, hdr->downTerm->categoryName, hdr->downTerm->docID, hdr->downTerm->lengthDoc);
			hdr->downTerm = hdr->downTerm->nextDown;
			i++;
		}*/
		printf("\n");
		hdr = hdr->nextTerm;
		counter++;
	}
	printf("%d",counter);	// total number of words in the linkedlist
    return 0; 
}
int insertDiscWord(discCat *header, char wordToAdd[]){		//this function basically inserts discriminating words to the linked list
	discWord *pWord, *qWord;
	pWord = (discWord*)malloc(sizeof(discWord));
	strcpy(pWord->termName,wordToAdd);
	pWord->nextDiscWord = NULL;
	
	if(header->downDiscWord == NULL)
		header->downDiscWord = pWord;
	else{
		qWord = header->downDiscWord;
		while(qWord->nextDiscWord != NULL)
			qWord = qWord->nextDiscWord;
		qWord->nextDiscWord = pWord;
	}
	return 1;
}
char *discWordFunc(downNode *hdrDown){		// this function determines whether the word is discriminating word
	int x = 0;
	int y = 0;
	int z = 0;
	
	while(hdrDown != NULL){
		if(strcmp(hdrDown->categoryName,"econ") == 0)		// if category of the term is econ then x will be 1
			x = 1;
		else if(strcmp(hdrDown->categoryName,"health") == 0)
			y = 1;
		else if(strcmp(hdrDown->categoryName,"magazin") == 0)
			z = 1;
		hdrDown = hdrDown->nextDown;
	}
	if(x == 1 && y == 0 && z == 0)						// if x is 1 and the others are 0 then we see this term a discriminating word for econ category
		return "econ";
	else if(x == 0 && y == 1 && z == 0)
		return "health";
	else if(x == 0 && y == 0 && z == 1)
		return "magazin";
	
	return "";
}
int stopWordsFunc(downNode *hdrDown){				// this function determines whether the word is stop word
	int x = 0;
	int y = 0;
	int z = 0;
	while(hdrDown != NULL){
		if(strcmp(hdrDown->categoryName,"econ") == 0)		// if the term is exist in econ category then x will be 1
			x = 1;
		else if(strcmp(hdrDown->categoryName,"health") == 0)
			y = 1;
		else if(strcmp(hdrDown->categoryName,"magazin") == 0)
			z = 1;
		hdrDown = hdrDown->nextDown;
	}
	int totalXYZ = x+y+z;									// if the total is 3 then we see this term is a stop word because that is exits in all categories
	return totalXYZ;
}
int insertStopWord(stopWords ** header, char wordToAdd[]){		// this function basically inserts stopwords to the linked list
	stopWords *pStop, *qStop;
	pStop = (stopWords*)malloc(sizeof(stopWords));
	strcpy(pStop->termName, wordToAdd);
	pStop->nextWord = NULL;
	if(*header == NULL)
		*header = pStop;
	else{
		qStop = *header;
		while(qStop->nextWord != NULL)
			qStop = qStop->nextWord;
		qStop->nextWord = pStop;
	}
	return 1;
}
int isAscending(char arr[], char arr2[]){						// this function helps to put words in alphabetical order
	int i = 0;
	int len = 0;
	for(i = 0; i < 27; i++){					// firstly we learn length of the short term
		if(arr[i] != '\0' && arr2[i] != '\0')
			len++;
		else
			break;
	}
	for(i= 0; i < len; i++){					// this loop determines the order of words
		if (arr[i] == arr2[i]){
			if(i == len-1 && arr2[i+1] > arr[i+1])		//arr = ali , arr2 = aliye
				return 1;
			else if(i == len-1 && arr2[i+1] < arr[i+1])		//arr = aliye , arr2 = ali
				return 0;
			else if(i == len-1 && arr2[i+1] == arr[i+1])		//the words are same
				return 0;
			continue;
		}
		else if (arr2[i] > arr[i])
			return 1;
		else if (arr2[i] < arr[i])
			return 0;
	}
}
int insertDown(node *header, char whichDoc[], int lengthOfDoc, char category[]){	// this function inserts down node to the MLL
	downNode *hdrDown, *pDown, *qDown;
	hdrDown = (downNode*)malloc(sizeof(downNode));
	hdrDown->nextDown = NULL;
	strcpy(hdrDown->docID,whichDoc);
	strcpy(hdrDown->categoryName,category);
	hdrDown->lengthDoc = lengthOfDoc;
	
	if(header->downTerm == NULL){				// there is no down node under the basic node which contain the terms
		header->downTerm = hdrDown;
		hdrDown->totalTimeofTerm = 1;			// when first down node is inserted, the totalTimeofTerm variable will be 1
	}
	else{
		pDown = header->downTerm;				//this if statement is for first down node
		if(strcmp(pDown->categoryName,hdrDown->categoryName) == 0 && strcmp(pDown->docID,hdrDown->docID) == 0)
			pDown->totalTimeofTerm++;			// if category name and document ID are same as the new term then the totalTimeofTerm will be increase
		while(pDown->nextDown != NULL){
			pDown = pDown->nextDown;			// this if statement is for the other down nodes
			if(strcmp(pDown->categoryName,hdrDown->categoryName) == 0 && strcmp(pDown->docID,hdrDown->docID) == 0)
				pDown->totalTimeofTerm++;
		}
		if(strcmp(pDown->categoryName,hdrDown->categoryName) != 0 || strcmp(pDown->docID,hdrDown->docID) != 0){
			pDown->nextDown = hdrDown;
			header->totalDocNum++;				//if the category name or document ID or both are not same as the new term then the totalDocNum will be increase
			hdrDown->totalTimeofTerm = 1;		//when second or other down nodes are inserted, the totalTimeofTerm variable starts with 1
		}
	}
	return 1;
}
int insert(node ** header, char wordToAdd[], char pathForDown[], int lengthofDoc, char category[]){		//this function inserts node for each term to the MLL
	node *p,*q,*r;
	// create node to insert and assign values to its fields
	p=(node*)malloc(sizeof(node));
	strcpy(p->termName,wordToAdd);
	p->nextTerm = NULL;
	p->downTerm = NULL;
	p->totalDocNum = 1;				// if a term was taken as input by this function then this term is exists at least in one document
	// if LL empty
	if (*header == NULL)			// if there is no node in MLL 
   		*header=p;
	// if LL not empty
	else {
		q=*header;
    	while (q != NULL && isAscending(q->termName,p->termName)) {	// this loop finds the location of the term in alphabetical order
    		r=q;
			q=q->nextTerm;
		}
		if (q!=NULL && strcmp(q->termName,p->termName) == 0){	// the words are same
			insertDown(q, pathForDown, lengthofDoc, category);	//if the words are same then I call the insertDown() func
			return 0;											//because the terms can be from different documents
		} 										//return 0, don't insert the term to LL	
		// insert node in a sorted fashion
		if (q!=NULL) 
			p->nextTerm=q;
		// if value to insert is the least in LL then have header point to node
		if (q==*header) 
			*header=p;
		// otherwise insert node in correct position
		else 
			r->nextTerm=p;	 
	}
	insertDown(p, pathForDown, lengthofDoc, category);
	return 1;
}
char *TurkishCharsFunc(char aTerm[]){		//this function basically editing Turkish characters. it is completely hard coding :)
	int i;
	for(i = 0; i < 27; i++){
      	if (aTerm[i] == '§')
      		aTerm[i] = 'i';
      	if (aTerm[i] == 'н')
      		aTerm[i] = 'i';
      	if (aTerm[i] == 'ч')
      		aTerm[i] = 'c';
      	if (aTerm[i] == 'Ч')
      		aTerm[i] = 'C';
      	if (aTerm[i] == '№')
      		aTerm[i] = 'g';
      	if (aTerm[i] == 'а')
      		aTerm[i] = 'G';
      	if (aTerm[i] == 'і')
      		aTerm[i] = 'o';
      	if (aTerm[i] == 'ж')
      		aTerm[i] = 'O';
      	if (aTerm[i] == 'ў')
      		aTerm[i] = 's';
      	if (aTerm[i] == 'о')
      		aTerm[i] = 'S';
      	if (aTerm[i] == 'ќ')
      		aTerm[i] = 'u';
      	if (aTerm[i] == 'м')
      		aTerm[i] = 'U';
	}
	return aTerm;
}
