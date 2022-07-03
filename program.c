//
//  p r o g r a m . c
//
//  BAREMETALHACK.COM --> public domain
//

#include "program.h"
#include "bcode.h"
#include "escape.h"

//
// Global variables
//

  byte_t Program[ PROG_MAXSIZE + BCODE_MAXSIZE ]; // Last region (BCODE_MAXSIZE)
                                                  //     stores command line data
  line_t Lines[ PROG_MAXLINE + 2 ];     // First element is COMMAND LINE which
                                        //      contains ZERO
                                        // Last element is a GUARD which contains
                                        //      PROG_BLANKLINE
     int Psize;                         // Program size in BYTES
     int Lsize;                         // How many LINES are there?
     int Pnum;                          // Current processing line number

//
// File scope variables
//

static reloc_t Relocs[ PROG_MAXRELOC ];
    static int Rsize;

static char * Results[] = {
    "PROGRAM \"ok\"",                   // PROG_SUCCESS
    "PROGRAM \"line not found\"",       // PROG_LINENOTFOUND
    "PROGRAM \"too many lines\"",       // PROG_TOOMANYLINES
    "PROGRAM \"size over\"",            // PROG_SIZEOVER
    "PROGRAM \"too many relocations\""  // PROG_TOOMANYRELOCS
};

//
// Program line processing functions
//

void
prog_init(void)
{
    int i;

    // Initialize program area and line holders

    for (i = 0; i < (PROG_MAXSIZE + BCODE_MAXSIZE); i++)
        Program[ i ] = PROG_BLANKDATA;

    for (i = 0; i <= (PROG_MAXLINE + 1); i++) {
        Lines[ i ].num = PROG_BLANKLINE;
        Lines[ i ].add = 0;
        Lines[ i ].len = 0;
    }
    Lines[ 0 ].num = 0;

    // Initialize control variables

    Psize = Lsize = 0;
}

int
prog_search(int linenum)
{
    int i = 1, find = 0;

    while (Lines[ i ].num != PROG_BLANKLINE) {
        if (Lines[ i ].num == linenum) {
            find = 1;
            break;
        } else if (Lines[ i ].num > linenum)
            break;
        i++;
    }

    // NOTE: Returns INDEX number of Lines[]
    //        postive number -> found (linenum == Lines[i].num)
    //       negative number -> not found (linenum < Lines[-i].num)

    return find ? i : -i;
}

static void
spacing(int fd)
{
    if (bcode_nextiseol())
        return;
    else if (bcode_nextisdlm()) {
        if (bcode_next() != BCODE_AT)
            return;
    }
    dprintf(fd, " ");
}

int
prog_list(int fd, int color, int startnum, int endnum)
{
    int sidx, eidx, i, loop;
    bcode_t b;
    byte_t *ptr, ch, esc;

    if (Lsize == 0)
        return PROG_SUCCESS;

    sidx = prog_search(startnum);
    if (sidx < 0)
        return PROG_LINENOTFOUND;
    eidx = prog_search(endnum);
    if (eidx < 0)
        return PROG_LINENOTFOUND;

    bcode_start(Program);
    for (i = sidx; i <= eidx; i++) {
        dprintf(fd, "%04d ", Lines[ i ].num);
        bcode_setpc(Lines[ i ].add);
        loop = 1;
        while (loop) {
            bcode_read(&b);
            switch (b.type) {
                case BCODE_TYPE_EOL:
                    loop = 0;
                    break;

                case BCODE_TYPE_NUMBER10:
                    dprintf(fd, "%d", b.num);
                    spacing(fd);
                    break;

                case BCODE_TYPE_NUMBER16:
                    if (b.num & 0xFF00)
                        dprintf(fd, "0x%04hX", b.num);
                    else
                        dprintf(fd, "0x%02hX", b.num);
                    spacing(fd);
                    break;

                case BCODE_TYPE_STRING:
                    dprintf(fd, "\"");
                    ptr = b.str;
                    while ((ch = *ptr++)) {
                        esc = 0;
                        if (ch < ' ' || ch == '\\') {
                            switch (ch) {
                                case '\007':
                                    esc = 'a';
                                    break;

                                case '\010':
                                    esc = 'b';
                                    break;

                                case '\011':
                                     esc = 't';
                                    break;

                                case '\012':
                                    esc = 'n';
                                    break;

                                case '\015':
                                    esc = 'r';
                                    break;

                                case '\033':
                                    esc = 'e';
                                    break;

                                case '\\':
                                    esc = '\\';
                                    break;

                                default:
                                    esc = 'x';
                                    break;
                            }
                        }
                        if (esc) {
                            if (esc == 'x')
                                dprintf(fd, "\\x%02X", ch);
                            else
                                dprintf(fd, "\\%c", esc);
                        } else
                            dprintf(fd, "%c", ch);
                    }
                    dprintf(fd, "\"");
                    break;

                case BCODE_TYPE_VARIABLE:
                    if (color)
                        dprintf(fd, ESCAPE_LBLUE);
                    dprintf(fd, "%c", b.idx + 'A');
                    if (color)
                        dprintf(fd, ESCAPE_DEFAULT);
                    spacing(fd);
                    break;

                case BCODE_TYPE_KEYWORD:
                    if (color)
                        dprintf(fd, ESCAPE_LGREEN);
                    dprintf(fd, "%s", token_keyword(b.idx));
                    if (color)
                        dprintf(fd, ESCAPE_DEFAULT);
                    spacing(fd);
                    break;

                case BCODE_TYPE_DELIMITER:
                    if (color && b.inst == BCODE_AT)
                        dprintf(fd, ESCAPE_LBLUE);
                    dprintf(fd, "%s", token_delimiter(b.idx));
                    if (color && b.inst == BCODE_AT)
                        dprintf(fd, ESCAPE_DEFAULT);
                    if (b.inst == BCODE_RPAREN || b.inst == BCODE_RBRACK)
                        if (! bcode_nextisdlm() && ! bcode_nextiseol())
                            dprintf(fd, " ");
                    break;
            }
        }
        dprintf(fd, "\n");
    }
    return PROG_SUCCESS;
}

int
prog_delete(int startnum, int endnum)
{
    int sidx, eidx, dlines, dbytes, mbytes, i;
    byte_t *src, *dst;

    if (Lsize == 0)
        return PROG_LINENOTFOUND;

    sidx = prog_search(startnum);
    if (sidx < 0)
        return PROG_LINENOTFOUND;
    eidx = prog_search(endnum);
    if (eidx < 0)
        return PROG_LINENOTFOUND;

    dlines = eidx - sidx + 1;
    dbytes = 0;
    for (i = sidx; i <= eidx; i++)
        dbytes += Lines[ i ].len;
    
    if (eidx != Lsize) {
        dst = &Program[ Lines[ sidx ].add ];
        src = &Program[ Lines[ eidx + 1 ].add ];
        mbytes = 0;
        for (i = eidx + 1; i <= Lsize; i++)
            mbytes += Lines[ i ].len;
        
        // Transfer tail program
        while (mbytes--)
            *dst++ = *src++;

        // Transfer tail line holder
        for  (i = eidx + 1; i <= Lsize; i++) {
            Lines[ i - dlines ].num = Lines[ i ].num;
            Lines[ i - dlines ].add = Lines[ i ].add - dbytes;
            Lines[ i - dlines ].len = Lines[ i ].len;
        }
    }

    for (i = Lsize - dlines + 1; i <= Lsize; i++) {
        Lines[ i ].num = PROG_BLANKLINE;
        Lines[ i ].add = 0;
        Lines[ i ].len = 0;
    }

    Psize -= dbytes;
    Lsize -= dlines;

    return PROG_SUCCESS;
}

int
prog_insert(int linenum, byte_t *src, int len)
{
    byte_t *dst, *tsrc, *tdst;
    int idx, i, start, move;

    if (Lsize == PROG_MAXLINE)
        return PROG_TOOMANYLINES;
    if ((Psize  + len) > PROG_MAXSIZE)
        return PROG_SIZEOVER;

    idx = prog_search(linenum);
    if (idx > 0) {
        prog_delete(linenum, linenum);
        idx = prog_search(linenum);
    }
    idx = -idx;

    start = 0;
    for (i = 1; i < idx; i++) {
        start += Lines[ i ].len;
    }
    move = Psize - start + 1;
    dst = &Program[ start ];

    if (idx != (Lsize + 1)) {
        tsrc = &Program[ Psize - 1 ];
        tdst = tsrc + len;
        while (move--) {
            *tdst-- = *tsrc--;
        }
        for  (i = Lsize; i >= idx; i--) {
            Lines[ i + 1 ].num = Lines[ i ].num;
            Lines[ i + 1 ].add = Lines[ i ].add + len;
            Lines[ i + 1 ].len = Lines[ i ].len;
        }
    }

    move = len;
    while (move--) {
        *dst++ = *src++;
    }

    Lines[ idx ].num = linenum;
    Lines[ idx ].add = start;
    Lines[ idx ].len = len;
    Psize += len;
    Lsize++;

    return PROG_SUCCESS;
}

void
prog_cmdline(byte_t *src, int size)
{
    int i;

    for (i = 0; i < size; i++)
        Program[ PROG_CMDLINE + i ] = *src++;
    Lines[ 0 ].num = 0;
    Lines[ 0 ].add = PROG_CMDLINE;
    Lines[ 0 ].len = size;
}

int
prog_renum(int start, int step)
{
    int i, add, scan, idx;
    bcode_t b;
    byte_t *ptr;

    Rsize = 0;
    for (i = 1; i <= Lsize; i++) {
        Pnum = Lines[ i ].num;
        add = Lines[ i ].add;
        bcode_start(&Program[ add ]);
        scan = 1;
        while (scan) {
            bcode_read(&b);
            switch (b.type) {
                case BCODE_TYPE_EOL:
                    scan = 0;
                    break;

                case BCODE_TYPE_NUMBER10:
                case BCODE_TYPE_NUMBER16:
                case BCODE_TYPE_STRING:
                case BCODE_TYPE_VARIABLE:
                case BCODE_TYPE_DELIMITER:
                    break;

                case BCODE_TYPE_KEYWORD:
                    if (b.inst == BCODE_GOSUB || b.inst == BCODE_GOTO) {
                        bcode_read(&b);
                        idx = prog_search(b.num);
                        if (idx < 0)
                            return PROG_LINENOTFOUND;
                        if ((Rsize + 1) > PROG_MAXRELOC)
                            return PROG_TOOMANYRELOCS;
                        Relocs[ Rsize ].idx = idx;
                        Relocs[ Rsize ].add = add + b.pos;
                        Rsize++;
                    }
                    break;
            }
        }
    }

    // Do renumber

    for (i = 1; i <= Lsize; i++)
        Lines[ i ].num = start + step * (i - 1);

    // Do relocation

    for (i = 0; i < Rsize; i++) {
        ptr = &Program[ Relocs[ i ].add ];
        ptr++;
        *((word_t*) ptr) = (word_t) Lines[ Relocs[ i ].idx ].num;
    }

    return PROG_SUCCESS;
}

char *
prog_result(int idx)
{
    return Results[ idx ];
}

#define BYTES_PERLINE    20
void
prog_dumpbytes(void)
{
    int i;

    printf("%sProgram contents%s\n", ESCAPE_LGREEN, ESCAPE_DEFAULT);
    printf("    Program size = %d bytes\n", Psize);
    for (i = 0; i < Psize; i++) {
        if ((i % BYTES_PERLINE) == 0)
            printf("    0d%05d : ", i);
        printf("%02hX ", Program[ i ]);
        if ((i % BYTES_PERLINE) == (BYTES_PERLINE - 1))
            printf("\n");
    }
    if ((i % BYTES_PERLINE) != (BYTES_PERLINE - 1))
        printf("\n");
}
void
prog_dumplines(void)
{
    int i;

    printf("%sLine information%s\n", ESCAPE_LGREEN, ESCAPE_DEFAULT);
    printf("    Total %d lines\n", Lsize);
    for (i = 1; i <= Lsize; i++) {
        printf("    Lines[%d].num = %d : Lines[%d].add = %d : ", \
                i, Lines[ i ].num, i, Lines[ i ].add);
        printf("Lines[%d].len = %d\n", i, Lines[ i ].len);
    }
}
