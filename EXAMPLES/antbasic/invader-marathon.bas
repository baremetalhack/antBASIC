0010 REM "Invader Marathon"
0020 REM "BAREMETALHACK.COM --> Public domain"
0030 :
0040 PRINT "Distance ";:INPUT D
0050 IF D<1 END
0060 IF D>21 END
0070 :
0080 FOR I=1 TO D
0090 PRINT " ";
0100 PRINT "👾\a";
0110 MSLEEP(200)
0120 PRINT "\b\b";
0130 NEXT
0140 PRINT " 👾\a";
0150 MSLEEP(200)
0160 :
0170 FOR I=1 TO D
0180 PRINT "\b\b\b";
0190 PRINT "👾\a";
0200 MSLEEP(200)
0210 NEXT
0220 PRINT
0230 END
