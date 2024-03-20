/*******************************************************************************************************************************
Hashtable Implementation (STsize = 1000)
Programmer : Kang Mina, Lim Chaemin, Lim Eunjee, Nafisa
Date: 3/ 20/ 2024

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
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#define FILE_NAME "noError1.txt" //name of test data file(.txt format) to run

#define STsize 30 //size of string table
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

int nextid = 0;		// current identifier
int nextfree = 0;	// the next available index of ST
int hashcode;		// hash code of identifier
int sameid;		// first index of identifier

int found; //for the previos occurence of identifier

ERRORtypes err;
FILE* fp; // to be a pointer to FILE
char input;

char buffer; //save invalid seperators

//PrintTeam - Print a team members.
void PrintTeam() {
	printf("\n[[ 컴파일러 6조 ]]");
	printf("\n 1976002 강민아, 1976333 임채민, 1985086 임은지, 1971091 Nafisa\n\n");
}

//PrintHeading - Print heading
void PrintHeading() {
	printf("\n [[ CURRENT FILE ]]\n %s\n\n\n\n", FILE_NAME);
	printf(" [[ STRING TABLE ]]\n");
	printf(" -------------- ------------\n");
	printf(" Index in ST	identifier\n");
	printf(" -------------- ------------\n");
}

//Initialize - open input file
// read one character from file
void initialize()
{
	fp = fopen(FILE_NAME, "r");
	input = fgetc(fp);
}

// IsSeperators - distinguish the seperator
int IsSeperators(char c)
{
	int sep_len;

	sep_len = strlen(seperators);
	for (int i=0; i < sep_len; i++) {
		if (c == seperators[i]) {
			return 1; // valid seperators
		} 
	}

	return 0; // invalid seperators + number + letter
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
	printf("\n < %5d characters are used in the string table     > \n", nextfree);
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
	case overst: // overflow
		printf("...ERROR...		OVERFLOW\n");
		PrintHStable();
		PrintTeam();
		exit(0);
		break;
	case illid: // start with digit
		printf(" ...ERROR...\t");
		while (input != EOF && (isLetter(input) || isDigit(input))) {
			printf("%c", input);
			input = fgetc(fp);
		}
		printf("\t\tstart with digit \n");
		break;
	case illsp: // invalid seperators
		printf(" ...ERROR...\t");
		while (input != EOF && (isLetter(input) || isDigit(input))) {
			input = fgetc(fp);
		}
		printf("%.*s", nextfree - nextid, &ST[nextid]);
		printf("\t%c is not allowed \n", buffer); // print invalid seperator
		break;
	case toolong: // too long string
		printf(" ...ERROR...\t");
		while (input != EOF && !(IsSeperators(input)) && (isLetter(input) || isDigit(input))) {
			ST[nextfree++] = input;
			input = fgetc(fp);
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
		if (!IsSeperators(input)) { // invalid seperator
			err = illsp;
			break;
		}
		input = fgetc(fp);
	}
}

// ReadIO - Read identifier from the input file the string table ST directly into
// ST(append it to the previous identifier).
// An identifier is a string of letters and digits, starting with a letter.
// If first letter is digit, print out error message.

void ReadID() {
	nextid = nextfree;
	if (isDigit(input)) {
		err = illid;
		PrintError(err);
	}
	else {
		while (input != EOF && !IsSeperators(input)) {
			if (isDigit(input) || isLetter(input)) {
				if (nextfree == STsize) { //오버플로우 체크
					nextfree = nextid;
					err = overst;
					PrintError(err);
				}
				ST[nextfree++] = input;
			}
			else {
				err = illsp;
				buffer = input;
				ST[nextfree++] = input;
			}
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
// If list head is not a null , it adds a new identifier to the head of the chain
void ADDHT(int hscode)
{
	HTpointer ptr;

	ptr = (HTpointer)malloc(sizeof(HTentry));
	ptr->index = nextid;
	ptr->next = HT[hscode];
	HT[hscode] = ptr;
}

/*
MAIN - Read the identifier from the file directly into ST.
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

			if (nextfree == STsize) { // Check overflow
				err = overst;
				PrintError(err);
			}
			if (nextfree - nextid > 12) { // Check if identifier is too long
				err = toolong;
				PrintError(err);
				nextfree = nextid;
				continue; 
			}
			if (err == illsp) { // Check invalid seperator
				PrintError(err);
				nextfree = nextid;
				continue;
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