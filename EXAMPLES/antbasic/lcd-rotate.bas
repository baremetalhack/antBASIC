0010 REM "LCD Rotate Screen"
0020 REM "BAREMETALHACK.COM --> Public domain"
0030 :
0040 GOSUB 5100:REM "Startup"
0050 GOSUB 6500:REM "Initialize"
0060 GOSUB 5900:REM "Clear display"
0070 A[0]=1:A[1]=1:A[2]=1:GOSUB 5700:REM "Display control"
0080 @="*  Welcome to antBASIC!  * Bare  Metal *"
0090 GOSUB 6700:REM "Print string"
0100 @="* Presented by DoctorBMH *  \x7FHacking\x7E  *"
0110 GOSUB 6700:REM "Print string"
0120 MSLEEP(2500)
0130 FOR I=1 TO 25
0140 A[0]=0:A[1]=1:GOSUB 5800:REM "Shift control"
0150 MSLEEP(500)
0160 NEXT
0170 END
0180 :
5000 REM "============================================="
5002 REM "=== Hitachi HD44780 LCD controller module ==="
5004 REM "============================================="
5006 REM "                                             "
5008 REM "  GPIO wiring:                               "
5010 REM "   HD44780      Pi                           "
5011 REM "      Vss  <-- GND                           "
5012 REM "      Vcc  <-- 3.3V                          "
5014 REM "      RS   --> BMH4 (RS is re-named as D/I)  "
5015 REM "      R/W  --> BMH5                          "
5016 REM "      E    --> BMH6                          "
5018 REM "      DB7  --> BMH7                          "
5020 REM "      DB6  --> BMH8                          "
5022 REM "      DB5  --> BMH9                          "
5024 REM "      DB4  --> BMH10                         "
5026 REM "      DB3  --> BMH11                         "
5028 REM "      DB2  --> BMH12                         "
5030 REM "      DB1  --> BMH13                         "
5032 REM "      DB0  --> BMH14                         "
5034 REM "                                             "
5036 REM "  Bit mapping:                               "
5038 REM "   HD44780     BASIC                         "
5040 REM "      D/I  --> M[10]  0:Instruction, 1:Data  "
5042 REM "      R/W  --> M[9]   0:Write, 1:Read        "
5044 REM "      E    --> M[8]   0:Disable, 1:Enable    "
5046 REM "      DB7  --> M[7]                          "
5048 REM "      DB6  --> M[6]                          "
5050 REM "      DB5  --> M[5]                          "
5052 REM "      DB4  --> M[4]                          "
5054 REM "      DB3  --> M[3]                          "
5056 REM "      DB2  --> M[2]                          "
5058 REM "      DB1  --> M[1]                          "
5060 REM "      DB0  --> M[0]                          "
5062 REM "                                             "
5064 REM "  Subroutines:                               "
5066 REM "      5100 = Setup                           "
5067 REM "      5200 = Read Busy Flag and Address      "
5068 REM "      5300 = Busy Wait                       "
5069 REM "      5400 = Update Instruction Register     "
5070 REM "      5500 = Function Set                    "
5071 REM "      5600 = Entry Mode Set                  "
5072 REM "      5700 = Display Control                 "
5073 REM "      5800 = Cursor and Shift Control        "
5074 REM "      5900 = Clear Display                   "
5075 REM "      6000 = Return Home                     "
5076 REM "      6100 = Set CGRAM Address               "
5077 REM "      6200 = Set DDRAM Address               "
5078 REM "      6300 = Read Data                       "
5079 REM "      6400 = Write Data                      "
5080 REM "      6500 = HD44780 Initialize              "
5081 REM "      6600 = Locate                          "
5082 REM "      6700 = Print String                    "
5083 REM "      6800 = Set Original Character          "
5084 REM "                                             "
5098 REM "=========================== Public domain ==="
5099 :
5100 REM "--- Setup ---"
5102 REM "    Declare A[]"
5104 REM "    Define M[]"
5106 DIM A[1,3]:REM "subroutine Arguments"
5108 DIM M[1,11]:REM "GPIO Mapping (DB0-DB7,E,R/W,D/I)"
5110 M[0]=7,8,9,10,11,12,13,14,6,5,4
5112 REM "DB0-DB7=0, E=0, R/W=0, D/I=0"
5114 OUT(M[0],0):OUT(M[1],0):OUT(M[2],0):OUT(M[3],0)
5116 OUT(M[4],0):OUT(M[5],0):OUT(M[6],0):OUT(M[7],0)
5118 OUT(M[8],0):OUT(M[9],0):OUT(M[10],0)
5120 RETURN
5122 :
5200 REM "--- Read Busy-flag and Address ---"
5202 REM "    Return B:Busy-flag"
5204 REM "           C:current address Counter"
5206 OUT(M[10],0):OUT(M[9],1):REM "D/I=0, R/W=1"
5208 OUT(M[8],1):REM "E=1"
5210 USLEEP(1):REM "Data Delay Time = max 360nsec"
5212 B=0:IF IN(M[7]) B=1
5214 C=0
5216 IF IN(M[6]) C=C+0x0040
5218 IF IN(M[5]) C=C+0x0020
5220 IF IN(M[4]) C=C+0x0010
5222 IF IN(M[3]) C=C+0x0008
5224 IF IN(M[2]) C=C+0x0004
5226 IF IN(M[1]) C=C+0x0002
5228 IF IN(M[0]) C=C+0x0001
5230 OUT(M[8],0):REM "E=0"
5232 RETURN
5234 :
5300 REM "--- Busy wait ---"
5302 REM "    Argument W:Wait counter"
5304 GOSUB 5200
5306 IF B W=W+1:MSLEEP(1):GOTO 5304
5308 RETURN
5310 :
5400 REM "--- Updade Instruction Register ---"
5402 REM "    Argument S:D/I, R:R/W, D:Data"
5404 REM "    Work V"
5406 GOSUB 5300
5408 OUT(M[10],S):OUT(M[9],R)
5410 OUT(M[8],1):REM "E=1"
5412 V=0:IF D&0x0080 V=1
5414 OUT(M[7],V)
5416 V=0:IF D&0x0040 V=1
5418 OUT(M[6],V)
5420 V=0:IF D&0x0020 V=1
5422 OUT(M[5],V)
5424 V=0:IF D&0x0010 V=1
5426 OUT(M[4],V)
5428 V=0:IF D&0x0008 V=1
5430 OUT(M[3],V)
5432 V=0:IF D&0x0004 V=1
5434 OUT(M[2],V)
5436 V=0:IF D&0x0002 V=1
5438 OUT(M[1],V)
5440 V=0:IF D&0x0001 V=1
5442 OUT(M[0],V)
5444 USLEEP(1):REM "Data Setup from Write = min 195nsec"
5446 OUT(M[8],0):REM "E=0"
5448 RETURN
5450 :
5500 REM "--- Function Set ---"
5502 REM "    Argument A[0]:F Font size (1=5x10, 0=5x8)"
5504 REM "             A[1]:N Number of lines (1=two lines, 0=single line)"
5506 REM "             A[2]:DL Data Length (1=eight-bits, 0=four-bits)"
5508 REM "NOTICE: Busy flag can't be checked before this instruction"
5510 D=0x0020
5512 IF A[0] D=D+4
5514 IF A[1] D=D+8
5516 IF A[2] D=D+16
5518 S=0:R=0:GOSUB 5400
5520 RETURN
5522 :
5600 REM "--- Entry Mode Set ---"
5602 REM "    Argument A[0]:S (1=enable Shift, 0=disable Shift)"
5604 REM "             A[1]:I/D (1=increment, 0=decrement)"
5606 GOSUB 5300
5608 D=0x0004
5610 IF A[0] D=D+1
5612 IF A[1] D=D+2
5614 S=0:R=0:GOSUB 5400
5616 RETURN
5618 :
5700 REM "--- Display Control ---"
5702 REM "    Argument A[0]:Blink, A[1]:Cursor, A[2]:Display (1=on, 0=off)"
5704 GOSUB 5300
5706 D=0x0008
5708 IF A[0] D=D+1
5710 IF A[1] D=D+2
5712 IF A[2] D=D+4
5714 S=0:R=0:GOSUB 5400
5716 RETURN
5718 :
5800 REM "--- Cursor and Shift Control ---"
5802 REM "    Argument A[0]:R/L (1=Right, 0=Left)"
5804 REM "             A[1]:S/C (1=display Shift, 0=Cursor move)"
5806 GOSUB 5300
5808 D=0x0010
5810 IF A[0] D=D+4
5812 IF A[1] D=D+8
5814 S=0:R=0:GOSUB 5400
5816 RETURN
5818 :
5900 REM "--- Clear Display ---"
5902 GOSUB 5300
5904 D=0x0001
5906 S=0:R=0:GOSUB 5400
5908 RETURN
5910 :
6000 REM "--- Return Home ---"
6002 GOSUB 5300
6004 D=0x0002
6006 S=0:R=0:GOSUB 5400
6008 RETURN
6010 :
6100 REM "--- Set CGRAM Address ---"
6102 REM "    Argument A:CGRAM Address"
6104 GOSUB 5300
6106 D=0x0040+A
6108 S=0:R=0:GOSUB 5400
6110 RETURN
6112 :
6200 REM "--- Set DDRAM Address ---"
6202 REM "    Argument A:DDRAM Address"
6204 GOSUB 5300
6206 D=0x0080+A
6208 S=0:R=0:GOSUB 5400
6210 RETURN
6212 :
6300 REM "--- Read Data ---"
6302 REM "    Return D:Data"
6304 GOSUB 5300
6306 OUT(M[10],1):OUT(M[9],1):REM "D/I=1, R/W=1"
6308 OUT(M[8],1):REM "E=1"
6310 USLEEP(1):REM "Data Delay Time = max 360nsec"
6312 D=0
6314 IF IN(M[7]) D=D+0x0080
6316 IF IN(M[6]) D=D+0x0040
6318 IF IN(M[5]) D=D+0x0020
6320 IF IN(M[4]) D=D+0x0010
6322 IF IN(M[3]) D=D+0x0008
6324 IF IN(M[2]) D=D+0x0004
6326 IF IN(M[1]) D=D+0x0002
6328 IF IN(M[0]) D=D+0x0001
6330 OUT(M[8],0):REM "E=0"
6332 RETURN
6334 :
6400 REM "--- Write Data ---"
6402 REM "    Argument D:Data"
6404 GOSUB 5300
6406 S=1:R=0:GOSUB 5400
6408 RETURN
6410 :
6500 REM "--- HD44780 Initialize ---"
6502 A[0]=0:A[1]=1:A[2]=1:GOSUB 5500:MSLEEP(5):REM "1st function set DL=8"
6504 A[0]=0:A[1]=1:A[2]=1:GOSUB 5500:USLEEP(200):REM "2nd function set DL=8"
6506 A[0]=0:A[1]=1:A[2]=1:GOSUB 5500:REM "3rd function set DL=8"
6508 A[0]=0:A[1]=1:A[2]=1:GOSUB 5500:REM "Final function set DL=8"
6510 A[0]=0:A[1]=0:A[2]=0:GOSUB 5700:REM "Display all off"
6512 GOSUB 5900:REM "Clear display"
6514 A[0]=0:A[1]=1:GOSUB 5600:REM "Entry mode set, shift disable, increment"
6516 RETURN
6518 :
6600 REM "--- Locate ---"
6602 REM "    Argument X:horizontal pos (0-15), Y:vertical pos (0-1)"
6604 IF X<0 X=0
6606 IF X>15 X=15
6608 IF Y<0 Y=0
6610 IF Y>1 Y=1
6612 A=Y*0x0040+X:GOSUB 6200:REM "Set DDRAM address"
6614 RETURN
6616 :
6700 REM "--- Print String ---"
6702 REM "    Argument @:string"
6704 REM "    Work Z"
6706 Z=0
6708 IF @[Z]==0 RETURN
6710 D=@[Z]:GOSUB 6400:REM "Write data"
6712 Z=Z+1
6714 GOTO 6708
6716 :
6800 REM "--- Set Original Character ---"
6802 REM "    Argument A: character code (1-7)"
6804 REM "             B[8]: character data"
6806 A=A*8:GOSUB 6100:REM "Set CGRAM address"
6808 FOR I=0 TO 7
6810 D=B[I]:GOSUB 6400:REM "Write data"
6812 NEXT
6814 RETURN
6816 :
