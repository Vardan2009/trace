00  REM This is a comment
10  PRINT "Welcome to BASIC! What is your name?"
20  INPUT NAME, STRING
30  PRINT "Hello, ", NAME, "! Nice to meet you!"
40  PRINT "Lets count to 10!"
50  LET I, 0
60  REM Counting loop
70    PRINT "Number ", I 
80    LET I, I + 1
81    REM "IF i is greater than 10, goto 110"
90    IF I > 10, 110
100 GOTO 60
110 PRINT "Nice!"
