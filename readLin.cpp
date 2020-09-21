#include "stdafx.h"


bool Find(char * line, int & pos, int stop, char * string)
{
	int len = (int)strlen(string);
	for (int i = 0; i < len; i++)
	{
		if (line[pos + i] != string[i])
		{
			i = -1;
			if (++pos > stop)
				return false;
		}
	}
	pos += len;
	return true;
}


bool GetCards(HandInfo * hand, char * line, int & pos, int player)
{
	int cards = 0;
	SUIT suit = NOSUIT;
	while (line[pos] != ',')
	{
		switch (line[pos])
		{
		case 'S':
			suit = SPADE;
			break;
		case 'H':
			suit = HEART;
			break;
		case 'D':
			suit = DIAMOND;
			break;
		case 'C':
			suit = CLUB;
			break;
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			cards++;
			hand->cardMask[player][suit] |= (1 << (line[pos] - '0'));
			break;
		case 'T':
			cards++;
			hand->cardMask[player][suit] |= (1 << 10);
			break;
		case 'J':
			cards++;
			hand->cardMask[player][suit] |= (1 << 11);
			break;
		case 'Q':
			cards++;
			hand->cardMask[player][suit] |= (1 << 12);
			break;
		case 'K':
			cards++;
			hand->cardMask[player][suit] |= (1 << 13);
			break;
		case 'A':
			cards++;
			hand->cardMask[player][suit] |= (1 << 14);
			break;
		default:
			break;
		}
		pos++;
	}
	pos++;
	if (cards == 13)
		return true;
	return false;
}


HandInfo * LinFile(FILE * file)
{
	int pos;
	int board = 0;
	char line[1024];
	char * res = fgets(line, 1024, file);
	int len = (int)strlen(line);
	if (res == NULL
		|| line[0] != 'p'
		|| line[1] != 'n'
		|| line[2] != '|')
		return NULL;
	pos = 3;
	if (Find(line, pos, len, "Board ") == false)		// skipping to board number
	{
		printf("ERROR: lin file cannot find board number!!\n");
		return NULL;
	}
	do
	{
		board = board * 10 + line[pos] - '0';
	} while (line[++pos] != '|');
	if (board < 1 || board > MAX_HANDS)
	{
		printf("ERROR: we can only handle hands 1-%d\n", MAX_HANDS);
		return NULL;
	}
	board--;											// board 1 uses index 0
	HandInfo * hand = &handInfo[board];
	if (hand->validHand)
	{
		printf("ERROR: we have already loaded hand %d\n", board + 1);
		return NULL;
	}
	hand->first = board & 3;							// this player bids first
														// get the cards for each hand
	pos = 3;
	if (Find(line, pos, len, "md|") == false)			// skipping deal
	{
		printf("ERROR: lin file cannot find the deal!!\n");
		return NULL;
	}
	pos++;												// skip the number 1-4
	if (GetCards(hand, line, pos, 2) == false)
	{
		printf("ERROR: could not parse South cards!!\n");
		return NULL;
	}
	if (GetCards(hand, line, pos, 3) == false)
	{
		printf("ERROR: could not parse West cards!!\n");
		return NULL;
	}
	if (GetCards(hand, line, pos, 0) == false)
	{
		printf("ERROR: could not parse North cards!!\n");
		return NULL;
	}
	int mask = hand->cardMask[2][0] | hand->cardMask[3][0] | hand->cardMask[0][0];
	hand->cardMask[1][0] = 0x7ffc & ~mask;
	mask = hand->cardMask[2][1] | hand->cardMask[3][1] | hand->cardMask[0][1];
	hand->cardMask[1][1] = 0x7ffc & ~mask;
	mask = hand->cardMask[2][2] | hand->cardMask[3][2] | hand->cardMask[0][2];
	hand->cardMask[1][2] = 0x7ffc & ~mask;
	mask = hand->cardMask[2][3] | hand->cardMask[3][3] | hand->cardMask[0][3];
	hand->cardMask[1][3] = 0x7ffc & ~mask;

	// get vulnerability
	if (Find(line, pos, len, "sv|") == false)			// skipping to side vulnerability
	{
		printf("ERROR: lin file cannot find vulnerability!!\n");
		return NULL;
	}
	if (line[pos] == 'o')
		hand->vul = VUL_NONE;
	else if (line[pos] == 'n')
		hand->vul = VUL_NS;
	else if (line[pos] == 'e')
		hand->vul = VUL_EW;
	else if (line[pos] == 'b')
		hand->vul = VUL_BOTH;
	else
	{
		printf("ERROR: lin file invalid vulnerability!!\n");
		return NULL;
	}
	// mark this hand as valid
	hand->validHand = true;
	return hand;
}
