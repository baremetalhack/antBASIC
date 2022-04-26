#--------------------------------------------------------------------------
# Makefile for antBASIC on the Raspberry Pi
#
# NOTICE  You need to pre-install "libreadline-dev", "atp" and
# 	  "ghostscript (includes ps2pdf)" packages before "make std|max".
#--------------------------------------------------------------------------
# PREP    $ sudo apt install libreadline-dev atp ghostscript
#--------------------------------------------------------------------------
# BUILD   $ make		--> main applications only
#         $ make std		--> main applications plus PDFs
#         $ make max		--> main and test applications plus PDFS
# INSTALL $ sudo make install	--> install main applications and MAN file
#--------------------------------------------------------------------------
# BAREMETALHACK.COM --> public domain
#--------------------------------------------------------------------------

CFLAGS := -Wall -Wextra
CFLAGS += -DREADLINE ### Build antBASIC with readline extension
CFLAGS += -DPI ### Build antBASIC targeted for Raspberry Pi
#CFLAGS += -DDEBUG ### Turn on in the case of debugging
LDLIBS := -lreadline ### Link with GNU Readline library
ATPFLAGS := -t 8 -date `date "+%Y-%m-%d"`
ATPFLAGS += -T A4 -M 1,1,2,2 cm	### Paper size and margin definition
#ATPFLAGS += -N ### With line number
GROFFFLAGS := -mandoc -Tps
GROFFFLAGS += -dpaper=a4 ### Paper size
TARGET_BIN := /usr/local/bin
TARGET_MAN := /usr/local/share/man/man1
TARGET_DOC := ./DOCUMENTS

HEADERS = byteword.h token.h bcode.h program.h container.h function.h eval.h basic.h debug.h escape.h
APPS = antbasic antcalib
TESTAPPS = test_token test_bcode test_prog test_eval test_assign
PDFS = $(TARGET_DOC)/src.pdf $(TARGET_DOC)/antbasic.pdf

.SECONDEXPANSION:

min: antbasic antcalib
std: antbasic antcalib pdfs
max: test_token test_bcode test_prog test_eval test_assign antbasic antcalib pdfs

test_token : $$@.c $(HEADERS) token.c 
	gcc $(CFLAGS) -o $@ $(filter %.c, $^)

test_bcode: $$@.c $(HEADERS) token.c bcode.c
	gcc $(CFLAGS) -o $@ $(filter %.c, $^)

test_prog: $$@.c $(HEADERS) token.c bcode.c program.c
	gcc $(CFLAGS) -o $@ $(filter %.c, $^)

test_eval: $$@.c $(HEADERS) token.c bcode.c container.c function.c eval.c pi_gpio.c
	gcc $(CFLAGS) -o $@ $(filter %.c, $^)

test_assign: $$@.c $(HEADERS) token.c bcode.c container.c function.c eval.c pi_gpio.c
	gcc $(CFLAGS) -o $@ $(filter %.c, $^)

antbasic: $(HEADERS) main.c token.c bcode.c program.c container.c function.c eval.c basic.h basic.c pi_gpio.c main.c
	gcc $(CFLAGS) -o $@ $(filter %.c, $^) -lreadline

antcalib: antcalib.c
	gcc $(CFLAGS) -o $@ antcalib.c

pdfs:
	atp $(ATPFLAGS) Makefile byteword.h debug.h escape.h token.h token.c test_token.c bcode.h bcode.c test_bcode.c program.h program.c test_prog.c container.h container.c function.h function.c eval.h eval.c test_eval.c test_assign.c basic.h basic.c pi_gpio.h pi_gpio.c main.c antcalib.c | ps2pdf - - > $(TARGET_DOC)/src.pdf
	groff $(GROFFFLAGS) antbasic.1 | ps2pdf - - > $(TARGET_DOC)/antbasic.pdf

install: antbasic antcalib
	install antbasic antcalib $(TARGET_BIN)
	install antbasic.1 $(TARGET_MAN)

clean:
	rm -f *.o $(APPS) $(TESTAPPS)

dist-clean:
	rm -f *.o $(APPS) $(TESTAPPS) $(PDFS)
	rm -f $(TARGET_BIN)/antbasic $(TARGET_BIN)/antcalib
	rm -f $(TARGET_MAN)/antbasic.1
