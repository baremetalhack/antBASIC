0010 REM "DIP Switch"
0020 REM "BAREMETALHACK.COM --> Public domain"
0030 :
0040 REM "=== GPIO Mapping ==="
0050 M=3:N=4:O=5:P=6:REM "DIP Switches => BMH3-BMH6"
0060 :
0070 FOR I=1 TO 20
0080 GOSUB 1000
0090 PRINT HEX2(Q)
0100 MSLEEP(500)
0110 NEXT
0120 END
0130 :
1000 REM "DIP Switch Reader (pin assignment: M-P, return: Q=value)"
1010 Q=0
1020 IF IN(M)==0 Q=Q+8
1030 IF IN(N)==0 Q=Q+4
1040 IF IN(O)==0 Q=Q+2
1050 IF IN(P)==0 Q=Q+1
1060 RETURN
