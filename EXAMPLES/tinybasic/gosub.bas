0010 REM "Subroutine Nesting"
0020 :
0030 PRINT "Calling A"
0040 GOSUB 80
0050 PRINT "Back from A"
0060 END
0070 REM "LEVEL 1"
0080 PRINT "\tCalling B"
0090 GOSUB 130
0100 PRINT "\tBack from B"
0110 RETURN
0120 REM "LEVEL 2"
0130 PRINT "\t\tCalling C"
0140 GOSUB 170
0150 PRINT "\t\tBack from C"
0160 RETURN
0170 REM "LEVEL 3"
0180 PRINT "\t\t\tWelcome to C!"
0190 RETURN
