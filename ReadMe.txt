========================================================================
    CONSOLE APPLICATION : BBO2DOOP Project Overview
========================================================================

This app tries to create a set of DOOP files from a set of BBO hands previously played.
To get the BBO hands go to: https://www.bridgebase.com/myhands/index.php
Then find the tournament desired and download the lin files for all hands.
Next view the traveler for each hand, copy the screen, and place it in text file
called result<n>.txt.  Now place all 24 files in a folder name (do not use spaces in the 
folder name).  Now drop this folder onto the .exe file and it will create the needed 
documents hands.pdf and results.pdf in the same folder.

The .lin file format is not documented so I have taken a stab from multiple samples. 
The results file is not guaranteed to stay the same either so I have  used the format 
currently used at this time.

Note that if a suit contains 12+ cards it will run into the next block and may be unreadable.
Of course with 13 cards in a suit it will be obvious since the other suits are void.

NOTE: there are actually two build options for the results file.  If the variable simpleResults
is true the results file use a larger font and the percentage is an integer instead of 
hundredth of a percentage.