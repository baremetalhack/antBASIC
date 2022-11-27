0010 REM "=== 7-Segments LED Hexadecimal Incrementer ==="
0020 REM "       +++ common ANODE, active LOW +++       "
0030 REM "BAREMETALHACK.COM --> Public domain"
0040 :
0050 REM "--- GPIO Mapping ---"
0060 G=7:F=8:A=9:B=10:E=11:D=12:C=13:H=14:REM "Segments => BMH7-BMH14 (H-->DP)"
0070 :
0080 DIM L[16,7]:REM "Segment data for hexadecimal number"
0090 L[0,0]=0,0,0,0,0,0,1:REM "0"
0100 L[1,0]=1,0,0,1,1,1,1:REM "1"
0110 L[2,0]=0,0,1,0,0,1,0:REM "2"
0120 L[3,0]=0,0,0,0,1,1,0:REM "3"
0130 L[4,0]=1,0,0,1,1,0,0:REM "4"
0140 L[5,0]=0,1,0,0,1,0,0:REM "5"
0150 L[6,0]=0,1,0,0,0,0,0:REM "6"
0160 L[7,0]=0,0,0,1,1,1,1:REM "7"
0170 L[8,0]=0,0,0,0,0,0,0:REM "8"
0180 L[9,0]=0,0,0,0,1,0,0:REM "9"
0190 L[10,0]=0,0,0,1,0,0,0:REM "A"
0200 L[11,0]=1,1,0,0,0,0,0:REM "B"
0210 L[12,0]=0,1,1,0,0,0,1:REM "C"
0220 L[13,0]=1,0,0,0,0,1,0:REM "D"
0230 L[14,0]=0,1,1,0,0,0,0:REM "E"
0240 L[15,0]=0,1,1,1,0,0,0:REM "F"
0250 :
0260 S=1:GOSUB 380:REM "Turn off all LEDs"
0270 FOR I=0 TO 15
0280 OUT(A,L[I,0]):OUT(B,L[I,1]):OUT(C,L[I,2]):OUT(D,L[I,3])
0290 OUT(E,L[I,4]):OUT(F,L[I,5]):OUT(G,L[I,6])
0300 MSLEEP(500)
0310 NEXT
0320 S=1:GOSUB 380
0330 OUT(H,0)
0340 MSLEEP(500)
0350 OUT(H,1)
0360 END
0370 :
0380 REM "--- Simultaneous LED control ---"
0390 OUT(A,S):OUT(B,S):OUT(C,S):OUT(D,S):OUT(E,S):OUT(F,S):OUT(G,S):OUT(H,S)
0400 RETURN
0410 :