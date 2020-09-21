#include "stdafx.h"

jmp_buf env;

// each hand is 320.0x250.0 which is little less than 4.5x3.5 inches 
// screen is assumed to be 11.0x8.5 (792.0x612.0)
// (0,0) is lower left corner
HPDF_REAL handWidth = 320.0;
HPDF_REAL handHeight = 250.0;
HPDF_REAL handH[HANDS] = { 75.0, 397.0, 75.0, 397.0 };	// leave 2 between areas
HPDF_REAL handV[HANDS] = { 307.0, 307.0, 55.0, 55.0 };

char	hands[4] = { 'N', 'E', 'S', 'W' };
char *  names[HANDS] = { "NORTH", "EAST", "SOUTH", "WEST" };
VUL		vulhand[HANDS] = { VUL_NS, VUL_EW,VUL_NS, VUL_EW };
HAND	dealer[MAX_HANDS] = {
	NORTH, EAST, SOUTH, WEST,
	NORTH, EAST, SOUTH, WEST,
	NORTH, EAST, SOUTH, WEST };


#ifdef HPDF_DLL
void  __stdcall
#else
void
#endif
error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void * user_data)
{
	printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
		(HPDF_UINT)detail_no);
	longjmp(env, 1);
}


void HandToString(char * string, int cardMask)
{
	int n = 0;
	for (int i = 12, mask = 0x4000; i >= 0; i--, mask >>= 1)
	{
		if ((mask & cardMask) != 0)
		{
			string[n++] = Cards[i];
		}
	}
	string[n] = 0;
}


void PrintHands(HPDF_Doc & pdf, HPDF_Page & page, HAND hand)
{
	HPDF_REAL h = handH[hand];
	HPDF_REAL v = handV[hand];
	HPDF_REAL h1 = handWidth / 4.0;					// size of each box
	HPDF_REAL v1 = handHeight / 4.0;
	HPDF_REAL tw;
	char string[MAX_STRING];

	HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);
	HPDF_Font font2 = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
	HPDF_Page_SetGrayFill(page, 0.2);
	HPDF_Page_SetGrayStroke(page, 0.2);
	HPDF_Page_SetLineWidth(page, 1.0);
	// draw h edges
	HPDF_Page_MoveTo(page, h - 0.5, v + 4.0*v1 - 0.5);			// top most line
	HPDF_Page_LineTo(page, h + handWidth, v + 4.0*v1 - 0.5);
	HPDF_Page_Stroke(page);
	HPDF_Page_MoveTo(page, h, v + 3.0*v1 - 0.5);
	HPDF_Page_LineTo(page, h + handWidth, v + 3.0*v1 - 0.5);
	HPDF_Page_Stroke(page);
	HPDF_Page_MoveTo(page, h - 0.5, v + 2.0*v1 - 0.5);	
	HPDF_Page_LineTo(page, h + handWidth, v + 2.0*v1 - 0.5);
	HPDF_Page_Stroke(page);
	HPDF_Page_MoveTo(page, h + 2.0*h1, v + v1 - 0.5);	
	HPDF_Page_LineTo(page, h + handWidth, v + v1 - 0.5);
	HPDF_Page_Stroke(page);
	HPDF_Page_MoveTo(page, h - 0.5 + 2.0*h1, v - 0.5);		
	HPDF_Page_LineTo(page, h + handWidth, v - 0.5);
	HPDF_Page_Stroke(page);
	// draw v edges
	HPDF_Page_MoveTo(page, h, v + handHeight - 0.5);	// left most edge
	HPDF_Page_LineTo(page, h, v + 2 * v1 - 0.5);
	HPDF_Page_Stroke(page);
	HPDF_Page_MoveTo(page, h + h1 - 0.5, v + handHeight - 0.5);	
	HPDF_Page_LineTo(page, h + h1 - 0.5, v + 2 * v1 - 0.5);
	HPDF_Page_Stroke(page);
	HPDF_Page_MoveTo(page, h + 2.0*h1, v + handHeight - 0.5);
	HPDF_Page_LineTo(page, h + 2.0*h1, v - 0.5);
	HPDF_Page_Stroke(page);
	HPDF_Page_MoveTo(page, h + 3.0*h1 - 0.5, v + handHeight - 0.5);
	HPDF_Page_LineTo(page, h + 3.0*h1 - 0.5, v - 0.5);
	HPDF_Page_Stroke(page);
	HPDF_Page_MoveTo(page, h + 4.0*h1 - 0.5, v + handHeight - 0.5);
	HPDF_Page_LineTo(page, h + 4.0*h1 - 0.5, v - 0.5);
	HPDF_Page_Stroke(page);

	// which hands are displayed
	HPDF_Page_SetFontAndSize(page, font2, 24);
	tw = HPDF_Page_TextWidth(page, names[hand]);
	HPDF_Page_BeginText(page);
	HPDF_Page_TextOut(page, h + (2.0 * h1 - tw) / 2.0, v + 2.0*v1 - 26, names[hand]);
	HPDF_Page_EndText(page);

	// session, date, time, etc
	HPDF_Page_SetFontAndSize(page, font, 18);
	sprintf(string, "BBO #%d", mySession);
	tw = HPDF_Page_TextWidth(page, string);
	HPDF_Page_BeginText(page);
	HPDF_Page_TextOut(page, h + (2.0 * h1 - tw) / 2.0, v + 2.0*v1 - 46, string);
	HPDF_Page_EndText(page);
	tw = HPDF_Page_TextWidth(page, playedDate);
	HPDF_Page_BeginText(page);
	HPDF_Page_TextOut(page, h + (2.0 * h1 - tw) / 2.0, v + 2.0*v1 - 66, playedDate);
	HPDF_Page_EndText(page);
	tw = HPDF_Page_TextWidth(page, playedTime);
	HPDF_Page_BeginText(page);
	HPDF_Page_TextOut(page, h + (2.0 * h1 - tw) / 2.0, v + 2.0*v1 - 86, playedTime);
	HPDF_Page_EndText(page);

	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_BeginText(page);
	sprintf(string, "%d pairs", numResults);
	tw = HPDF_Page_TextWidth(page, string);
	HPDF_Page_TextOut(page, h + (2.0 * h1 - tw) / 2.0, v + 2.0*v1 - 100, string);
	HPDF_Page_EndText(page);
	HPDF_Page_BeginText(page);
	HPDF_Page_TextOut(page, h + 4, v + 4, "BBO2DOOP created by Ed Logg");
	HPDF_Page_EndText(page);

	// now show each hand
	HPDF_REAL x, y;
	for (int n = 0; n < MAX_HANDS; n++)
	{
		if (handInfo[n].validHand == false)
		{
			printf("ERROR: missing hand %d!!\n", n + 1);
			continue;
		}
		switch (n)
		{
		case 0:
			x = h;
			y = v + 3.0 * v1;
			break;
		case 1:
			x = h + h1;
			y = v + 3.0 * v1;
			break;
		case 2:
			x = h + h1;
			y = v + 2.0 * v1;
			break;
		case 3:
			x = h;
			y = v + 2.0 * v1;
			break;
		case 4:
			x = h + 2.0 * h1;
			y = v + 3.0 * v1;
			break;
		case 5:
			x = h + 3.0 * h1;
			y = v + 3.0 * v1;
			break;
		case 6:
			x = h + 3.0 * h1;
			y = v + 2.0 * v1;
			break;
		case 7:
			x = h + 2.0 * h1;
			y = v + 2.0 * v1;
			break;
		case 8:
			x = h + 2.0 * h1;
			y = v + v1;
			break;
		case 9:
			x = h + 3.0 * h1;
			y = v + v1;
			break;
		case 10:
			x = h + 3.0 * h1;
			y = v;
			break;
		case 11:
			x = h + 2.0 * h1;
			y = v;
			break;
		}
		HPDF_Page_SetFontAndSize(page, font, 10);
		HPDF_REAL v2 = v1 / 5.0;				// position of each row in each box
		y += 3;

		// draw hand number
		sprintf(string, "%2d", n + 1);		
		HPDF_Page_BeginText(page);
		HPDF_Page_TextOut(page, x + 2.0, y + 4.0 * v2, string);
		HPDF_Page_EndText(page);

		// draw vul
		HPDF_Page_BeginText(page);	
		switch (handInfo[n].vul)
		{
		default:
		case VUL_NONE:
			HPDF_Page_TextOut(page, x + 30.0, y + 4.0 * v2, "NONE");
			break;
		case VUL_NS:
		case VUL_EW:
			if (handInfo[n].vul == vulhand[hand])
				HPDF_Page_TextOut(page, x + 30.0, y + 4.0 * v2, " WE");
			else
				HPDF_Page_TextOut(page, x + 30.0, y + 4.0 * v2, "THEY");
			break;
		case VUL_BOTH:
			HPDF_Page_TextOut(page, x + 30.0, y + 4.0 * v2, "BOTH");
			break;
		}
		HPDF_Page_EndText(page);

		// draw dealer
		HPDF_Page_BeginText(page);
		if (dealer[n] == hand)
			HPDF_Page_TextOut(page, x + 70.0, y + 4.0 * v2, "D");
		else switch (dealer[n])
		{
		case NORTH:
			HPDF_Page_TextOut(page, x + 70.0, y + 4.0 * v2, "N");
			break;
		case EAST:
			HPDF_Page_TextOut(page, x + 70.0, y + 4.0 * v2, "E");
			break;
		case SOUTH:
			HPDF_Page_TextOut(page, x + 70.0, y + 4.0 * v2, "S");
			break;
		case WEST:
			HPDF_Page_TextOut(page, x + 70.0, y + 4.0 * v2, "W");
			break;
		}
		HPDF_Page_EndText(page);

		// draw cards
		HPDF_Page_BeginText(page);
		HPDF_Page_TextOut(page, x + 2.0, y + 3.0 * v2, "S");
		HPDF_Page_EndText(page);
		HPDF_Page_BeginText(page);
		HandToString(string, handInfo[n].cardMask[hand][SPADE]);
		HPDF_Page_TextOut(page, x + 15.0, y + 3.0 * v2, string);
		HPDF_Page_EndText(page);
		HPDF_Page_BeginText(page);
		HPDF_Page_TextOut(page, x + 2.0, y + 2.0 * v2, "H");
		HPDF_Page_EndText(page);
		HPDF_Page_BeginText(page);
		HandToString(string, handInfo[n].cardMask[hand][HEART]);
		HPDF_Page_TextOut(page, x + 15.0, y + 2.0 * v2, string);
		HPDF_Page_EndText(page);
		HPDF_Page_BeginText(page);
		HPDF_Page_TextOut(page, x + 2.0, y + 1.0 * v2, "D");
		HPDF_Page_EndText(page);
		HPDF_Page_BeginText(page);
		HandToString(string, handInfo[n].cardMask[hand][DIAMOND]);
		HPDF_Page_TextOut(page, x + 15.0, y + 1.0 * v2, string);
		HPDF_Page_EndText(page);
		HPDF_Page_BeginText(page);
		HPDF_Page_TextOut(page, x + 2.0, y, "C");
		HPDF_Page_EndText(page);
		HPDF_Page_BeginText(page);
		HandToString(string, handInfo[n].cardMask[hand][CLUB]);
		HPDF_Page_TextOut(page, x + 15.0, y, string);
		HPDF_Page_EndText(page);
	}
}


bool PDFPrintHands(char * folder)
{
	HPDF_Doc  pdf = NULL;
	HPDF_Page page = NULL;
	char fname[MAX_PATH];
	HPDF_REAL width, height;

	pdf = HPDF_New(error_handler, NULL);
	if (!pdf)
	{
		printf("ERROR: Cannot create PDF Doc object for hands!\n");
		return false;
	}
	if (setjmp(env))
	{
		HPDF_Free(pdf);
		printf("ERROR: Cannot create PDF Doc object for hands!\n");
		return false;
	}
	HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);		// set compression mode
	page = HPDF_AddPage(pdf);							// create page

	// Use landscape format
	// The default resolution of PDF format is 72dpi.
	HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_LETTER, HPDF_PAGE_LANDSCAPE);
	width = HPDF_Page_GetWidth(page);					// 792.0
	height = HPDF_Page_GetHeight(page);					// 612.0

	// show each hand
	PrintHands(pdf, page, NORTH);
	PrintHands(pdf, page, EAST);
	PrintHands(pdf, page, SOUTH);
	PrintHands(pdf, page, WEST);

	strcpy(fname, folder);
	strncat(fname, "\\hands.pdf", MAX_PATH);
	HPDF_SaveToFile(pdf, fname);						// save the document to a file 
	if (pdf != NULL)
		HPDF_Free(pdf);									// release object
	page = NULL;
	pdf = NULL;
	return true;
}


bool PDFPrintResults(char * folder)
{
	HPDF_Doc  pdf = NULL;
	HPDF_Page page = NULL;
	char fname[MAX_PATH];
	HPDF_REAL width, height;
	bool printedHCP = false;
	bool printedVoid = false;
	bool printedSingleton = false;
	bool printedLong = false;

	pdf = HPDF_New(error_handler, NULL);
	if (!pdf)
	{
		printf("ERROR: Cannot create PDF Doc object!\n");
		return false;
	}
	if (setjmp(env))
	{
		HPDF_Free(pdf);
		printf("ERROR: Cannot create PDF file!\n");
		return false;
	}
	HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);
	HPDF_Font font2 = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
	HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);		// set compression mode
	page = HPDF_AddPage(pdf);							// create page

	// Use landscape format
	// The default resolution of PDF format is 72dpi.
	HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_LETTER, HPDF_PAGE_PORTRAIT);
	width = HPDF_Page_GetWidth(page);					// 612.0 
	height = HPDF_Page_GetHeight(page);					// 792.0

	// draw header 
	HPDF_REAL hmargin = width * 0.5 / 8.5;				// 1/2 inch margin
	HPDF_REAL vmargin = height * 0.75 / 11.0;			// 3/4 inch margin
	HPDF_REAL hstart = hmargin;
	HPDF_REAL hend = width - hmargin;
	HPDF_REAL vstart = vmargin;
	HPDF_REAL vend = height - vmargin;
	char string[256];
	HPDF_Page_SetFontAndSize(page, font2, 15);
	HPDF_Page_BeginText(page);
	if (mySession >= 0)
		sprintf(string, "BBO #%d    %s    %s", mySession, playedDate, playedTime);
	else
		sprintf(string, "BBO    %s    %s", playedDate, playedTime);
	HPDF_Page_TextOut(page, hstart, vend - 15.0, string);
	HPDF_Page_EndText(page);
	HPDF_Page_SetFontAndSize(page, font, 8);
	HPDF_Page_BeginText(page);
	if (myResult > 0.0)
		sprintf(string, "%s played %s (%.2f%%)",
			myName, (playedNS ? "NS" : "EW"), myResult);
	else
		sprintf(string, "%s played %s",
			myName, (playedNS ? "NS" : "EW"));
	HPDF_REAL tw = HPDF_Page_TextWidth(page, string);
	// 240 to 380 is open for this string
	HPDF_Page_TextOut(page, hstart + 240.0 + (140.0 - tw) / 2.0, vend - 15.0, string);
	HPDF_Page_EndText(page);

	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_BeginText(page);
	HPDF_Page_TextOut(page, hstart + 390, vend - 15.0, "BBO2DOOP created by Ed Logg");
	HPDF_Page_EndText(page);
	vend -= 20.0;									

	// draw results 6 wide
	HPDF_Page_SetGrayFill(page, 0.2);
	HPDF_Page_SetGrayStroke(page, 0.2);
	HPDF_Page_SetLineWidth(page, 1.0);
	HPDF_REAL x = hstart;								// left edge of hand group
	HPDF_REAL y = vend;									// top of our box
	HPDF_REAL h = (width - 2.0 * hmargin) / 6;			// width of each hand group
	HPDF_REAL v = (vend - vstart) / 2.0;				// height of each hand group
	for (int n = 0; n < MAX_HANDS; n++, x += h)
	{
		if (handInfo[n].validResults == false)
		{
			printf("ERROR: missing results for hand %d!!\n", n + 1);
			continue;
		}
		if (n == 6)
		{
			x = hstart;
			y = vend - v;
		}
		HPDF_Page_MoveTo(page, x, y - 0.5);				// left most v line
		HPDF_Page_LineTo(page, x, y - v - 0.5);
		HPDF_Page_Stroke(page);
		HPDF_Page_MoveTo(page, x - 0.5, y);				// h lines above hand number
		HPDF_Page_LineTo(page, x + h + 0.5, y);
		HPDF_Page_Stroke(page);
		HPDF_REAL cur = y - 12.0;
		HPDF_Page_MoveTo(page, x - 0.5, cur);			// h lines below hand number
		HPDF_Page_LineTo(page, x + h + 0.5, cur);
		HPDF_Page_Stroke(page);
		HPDF_Page_SetFontAndSize(page, font, 10);
		tw = HPDF_Page_TextWidth(page, string);
		sprintf(string, "Hand %2d", n + 1);
		tw = HPDF_Page_TextWidth(page, string);
		HPDF_Page_BeginText(page);
		HPDF_Page_TextOut(page, x + (h - tw) / 2.0, cur + 2.0, string);
		HPDF_Page_EndText(page);
		int better = 0;
		double count = handInfo[n].resultCount;
		if (simpleResults)
		{
			HPDF_REAL vsize = (v - 12.0) / handInfo[n].resultsEntries;
			vsize -= 2.0;
			if (vsize > 10.0)
				vsize = 10.0;
			else
				vsize = (HPDF_REAL)((int)vsize);
			cur -= vsize;
			HPDF_Page_SetFontAndSize(page, font, vsize);
			// we will modify the percentages given to us
			// because we we match a score the score changes 
			// the formula assumes we have count entries so scores
			// would normally be 0 to count-1, so when we add 1
			// scores will go from 0 to count.
			// If we know the number of better scores the new percentage
			// is (count-better +...+ count-better-repeat)/N = 
			// (count-better)/count - repeat/(2*count)
			// as you can see for the in between scores the percentage is
			// (count-better)/count
			for (int r = 0; r < handInfo[n].resultsEntries; r++, cur -= vsize + 2.0)
			{
				// in between score
				// add code to ignore impossible scores (N to N+10 and -50 to 0 and 0 to 50)
				double percent = (count - better) / count;
				if (r != 0)
				{
					sprintf(string, "%3d%%", (int)(100.0 * percent + 0.5));
					HPDF_Page_BeginText(page);
					HPDF_Page_TextOut(page, x + 65.0, cur + vsize/2.0, string);
					HPDF_Page_EndText(page);
				}
				// recalculate percentage score
				percent -= handInfo[n].results[r].count / (2.0 * count);
				sprintf(string, "%5d %3d%%",
					handInfo[n].results[r].score, (int)(100.0 * percent + 0.5));
				HPDF_Page_BeginText(page);
				HPDF_Page_TextOut(page, x + 2.0, cur, string);
				HPDF_Page_EndText(page);
				better += handInfo[n].results[r].count;
			}
		}
		else
		{
			cur -= 14.0;
			HPDF_Page_SetFontAndSize(page, font, 7);
			// we will modify the percentages given to us
			// because we we match a score the score changes 
			// the formula assumes we have count entries so scores
			// would normally be 0 to count-1, so when we add 1
			// scores will go from 0 to count.
			// If we know the number of better scores the new percentage
			// is (count-better +...+ count-better-repeat)/N = 
			// (count-better)/count - repeat/(2*count)
			// as you can see for the in between scores the percentage is
			// (count-better)/count
			for (int r = 0; r < handInfo[n].resultsEntries; r++, cur -= 8.0)
			{
				// in between score
				// add code to ignore impossible scores (N to N+10 and -50 to 0 and 0 to 50)
				double percent = (count - better) / count;
				sprintf(string, "--> %6.2f%%", 100.0 * percent);
				HPDF_Page_BeginText(page);
				HPDF_Page_TextOut(page, x + 50.0, cur + 4.0, string);
				HPDF_Page_EndText(page);
				// recalculate percentage score
				percent -= handInfo[n].results[r].count / (2.0 * count);
				sprintf(string, "%5d %6.2f%%",
					handInfo[n].results[r].score, 100.0 * percent);
				HPDF_Page_BeginText(page);
				HPDF_Page_TextOut(page, x + 2.0, cur, string);
				HPDF_Page_EndText(page);
				better += handInfo[n].results[r].count;
			}
		}

		// print stats
		if (n >= 6
		&& cur >= vstart + 45.0)					// if there is room
		{
			HPDF_Page_SetFontAndSize(page, font, 10);
			cur = vstart + 45.0;
			if (printedHCP == false)
			{
				HPDF_Page_BeginText(page);
				HPDF_Page_TextOut(page, x + 10.0, cur, "Average HCP");
				HPDF_Page_EndText(page);
				for (int h = 0; h < HANDS; h++)
				{
					cur -= 10;
					sprintf(string, "%c %4.1f", hands[h], points[h] / 12.0);
					HPDF_Page_BeginText(page);
					HPDF_Page_TextOut(page, x + 20.0, cur, string);
					HPDF_Page_EndText(page);
				}
				printedHCP = true;
			}
			else if (printedVoid == false)
			{
				HPDF_Page_BeginText(page);
				HPDF_Page_TextOut(page, x + 20.0, cur, "Voids");
				HPDF_Page_EndText(page);
				for (int h = 0; h < HANDS; h++)
				{
					cur -= 10;
					sprintf(string, "%c  %d", hands[h], voids[h]);
					HPDF_Page_BeginText(page);
					HPDF_Page_TextOut(page, x + 25.0, cur, string);
					HPDF_Page_EndText(page);
				}
				printedVoid = true;
			}
			else if (printedSingleton == false)
			{
				HPDF_Page_BeginText(page);
				HPDF_Page_TextOut(page, x + 10.0, cur, "Singletons");
				HPDF_Page_EndText(page);
				for (int h = 0; h < HANDS; h++)
				{
					cur -= 10;
					sprintf(string, "%c  %d", hands[h], singletons[h]);
					HPDF_Page_BeginText(page);
					HPDF_Page_TextOut(page, x + 25.0, cur, string);
					HPDF_Page_EndText(page);
				}
				printedSingleton = true;

			}
			else if (printedLong == false)
			{
				HPDF_Page_BeginText(page);
				HPDF_Page_TextOut(page, x + 10.0, cur, "Long Suits (>6)");
				HPDF_Page_EndText(page);
				for (int h = 0; h < HANDS; h++)
				{
					cur -= 10;
					sprintf(string, "%c  %d", hands[h], longSuits[h]);
					HPDF_Page_BeginText(page);
					HPDF_Page_TextOut(page, x + 25.0, cur, string);
					HPDF_Page_EndText(page);
				}
				printedLong = true;
			}
		}
	}
	HPDF_Page_MoveTo(page, hstart - 0.5, vstart);		// bottom most h line
	HPDF_Page_LineTo(page, hend - 0.5, vstart);
	HPDF_Page_Stroke(page);
	HPDF_Page_MoveTo(page, x, vend - 0.5);				// right most v line
	HPDF_Page_LineTo(page, x, vstart - 0.5);
	HPDF_Page_Stroke(page);

	// save file now
	strcpy(fname, folder);
	strncat(fname, "\\results.pdf", MAX_PATH);
	HPDF_SaveToFile(pdf, fname);						// save the document to a file 
	if (pdf != NULL)
		HPDF_Free(pdf);									// release object
	page = NULL;
	pdf = NULL;
	return true;
}

