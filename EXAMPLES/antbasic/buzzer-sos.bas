0010 REM "=== SOS Beeper ==="
0020 REM "BAREMTALHACK.COM --> Public domain"
0030 :
0040 REM "--- GPIO Mapping and Constants ---"
0050 B=10:REM "Buzzer pin => BMH10"
0060 T=100:REM "Timing unit => 100msec"
0070 :
0080 COLOR(15,0)
0090 FOR I=1 TO 3
0100 PRINT "S";
0110 GOSUB 2000:GOSUB 2200:GOSUB 2000:GOSUB 2200:GOSUB 2000:GOSUB 2300:REM "S"
0120 PRINT "O";
0130 GOSUB 2100:GOSUB 2200:GOSUB 2100:GOSUB 2200:GOSUB 2100:GOSUB 2300:REM "O"
0140 PRINT "S";
0150 GOSUB 2000:GOSUB 2200:GOSUB 2000:GOSUB 2200:GOSUB 2000:GOSUB 2400:REM "S"
0160 PRINT " ";
0170 NEXT
0180 COLOR(7,0)
0190 PRINT
0200 END
0210 :
2000 REM "--- Short mark ---"
2010 OUT(B,1)
2020 MSLEEP(T)
2030 OUT(B,0)
2040 RETURN
2050 :
2100 REM "--- Long mark ---"
2110 OUT(B,1)
2120 MSLEEP(T*3)
2130 OUT(B,0)
2140 RETURN
2150 :
2200 REM "--- Intra-character gap ---"
2210 MSLEEP(T)
2220 RETURN
2230 :
2300 REM "--- Letter gap ---"
2310 MSLEEP(T*3)
2320 RETURN
2330 :
2400 REM "--- Word gap ---"
2410 MSLEEP(T*7)
2420 RETURN
2430 :
