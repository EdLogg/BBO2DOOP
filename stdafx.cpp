#include "stdafx.h"

bool		simpleResults;				// if true the results use a larger font and use whole percentage values

HandInfo	handInfo[MAX_HANDS];
bool		playedNS;
double		myResult;
double		myResultSoFar;				// results taken from lines with our name
int			myResultCounted;			// number of times our name has appeared
int			myResultCount;
char		myName[MAX_STRING];
int			mySession;
int			numResults;
int			numResultSoFar;
int			numResultCount;
char		playedDate[MAX_STRING];
char		playedTime[MAX_STRING];
int			points[SUITS];
int			voids[SUITS];
int			singletons[SUITS];
int			longSuits[SUITS];
