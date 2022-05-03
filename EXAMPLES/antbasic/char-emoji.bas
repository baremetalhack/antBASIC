0010 REM "Emoji Dump"
0020 REM "NOTICE: You must pre-install Noto-Color-Emoji font package."
0030 REM "    $ sudo apt install fonts-noto-color-emoji"
0040 REM "    Restart LXTerminal, and execute this program."
0050 REM "BAREMETALHACK.COM --> Public domain"
0060 :
0070 @[0]=0xF0:@[1]=0x9F:@[2]=0x98:@[3]=0x00:@[4]=0x00
0080 FOR I=0x81 TO 0xBF
0090 @[3]=I
0100 PRINT @;" ";
0110 NEXT
0120 PRINT
0130 END
