#include "stdafx.h"

char Suits[SUITS] = { 'C', 'D', 'H', 'S', 'N' };
char * 	Doubles[3] = { "", "x", "xx" };
char * Players[4] = { "North", "East", "South", "West" };
char Cards[13] = { '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A' };


void PrintCards(int cardMask)
{
	for (int i = 12, mask = 0x4000; i >= 0; i--, mask >>= 1)
	{
		if ((mask & cardMask) != 0)
		{
			printf("%c", Cards[i]);
		}
	}
	printf("\n");
}


void PrintHand(HandInfo * hand, int player)
{
	printf("  %s:\n", Players[player]);
	printf("    S:");
	PrintCards(hand->cardMask[player][SPADE]);
	printf("    H:");
	PrintCards(hand->cardMask[player][HEART]);
	printf("    D:");
	PrintCards(hand->cardMask[player][DIAMOND]);
	printf("    C:");
	PrintCards(hand->cardMask[player][CLUB]);
}


int CountMask(int mask)
{
	int count = 0;
	while (mask != 0)
	{
		if ((mask & 1) != 0)
			count++;
		mask >>= 1;
	}
	return count;
}


int CountHCP(int mask)
{
	int count = 0;
	if ((mask & (1 << CARD_A)) != 0)
		count += 4;
	if ((mask & (1 << CARD_K)) != 0)
		count += 3;
	if ((mask & (1 << CARD_Q)) != 0)
		count += 2;
	if ((mask & (1 << CARD_J)) != 0)
		count += 1;
	return count;
}


void UpdateStats(HandInfo * hand)
{
	for (int h = 0; h < HANDS; h++)
	{
		for (int s = 0; s < SUITS - 1; s++)
		{
			int c = CountMask(hand->cardMask[h][s]);
			if (c == 0)
				voids[h]++;
			else if (c == 1)
				singletons[h]++;
			else if (c > 6)
				longSuits[h]++;
			points[h] += CountHCP(hand->cardMask[h][s]);

		}
	}
}


int _tmain(int argc, TCHAR *argv[])
{
	WIN32_FIND_DATA ffd;
	TCHAR szDir[MAX_PATH];
	char folder[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	simpleResults = true;							// use whole percentage points and not hundredth of a percentage

	// Directories are specified as command-line arguments.
	int ret = 0;
	for (int i = 1; i < argc; i++)
	{
		// Check that the input path plus 3 is not longer than MAX_PATH.
		// Three characters are for the "\*" plus NULL appended below.
		StringCchLength(argv[i], MAX_PATH, &length_of_arg);
		if (length_of_arg > (MAX_PATH - 3))
		{
			_tprintf(TEXT("\nDirectory path is too long.\n"));
			return (-1);
		}
		_tprintf(TEXT("\nTarget directory is %s\n\n"), argv[i]);

		// Prepare string for use with FindFile functions.  First, copy the
		// string to a buffer, then append '\*' to the directory name.
		folder[MAX_PATH - 1] = 0;
		for (int n = 0; n < MAX_PATH - 1; n++)
		{
			folder[n] = (char)argv[i][n];
			if (folder[n] == 0)
				break;
		}
		StringCchCopy(szDir, MAX_PATH, argv[i]);
		StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

		// Find the first file in the directory.
		hFind = FindFirstFile(szDir, &ffd);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			_tprintf(TEXT("ERROR Processing: %s\n"), ffd.cFileName);
			ret = -1;
			printf("Press return to exit:");
			getchar();
			break;
		}

		// List all the files in the directory and process them
		playedDate[0] = 0;
		playedTime[0] = 0;
		myName[0] = 0;
		numResults = 0;
		mySession = -1;
		myResult = 0.0;
		myResultSoFar = 0.0;
		myResultCounted = 0;
		myResultCount = 0;
		for (int i = 0; i < MAX_HANDS; i++)
		{
			handInfo[i].validHand = false;
			for (int h = 0; h < HANDS; h++)
			{
				for (int s = 0; s < SUITS - 1; s++)
				{
					handInfo[i].cardMask[h][s] = 0;
				}
			}
			handInfo[i].validResults = false;
			handInfo[i].resultCount = 0;
			handInfo[i].resultsEntries = 0;
		}
		for (int n = 0; n < SUITS; n++)				// clear stats
			points[n] = voids[n] = singletons[n] = longSuits[n] = 0;
		do
		{
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			StringCchCopy(szDir, MAX_PATH, argv[i]);
			StringCchCat(szDir, MAX_PATH, TEXT("\\"));
			StringCchCat(szDir, MAX_PATH, ffd.cFileName);
			FILE * file = _tfopen(szDir, TEXT("r"));
			HandInfo * hand;
			_tprintf(TEXT("Processing: %s\n"), ffd.cFileName);
			if ((hand = LinFile(file)) != NULL)
			{
				UpdateStats(hand);
#ifdef _DEBUG
				printf("  Board: %d\n", (int)(hand - handInfo + 1));
				PrintHand(hand, 0);
				PrintHand(hand, 3);
				PrintHand(hand, 1);
				PrintHand(hand, 2);
#endif
			}
			else if ((hand = ResultsFile(file, ffd.cFileName)) != NULL)
			{
#ifdef _DEBUG
				printf("  Board: %d results=%d unique=%d\n", 
					(int)(hand - handInfo + 1), hand->resultCount,
					hand->resultsEntries);
				for (int i = 0; i < hand->resultsEntries; i++)
				{
					printf("    %5d %6.2f%% (%d results)\n",
						hand->results[i].score, hand->results[i].percent,
						hand->results[i].count);
				}
#endif
			}
		} while (FindNextFile(hFind, &ffd) != 0);
		if (myResultCounted != 12)
		{
			printf("\nWARNING: we expected to find 12 results but only found %d results\n\tPerhaps there was an average result reported.\n", 
				myResultCounted);
		}
		else
		{
			double r = myResultSoFar / myResultCounted;
			if (myResult == 0.0)
				myResult = r;
			else if (r < myResult - .02
			|| r > myResult + .02)
			{
				printf("\nWARNING: expected result=%.2f%% but computed result=%.2f%%\n",
					myResult, r);
			}
		}
		if (myResult > 0.0)
			printf("\nPlayed on %s %s  Direction=%s Score=%.2f%%\n", 
				playedDate, playedTime, (playedNS ? "NS" : "EW"), myResult);
		else
			printf("\nPlayed on %s %s  Direction=%s\n",
				playedDate, playedTime, (playedNS ? "NS" : "EW"));

		PDFPrintHands(folder);
		PDFPrintResults(folder);

		printf("Press return to continue:");
		getchar();
	}
	return ret;
}

