#include "stdafx.h"


void SkipTab(FILE * file)
{
	char c;
	do
	{
		c = fgetc(file);
		if (c == EOF
			|| c == '\t')
			return;
	} while (true);
}


void ToLower(char * string)
{
	while (string[0] != 0)
	{
		string[0] = tolower(string[0]);
		string++;
	}
}


HandInfo * ResultsFile(FILE * file, WCHAR * name)
{
	char date[MAX_STRING];
	char time[MAX_STRING];
	char names[4][MAX_STRING];
	char contract[MAX_STRING];
	char prefix[MAX_STRING];
	char direction[MAX_STRING];
	char line[1024];
	char start;
	int num;
	int score;
	double points, percent;

	if (name[0] != 'r'
		|| name[1] != 'e'
		|| name[2] != 's'
		|| name[3] != 'u'
		|| name[4] != 'l'
		|| name[5] != 't')
		return NULL;
	if (name[6] < '0'
		|| name[6] > '9')
		return NULL;
	int board = name[6] - '0';
	if (name[7] != '.')
		board = 10 * board + name[7] - '0';
	if (board < 1 || board > MAX_HANDS)
	{
		printf("ERROR: we can only handle hands 1-%d!!\n", MAX_HANDS);
		return NULL;
	}
	board--;
	HandInfo * hand = &handInfo[board];
	hand->validResults = false;
	// find if EW or NS direction
	do
	{
		int cnt = fscanf(file, "%s", prefix);
		if (cnt <= 0)
		{
			printf("ERROR: We could not find my direction!!\n");
			return NULL;
		}
		if (strcmp(prefix, "Logged") == 0)
		{
			cnt = fscanf(file, "%s %s %s", time, names[0], names[1]);
			if (cnt == 3)
			{
				ToLower(names[1]);
				strncpy(myName, names[1], sizeof(names[1]));
			}
		}
		else if (prefix[0] == 'N'
		&&  prefix[1] == (char)0xc2)
		{
			int cnt = fscanf(file, "%s %s %s %s %s Result %s",
				time, names[0], names[1], names[2], names[3], direction);
			if (cnt == 6)
			{
				if (direction[0] == 'N'
					&&  direction[1] == 'S')
					playedNS = true;
				else
					playedNS = false;
				fgets(line, 1024, file);				// skip the rest of the line
				break;
			}
			else
			{
				printf("ERROR: We could not Parse direction line!!\n");
				return NULL;
			}
		}
		fgets(line, 1024, file);						// skip the rest of the line
	} while (true);

	// find our result
	int cnt = fscanf(file, "%c", &start);
	// new files have the session number and our result
	if (cnt == 1
	&& start == '#')
	{
		cnt = fscanf(file, "%d %s %s %s %s %s %s %lf %lf&&",
			&num, prefix, date, time, names[0], names[1], names[2], &points, &percent);
		if (cnt == 8)									// if we did not place the points is not present
		{
			mySession = num;
			myResult = points;
		}
		else if (cnt == 9)
		{
			mySession = num;
			myResult = percent;
		}
	}
	fgets(line, 1024, file);							// skip the rest of the line

	// read each result
	do
	{
		int cnt = fscanf(file, "%d %s %s", &num, date, time);
		if (cnt == 0)
		{
			break;
		}
		if (cnt != 3)
		{
			printf("ERROR: We could not process line %d!!\n", num);
			return NULL;
		}
		if (playedDate[0] == 0)
		{
			int l = (int)sizeof(playedDate) - 1;
			strncpy(playedDate, date, l);
			playedDate[l] = 0;
			l = (int)sizeof(playedTime) - 1;
			strncpy(playedTime, time, l);
			playedTime[l] = 0;
		}
		// now the names may have spaces but are delimited by tabs so read the names and contract
		SkipTab(file);
		fscanf(file, "%[^\t]s", names[0]);
		SkipTab(file);
		fscanf(file, "%[^\t]s", names[1]);
		SkipTab(file);
		fscanf(file, "%[^\t]s", names[2]);
		SkipTab(file);
		fscanf(file, "%[^\t]s", names[3]);
		SkipTab(file);
		fscanf(file, "%[^\t]s", contract);
		SkipTab(file);
		cnt = fscanf(file, "%d %lf", &score, &percent);
		if (cnt == 0)									// A== is used for an average check for our score
		{
			char noscore[16];
			cnt = fscanf(file, "%s %lf", &noscore, &percent);
			fgets(line, 1024, file);					// skip the rest of the line
			if (cnt == 0)								// skip							
				continue;
			score = INVALID;							// mark score as invalid
		}
		else
			fgets(line, 1024, file);					// skip the rest of the line
		if (cnt != 2)
		{
			printf("ERROR: We could not process the score in line %d!!\n", num);
			return NULL;
		}
		// older versions of the results file made my name all upper case for unknown reason
		ToLower(names[0]);
		ToLower(names[1]);
		ToLower(names[2]);
		ToLower(names[3]);
		size_t l = strlen(myName);
		if (strncmp(myName, names[0], l) == 0
		|| strncmp(myName, names[1], l) == 0
		|| strncmp(myName, names[2], l) == 0
		|| strncmp(myName, names[3], l) == 0)
		{
			myResultSoFar += percent;
			myResultCounted++;
		}
		hand->resultCount++;
		if (score != INVALID)
		{
			if (hand->resultsEntries == 0
				|| hand->results[hand->resultsEntries - 1].score != score)
			{
				hand->results[hand->resultsEntries].count = 1;
				hand->results[hand->resultsEntries].score = score;
				hand->results[hand->resultsEntries].percent = percent;
				hand->resultsEntries++;
			}
			else
			{
				hand->results[hand->resultsEntries - 1].count++;
			}
		}
	} while (true);
	if (numResults < hand->resultCount)
		numResults = hand->resultCount;
	hand->validResults = true;

	// adjust EW scores and percentages
	if (playedNS == false)
	{
		for (int i = 0; i < hand->resultsEntries; i++)
		{
			hand->results[i].score = -hand->results[i].score;
			hand->results[i].percent = 100.0 - hand->results[i].percent;
		}
		for (int i = 0; i < hand->resultsEntries / 2; i++)
		{
			Result r = hand->results[i];
			hand->results[i] = hand->results[hand->resultsEntries - 1 - i];
			hand->results[hand->resultsEntries - 1 - i] = r;
		}
	}
	return hand;
}
