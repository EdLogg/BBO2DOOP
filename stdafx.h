// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <strsafe.h>
#pragma comment(lib, "User32.lib")
#include "hpdf.h"				// PDF definitions
#include <setjmp.h>				// used by PDF routines


#define MAX_HANDS		12		// max hands we can deal with
#define MAX_RESULTS		64		// max number of results
#define MAX_STRING		32
#define INVALID			1000000	// invalid score

enum VUL
{
	VUL_NONE = 0,
	VUL_NS,
	VUL_EW,
	VUL_BOTH
};

enum SUIT
{
	NOSUIT = -1,
	CLUB = 0,
	DIAMOND,
	HEART,
	SPADE,
	NOTRUMP,
	SUITS
};

enum HAND
{
	NORTH = 0,
	EAST,
	SOUTH,
	WEST,
	HANDS
};

enum CARDS
{
	CARD_2 = 2,
	CARD_3,
	CARD_4,
	CARD_5,
	CARD_6,
	CARD_7,
	CARD_8,
	CARD_9,
	CARD_T,
	CARD_J,
	CARD_Q,
	CARD_K,
	CARD_A,
	CARDS
};

enum DOUBLED
{
	NODOUBLE = 0,
	ONEDOUBLE,
	REDOUBLE
};

struct Result
{
	int		score;						// score based on NS
	int		count;						// number of these scores
	double	percent;					// percentage for NS
};

struct HandInfo
{
	bool	validHand;					// if the hand info is valid
	int		first;						// which player bids first
	VUL		vul;						// which side is vulnerable
	int		cardMask[4][4];				// mask of cards per player for each suit
	// results
	bool	validResults;				// if the results are valid 
	int		resultCount;				// total number of results 
	int		resultsEntries;				// number of entries below
	Result	results[MAX_RESULTS];		// the results
};


extern bool		simpleResults;
extern HandInfo	handInfo[MAX_HANDS];
extern bool		playedNS;
extern double	myResult;
extern double	myResultSoFar;
extern int		myResultCounted;
extern int		myResultCount;
extern char 	myName[MAX_STRING];
extern int		mySession;
extern int		numResults;
extern char		playedDate[MAX_STRING];
extern char		playedTime[MAX_STRING];
extern int		points[SUITS];
extern int		voids[SUITS];
extern int		singletons[SUITS];
extern int		longSuits[SUITS];
extern char		Cards[13];

bool PDFPrintHands(char * folder); 
bool PDFPrintResults(char * folder);

HandInfo * LinFile(FILE * file);
HandInfo * ResultsFile(FILE * file, WCHAR * name);