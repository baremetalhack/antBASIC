0010 REM "MICROTREK GAME"
0020 :
0030 DIM Z[7,7]:REM "Sector map 0:space, 1:star, 2:Klingon, 3:Enterprise"
0040 :
0050 PRINT
0060 PRINT "*** MICROTREK by Prof Haruhisa Ishida, 1978 ***"
0070 D=RND()/384+2200:REM "Star date (2200~2285)"
0080 Q=RND()/1639+1:REM "Maximum number of stars and Klingons (1~20)"
0090 L=255:REM "Energy"
0100 W=0:REM "Number of Klingons"
0110 T=0:REM "Total number of stars and Klingons"
0120 FOR X=0 TO 6
0130 FOR J=0 TO 6
0140 Z[X,J]=0
0150 IF RND()/128<170 GOTO 200:REM "33% chance of star/Klingon creation"
0160 IF T==Q GOTO 140
0170 T=T+1
0180 Z[X,J]=RND()/19660+1:REM "1)star or 2)Klingon"
0190 IF Z[X,J]==2 W=W+1
0200 NEXT
0210 NEXT
0220 IF W==0 GOTO 110
0230 E=RND()/4864:REM "Enterprise sector X position (0~6)"
0240 F=RND()/4864:REM "Enterprise sector Y position (0~6)"
0250 Z[E,F]=3
0260 :
0270 D=D+1
0280 PRINT
0290 PRINT "  1234567X"
0300 X=0
0310 GOSUB 1240
0320 PRINT " <=ENTERPRISE=>"
0330 X=1
0340 GOSUB 1240
0350 PRINT
0360 X=2
0370 GOSUB 1240
0380 PRINT " STARDATE: ";D
0390 X=3
0400 GOSUB 1240
0410 PRINT " SECTOR: Y=";E+1;",X=";F+1
0420 X=4
0430 GOSUB 1240
0440 PRINT " ENERGY: ";L
0450 X=5
0460 GOSUB 1240
0470 PRINT " KLINGONS: ";W
0480 X=6
0490 GOSUB 1240
0500 IF W==0 PRINT " CONDITION: BLUE":GOTO 520
0510 PRINT " CONDITION: RED"
0520 PRINT "Y 1234567X"
0530 PRINT
0540 :
0550 IF W==0 GOTO 910
0560 IF D==255 GOTO 930:REM "Time Over!"
0570 IF RND()/128>25 GOTO 610:REM "90% chance of attack free"
0580 H=RND()/6400+2:REM "Attack unit from Klingon (2~7)"
0590 PRINT H;" UNIT HIT ROM KLINGONS\a"
0600 L=L-H
0610 IF L<0 GOTO 950:REM "Energy Out!"
0620 :
0630 PRINT "CAPTAIN: 1)fire 2)move 3)warp 4)exit ";
0640 INPUT A
0650 IF A==1 GOTO 990:REM "Photon torpedo"
0660 IF A==2 GOTO 710:REM "Go to new sector"
0670 IF A==3 GOTO 1200:REM "Warp to another space"
0680 IF A!=4 GOTO 630
0690 END
0700 :
0710 PRINT "WHAT SECTOR(Y,X) TO GO TO"
0720 PRINT "Y ";:INPUT M
0730 IF M<1 GOTO 720
0740 IF M>7 GOTO 720
0750 M=M-1
0760 PRINT "X ";:INPUT N
0770 IF N<1 GOTO 760
0780 IF N>7 GOTO 760
0790 N=N-1
0800 G=(M-E)*(M-E)+(N-F)*(N-F)
0810 IF Z[M,N]!=0 GOTO 890
0820 Z[E,F]=0
0830 Z[M,N]=3
0840 E=M
0850 F=N
0860 L=L-G
0870 GOTO 270
0880 :
0890 PRINT "<<< CAN'T GO >>>\a"
0900 GOTO 710
0910 PRINT "<<< YOU WIN! >>>\a"
0920 GOTO 50
0930 PRINT "<<< TIME UP! >>>\a"
0940 GOTO 960
0950 PRINT "<<< NO ENERGY! >>>"
0960 PRINT "<<< YOU ARE DEAD...>>>\a"
0970 GOTO 50
0980 :
0990 PRINT "WHAT SECTOR(Y,X) TO FIRE AT"
1000 PRINT "Y ";:INPUT M
1010 IF M<1 GOTO 1000
1020 IF M>7 GOTO 1000
1030 M=M-1
1040 PRINT "X ";:INPUT N
1050 IF N<1 GOTO 1040
1060 IF N>7 GOTO 1040
1070 N=N-1
1080 G=(M-E)*(M-E)+(N-F)*(N-F)
1090 IF RND()/128<30 GOTO 1140:REM "12% chance of miss-fire"
1100 IF Z[M,N]==2 GOTO 1160
1110 PRINT "SPOCK: YOU MISSED. TO ERR IS HUMAN."
1120 L=L-2*G
1130 GOTO 270
1140 PRINT "<<< MISS-FIRED >>>"
1150 GOTO 1120
1160 PRINT "<<< KLINGON DESTROYED! >>>\a"
1170 Z[M,N]=0
1180 W=W-1
1190 GOTO 1120
1200 L=L-6
1210 GOTO 100
1220 :
1230 REM "MAKE UP GALAXY MAP"
1240 PRINT X+1;" ";
1250 FOR J=0 TO 6
1260 C=Z[X,J]
1270 IF C==0 PRINT " ";
1280 IF C==1 PRINT "*";
1290 IF C==2 PRINT "K";
1300 IF C==3 PRINT "E";
1310 NEXT
1320 RETURN
