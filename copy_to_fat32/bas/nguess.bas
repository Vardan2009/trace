00  REM Guess the number game

10  RANDOMIZE TIMER
20  LET ANSWER, (RND % 101)
30  LET REMAIN, 10

40  PRINT "Guess the number between 0-100", NEWLINE

50  PRINT REMAIN, " attempts left", NEWLINE
55  IF REMAIN < 1, 180
60  INPUT GUESS, INT

70  IF GUESS = ANSWER, 100
80  IF GUESS < ANSWER, 120
90  IF GUESS > ANSWER, 150


100 PRINT "You guessed the number!", NEWLINE
110 GOTO 256


120 PRINT "Go higher!", NEWLINE
130 LET REMAIN, (REMAIN - 1)
140 GOTO 50


150 PRINT "Go lower!", NEWLINE
160 LET REMAIN, (REMAIN - 1)
170 GOTO 50

180 PRINT "You lost!", NEWLINE
190 PRINT "The number was ", ANSWER, NEWLINE
