0010 REM "Morse Coder"
0020 REM "BAREMETALHACK.COM --> Public doman"
0030 :
0040 REM "=== GPIO Mapping and Constant ==="
0050 B=10:REM "Buzzer pin => BMH10"
0060 T=100:REM "Timing unit => 100msec"
0070 :
0080 GOSUB 1000
0090 DIM L[64]:REM "Letter holder"
0100 :
0110 PRINT "Message ";
0120 INPUT @
0130 IF @[0]==0 END
0140 I=-1:J=-1
0150 I=I+1:J=J+1
0160 C=@[I]
0170 IF C==0 L[J]=0:GOTO 280
0180 IF C==0x20 L[J]=0x20::GOTO 150
0190 IF C>0x7A GOTO 250
0200 IF C<0x41 GOTO 250
0210 IF C>=0x61 L[J]=C-0x20:GOTO 150
0220 IF C>0x5A GOTO 250
0230 L[J]=C:GOTO 150
0240 :
0250 PRINT "Illegal character!":PRINT
0260 GOTO 110
0270 :
0280 I=0
0290 C=L[I]:I=I+1
0300 IF C==0 PRINT:PRINT:GOTO 110
0310 @[0]=C:@[1]=0:PRINT @;
0320 IF C==0x20 GOSUB 2400:GOTO 290
0330 C=C-0x41
0340 GOSUB 1400
0350 GOSUB 2300
0360 GOTO 290
0370 :
1000 REM "--- Morse coder setup ---"
1030 DIM M[26,5]:REM "Morse code holder: 0=short, 1=long, 9=end"
1040 M[0,0]=0,1,9:REM "A"
1050 M[1,0]=1,0,0,0,9:REM "B"
1060 M[2,0]=1,0,1,0,9:REM "C"
1070 M[3,0]=1,0,0,9:REM "D"
1080 M[4,0]=0,9:REM "E"
1090 M[5,0]=0,0,1,0,9:REM "F"
1100 M[6,0]=1,1,0,9:REM "G"
1110 M[7,0]=0,0,0,0,9:REM "H"
1120 M[8,0]=0,0,9:REM "I"
1130 M[9,0]=0,1,1,1,9:REM "J"
1140 M[10,0]=1,0,1,9:REM "K"
1150 M[11,0]=0,1,0,0,9:REM "L"
1160 M[12,0]=1,1,9:REM "M"
1170 M[13,0]=1,0,9:REM "N"
1180 M[14,0]=1,1,1,9:REM "O"
1190 M[15,0]=0,1,1,0,9:REM "P"
1200 M[16,0]=1,1,0,1,9:REM "Q"
1210 M[17,0]=0,1,0,9:REM "R"
1220 M[18,0]=0,0,0,9:REM "S"
1230 M[19,0]=1,9:REM "T"
1240 M[20,0]=0,0,1,9:REM "U"
1250 M[21,0]=0,0,0,1,9:REM "V"
1260 M[22,0]=0,1,1,9:REM "W"
1270 M[23,0]=1,0,0,1,9:REM "X"
1280 M[24,0]=1,0,1,1,9:REM "Y"
1290 M[25,0]=1,1,0,0,9:REM "Z"
1300 RETURN
1310 :
1400 REM "--- Letter handler ---"
1410 J=0
1420 IF M[C,J]==0:GOSUB 2000:GOTO 1440
1430 IF M[C,J]==1:GOSUB 2100
1440 GOSUB 2200
1450 J=J+1
1460 IF M[C,J]!=9 GOTO 1420
1470 RETURN
1480 :
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