/*******************************************************************************************************************************
Hashtable Implementation (STsize = 1000)
Programmer : Kang Mina, Lim Chaemin, Lim Eunjee, Nafisa
Date: 3/ 18/ 2024

Description :
The input to the program is a file , consisting of identifiers seperated by
spaces,tab characters, newlines and punctuation marks . , , , ; , :, ? , ! .
An identifier is a string of letters and digits,starting with a letter.Case is insignificant.
:
If the ST overflows,prints the hashtable as above and abort by calling
the function "exit()".

Input :
A file consisting of identifiers seperated by spaces, tab characters,newlines and
punctuation marks.
An identifier is a string of letters and digits,starting with a letter.

Output :
The identifier,its index in the stringtable and whether entered or present.
Prints error message for illegal identifier(starting with a digit),illegal seperator and

Restriction:
If the ST overflows, print the hashtable as above, and abort by calling
the function "exit()". "exit()" terminates the execution of a program.

Global variables :
ST - Array of string table
HT - Array of list head of hashtable
letters - Set of letters A..Z, a..z, _
digits - Set of digits 0..9
seperators - null , . ; : ? ! \t \n
*****************************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable:4996)

#define FILE_NAME "testdata44.txt" //name of test data file(.txt format) to run

#define STsize 1000 //size of string table
#define HTsize 100 //size of hash table

#define FALSE 0
#define TRUE 1

#define isLetter(x) ( ((x) >= 'a' && (x) <= 'z') || ((x) >= 'A' && (x) <= 'Z') || ((x) == '_'))
#define isDigit(x) ( (x) >= '0' && (x) <= '9' )

typedef struct HTentry *HTpointer;
typedef struct HTentry {
	int index; //index of identifier in ST
	HTpointer next; //pointer to next identifier
} HTentry;

enum errorTypes { noerror, illsp, illid, overst, toolong};
typedef enum errorTypes ERRORtypes;

char seperators[] = " .,;:?!\t\n";	

HTpointer HT[HTsize];
char ST[STsize];

int nextid = 0;		//current identifier
int nextfree = 0;	//the next available index of ST
int hashcode;		//hash code of identifier
int sameid;		//first index of identifier

int found; //for the previos occurence of identifier

ERRORtypes err;
FILE* fp; //to be a pointer to FILE
char input;

//PrintHeading - Print heading
void PrintHeading() {
	printf("\n[[ CURRENT FILE ]]\n %s\n\n\n", FILE_NAME);
	printf(" [[ STRING TABLE ]]\n");
	printf(" -------------- ------------\n");
	printf(" Index in ST	identifier\n");
	printf(" -------------- ------------\n");
}

//Initialize - open input file
//read one character from file
void initialize()
{
	fp = fopen(FILE_NAME, "r");
	input = fgetc(fp);
}

//IsSeperators - distinguish the seperator
int IsSeperators(char c)
{
	int sep_len;

	sep_len = strlen(seperators);
	for (int i=0; i < sep_len; i++) {
		if (c == seperators[i])
			return 1; // valid seperators 
	}
	return 0; // invalid seperators
}

// PrintHStable - Prints the hash table.write out the hashcode and the list of identifiers
// associated with each hashcode, but only for non-empty lists.
// Print out the number of characters used up in ST.
void PrintHStable()
{
	int i, j;
	HTpointer here;

	printf("\n\n\n [[ HASH TABLE ]] \n\n");

	for (i = 0; i < HTsize; i++)
		if (HT[i] != NULL) {
			printf(" Hash Code %3d: ", i);
			for (here = HT[i]; here != NULL; here = here->next) {
				j = here->index;
				while (ST[j] != '\0' && j < STsize)
					printf("%c", ST[j++]);
				printf("   ");
			}
			printf("\n");
		}
	printf("\n < %5d characters are used in the string table > \n", nextfree);
}

// PrintError - Print out error messages
// overst : overflow in ST
//			print the hashtable and terminate by calling the function "exit(0)".
// illid : illegal identifier
// illsp :illegal seperator
// toolong : length of identifier is over 12
void PrintError(ERRORtypes err)
{
	switch (err) {
	case overst:
		printf("...ERROR...		OVERFLOW\n");
		PrintHStable();
		exit(0);
		break;
	case illid:
		printf(" ...ERROR...\t");
		while (input != EOF && (isLetter(input) || isDigit(input))) {
			printf("%c", input);
			input = fgetc(fp);
		}
		printf("\t\tstart with digit \n");
		break;
	case illsp:
		printf(" ...ERROR...\t");
		while (input != EOF && (isLetter(input) || isDigit(input))) {
			input = fgetc(fp);
		}
		printf("%c", input);
		printf("\t\t%c is not allowed \n", input);
		break;
	case toolong:
		printf(" ...ERROR...\t");
		int length = 12;
		int i = 0;
		while (input != EOF && !(IsSeperators(input)) && (isLetter(input) || isDigit(input))) {
			ST[nextfree++] = input;
			input = fgetc(fp);
			length++;
		}
		printf("%.*s", nextfree - nextid, &ST[nextid]);
		printf("\ttoo long identifier\n");
		break;
	}
}

// Skip Seperators - skip over strings of spaces,tabs,newlines, . , ; : ? !
// use seperators[]
// if illegal seperators, print out error message.
void SkipSeperators()
{
	while (input != EOF && !(isLetter(input) || isDigit(input))) {
		//if EOF도 아니고 letters와 digit도 아닐 때
		if (!IsSeperators(input)) {
			//
			err = illsp;
			PrintError(err);
		}
		input = fgetc(fp);
	}
}

//ReadIO - Read identifier from the input file the string table ST directly into
// ST(append it to the previous identifier).
// An identifier is a string of letters and digits, starting with a letter.
// If first letter is digit, print out error message.
void ReadID()
{
	nextid = nextfree;
	int length = 0;

	if (isDigit(input)) { //숫자로 시작하는지 체크
		err = illid;
		PrintError(err);
	}
	else {
		while (input != EOF && (isLetter(input) || isDigit(input))) { //valid character
			if (nextfree == STsize) { //overflow
				err = overst;
				PrintError(err);
			}
			ST[nextfree++] = input;
			length++;
			input = fgetc(fp);
		}
	}
}

// ComputeHS - Compute the hash code of identifier by summing the ordinal values of its
// characters and then taking the sum modulo the size of HT.
void ComputeHS(int nid, int nfree)
{
	int code, i;
	code = 0;
	for (i = nid; i < nfree - 1; i++)
		code += (int)ST[i];
	hashcode = code % HTsize;
}

// LookupHS -For each identifier,Look it up in the hashtable for previous occurrence
// of the identifier.If find a match, set the found flag as true.
// Otherwise false.
// If find a match, save the starting index of ST in same id. 
void LookupHS(int nid, int hscode)
{
	HTpointer here;
	int i, j;

	found = FALSE;
	if (HT[hscode] != NULL) {
		here = HT[hscode];
		while (here != NULL && found == FALSE) {
			found = TRUE;
			i = here->index;
			j = nid;
			sameid = i;

			while (ST[i] != '\0' && ST[j] != '\0' && found == TRUE) {
				if (ST[i] != ST[j])
					found = FALSE;
				else {
					i++;
					j++;
				}
			}
			here = here->next;
		}
	}
}

// ADDHT - Add a new identifier to the hash table.
// If list head ht[hashcode] is null, simply add a list element with
// starting index of the identifier in ST.
// IF list head is not a null , it adds a new identifier to the head of the chain
void ADDHT(int hscode)
{
	HTpointer ptr;

	ptr = (HTpointer)malloc(sizeof(HTentry));
	ptr->index = nextid;
	ptr->next = HT[hscode];
	HT[hscode] = ptr;
}

//PrintTeam - Print a team members.
void PrintTeam() {
	printf("\n\n\n [[ 컴파일러 6조 ]]");
	printf("\n 1976002 강민아, 1976333 임채민, 1985086 임은지, 1971091 Nafisa\n\n");
}

/*
*  MAIN - Read the identifier from the file directly into ST.
Compute its hashcode.
Look up the idetifier in hashtable HT[hashcode]
If matched, delete the identifier from ST and print ST - index
of the matching identifier.
If not matched, add a new element to the list, pointing to new identifier.
Print the identifier, its index in ST, and whether it was (entered) or (already existed) or errortype.
Print out the hashtable, and number of characters used up in ST
*/
int main()
{
	int i;
	PrintHeading();
	initialize();

	while (input != EOF) {
		err = noerror;
		SkipSeperators();
		ReadID();
		if (input != EOF && err != illid) {
			if (nextfree == STsize) {
				err = overst;
				PrintError(err);
			}
			if (nextfree - nextid > 12) { // Check if identifier is too long
				err = toolong;
				PrintError(err);
				nextfree = nextid; // Reset nextfree to ignore the too long identifier
				continue; // Skip adding to hash table
			}
			ST[nextfree++] = '\0';

			ComputeHS(nextid, nextfree);
			LookupHS(nextid, hashcode);

			if (!found) {
				printf("  %6d\t", nextid);
				for (i = nextid; i < nextfree - 1; i++)
					printf("%c", ST[i]);
				printf("\t\t(entered)\n");
				ADDHT(hashcode);
			}
			else {
				printf("  %6d\t", sameid);
				for (i = nextid; i < nextfree - 1; i++)
					printf("%c", ST[i]);
				printf("\t\t(already existed)\n");
				nextfree = nextid;
			}
		}
	}
	PrintHStable();
	PrintTeam();

	return 0;
}