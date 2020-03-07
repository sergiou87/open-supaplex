// include GLOBALS.INC

// ; ===========================================================================
// code segment para public 'CODE' use16
//         assume cs:code

// ; =============== S U B R O U T I N E =======================================

// ; Attributes: noreturn

const int levelDataLength = 1536;

//         public start
void main(int argc, char **argv)
{
//; FUNCTION CHUNK AT 027F SIZE 00000217 BYTES

        // mov dx, seg data
        // mov ds, dx
        // assume ds:data

programStart:               //; CODE XREF: runMainMenu+28Fp
                    //; DATA XREF: data:000Ao
        //;db 26h, 8Ah, 0Eh, 80h, 00h
        cl = *(es:80h); // 0x80 -> Number of bytes on command-line (https://en.wikipedia.org/wiki/Program_Segment_Prefix)
        cbCommandLine = cl
        if (cl >= 2)
        {
            goto hasCommandLine
        }
        goto doesNotHaveCommandLine
//// ; ---------------------------------------------------------------------------

hasCommandLine:             //; CODE XREF: start+11j
        //cld
        ch = 0; // cx = number of command line bytes, this cleans cx MSBs
        di = 0x81; // '?' start of command line args in PSP (https://en.wikipedia.org/wiki/Program_Segment_Prefix)
        push(di);
        push(cx);

readAndProcessCommandLine:
        si = &commandLine

// Copy char by char the command line into commandLine
copyNextCmdLineByte:            //; CODE XREF: start+28j
        al = es:[di];
        *si = al;
        si++;
        di++;
        cx--;
        if (cx > 0)
        {
            goto copyNextCmdLineByte
        }
        pop(cx) // recovers original value (number of command line bytes)
        pop(di)
        di = ds
        es = di
        // assume es:data
        di = &commandLine
        push(di)
        push(cx) // saves number of cmd line bytes again

processCommandLine:         //; CODE XREF: start+F7j
        if (strchr(commandLine, ':') == NULL)
        {
            goto nohascolon;
        }
        else
        {
            goto hascolon;
        }
//// ; ---------------------------------------------------------------------------

hascolon:                //; CODE XREF: start+39j
        al = 0x20; // ' ' (space)
        si = &demoFileName;

// This copies the argument to demoFileName, stops when it finds a space or the end of the command line
copyArgument:              //; CODE XREF: start+52j
        if (cx == 0) // check if the : was found in the last byte of the command line (cx was used to iterate through it)
        {
            goto finishArgumentCopy;
        }
        ah = es:[di];
        if (ah == al)
        {
            goto finishArgumentCopy;
        }
        *si = ah;
        si++;
        di++;
        cx--;
        if (cx > 0)
        {
            goto copyArgument;
        }

finishArgumentCopy:              //; CODE XREF: start+45j start+4Cj
        al = 0;
        *si = al; // Adds \0 at the end?
        pop(cx); // recover number of cmd line bytes
        pop(di); // recover command line string
        push(di); // save command line string
        push(cx); // save number of cmd line bytes
        al = 0x3A; // ':'
        di = strchr(di, ':'); //repne scasb
        di--;
        cx++;
        al = 0x20; // ' ' (espacio)

// No idea why (yet) but this replaces the argument that was just copied with spaces :shrug:
removeArgumentByteFromCommandLine:              //; CODE XREF: start+6Aj
        if (es:[di] == al)
        {
            goto openDemoFile;
        }
        es:[di] = al; // stosb
        cx--;
        if (cx > 0)
        {
            goto removeArgumentByteFromCommandLine;
        }

openDemoFile:              //; CODE XREF: start+67j
        ax = 0x3D00;
        dx = &demoFileName;
        FILE *file = fopen(demoFileName, 'r');
        // int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
        //             ; DS:DX -> ASCIZ filename
        //             ; AX = action -> 0x3D - open file
        //             ; AL = access mode -> 0 - read
        if (file == NULL)
        {
            goto errorReadingDemoFile;
        }
        bx = file;
        push(bx);

loc_46C99:
        ax = 0x4202;
        cx = 0;
        dx = cx;
        int result = fseek(file, 0, SEEK_END);
        // int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
        //             ; AL = method: 0x02 -> offset from end of file
        //             ; dx:ax is the new seek position
        pop(bx);
        pushf();
        if (result < 0) // jb  short errorSeekingDemoFile
        {
            goto errorSeekingDemoFile
        }
        
        if (dx != 0)
        {
            goto errorSeekingDemoFile
        }

loc_46CAA:
        if (ax < levelDataLength)
        {

loc_46CAD:
            goto loc_46CB7;
        }

errorSeekingDemoFile:              //; CODE XREF: start+84j start+88j
        push(ax);
        push(dx);
        ah = 0x3E;
        fclose(file);
        // int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
        //             ; BX = file handle
        pop(dx);
        pop(ax);

loc_46CB7:              //; CODE XREF: start:loc_46CADj
        db 8Dh, 36h, 65h, 9Fh
        si = **aFileDemo; //;lea si, [aFileDemo] ; "!! File >> Demo: "
        
        word_599DA = 0
        popf();
        
        jb  short errorReadingDemoFile
        if (dx > 0)
        {
            goto loc_46CE4;
        }
        if (ax > 0xC60A)
        {
            goto loc_46CE4;
        }
        si = *aFileLevel; // lea si, aFileLevel    ; "!! File < Level: "

loc_46CD1:
        if (ax >= levelDataLength)
        {
            goto loc_46CF4;
        }
        fileReadUnk1();
        word_599DA = ax;
        pushf();
        ah = 0x3E;
        fclose(file);
        // int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
        //             ; BX = file handle
        popf();
        // jnb short loc_46CF6  // where do the flags come from??

loc_46CE4:              //; CODE XREF: start+A6j start+ABj
        pop(cx);
        pop(di);
        push(di);
        push(cx);
        al = 0x40; // '@'
        if (strchr(some_string, '@') == NULL)
        {
            goto errorReadingDemoFile;
        }
        
        conprintln();
        goto immediateexit;
// ; ---------------------------------------------------------------------------

loc_46CF4:              //; CODE XREF: start+B4j
        if (ax == levelDataLength)
        {
            goto loc_46CFB;
        }

loc_46CF6:              //; CODE XREF: start+C2j
        byte_599D5 = 1;

loc_46CFB:              //; CODE XREF: start:loc_46CF4j
        if (demoFileName == 0)
        {
            goto errorReadingDemoFile;
        }
        
        byte_599D4 = 2;
        goto loc_46D13;
// ; ---------------------------------------------------------------------------

errorReadingDemoFile:              //; CODE XREF: start+74j start+A2j ...
        demoFileName = 0;
        byte_599D4 = 0;

loc_46D13:              //; CODE XREF: start+E7j
        pop(cx);
        pop(di);
        push(di);
        push(cx);
        goto processCommandLine;
// ; ---------------------------------------------------------------------------

nohascolon:               ; CODE XREF: start+3Bj
        pop(cx);
        pop(di);
        push(di);
        push(cx);
        al = '@';
        int hasAt = strch(some_string, '@'); // repne scasb
        pop(cx);
        pop(di);
        if (hasAt == NULL)
        {
            goto runSpFile;
        }
        //; When @ is specd we need a valid file
        if (demoFileName != 0)
        {
            goto demoFileNotMissing;
        }
        si = *a@ErrorBadOrMis; // "\"@\"-ERROR: Bad or missing \":filename"...
        conprint();
        goto immediateexit;
// ; ---------------------------------------------------------------------------

demoFileNotMissing:              //; CODE XREF: start+10Bj
        if (byte_599D5 == 1)
        {
            goto spHasAtAndDemo;
        }
        si = *aSpWithoutDemo; //"SP without demo: "
        conprintln();
        goto immediateexit;
// ; ---------------------------------------------------------------------------

spHasAtAndDemo:              //; CODE XREF: start+11Cj
        fastMode = 1;
        speed? = 0

runSpFile:              //; CODE XREF: start+104j
        push(di);
        push(cx);
        al = 0x41h; // 'A'
        goto loc_46D5A;
// ; ---------------------------------------------------------------------------

loc_46D58:              ; CODE XREF: start+14Fj
        al = 0x61; // 'a'

loc_46D5A:              ; CODE XREF: start+136j
        bx = *word_59B60;

loc_46D5E:              ; CODE XREF: start+14Dj start+153j
        int result = strchr(some_string, 'a');
        if (result == NULL)
        {
            goto loc_46D64;
        }
        [bx] = [bx] | al; // check if whatever is in bx is 'a'??

loc_46D64:              //; CODE XREF: start+140j
        pop(cx);
        pop(di);
        push(di);
        push(cx);
        bx++;
        al++;
        if (al < 0x5B) // '['
        {
            goto loc_46D5E;
        }
        else if (al == 0x5B) // '['
        {
            goto loc_46D58;
        }
        
        if (al < 0x7B) // '{'
        {
            goto loc_46D5E;
        }
        
        if (byte_59B63 == 0)
        {
            goto loc_46D82;
        }
        word_51970 = 1;

loc_46D82:              //; CODE XREF: start+15Aj
        al = 0x21; // '!'
        int result = strchr(some_string, '!');
        if (result == NULL)
        {
            goto loc_46DBF;
        }
        
        if (cx < 1)
        {
            goto loc_46DBF;
        }
        ax = es:[di];
        if (al < 0x30) // '0'
        {
            goto loc_46DBF;
        }
        
        if (al > 0x39) // '9'
        {
            goto loc_46DBF;
        }

        if (ah < 0x30) // '0'
        {
            goto loc_46DA2;
        }

        if (ah <0 0x39) // '9'
        {
            goto modifysetfns;
        }

loc_46DA2:              //; CODE XREF: start+17Bj
        ah = al;
        al = 0x30; // '0'

modifysetfns:              //; CODE XREF: start+180j
        // This changes the suffix of the files with whatever is in AX? like LEVELS.DAT with LEVELS.D00 or LEVEL.LST with LEVEL.L10 ??
        aLevels_dat_0[8] = ax; // "AT"
        aLevel_lst[7] = ax; // "ST"
        aDemo0_bin[7] = ax; // "IN"
        aPlayer_lst[8] = ax; // "ST"
        aHallfame_lst[0Ah] = ax; // "ST"
        if (*dword_59B76 != 0) // cmp byte ptr dword_59B76, 0
        {
            goto loc_46DBF;
        }
        
        aSavegame_sav[0Ah] = ax; //"AV"

loc_46DBF:              //; CODE XREF: start+166j start+16Bj ...
        pop(cx);
        pop(di);
        push(di);
        push(cx);
        al = 0x23; //'#'
        
        if (strchr(something, '#') == NULL)
        {
            goto loc_46E18;
        }
        byte_59B85 = 0x0FF;
        word_58DAC = 0x4628;
        word_58DAE = 0x524F;
        word_58DB0 = 0x4543;
        word_58DB2 = 0x2944;
        di = 0x8AA8;
        cx = 0x6F; // 'o'
        al = 2;
        push(es);
        push(ds);
        pop(es);
        rep stosb
        al = 0x20; // ' '
        cx = 8;
        di = 0x8B1C; //
        rep stosb
        cx = 8;
        di = 0x8B9C;
        rep stosb
        cx = 0x16;
        di = 0x8853;
        rep stosb
        cx = 0x16;
        di = 0x886A;
        rep stosb
        pop(es);
        // assume es:nothing
        goto loc_46E39;
//// ; ---------------------------------------------------------------------------

loc_46E18:              //; CODE XREF: start+1A7j
        pop(cx);
        pop(di);
        push(di);
        push(cx);
        al = 0x26; // '&'
        
        if (strchr(something, '&') == NULL)
        {
            goto loc_46E39;
        }
        atoi(something) // reset ax
        if (al >= 1)
        {
            goto loc_46E2B;
        }
        al++;

loc_46E2B:              //; CODE XREF: start+207j
        if (al <= 0x14) // In ascii this is DC4 - Device Control 4??
        {
            goto loc_46E31;
        }
        al = 0x14;

loc_46E31:              //; CODE XREF: start+20Dj
        al--;
        byte_5981F = al;
        byte_58D46 = al;

loc_46E39:              //; CODE XREF: start+1F6j start+200j
        pop(cx);
        pop(di);
        push(di);
        push(cx);
        al = 0x2A; // '*'
        if (strchr(something, '*') == NULL)
        {
            goto loc_46E59;
        }
        atoi(whatever)        // ; reset ax
        if (al <= 0x0A) // '\n'
        {
            goto loc_46E4C;
        }
        al = 0x0A; // '\n'

loc_46E4C:              //; CODE XREF: start+228j
        ah =  0x0A;
        ah -= al;
        if (ah < 0)
        {
            goto loc_46E59;
        }
        
        ah |= 0x0C0;
        speed?3 = ah;

loc_46E59:              //; CODE XREF: start+221j start+230j
        pop(cx);
        pop(di);
        al = 0x2F; // '/'
        if (strchr(something, '/') == NULL)
        {
            goto doesNotHaveCommandLine;
        }
        byte_59B84 = 0x0FF;
        atoi()        //; reset ax
        
        if (al >= 1)
        {
            goto loc_46E6F;
        }
        al++;

loc_46E6F:              //; CODE XREF: start+24Bj
        if (al <= 0x6F) // 'o'
        {
            goto loc_46E75;
        }
        al = 0x6F; // 'o'

loc_46E75:              //; CODE XREF: start+251j
        byte_59B84 = al;

doesNotHaveCommandLine:         //; CODE XREF: start+13j start+23Fj ...
        getTime();
        checkVideo();
        if (byte_59B64 == 0)
        {
            goto leaveVideoStatus;
        }
        videoStatusUnk = 2;

leaveVideoStatus:           //; CODE XREF: start+28Aj
        ax = 0xA000;
        es = ax;
        // assume es:nothing
        sub_4D2BF();
        sub_4D2E9();
        setint8();
        setint24();
        setint9();
        sub_4D8B0();
        if (fastMode != 0)
        {
            goto isFastMode;
        }
        sub_4D517();
        sub_4D4E4();
        si = 0x60D5;
        sub_4D836();
        readTitleDatAndGraphics?();
        si = 0x5F15;
        fade();

isFastMode:              //; CODE XREF: start+2ADj
        readMoving();
        sub_4D4E4();
        // Conditions to whether show 
        al = byte_59B6B;
        al |= byte_59B84;
        al |= byte_599D4;
        al |= fastMode;
        if (al == 0)
        {
            goto openingSequence;
        }
        readEverything();
        goto afterOpeningSequence;
//// ; ---------------------------------------------------------------------------

openingSequence:
        loadScreen2();
        readEverything();
        sub_502CF();
        openCreditsBlock();
        showNewCredits();

afterOpeningSequence:              //; CODE XREF: start+2DEj
        readConfig();
        if (byte_50946 == 0)
        {
            goto loc_46F20;
        }
        byte_50940 = 1;
        goto loc_46F25;
//// ; ---------------------------------------------------------------------------

loc_46F20:              //; CODE XREF: start+2F7j
        byte_50940 = 0;

loc_46F25:              //; CODE XREF: start+2FEj
        if (fastMode == 0)
        {
            goto isNotFastMode;
        }
        goto loc_46FBE;
//// ; ---------------------------------------------------------------------------

isNotFastMode:              //; CODE XREF: start+30Aj
        si = 0x60D5;
        fade();
        word_58467 = 1;
        goto loc_46FBE;
//// ; ---------------------------------------------------------------------------

loc_46F3E:              //; CODE XREF: start+428j start+444j
        readLevels();
        si = 0x60D5;
        fade();
        byte_5A33F = 0;
        sub_4C293();
        sub_48A20();
        sub_4D464();
        sub_48F6D();
        sub_501C0();
        sub_4A2E6();
        sub_4A3BB();
        enableFloppy?();
        findMurphy();
        si = 0x6015;
        fade();
        if (byte_59886 != 0)
        {
            goto loc_46F77;
        }
        sound?3();

loc_46F77:              //; CODE XREF: start+352j
        byte_5A33F = 1;
        runLevel();
        byte_599D4 = 0;
        if (word_5197A == 0)
        {
            goto loc_46F8E;
        }
        goto loc_47067;
//// ; ---------------------------------------------------------------------------

loc_46F8E:              //; CODE XREF: start+369j
        if (fastMode != 1)
        {
            goto isNotFastMode2;
        }
        goto doneWithDemoPlayback;
//// ; ---------------------------------------------------------------------------

isNotFastMode2:              //; CODE XREF: start+373j
        slideDownGameDash();
        if (byte_59B71 == 0)
        {
            goto loc_46FA5;
        }
        readMoving();

loc_46FA5:              //; CODE XREF: start+380j
        byte_5A33F = 0;
        if (word_5197A == 0)
        {
            goto loc_46FB4;
        }
        goto loc_47067;
//// ; ---------------------------------------------------------------------------

loc_46FB4:              //; CODE XREF: start+38Fj
        if (byte_59886 != 0)
        {
            goto loc_46FBE;
        }
        sound?2();

loc_46FBE:              //; CODE XREF: start+30Cj start+31Bj ...
        enableFloppy?();
        sub_49544();
        if (byte_599D4 != 2)
        {
            goto loc_46FFF;
        }
        byte_599D4 = 1;
        if (byte_599D5 != 1)
        {
            goto loc_46FDF;
        }
        ax = 0;
        demoSomething?();
        goto loc_46FE4;
//// ; ---------------------------------------------------------------------------

loc_46FDF:              //; CODE XREF: start+3B5j
        byte_510DE = 0;

loc_46FE4:              //; CODE XREF: start+3BDj
        ax = 1;
        byte_510B3 = 0;
        byte_5A2F9 = 1;
        a00s0010_sp[3] = 0x2D; // '-' ; "001$0.SP"
        a00s0010_sp[4] = 0x2D2D; // "01$0.SP"
        push(ax);
        goto loc_4701A;
//// ; ---------------------------------------------------------------------------

loc_46FFF:              //; CODE XREF: start+3A9j
        al = byte_59B84
        byte_59B84 = 0;
        byte_510DE = 0;
        al++;
        if (al == 0)
        {
            goto loc_4704B;
        }
        al--;
        if (al == 0)
        {
            goto loc_4704B;
        }
        ah = 0;
        push(ax);
        sub_4BF4A();

loc_4701A:              //; CODE XREF: start+3DDj start+433j
        byte_5A33F = 1;
        byte_51ABE = 1;
        sub_4C34A();
        sub_4C293();
        word_58467 = 0;
        sound?2();
        pop(ax);
        word_51ABC = ax;
        sub_4B899();
        sub_4C141();
        word_5196C = 0;
        byte_5A19B = 0;
        goto loc_46F3E;
//// ; ---------------------------------------------------------------------------

loc_4704B:              //; CODE XREF: start+3EEj start+3F2j
        ax = 1;
        if (byte_59B6B != 0)
        {
            goto loc_4701A;
        }
        byte_5A2F9 = 0;
        runMainMenu();
        if (word_5197A != 0)
        {
            goto loc_47067;
        }
        goto loc_46F3E;
//// ; ---------------------------------------------------------------------------

loc_47067:              //; CODE XREF: start+36Bj start+391j ...
        si = 0x60D5;
        fade();

doneWithDemoPlayback:           //; CODE XREF: start+375j
        resetint9();
        sub_4D2E1();
        if (fastMode != 1)
        {
            goto isNotFastMode3;
        }
        
        if (byte_5A19C == 0)
        {
            goto loc_47094;
        }
        
        if (byte_5A19B == 0)
        {
            goto loc_4708E;
        }
        
        si = *aDemoSuccessful; // "Demo successful: "
        goto printMessageAfterward;
//// ; ---------------------------------------------------------------------------

loc_4708E:              //; CODE XREF: start+466j
        si = *aDemoFailed; //"Demo failed:     "
        goto printMessageAfterward;
//// ; ---------------------------------------------------------------------------

loc_47094:              //; CODE XREF: start+45Fj
        if (byte_5A19B == 0)
        {
            goto loc_470A1;
        }
        si = *a@ErrorLevel?Su; // "\"@\"-ERROR: Level(?) successful: "
        goto printMessageAfterward;
//// ; ---------------------------------------------------------------------------

loc_470A1:              //; CODE XREF: start+479j
        si = *a@ErrorLevel?Fa; // "\"@\"-ERROR: Level(?) failed:     "

printMessageAfterward:          //; CODE XREF: start+46Cj start+472j ...
        conprintln();

isNotFastMode3:              //; CODE XREF: start+458j
        resetint24();
        resetint8();
        soundShutdown?();

immediateexit:              //; CODE XREF: start+D1j start+114j ...
        exit(0);
//         mov ax, 4C00h
//         int 21h     ; DOS - 2+ - QUIT WITH EXIT CODE (EXIT)
// ; END OF FUNCTION CHUNK FOR start   ; AL = exit code


// ; ---------------------------------------------------------------------------
// ; START OF FUNCTION CHUNK FOR loadScreen2

exit:                   //; CODE XREF: readConfig:loc_474BBj
                    //; readConfig+3Bj ...
        push(ax);
        resetint9();
        resetint24();
        soundShutdown?();
        sub_4D2E1();
        resetint8();
        pop(ax);
        push(ax);
        writeexitmessage();
        pop(ax);
        if (al != 0)
        {
            goto exitnow;
        }
        al--;

exitnow:                //; CODE XREF: loadScreen2-7C9j
        exit(al);  //mov ah, 4Ch
//         int 21h     ; DOS - 2+ - QUIT WITH EXIT CODE (EXIT)
// ; END OF FUNCTION CHUNK FOR loadScreen2 ; AL = exit code
}

//; =============== S U B R O U T I N E =======================================


void fileReadUnk1() //    proc near       ; CODE XREF: start+B6p readDemo+81p
{
        push    bx
        lea bx, word_599DC
        mov cx, 0Ah     ; cx = 10

loop_:                  ; CODE XREF: fileReadUnk1+Fj
        cmp [bx], ax
        jz  short loc_47105
        add bx, 2
        loop    loop_
        jmp short bail
// ; ---------------------------------------------------------------------------

loc_47105:              ; CODE XREF: fileReadUnk1+Aj
        pop bx
        push    bx
        push(cx);
        mov ax, 4200h
        xor cx, cx
        mov dx, cx
        int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
                    ; AL = method: offset from beginning of file
        pop ax
        jb  short bail
        pop bx
        push    bx
        push    ax
        mov cx, 4
        lea dx, fileLevelData
        mov ax, 3F00h
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        pop bx
        jb  short bail
        sub bx, 0Ah
        neg bx
        shl bx, 1
        shl bx, 1
        mov si, offset fileLevelData
        mov ax, [bx-691Eh]
        cmp ax, word_50A7A
        jnz short bail
        mov ax, [bx-6920h]
        cmp ax, fileLevelData
        jnz short bail
        xor ah, ah
        clc
        jmp short done
// ; ---------------------------------------------------------------------------

bail:                   ; CODE XREF: fileReadUnk1+11j
                    ; fileReadUnk1+20j ...
        xor ax, ax
        stc

done:                   ; CODE XREF: fileReadUnk1+57j
        pop bx
        retn
}


; =============== S U B R O U T I N E =======================================


crt?2       proc near       ; CODE XREF: slideDownGameDash:loc_4720Cp
        cmp byte ptr word_510C1, 0
        jz  short loc_4715C
        mov cx, 5Fh ; '_'
        jmp short loc_4715F
// ; ---------------------------------------------------------------------------

loc_4715C:              ; CODE XREF: crt?2+5j
        mov cx, 90h ; '?'

loc_4715F:              ; CODE XREF: crt?2+Aj crt?2+5Fj
        mov dx, 3D4h
        al = 18h
        out dx, al      ; Video: CRT cntrlr addr
                    ; line compare (scan line). Used for split screen operations.
        inc dx
        al = cl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 7
        out dx, al      ; Video: CRT cntrlr addr
                    // ; bit 8 for certain CRTC regs. Data bits:
                    // ; 0: vertical total (Reg 06)
                    // ; 1: vert disp'd enable end (Reg 12H)
                    // ; 2: vert retrace start (Reg 10H)
                    // ; 3: start vert blanking (Reg 15H)
                    // ; 4: line compare (Reg 18H)
                    // ; 5: cursor location (Reg 0aH)
        inc dx
        al = 3Fh ; '?'
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 9
        out dx, al      ; Video: CRT cntrlr addr
                    ; maximum scan line
        inc dx
        al = 80h ; '?'
        out dx, al      ; Video: CRT controller internal registers
        mov bx, word_51967
        cmp bx, 4DAEh
        jbe short loc_4718E
        sub bx, 7Ah ; 'z'
        mov word_51967, bx

loc_4718E:              ; CODE XREF: crt?2+35j
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        call    videoloop
        call    sub_4D457
        add cx, 1
        cmp cx, 90h ; '?'
        jbe short loc_4715F
        retn
crt?2       endp


; =============== S U B R O U T I N E =======================================


crt?1       proc near       ; CODE XREF: slideDownGameDash+7p
        cmp byte ptr word_510C1, 0
        jz  short loc_471BE
        mov cx, 0B0h ; '?'
        jmp short loc_471C1
// ; ---------------------------------------------------------------------------

loc_471BE:              ; CODE XREF: crt?1+5j
        mov cx, 0C8h ; '?'

loc_471C1:              ; CODE XREF: crt?1+Aj crt?1+4Bj
        mov dx, 3D4h
        al = 18h
        out dx, al      ; Video: CRT cntrlr addr
                    ; line compare (scan line). Used for split screen operations.
        inc dx
        al = cl
        out dx, al      ; Video: CRT controller internal registers
        mov bx, word_51967
        cmp bx, 4DAEh
        jbe short loc_471DC
        sub bx, 7Ah ; 'z'
        mov word_51967, bx

loc_471DC:              ; CODE XREF: crt?1+21j
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        call    videoloop
        call    sub_4D457
        add cx, 1
        cmp cx, 0C8h ; '?'
        jbe short loc_471C1
        retn
crt?1       endp


; =============== S U B R O U T I N E =======================================


slideDownGameDash proc near     ; CODE XREF: start:isNotFastMode2p
        cmp videoStatusUnk, 2
        jnz short loc_4720C
        call    crt?1
        jmp short locret_4720F
// ; ---------------------------------------------------------------------------

loc_4720C:              ; CODE XREF: slideDownGameDash+5j
        call    crt?2

locret_4720F:               ; CODE XREF: slideDownGameDash+Aj
        retn
slideDownGameDash endp


; =============== S U B R O U T I N E =======================================


setint9     proc near       ; CODE XREF: start+2A2p

; waits for shift, ctrl, alt, system, and suspend to NOT be pressed
        push    ds
        mov ax, 0040h;
        mov ds, ax
        assume ds:nothing

keysstillpressed:
        ; test keyboard state flags at 0040:0017
        test    word ptr [ds:0017h], 0F70Fh 
        jnz short keysstillpressed 

; keys no longer pressed, we can continue
        pop ds
        assume ds:data
        push    ds
        push    es
        mov ah, 35h ; '5'
        al = 9
        int 21h     ; DOS - 2+ - GET INTERRUPT VECTOR
                    ; AL = interrupt number
                    ; Return: ES:BX = value of interrupt vector
        mov int9seg, es
        mov int9off, bx
        mov dx, offset int9handler
        mov ax, seg code
        mov ds, ax
        assume ds:code
        mov ah, 25h ; '%'
        al = 9
        int 21h     ; DOS - SET INTERRUPT VECTOR
                    ; AL = interrupt number
                    ; DS:DX = new vector to be used for specified interrupt
        pop es
        assume es:nothing
        pop ds
        assume ds:data
        retn
setint9     endp


; =============== S U B R O U T I N E =======================================


void resetint9() //   proc near       ; CODE XREF: start:doneWithDemoPlaybackp
{
                    // ; loadScreen2-7E0p ...
        cmp byte_5199A, 0
        jnz short resetint9
        cmp byte ptr word_519A7, 0
        jnz short resetint9
        cmp byte ptr word_519B3, 0
        jnz short resetint9
        cmp byte_519B5, 0
        jnz short resetint9
        cmp byte_519B7, 0
        jnz short resetint9
        cmp byte_519C2, 0
        jnz short resetint9
        cmp byte_519C3, 0
        jnz short resetint9
        cmp byte_519CF, 0
        jnz short resetint9
        cmp byte_519D1, 0
        jnz short resetint9
        push    ds
        push    es
        mov dx, int9off
        mov ax, int9seg
        mov ds, ax
        mov ah, 25h ; '%'
        al = 9
        int 21h     ; DOS - SET INTERRUPT VECTOR
                    ; AL = interrupt number
                    ; DS:DX = new vector to be used for specified interrupt
        pop es
        pop ds
        retn
}

// ; ---------------------------------------------------------------------------
        // nop

// ; =============== S U B R O U T I N E =======================================


void int9handler() // proc far        ; DATA XREF: setint9+1Fo
{
        push    ax
        push    bx
        push(cx);
        push    ds
        mov ax, seg data
        mov ds, ax
        in  al, 60h     ; 8042 keyboard input buffer
        mov cl, al
        mov bl, al
        in  al, 61h     ; PC/XT PPI port B bits:
                    ; 0: Tmr 2 gate ??? OR 03H=spkr ON
                    ; 1: Tmr 2 data ?  AND  0fcH=spkr OFF
                    ; 3: 1=read high switches
                    ; 4: 0=enable RAM parity checking
                    ; 5: 0=enable I/O channel check
                    ; 6: 0=hold keyboard clock low
                    ; 7: 0=enable kbrd
        or  al, 10000000b
        out 61h, al     ; enable keyboard
        and al, 1111111b
        out 61h, al     ; disable keyboard
        xor al, al
        xor bh, bh
        shl bl, 1
        cmc
        rcl al, 1
        shr bl, 1
        mov [bx+166Dh], al
        test    cl, 80h     ; think key up
        jz  short storeKey
        jmp storeNoKeyPressed
// ; ---------------------------------------------------------------------------

storeKey:               ; CODE XREF: int9handler+2Bj
        mov keyPressed, cl
        cmp speed?3, 0
        jl  short checkX
        cmp cl, 37h ; '7'   ; Grey *
        jnz short checkSlash
        cmp speed?3, 0
        jl  short checkSlash
        mov cl, speed?3
        mov gameSpeed, cl
        jmp short loc_47320
// ; ---------------------------------------------------------------------------

checkSlash:             ; CODE XREF: int9handler+3Ej
                    ; int9handler+45j
        cmp cl, 35h ; '5'   ; /
        jnz short checkPlus
        mov speed?, 0
        mov gameSpeed, 0Ah
        jmp short checkX
// ; ---------------------------------------------------------------------------

checkPlus:              ; CODE XREF: int9handler+54j
        cmp cl, 4Eh ; 'N'   ; Grey +
        jnz short checkMinus
        mov speed?, 0
        cmp gameSpeed, 0Ah
        jnb short checkMinus
        inc gameSpeed

checkMinus:             ; CODE XREF: int9handler+65j
                    ; int9handler+71j
        cmp cl, 4Ah ; 'J'   ; Grey -
        jnz short checkX
        mov speed?, 0
        cmp gameSpeed, 0
        jz  short checkX
        dec gameSpeed

loc_47320:              ; CODE XREF: int9handler+4Fj
        push(cx);
        mov cl, speed?2
        and cl, 0F0h
        or  cl, gameSpeed
        cmp speed?2, cl
        jbe short dontUpdateSpeed?2
        mov speed?2, cl

dontUpdateSpeed?2:          ; CODE XREF: int9handler+9Cj
        pop(cx);

checkX:                 ; CODE XREF: int9handler+39j
                    ; int9handler+60j ...
        cmp cl, 2Dh ; '-'   ; X
        jnz short doneexit
        cmp byte_519B5, 0
        jz  short doneexit
        mov word_51974, 1
        mov word_5197A, 1
        jmp short doneexit
// ; ---------------------------------------------------------------------------

storeNoKeyPressed:          ; CODE XREF: int9handler+2Dj
        mov keyPressed, 0

doneexit:                   ; CODE XREF: int9handler+A6j
                    ; int9handler+ADj ...
        al = 100000b ; nonspecific EOI
        out 20h, al     ; Interrupt controller, 8259A.
        pop ds
        pop(cx);
        pop bx
        pop ax
        iret
int9handler endp

// ; ---------------------------------------------------------------------------
        nop

// ; =============== S U B R O U T I N E =======================================


void int8handler() // proc far        ; DATA XREF: setint8+10o
{
        push    ds
        push    dx
        push    ax
        mov ax, seg data
        mov ds, ax
        inc byte_59B96
        cmp byte_510AE, 0
        jz  short loc_473AA
        al = byte_510AF
        al++;
        mov byte_510AF, al
        cmp al, 32h ; '2'
        jl  short loc_473AA
        mov byte_510AF, 0
        al = byte_510B0
        al++;
        mov byte_510B0, al
        cmp al, 3Ch ; '<'
        jl  short loc_473AA
        mov byte_510B0, 0
        al = byte_510B1
        al++;
        mov byte_510B1, al
        cmp al, 3Ch ; '<'
        jl  short loc_473AA
        mov byte_510B1, 0
        inc byte_510B2

loc_473AA:              ; CODE XREF: int8handler+11j
                    ; int8handler+1Dj ...
        cmp soundEnabled?, 0
        jz  short loc_473B4
        call    sound?11

loc_473B4:              ; CODE XREF: int8handler+4Fj
        cmp byte_5988B, 0
        jz  short loc_473C6
        dec byte_5988B
        jnz short loc_473C6
        mov byte_59889, 0

loc_473C6:              ; CODE XREF: int8handler+59j
                    ; int8handler+5Fj
        cmp byte_5988C, 0
        jz  short loc_473D8
        dec byte_5988C
        jnz short loc_473D8
        mov byte_5988A, 0

loc_473D8:              ; CODE XREF: int8handler+6Bj
                    ; int8handler+71j
        inc byte_5A320
        inc byte_5A321
        cmp byte_5A320, 3
        jnz short loc_473F0
        inc byte_5A322
        mov byte_5A320, 0

loc_473F0:              ; CODE XREF: int8handler+85j
        cmp byte_5A321, 21h ; '!'
        jnz short loc_47400
        inc byte_5A322
        mov byte_5A321, 0

loc_47400:              ; CODE XREF: int8handler+95j
        cmp byte_5A322, 0
        jz  short loc_47414
        dec byte_5A322
        pop ax
        pop dx
        pushf
        call   [int8loc]
        pop ds
        iret
// ; ---------------------------------------------------------------------------

loc_47414:              ; CODE XREF: int8handler+A5j
        al = 20h ; ' '
        out 20h, al     ; Interrupt controller, 8259A.
        pop ax
        pop dx
        pop ds
        iret
// int8handler endp
}


; =============== S U B R O U T I N E =======================================


setint8     proc near       ; CODE XREF: start+29Cp
        push    ds
        push    es
        mov ah, 35h ; '5'
        al = 8
        int 21h     ; DOS - 2+ - GET INTERRUPT VECTOR
                    ; AL = interrupt number
                    ; Return: ES:BX = value of interrupt vector
        mov word ptr int8loc+2, es
        mov word ptr int8loc, bx
        mov dx, offset int8handler
        mov ax, seg code
        mov ds, ax
        assume ds:code
        mov ah, 25h ; '%'
        al = 8
        int 21h     ; DOS - SET INTERRUPT VECTOR
                    ; AL = interrupt number
                    ; DS:DX = new vector to be used for specified interrupt
        al = 36h ; '6'
        out 43h, al     ; Timer 8253-5 (AT: 8254.2).
        al = 38h ; '8'
        out 40h, al     ; Timer 8253-5 (AT: 8254.2).
        al = 5Dh ; ']'
        out 40h, al     ; Timer 8253-5 (AT: 8254.2).
        pop es
        pop ds
        assume ds:data
        retn
setint8     endp


; =============== S U B R O U T I N E =======================================


resetint8   proc near       ; CODE XREF: start+48Bp
                    ; loadScreen2-7D4p
        push    ds
        push    es
        mov dx, word ptr int8loc
        mov ax, word ptr int8loc+2
        mov ds, ax
        mov ah, 25h ; '%'
        al = 8
        int 21h     ; DOS - SET INTERRUPT VECTOR
                    ; AL = interrupt number
                    ; DS:DX = new vector to be used for specified interrupt
        al = 36h ; '6'
        out 43h, al     ; Timer 8253-5 (AT: 8254.2).
        al = 0FFh
        out 40h, al     ; Timer 8253-5 (AT: 8254.2).
        al = 0FFh
        out 40h, al     ; Timer 8253-5 (AT: 8254.2).
        pop es
        pop ds
        retn
resetint8   endp


; =============== S U B R O U T I N E =======================================


setint24    proc near       ; CODE XREF: start+29Fp
        push    ds
        push    es
        mov ah, 35h ; '5'
        al = 24h ; '$'
        int 21h     ; DOS - 2+ - GET INTERRUPT VECTOR
                    ; AL = interrupt number
                    ; Return: ES:BX = value of interrupt vector
        mov word_510A9, es
        mov word_510A7, bx
        mov dx, offset int24handler
        mov ax, seg code
        mov ds, ax
        assume ds:code
        mov ah, 25h ; '%'
        al = 24h ; '$'
        int 21h     ; DOS - SET INTERRUPT VECTOR
                    ; AL = interrupt number
                    ; DS:DX = new vector to be used for specified interrupt
        pop es
        pop ds
        assume ds:data
        retn
setint24    endp


; =============== S U B R O U T I N E =======================================


resetint24  proc near       ; CODE XREF: start:isNotFastMode3p
                    ; loadScreen2-7DDp
        push    ds
        push    es
        mov dx, word_510A7
        mov ax, word_510A9
        mov ds, ax
        mov ah, 25h ; '%'
        al = 24h ; '$'
        int 21h     ; DOS - SET INTERRUPT VECTOR
                    ; AL = interrupt number
                    ; DS:DX = new vector to be used for specified interrupt
        pop es
        pop ds
        retn
resetint24  endp


; =============== S U B R O U T I N E =======================================


int24handler    proc far        ; DATA XREF: setint24+10o
        al = 1
        iret
int24handler    endp


; =============== S U B R O U T I N E =======================================


readConfig  proc near       ; CODE XREF: start:loc_46F0Fp
        mov ax, 3D00h
        mov dx, offset aSupaplex_cfg ; "SUPAPLEX.CFG"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_474BE
        cmp ax, 2
        jnz short loc_474B3
        jmp loc_47551
// ; ---------------------------------------------------------------------------

loc_474B3:              ; CODE XREF: readConfig+Dj
        cmp ax, 3
        jnz short loc_474BB
        jmp loc_47551
// ; ---------------------------------------------------------------------------

loc_474BB:              ; CODE XREF: readConfig+15j
        jmp exit
// ; ---------------------------------------------------------------------------

loc_474BE:              ; CODE XREF: readConfig+8j
        mov lastFileHandle, ax
        mov bx, lastFileHandle
        mov ax, 3F00h
        mov cx, 4
        mov dx, offset fileLevelData
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        pushf
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short loc_474DF
        jmp exit
// ; ---------------------------------------------------------------------------

loc_474DF:              ; CODE XREF: readConfig+39j
        popf
        jnb short loc_474E5
        jmp exit
// ; ---------------------------------------------------------------------------

loc_474E5:              ; CODE XREF: readConfig+3Fj
        mov si, offset fileLevelData
        cmp byte ptr [si], 73h ; 's'
        jnz short loc_474F2
        call    loadBeep2
        jmp short loc_4751D
// ; ---------------------------------------------------------------------------

loc_474F2:              ; CODE XREF: readConfig+4Aj
        cmp byte ptr [si], 61h ; 'a'
        jnz short loc_474FC
        call    loadAdlib
        jmp short loc_4751D
// ; ---------------------------------------------------------------------------

loc_474FC:              ; CODE XREF: readConfig+54j
        cmp byte ptr [si], 62h ; 'b'
        jnz short loc_47506
        call    loadBlaster
        jmp short loc_4751D
// ; ---------------------------------------------------------------------------

loc_47506:              ; CODE XREF: readConfig+5Ej
        cmp byte ptr [si], 72h ; 'r'
        jnz short loc_47510
        call    loadRoland
        jmp short loc_4751D
// ; ---------------------------------------------------------------------------

loc_47510:              ; CODE XREF: readConfig+68j
        cmp byte ptr [si], 63h ; 'c'
        jnz short loc_4751A
        call    loadCombined
        jmp short loc_4751D
// ; ---------------------------------------------------------------------------

loc_4751A:              ; CODE XREF: readConfig+72j
        call    loadBeep

loc_4751D:              ; CODE XREF: readConfig+4Fj
                    ; readConfig+59j ...
        inc si
        mov byte_50940, 0
        cmp byte ptr [si], 6Ah ; 'j'
        jnz short loc_47530
        mov byte_50940, 1
        call    sub_4921B

loc_47530:              ; CODE XREF: readConfig+85j
        inc si
        mov byte_59886, 0
        cmp byte ptr [si], 6Dh ; 'm'
        jnz short loc_47540
        mov byte_59886, 1

loc_47540:              ; CODE XREF: readConfig+98j
        inc si
        mov byte_59885, 0
        cmp byte ptr [si], 78h ; 'x'
        jnz short locret_47550
        mov byte_59885, 1

locret_47550:               ; CODE XREF: readConfig+A8j
        retn
// ; ---------------------------------------------------------------------------

loc_47551:              ; CODE XREF: readConfig+Fj
                    ; readConfig+17j
        call    loadBeep
        mov byte_50940, 0
        retn
readConfig  endp

// ; ---------------------------------------------------------------------------
sub_4755A       proc near               ; CODE XREF: code:loc_4CAECp
                mov     ax, 3C00h
                mov     cx, 0
                mov     dx, 371Dh
                int     21h             ; DOS - 2+ - CREATE A FILE WITH HANDLE (CREAT)
                                        ; CX = attributes for file
                                        ; DS:DX -> ASCIZ filename (may include drive and path)
                jnb     short loc_4756A
                jmp     exit
// ; ---------------------------------------------------------------------------

loc_4756A:                              ; CODE XREF: sub_4755A+Bj
                mov     lastFileHandle, ax
                mov     si, offset fileLevelData
                cmp     sndType, 2
                jnz     short loc_4757C
                mov     byte ptr [si], 73h ; 's'
                jmp     short loc_475AF
// ; ---------------------------------------------------------------------------

loc_4757C:                              ; CODE XREF: sub_4755A+1Bj
                cmp     sndType, 1
                jnz     short loc_47588
                mov     byte ptr [si], 69h ; 'i'
                jmp     short loc_475AF
// ; ---------------------------------------------------------------------------

loc_47588:                              ; CODE XREF: sub_4755A+27j
                cmp     sndType, 3
                jnz     short loc_47594
                mov     byte ptr [si], 61h ; 'a'
                jmp     short loc_475AF
// ; ---------------------------------------------------------------------------

loc_47594:                              ; CODE XREF: sub_4755A+33j
                cmp     sndType, 5
                jnz     short loc_475A0
                mov     byte ptr [si], 72h ; 'r'
                jmp     short loc_475AF
// ; ---------------------------------------------------------------------------

loc_475A0:                              ; CODE XREF: sub_4755A+3Fj
                cmp     musType, 5
                jnz     short loc_475AC
                mov     byte ptr [si], 63h ; 'c'
                jmp     short loc_475AF
// ; ---------------------------------------------------------------------------

loc_475AC:                              ; CODE XREF: sub_4755A+4Bj
                mov     byte ptr [si], 62h ; 'b'

loc_475AF:                              ; CODE XREF: sub_4755A+20j
                                        ; sub_4755A+2Cj ...
                inc     si
                cmp     byte_50940, 0
                jnz     short loc_475BC
                mov     byte ptr [si], 6Bh ; 'k'
                jmp     short loc_475BF
// ; ---------------------------------------------------------------------------

loc_475BC:                              ; CODE XREF: sub_4755A+5Bj
                mov     byte ptr [si], 6Ah ; 'j'

loc_475BF:                              ; CODE XREF: sub_4755A+60j
                inc     si
                cmp     byte_59886, 0
                jz      short loc_475CC
                mov     byte ptr [si], 6Dh ; 'm'
                jmp     short loc_475CF
// ; ---------------------------------------------------------------------------

loc_475CC:                              ; CODE XREF: sub_4755A+6Bj
                mov     byte ptr [si], 6Eh ; 'n'

loc_475CF:                              ; CODE XREF: sub_4755A+70j
                inc     si
                cmp     byte_59885, 0
                jz      short loc_475DC
                mov     byte ptr [si], 78h ; 'x'
                jmp     short loc_475DF
// ; ---------------------------------------------------------------------------

loc_475DC:                              ; CODE XREF: sub_4755A+7Bj
                mov     byte ptr [si], 79h ; 'y'

loc_475DF:                              ; CODE XREF: sub_4755A+80j
                mov     bx, lastFileHandle
                mov     ax, 4000h
                mov     cx, 4
                mov     dx, offset fileLevelData
                int     21h             ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                                        ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
                jnb     short loc_475F3
                jmp     exit
// ; ---------------------------------------------------------------------------

loc_475F3:                              ; CODE XREF: sub_4755A+94j
                mov     ax, 3E00h
                mov     bx, lastFileHandle
                int     21h             ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                                        ; BX = file handle
                jnb     short locret_47601
                jmp     exit
// ; ---------------------------------------------------------------------------

locret_47601:                           ; CODE XREF: sub_4755A+A2j
                retn
sub_4755A       endp

; =============== S U B R O U T I N E =======================================


enableFloppy?   proc near       ; CODE XREF: start+341p
                    ; start:loc_46FBEp ...
        mov dx, 3F2h
        al = 1111b
        out dx, al      ; Floppy: digital output reg bits:
                    ; 0-1: Drive to select 0-3 (AT: bit 1 not used)
                    ; 2:   0=reset diskette controller; 1=enable controller
                    ; 3:   1=enable diskette DMA and interrupts
                    ; 4-7: drive motor enable.  Set bits to turn drive ON.
                    ;
        retn
enableFloppy?   endp


; =============== S U B R O U T I N E =======================================


readDemo    proc near       ; CODE XREF: readEverything+12p
                    ; sub_4B159p ...
        push    es
        mov ax, seg demoseg
        mov es, ax
        assume es:demoseg
        mov bx, 16h
        mov word_510DF, bx
        mov word_5A33C, bx
        mov ax, 0FFFFh
        mov di, 0
        mov cx, 0Bh
        cld
        rep stosw
        mov cx, 0

loc_47629:              ; CODE XREF: readDemo+175j
        push(cx);
        mov word_599D8, 0
        cmp byte_599D4, 1
        jnz short loc_4763C
        mov dx, offset demoFileName
        jmp short loc_47647
// ; ---------------------------------------------------------------------------

loc_4763C:              ; CODE XREF: readDemo+2Cj
        mov bx, offset aDemo0_bin ; "DEMO0.BIN"
        add cl, 30h ; '0'
        mov [bx+4], cl
        mov dx, bx

loc_47647:              ; CODE XREF: readDemo+31j
        mov ax, 3D00h
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_47651
        jmp loc_47783
// ; ---------------------------------------------------------------------------

loc_47651:              ; CODE XREF: readDemo+43j
        mov lastFileHandle, ax
        mov bx, ax
        cmp byte_599D4, 1
        jnz short loc_47674
        cmp word_599DA, 0
        jnz short loc_476DB
        mov ax, 4200h
        xor cx, cx
        mov dx, levelDataLength
        int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
                    ; AL = method: offset from beginning of file
        mov bx, lastFileHandle
        jmp short loc_476DB
// ; ---------------------------------------------------------------------------

loc_47674:              ; CODE XREF: readDemo+52j
        push    bx
        mov ax, 4202h
        xor cx, cx
        mov dx, cx
        int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
                    ; AL = method: offset from end of file
        pop bx
        jb  short loc_47690
        or  dx, dx
        jnz short loc_47690
        cmp ax, levelDataLength
        jnb short loc_47690
        call    fileReadUnk1
        mov word_599D8, ax

loc_47690:              ; CODE XREF: readDemo+76j readDemo+7Aj ...
        mov ax, 4200h
        xor cx, cx
        mov dx, cx
        int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
                    ; AL = method: offset from beginning of file
        cmp word_599D8, 0
        jnz short loc_476DB
        mov ax, 3F00h
        mov bx, lastFileHandle
        pop(cx);
        push(cx);
        mov dx, cx
        shl cx, 1
        add dx, cx
        mov cl, 9
        shl dx, cl
        add dx, 0BE20h
        mov cx, levelDataLength
        push    ds
        push    dx
        push    es
        pop ds
        assume ds:demoseg
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        pop bx
        mov dx, [bx+5FEh]
        pop ds
        assume ds:data
        jnb short loc_476CB
        jmp loc_47783
// ; ---------------------------------------------------------------------------

loc_476CB:              ; CODE XREF: readDemo+BDj
        cmp ax, levelDataLength
        jz  short loc_476D3
        jmp loc_47783
// ; ---------------------------------------------------------------------------

loc_476D3:              ; CODE XREF: readDemo+C5j
        pop bx
        push    bx
        shl bx, 1
        mov [bx-67CAh], dx

loc_476DB:              ; CODE XREF: readDemo+59j readDemo+69j ...
        mov cx, 0BE09h
        sub cx, word_510DF
        cmp cx, 0BE09h
        jbe short loc_476EA
        xor cx, cx

loc_476EA:              ; CODE XREF: readDemo+DDj
        cmp cx, 0
        jnz short loc_476F3
        xor ax, ax
        jmp short loc_4771A
// ; ---------------------------------------------------------------------------

loc_476F3:              ; CODE XREF: readDemo+E4j
        mov dx, word_510DF
        mov bx, lastFileHandle
        push    ds
        mov ax, es
        mov ds, ax
        assume ds:nothing
        mov ax, 3F00h
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_47719
        pop ds
        assume ds:data
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        mov ax, 0
        jnb short loc_47783
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47719:              ; CODE XREF: readDemo+FCj
        pop ds

loc_4771A:              ; CODE XREF: readDemo+E8j
        push    ax
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short loc_47729
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47729:              ; CODE XREF: readDemo+11Bj
        pop ax
        mov bx, word_510DF
        and byte ptr es:[bx], 7Fh
        cmp word_599D8, 0
        mov word_599D8, 0
        jnz short loc_47743
        or  byte ptr es:[bx], 80h

loc_47743:              ; CODE XREF: readDemo+134j
        mov cx, bx
        add bx, ax
        push    ds
        push    es
        pop ds
        assume ds:nothing
        dec bx
        cmp bx, 0FFFFh
        jz  short loc_4775A
        cmp ax, 1
        jbe short loc_4775A
        cmp byte ptr [bx], 0FFh
        jz  short loc_47765

loc_4775A:              ; CODE XREF: readDemo+145j
                    ; readDemo+14Aj
        cmp bx, maxdemolength
        jnb short loc_47765
        inc bx
        inc ax
        mov byte ptr [bx], 0FFh

loc_47765:              ; CODE XREF: readDemo+14Fj
                    ; readDemo+155j
        pop ds
        assume ds:data
        pop(cx);
        mov bx, cx
        shl bx, 1
        mov dx, word_510DF
        db 26h, 89h, 97h, 00h, 00h; mov es:[bx+0], dx
        add word_510DF, ax
        inc cx
        cmp cx, 0Ah
        jz  short loc_47781
        jmp loc_47629
// ; ---------------------------------------------------------------------------

loc_47781:              ; CODE XREF: readDemo+173j
        pop es
        assume es:nothing
        retn
// ; ---------------------------------------------------------------------------

loc_47783:              ; CODE XREF: readDemo+45j readDemo+BFj ...
        pop(cx);
        pop es
        retn
readDemo    endp


; =============== S U B R O U T I N E =======================================


readPallettes   proc near       ; CODE XREF: readEverythingp
        mov ax, 3D00h
        mov dx, offset aPalettes_dat ; "PALETTES.DAT"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_4779F
        cmp ax, 2
        jnz short loc_4779C
        call    sub_47F39
        jb  short loc_4779C
        jmp short loc_4779F
// ; ---------------------------------------------------------------------------

loc_4779C:              ; CODE XREF: readPallettes+Dj
                    ; readPallettes+12j
        jmp exit
// ; ---------------------------------------------------------------------------

loc_4779F:              ; CODE XREF: readPallettes+8j
                    ; readPallettes+14j
        mov lastFileHandle, ax
        mov bx, lastFileHandle
        mov ax, 3F00h
        mov cx, 100h
        mov dx, 5FD5h
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_477B6
        jmp exit
// ; ---------------------------------------------------------------------------

loc_477B6:              ; CODE XREF: readPallettes+2Bj
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short locret_477C4
        jmp exit
// ; ---------------------------------------------------------------------------

locret_477C4:               ; CODE XREF: readPallettes+39j
        retn
readPallettes   endp


; =============== S U B R O U T I N E =======================================

; Attributes: bp-based frame

openCreditsBlock proc near      ; CODE XREF: start+2E9p

var_6       = word ptr -6
var_4       = word ptr -4
var_2       = word ptr -2

        push    bp
        mov bp, sp
        add sp, 0FFFAh
        push    ds
        mov ax, ds
        mov [bp+var_6], ax
        mov ax, es
        mov ds, ax
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: sequencer data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov [bp+var_2], 5A23h
        mov [bp+var_4], 5A24h
        mov dx, 0Fh

loc_47800:              ; CODE XREF: openCreditsBlock+AFj
        push    dx
        mov ax, [bp+var_6]
        mov ds, ax
        mov cx, 2

loc_47809:              ; CODE XREF: openCreditsBlock+4Cj
        push(cx);
        call    videoloop
        call    sub_4D457
        pop(cx);
        loop    loc_47809
        mov ax, es
        mov ds, ax
        pop dx
        mov cx, 94h ; '?'
        mov si, [bp+var_2]
        dec si
        mov di, si
        dec di

loc_47822:              ; CODE XREF: openCreditsBlock+65j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_47822
        mov cx, 94h ; '?'
        mov si, [bp+var_2]
        mov di, si
        add si, 28h ; '('

loc_47837:              ; CODE XREF: openCreditsBlock+79j
        movsb
        add si, 79h ; 'y'
        add di, 79h ; 'y'
        loop    loc_47837
        std
        mov cx, 94h ; '?'
        mov si, [bp+var_4]
        add si, 4689h
        mov di, si
        inc di

loc_4784E:              ; CODE XREF: openCreditsBlock+91j
        movsb
        movsb
        sub si, 78h ; 'x'
        sub di, 78h ; 'x'
        loop    loc_4784E
        cld
        mov cx, 94h ; '?'
        mov si, [bp+var_4]
        mov di, si
        add si, 28h ; '('

loc_47864:              ; CODE XREF: openCreditsBlock+A6j
        movsb
        add si, 79h ; 'y'
        add di, 79h ; 'y'
        loop    loc_47864
        dec [bp+var_2]
        inc [bp+var_4]
        dec dx
        jnz short loc_47800
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        pop ds
        mov cx, 1

loc_47884:              ; CODE XREF: openCreditsBlock+C7j
        push(cx);
        call    videoloop
        call    sub_4D457
        pop(cx);
        loop    loc_47884
        mov bx, 4DD4h
        mov word_51967, bx
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        mov si, 5F55h
        call    fade
        mov sp, bp
        pop bp
        retn
openCreditsBlock endp


; =============== S U B R O U T I N E =======================================


loadScreen2 proc near       ; CODE XREF: start:loc_46F00p

; FUNCTION CHUNK AT 04B2 SIZE 00000020 BYTES

        mov ax, 3D00h
        mov dx, 375Ch
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_478C0
        jmp exit
// ; ---------------------------------------------------------------------------

loc_478C0:              ; CODE XREF: loadScreen2+8j
        mov lastFileHandle, ax
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register

loc_478CD:
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        mov cx, 0C8h ; '?'
        mov di, 4DACh

loc_478E7:              ; CODE XREF: loadScreen2+6Bj
        push(cx);
        mov ax, 3F00h
        mov bx, lastFileHandle
        mov cx, 0A0h ; '?'
        mov dx, offset fileLevelData
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_478FC
        jmp exit
// ; ---------------------------------------------------------------------------

loc_478FC:              ; CODE XREF: loadScreen2+44j
        mov si, offset fileLevelData
        mov ah, 1

loc_47901:              ; CODE XREF: loadScreen2+65j
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = ah
        out dx, al      ; EGA port: sequencer data register
        mov cx, 28h ; '('
        rep movsb
        sub di, 28h ; '('
        shl ah, 1
        test    ah, 0Fh
        jnz short loc_47901
        add di, 7Ah ; 'z'
        pop(cx);
        loop    loc_478E7
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short loc_4792E
        jmp exit
// ; ---------------------------------------------------------------------------

loc_4792E:              ; CODE XREF: loadScreen2+76j
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: sequencer data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov bx, 4DACh
        mov word_51967, bx
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        call    videoloop
        mov byte_510A6, 0
        mov si, 5F95h
        call    sub_4D836
        mov ax, 3D00h
        mov dx, 3767h
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_47978
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47978:              ; CODE XREF: loadScreen2+C0j
        mov lastFileHandle, ax
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        mov cx, 0C8h ; '?'
        mov di, 4DD4h

loc_47995:              ; CODE XREF: loadScreen2+119j
        push(cx);
        mov ax, 3F00h
        mov bx, lastFileHandle
        mov cx, 0A0h ; '?'
        mov dx, offset fileLevelData
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_479AA
        jmp exit
// ; ---------------------------------------------------------------------------

loc_479AA:              ; CODE XREF: loadScreen2+F2j
        mov si, offset fileLevelData
        mov ah, 1

loc_479AF:              ; CODE XREF: loadScreen2+113j
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = ah
        out dx, al      ; EGA port: sequencer data register
        mov cx, 28h ; '('
        rep movsb
        sub di, 28h ; '('
        shl ah, 1
        test    ah, 0Fh
        jnz short loc_479AF
        add di, 7Ah ; 'z'
        pop(cx);
        loop    loc_47995
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short loc_479DC
        jmp exit
// ; ---------------------------------------------------------------------------

loc_479DC:              ; CODE XREF: loadScreen2+124j
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: sequencer data register
        retn
loadScreen2 endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================

; Attributes: bp-based frame

readMoving  proc near       ; CODE XREF: start:isFastModep
                    ; start+382p ...

var_6       = word ptr -6
var_4       = word ptr -4
var_1       = byte ptr -1

        push    bp
        mov bp, sp
        add sp, 0FFFAh

loc_479ED:              ; CODE XREF: readMoving+27j
        mov ax, 3D00h
        mov dx, offset aMoving_dat ; "MOVING.DAT"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_47A13
        cmp ax, 2
        jnz short loc_47A10
        call    sub_47F39
        pushf
        cmp byte_59B86, 0FFh
        jnz short loc_47A0B
        popf
        jmp loc_47AE3
// ; ---------------------------------------------------------------------------

loc_47A0B:              ; CODE XREF: readMoving+1Ej
        popf
        jb  short loc_47A10
        jmp short loc_479ED
// ; ---------------------------------------------------------------------------

loc_47A10:              ; CODE XREF: readMoving+13j
                    ; readMoving+25j
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47A13:              ; CODE XREF: readMoving+Ej
        mov lastFileHandle, ax
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov ax, 0B72h
        mov [bp+var_6], ax
        mov ax, 0
        mov [bp+var_4], ax

loc_47A40:              ; CODE XREF: readMoving+BAj
        al = 0
        mov [bp+var_1], al

loc_47A45:              ; CODE XREF: readMoving+AEj
        mov ax, 3F00h
        mov bx, lastFileHandle
        mov cx, 28h ; '('
        mov dx, offset fileLevelData
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_47A59
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47A59:              ; CODE XREF: readMoving+6Dj
        mov si, offset fileLevelData
        mov ax, [bp+var_4]
        mov bx, 7Ah ; 'z'
        mul bx
        mov di, ax
        add di, [bp+var_6]

loc_47A69:              ; CODE XREF: readMoving+8Cj
        cmp di, 4D34h
        jb  short loc_47A75
        sub di, 4D0Ch
        jmp short loc_47A69
// ; ---------------------------------------------------------------------------

loc_47A75:              ; CODE XREF: readMoving+86j
        mov cl, [bp+var_1]
        mov ah, 1
        shl ah, cl
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = ah
        out dx, al      ; EGA port: sequencer data register
        mov cx, 14h
        rep movsw
        al = [bp+var_1]
        al++;
        mov [bp+var_1], al
        cmp al, 4
        jl  short loc_47A45
        mov ax, [bp+var_4]
        inc ax
        mov [bp+var_4], ax
        cmp ax, 1D0h
        jl  short loc_47A40
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short loc_47AB1
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47AB1:              ; CODE XREF: readMoving+C5j
        mov ax, 3D00h
        mov dx, offset aFixed_dat ; "FIXED.DAT"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_47ABE
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47ABE:              ; CODE XREF: readMoving+D2j
        mov lastFileHandle, ax
        mov ax, 3F00h
        mov bx, lastFileHandle
        mov cx, 1400h
        mov dx, 3815h
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_47AD5
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47AD5:              ; CODE XREF: readMoving+E9j
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short loc_47AE3
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47AE3:              ; CODE XREF: readMoving+21j
                    ; readMoving+F7j
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: sequencer data register
        mov sp, bp
        pop bp
        retn
readMoving  endp


; =============== S U B R O U T I N E =======================================


readPanelDat    proc near       ; CODE XREF: readPanelDat+14j
                    ; readEverything+6p
        mov ax, 3D00h
        mov dx, offset aPanel_dat ; "PANEL.DAT"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_47B0A
        cmp ax, 2
        jnz short loc_47B07
        call    sub_47F39
        jb  short loc_47B07
        jmp short readPanelDat
// ; ---------------------------------------------------------------------------

loc_47B07:              ; CODE XREF: readPanelDat+Dj
                    ; readPanelDat+12j
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47B0A:              ; CODE XREF: readPanelDat+8j
        mov lastFileHandle, ax
        mov ax, 3F00h
        mov bx, lastFileHandle
        mov cx, 0F00h
        mov dx, 4C15h
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_47B21
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47B21:              ; CODE XREF: readPanelDat+2Bj
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short locret_47B2F
        jmp exit
// ; ---------------------------------------------------------------------------

locret_47B2F:               ; CODE XREF: readPanelDat+39j
        retn
readPanelDat    endp


; =============== S U B R O U T I N E =======================================


readBackDat proc near       ; CODE XREF: readBackDat+14j
                    ; readEverything+15p
        mov ax, 3D00h
        mov dx, offset aBack_dat ; "BACK.DAT"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_47B49
        cmp ax, 2
        jnz short loc_47B46
        call    sub_47F39
        jb  short loc_47B46
        jmp short readBackDat
// ; ---------------------------------------------------------------------------

loc_47B46:              ; CODE XREF: readBackDat+Dj
                    ; readBackDat+12j
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47B49:              ; CODE XREF: readBackDat+8j
        mov lastFileHandle, ax
        push    ds
        mov ax, seg backseg
        mov bx, lastFileHandle
        mov ds, ax
        assume ds:nothing
        mov ax, 3F00h
        mov cx, 7D00h
        mov dx, 0
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_47B67
        pop ds
        assume ds:data
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47B67:              ; CODE XREF: readBackDat+31j
        pop ds
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short locret_47B76
        jmp exit
// ; ---------------------------------------------------------------------------

locret_47B76:               ; CODE XREF: readBackDat+41j
        retn
readBackDat endp


; =============== S U B R O U T I N E =======================================


readChars6Dat   proc near       ; CODE XREF: readChars6Dat+14j
                    ; readEverything+3p
        mov ax, 3D00h
        mov dx, offset aChars6_dat ; "CHARS6.DAT"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_47B90
        cmp ax, 2
        jnz short loc_47B8D
        call    sub_47F39
        jb  short loc_47B8D
        jmp short readChars6Dat
// ; ---------------------------------------------------------------------------

loc_47B8D:              ; CODE XREF: readChars6Dat+Dj
                    ; readChars6Dat+12j
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47B90:              ; CODE XREF: readChars6Dat+8j
        mov lastFileHandle, ax
        mov ax, 3F00h
        mov bx, lastFileHandle
        mov cx, 200h
        mov dx, 5B15h
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_47BA7
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47BA7:              ; CODE XREF: readChars6Dat+2Bj
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short loc_47BB5
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47BB5:              ; CODE XREF: readChars6Dat+39j
        mov ax, 3D00h
        mov dx, 378Eh
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_47BC2
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47BC2:              ; CODE XREF: readChars6Dat+46j
        mov lastFileHandle, ax
        mov ax, 3F00h
        mov bx, lastFileHandle
        mov cx, 200h
        mov dx, 5D15h
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_47BD9
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47BD9:              ; CODE XREF: readChars6Dat+5Dj
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short locret_47BE7
        jmp exit
// ; ---------------------------------------------------------------------------

locret_47BE7:               ; CODE XREF: readChars6Dat+6Bj
        retn
readChars6Dat   endp


; =============== S U B R O U T I N E =======================================


readTitleDatAndGraphics? proc near  ; CODE XREF: start+2BBp
        mov bx, 4D84h
        mov word_51967, bx
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        call    videoloop
        mov byte_510A6, 0
        mov ax, 3D00h
        mov dx, offset aTitle_dat ; "TITLE.DAT"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_47C18
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47C18:              ; CODE XREF: readTitleDatAndGraphics?+2Bj
        mov lastFileHandle, ax
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        mov cx, 0C8h ; '?'
        mov di, (offset videoStatusUnk+1570h)

loc_47C3F:              ; CODE XREF: readTitleDatAndGraphics?+8Ej
        push(cx);
        mov ax, 3F00h
        mov bx, lastFileHandle
        mov cx, 0A0h ; '?'
        mov dx, offset fileLevelData
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_47C54
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47C54:              ; CODE XREF: readTitleDatAndGraphics?+67j
        mov si, offset fileLevelData
        mov ah, 1

loc_47C59:              ; CODE XREF: readTitleDatAndGraphics?+88j
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = ah
        out dx, al      ; EGA port: sequencer data register
        mov cx, 28h ; '('
        rep movsb
        sub di, 28h ; '('
        shl ah, 1
        test    ah, 0Fh
        jnz short loc_47C59
        add di, 7Ah ; 'z'
        pop(cx);
        loop    loc_47C3F
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short loc_47C86
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47C86:              ; CODE XREF: readTitleDatAndGraphics?+99j
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: sequencer data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        retn
readTitleDatAndGraphics? endp


; =============== S U B R O U T I N E =======================================


readLevelsLst   proc near       ; CODE XREF: readLevelsLst+CCj
                    ; readEverything+Fp ...
        mov ax, 3D00h
        mov dx, offset aLevel_lst ; "LEVEL.LST"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jb  short loc_47CA8
        jmp loc_47D6D
// ; ---------------------------------------------------------------------------

loc_47CA8:              ; CODE XREF: readLevelsLst+8j
        mov ax, 3D00h
        mov dx, 17AFh
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_47CB5
        jmp loc_47D5D
// ; ---------------------------------------------------------------------------

loc_47CB5:              ; CODE XREF: readLevelsLst+15j
        mov lastFileHandle, ax
        mov bx, 2A6Ch
        mov ax, 3030h
        mov dx, 2031h
        mov cx, 6Fh ; 'o'

loc_47CC4:              ; CODE XREF: readLevelsLst:loc_47CE4j
        mov [bx], ax
        mov [bx+2], dx
        add bx, 1Ch
        mov byte ptr [bx-1], 0Ah
        inc dl
        cmp dl, 39h ; '9'
        jbe short loc_47CE4
        mov dl, 30h ; '0'
        inc ah
        cmp ah, 39h ; '9'
        jbe short loc_47CE4
        mov ah, 30h ; '0'
        al++;

loc_47CE4:              ; CODE XREF: readLevelsLst+3Aj
                    ; readLevelsLst+43j
        loop    loc_47CC4
        mov cx, 6Fh ; 'o'
        mov dx, 5A6h
        xor ax, ax
        mov bx, 2A70h

loc_47CF1:              ; CODE XREF: readLevelsLst+83j
        push(cx);
        push    dx
        push    ax
        push    bx
        mov cx, ax
        mov bx, lastFileHandle
        mov ax, 4200h
        int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
                    ; AL = method: offset from beginning of file
        pop dx
        push    dx
        mov cx, 17h
        mov bx, lastFileHandle
        mov ax, 3F00h
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        pop bx
        pop ax
        pop dx
        pop(cx);
        jb  short loc_47D81
        add dx, levelDataLength
        adc ax, 0
        add bx, 1Ch
        loop    loc_47CF1
        cmp byte_59B62, 0
        jz  short loc_47D8D
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short loc_47D35
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47D35:              ; CODE XREF: readLevelsLst+95j
        mov ax, 3C00h
        mov cx, 0
        mov dx, offset aLevel_lst ; "LEVEL.LST"
        int 21h     ; DOS - 2+ - CREATE A FILE WITH HANDLE (CREAT)
                    ; CX = attributes for file
                    ; DS:DX -> ASCIZ filename (may include drive and path)
        jnb short loc_47D45
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47D45:              ; CODE XREF: readLevelsLst+A5j
        mov bx, ax
        mov lastFileHandle, bx
        mov cx, 0C24h
        mov dx, 2A6Ch
        mov ax, 4000h
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        jnb short loc_47D5B
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47D5B:              ; CODE XREF: readLevelsLst+BBj
        jmp short loc_47D8D
// ; ---------------------------------------------------------------------------

loc_47D5D:              ; CODE XREF: readLevelsLst+17j
        cmp ax, 2
        jnz short loc_47D6A
        call    sub_47F39
        jb  short loc_47D6A
        jmp readLevelsLst
// ; ---------------------------------------------------------------------------

loc_47D6A:              ; CODE XREF: readLevelsLst+C5j
                    ; readLevelsLst+CAj
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47D6D:              ; CODE XREF: readLevelsLst+Aj
        mov lastFileHandle, ax
        mov bx, lastFileHandle
        mov ax, 3F00h
        mov cx, 0C24h
        mov dx, 2A6Ch
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_47D8D

loc_47D81:              ; CODE XREF: readLevelsLst+77j
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47D8D:              ; CODE XREF: readLevelsLst+8Aj
                    ; readLevelsLst:loc_47D5Bj ...
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short locret_47D9B
        jmp exit
// ; ---------------------------------------------------------------------------

locret_47D9B:               ; CODE XREF: readLevelsLst+FBj
        retn
readLevelsLst   endp


; =============== S U B R O U T I N E =======================================


readGfxDat  proc near       ; CODE XREF: readGfxDat+14j
                    ; readEverything+1Ep
        mov ax, 3D00h
        mov dx, offset aGfx_dat ; "GFX.DAT"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_47DB5
        cmp ax, 2
        jnz short loc_47DB2
        call    sub_47F39
        jb  short loc_47DB2
        jmp short readGfxDat
// ; ---------------------------------------------------------------------------

loc_47DB2:              ; CODE XREF: readGfxDat+Dj
                    ; readGfxDat+12j
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47DB5:              ; CODE XREF: readGfxDat+8j
        mov lastFileHandle, ax
        mov bx, lastFileHandle
        push    ds
        mov ax, seg gfxseg
        mov ds, ax
        assume ds:nothing
        mov ax, 3F00h
        mov cx, 7D00h
        mov dx, 0
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_47DD3
        pop ds
        assume ds:data
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47DD3:              ; CODE XREF: readGfxDat+31j
        pop ds
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short locret_47DE2
        jmp exit
// ; ---------------------------------------------------------------------------

locret_47DE2:               ; CODE XREF: readGfxDat+41j
        retn
readGfxDat  endp


; =============== S U B R O U T I N E =======================================


readControlsDat proc near       ; CODE XREF: readControlsDat+14j
                    ; readEverything+Cp
        mov ax, 3D00h
        mov dx, offset aControls_dat ; "CONTROLS.DAT"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_47DFC
        cmp ax, 2
        jnz short loc_47DF9
        call    sub_47F39
        jb  short loc_47DF9
        jmp short readControlsDat
// ; ---------------------------------------------------------------------------

loc_47DF9:              ; CODE XREF: readControlsDat+Dj
                    ; readControlsDat+12j
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47DFC:              ; CODE XREF: readControlsDat+8j
        mov lastFileHandle, ax
        mov bx, lastFileHandle
        push    ds
        mov ax, seg controlsseg
        mov ds, ax
        assume ds:controlsseg
        mov ax, 3F00h
        mov cx, 7D00h
        mov dx, 0
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_47E1A
        pop ds
        assume ds:data
        jmp exit
// ; ---------------------------------------------------------------------------

loc_47E1A:              ; CODE XREF: readControlsDat+31j
        pop ds
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short locret_47E29
        jmp exit
// ; ---------------------------------------------------------------------------

locret_47E29:               ; CODE XREF: readControlsDat+41j
        retn
readControlsDat endp


; =============== S U B R O U T I N E =======================================


readPlayersLst  proc near       ; CODE XREF: readEverything+1Bp
                    ; sub_4B419+149p
        cmp byte_59B85, 0
        jnz short locret_47E4F
        mov ax, 3D00h
        mov dx, offset aPlayer_lst ; "PLAYER.LST"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jb  short locret_47E4F
        mov bx, ax
        mov ax, 3F00h
        mov cx, 0A00h
        mov dx, 8A9Ch
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jb  short locret_47E4F
        mov ax, 3E00h
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle

locret_47E4F:               ; CODE XREF: readPlayersLst+5j
                    ; readPlayersLst+Fj ...
        retn
readPlayersLst  endp


; =============== S U B R O U T I N E =======================================


readHallfameLst proc near       ; CODE XREF: readEverything+18p
                    ; sub_4B419+146p
        cmp byte_59B85, 0
        jnz short locret_47E75
        mov ax, 3D00h
        mov dx, offset aHallfame_lst ; "HALLFAME.LST"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jb  short locret_47E75
        mov bx, ax
        mov ax, 3F00h
        mov cx, 24h ; '$'
        mov dx, 9514h
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jb  short locret_47E75
        mov ax, 3E00h
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle

locret_47E75:               ; CODE XREF: readHallfameLst+5j
                    ; readHallfameLst+Fj ...
        retn
readHallfameLst endp


; =============== S U B R O U T I N E =======================================


readEverything  proc near       ; CODE XREF: start+2DBp start+2E3p ...
        call    readPallettes
        call    readChars6Dat
        call    readPanelDat
        call    readMenuDat
        call    readControlsDat
        call    readLevelsLst
        call    readDemo
        call    readBackDat
        call    readHallfameLst
        call    readPlayersLst
        call    readGfxDat
        retn
readEverything  endp


; =============== S U B R O U T I N E =======================================


sub_47E98   proc near       ; CODE XREF: sub_47F39+4Ap
                    ; sub_4AF0C+216p ...
        mov byte_59B86, 0

keyIsPressed:               ; CODE XREF: sub_47E98+16j
        cmp byte_5197E, 0
        jz  short loc_47EA9
        mov byte_59B86, 0FFh

loc_47EA9:              ; CODE XREF: sub_47E98+Aj
        cmp keyPressed, 0
        jnz short keyIsPressed

mouseIsClicked:              ; CODE XREF: sub_47E98+1Ej
        call    getMouseStatus
        cmp bx, 0
        jnz short mouseIsClicked     ; wait for mouse to not be clicked

loc_47EB8:              ; CODE XREF: sub_47E98+28j
        call    sub_48E59
        cmp byte_50941, 4
        jg  short loc_47EB8
        mov cx, 1068h
        push(cx);

loc_47EC6:              ; CODE XREF: sub_47E98+57j
        pop(cx);
        dec cx
        push(cx);
        call    videoloop
        call    sub_4D457
        pop(cx);
        push(cx);
        cmp cx, 0
        jz  short loc_47EF1
        call    getMouseStatus
        cmp bx, 0
        jnz short loc_47F02
        cmp keyPressed, 0
        jnz short loc_47F0C
        call    sub_48E59
        cmp byte_50941, 4
        jg  short loc_47F21
        jmp short loc_47EC6
// ; ---------------------------------------------------------------------------

loc_47EF1:              ; CODE XREF: sub_47E98+3Cj
                    ; sub_47E98+72j ...
        pop(cx);
        mov word_510A2, 0
        cmp byte_59B86, 0
        clc
        jz  short locret_47F01
        stc

locret_47F01:               ; CODE XREF: sub_47E98+66j
        retn
// ; ---------------------------------------------------------------------------

loc_47F02:              ; CODE XREF: sub_47E98+44j
                    ; sub_47E98+70j
        call    getMouseStatus
        cmp bx, 0
        jnz short loc_47F02
        jmp short loc_47EF1
// ; ---------------------------------------------------------------------------

loc_47F0C:              ; CODE XREF: sub_47E98+4Bj
                    ; sub_47E98+85j
        cmp byte_5197E, 0
        jz  short loc_47F18
        mov byte_59B86, 0FFh

loc_47F18:              ; CODE XREF: sub_47E98+79j
        cmp keyPressed, 0
        jnz short loc_47F0C
        jmp short loc_47EF1
// ; ---------------------------------------------------------------------------

loc_47F21:              ; CODE XREF: sub_47E98+55j
                    ; sub_47E98+9Dj
        cmp byte_5197E, 0
        jz  short loc_47F2D
        mov byte_59B86, 0FFh

loc_47F2D:              ; CODE XREF: sub_47E98+8Ej
        call    sub_48E59
        cmp byte_50941, 4
        jg  short loc_47F21
        jmp short loc_47EF1
sub_47E98   endp


; =============== S U B R O U T I N E =======================================


sub_47F39   proc near       ; CODE XREF: readPallettes+Fp
                    ; readMoving+15p ...
        call    enableFloppy?
        mov si, 60D5h
        call    sub_4D836
        call    vgaloadbackseg
        cmp byte_53A10, 1
        jnz short loc_47F56
        mov si, 370Fh
        mov byte_53A10, 2
        jmp short loc_47F5E
// ; ---------------------------------------------------------------------------

loc_47F56:              ; CODE XREF: sub_47F39+11j
        mov si, 3701h
        mov byte_53A10, 1

loc_47F5E:              ; CODE XREF: sub_47F39+1Bj
        mov di, 7D3Bh
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, 5FD5h
        call    sub_4D836
        mov bx, 4D84h
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        call    sub_47E98
        pushf
        mov si, 60D5h
        call    sub_4D836
        popf
        retn
sub_47F39   endp

// ; ---------------------------------------------------------------------------
        nop

; =============== S U B R O U T I N E =======================================


movefun   proc near       ; DATA XREF: data:160Co
        cmp byte ptr leveldata[si], 1
        jz  short loc_47F98
        retn
// ; ---------------------------------------------------------------------------

loc_47F98:              ; CODE XREF: movefun+5j
        mov ax, leveldata[si]
        cmp ax, 1
        jz  short loc_47FA4
        jmp loc_48035
// ; ---------------------------------------------------------------------------

loc_47FA4:              ; CODE XREF: movefun+Fj
        cmp byte_51035, 2
        jnz short loc_47FAC
        retn
// ; ---------------------------------------------------------------------------

loc_47FAC:              ; CODE XREF: movefun+19j
        mov ax, leveldata[si+78h]
        cmp ax, 0
        jz  short loc_47FF4
        cmp ax, 1
        jz  short loc_47FC5
        cmp ax, 4
        jz  short loc_47FC5
        cmp ax, 5
        jz  short loc_47FC5
        retn
// ; ---------------------------------------------------------------------------

loc_47FC5:              ; CODE XREF: movefun+28j
                    ; movefun+2Dj ...
        cmp leveldata[si+78h-2], 0
        jz  short loc_47FFB
        cmp leveldata[si+78h-2], 8888h
        jz  short loc_47FFB
        cmp leveldata[si+78h-2], 0AAAAh
        jz  short loc_47FFB

loc_47FDC:              ; CODE XREF: movefun+72j
                    ; movefun+1F1j
        cmp leveldata[si+78h+2], 0
        jz  short loc_48011
        cmp leveldata[si+78h+2], 8888h
        jz  short loc_48011
        cmp leveldata[si+78h+2], 0AAAAh
        jz  short loc_48011
        retn
// ; ---------------------------------------------------------------------------

loc_47FF4:              ; CODE XREF: movefun+23j
        mov byte ptr leveldata[si+1], 40h ; '@'
        jmp short loc_48035
// ; ---------------------------------------------------------------------------

loc_47FFB:              ; CODE XREF: movefun+3Aj
                    ; movefun+42j ...
        cmp leveldata[si-2], 0
        jz  short loc_48004
        jmp short loc_47FDC
// ; ---------------------------------------------------------------------------

loc_48004:              ; CODE XREF: movefun+70j
        mov byte ptr leveldata[si+1], 50h ; 'P'
        mov leveldata[si-2], 8888h
        jmp short loc_48035
// ; ---------------------------------------------------------------------------

loc_48011:              ; CODE XREF: movefun+51j
                    ; movefun+59j ...
        cmp leveldata[si+2], 0
        jz  short loc_48028
        cmp leveldata[si+2], 9999h
        jnz short locret_48027
        cmp leveldata[si-78h+2h], 1
        jz  short loc_48028

locret_48027:               ; CODE XREF: movefun+8Ej
        retn
// ; ---------------------------------------------------------------------------

loc_48028:              ; CODE XREF: movefun+86j
                    ; movefun+95j
        mov byte ptr leveldata[si+1], 60h ; '`'
        mov leveldata[si+2], 8888h
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_48035:              ; CODE XREF: movefun+11j
                    ; movefun+69j ...
        mov bl, byte ptr leveldata[si+1]
        xor bh, bh
        al = bl
        and al, 0F0h
        cmp al, 10h
        jnz short loc_48045
        jmp short loc_48078
// ; ---------------------------------------------------------------------------

loc_48045:              ; CODE XREF: movefun+B1j
        cmp al, 20h ; ' '
        jnz short loc_4804C
        jmp loc_48212
// ; ---------------------------------------------------------------------------

loc_4804C:              ; CODE XREF: movefun+B7j
        cmp al, 30h ; '0'
        jnz short loc_48053
        jmp loc_48277
// ; ---------------------------------------------------------------------------

loc_48053:              ; CODE XREF: movefun+BEj
        cmp byte_51035, 2
        jnz short loc_4805B
        retn
// ; ---------------------------------------------------------------------------

loc_4805B:              ; CODE XREF: movefun+C8j
        cmp al, 40h ; '@'
        jnz short loc_48062
        jmp loc_482DC
// ; ---------------------------------------------------------------------------

loc_48062:              ; CODE XREF: movefun+CDj
        cmp al, 50h ; 'P'
        jnz short loc_48069
        jmp loc_4830A
// ; ---------------------------------------------------------------------------

loc_48069:              ; CODE XREF: movefun+D4j
        cmp al, 60h ; '`'
        jnz short loc_48070
        jmp loc_48378
// ; ---------------------------------------------------------------------------

loc_48070:              ; CODE XREF: movefun+DBj
        cmp al, 70h ; 'p'
        jnz short locret_48077
        jmp loc_483E6
// ; ---------------------------------------------------------------------------

locret_48077:               ; CODE XREF: movefun+E2j
        retn
// ; ---------------------------------------------------------------------------

loc_48078:              ; CODE XREF: movefun+B3j
                    ; movefun+475j
        xor bh, bh
        shl bl, 1
        shl bl, 1
        and bl, 1Fh
        mov di, [si+60DDh]
        add di, [bx+6C95h]
        push    si
        mov si, word_515A2
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 12h

loc_48096:              ; CODE XREF: movefun+10Ej
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_48096
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 16h
        jnz short loc_480BB
        mov [si+1835h], bl
        sub si, 78h ; 'x'
        call    sub_488DC
        add si, 78h ; 'x'
        retn
// ; ---------------------------------------------------------------------------

loc_480BB:              ; CODE XREF: movefun+11Bj
        cmp bl, 18h
        jge short loc_480C5
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_480C5:              ; CODE XREF: movefun+12Ej
        mov byte ptr [si+1835h], 0
        cmp byte_51035, 2
        jnz short loc_480D2
        retn
// ; ---------------------------------------------------------------------------

loc_480D2:              ; CODE XREF: movefun+13Fj
        cmp word ptr [si+18ACh], 0
        jnz short loc_480DC
        jmp loc_4816D
// ; ---------------------------------------------------------------------------

loc_480DC:              ; CODE XREF: movefun+147j
        cmp word ptr [si+18ACh], 9999h
        jnz short loc_480E7
        jmp loc_4816D
// ; ---------------------------------------------------------------------------

loc_480E7:              ; CODE XREF: movefun+152j
        cmp byte ptr [si+18ACh], 3
        jnz short loc_480F1
        jmp loc_481A4
// ; ---------------------------------------------------------------------------

loc_480F1:              ; CODE XREF: movefun+15Cj
        cmp byte ptr [si+18ACh], 11h
        jnz short loc_480FB
        jmp loc_481FE
// ; ---------------------------------------------------------------------------

loc_480FB:              ; CODE XREF: movefun+166j
        cmp word ptr [si+18ACh], 2BBh
        jnz short loc_48106
        jmp loc_481C6
// ; ---------------------------------------------------------------------------

loc_48106:              ; CODE XREF: movefun+171j
        cmp word ptr [si+18ACh], 4BBh
        jnz short loc_48111
        jmp loc_481E2
// ; ---------------------------------------------------------------------------

loc_48111:              ; CODE XREF: movefun+17Cj
        cmp byte ptr [si+18ACh], 18h
        jnz short loc_4811B
        jmp loc_481FE
// ; ---------------------------------------------------------------------------

loc_4811B:              ; CODE XREF: movefun+186j
        cmp word ptr [si+18ACh], 8
        jnz short loc_48125
        jmp loc_48205
// ; ---------------------------------------------------------------------------

loc_48125:              ; CODE XREF: movefun+190j
        call    sound?7
        cmp word ptr [si+18ACh], 1
        jz  short loc_4813E
        cmp word ptr [si+18ACh], 4
        jz  short loc_4813E
        cmp word ptr [si+18ACh], 5
        jz  short loc_4813E
        retn
// ; ---------------------------------------------------------------------------

loc_4813E:              ; CODE XREF: movefun+19Dj
                    ; movefun+1A4j ...
        cmp word ptr [si+18AAh], 0
        jz  short loc_4817A
        cmp word ptr [si+18AAh], 8888h
        jz  short loc_4817A
        cmp word ptr [si+18AAh], 0AAAAh
        jz  short loc_4817A
        cmp word ptr [si+18AEh], 0
        jz  short loc_48190
        cmp word ptr [si+18AEh], 8888h
        jz  short loc_48190
        cmp word ptr [si+18AEh], 0AAAAh

loc_4816A:
        jz  short loc_48190
        retn
// ; ---------------------------------------------------------------------------

loc_4816D:              ; CODE XREF: movefun+149j
                    ; movefun+154j
        mov word ptr leveldata[si], 7001h
        mov word ptr [si+18ACh], 9999h
        retn
// ; ---------------------------------------------------------------------------

loc_4817A:              ; CODE XREF: movefun+1B3j
                    ; movefun+1BBj ...
        cmp word ptr [si+1832h], 0
        jz  short loc_48184
        jmp loc_47FDC
// ; ---------------------------------------------------------------------------

loc_48184:              ; CODE XREF: movefun+1EFj
        mov byte ptr [si+1835h], 50h ; 'P'
        mov word ptr [si+1832h], 8888h
        retn
// ; ---------------------------------------------------------------------------

loc_48190:              ; CODE XREF: movefun+1CAj
                    ; movefun+1D2j ...
        cmp word ptr [si+1836h], 0
        jz  short loc_48198
        retn
// ; ---------------------------------------------------------------------------

loc_48198:              ; CODE XREF: movefun+205j
        mov byte ptr [si+1835h], 60h ; '`'
        mov word ptr [si+1836h], 8888h
        retn
// ; ---------------------------------------------------------------------------

loc_481A4:              ; CODE XREF: movefun+15Ej
        mov bl, [si+18ADh]
        cmp bl, 0Eh
        jz  short locret_48211
        cmp bl, 0Fh
        jz  short locret_48211
        cmp bl, 28h ; '('
        jz  short locret_48211
        cmp bl, 29h ; ')'
        jz  short locret_48211
        cmp bl, 25h ; '%'
        jz  short locret_48211
        cmp bl, 26h ; '&'
        jz  short locret_48211

loc_481C6:              ; CODE XREF: movefun+173j
        cmp byte ptr [si+18AAh], 18h
        jnz short loc_481D3
        mov word ptr [si+18ACh], 18h

loc_481D3:              ; CODE XREF: movefun+23Bj
        cmp byte ptr [si+18AAh], 1Fh
        jz  short loc_481E0
        mov word ptr [si+18AAh], 0

loc_481E0:              ; CODE XREF: movefun+248j
        jmp short loc_481FE
// ; ---------------------------------------------------------------------------

loc_481E2:              ; CODE XREF: movefun+17Ej
        cmp byte ptr [si+18AEh], 18h
        jnz short loc_481EF
        mov word ptr [si+18ACh], 18h

loc_481EF:              ; CODE XREF: movefun+257j
        cmp byte ptr [si+18AEh], 1Fh
        jz  short loc_481FC
        mov word ptr [si+18AEh], 0

loc_481FC:              ; CODE XREF: movefun+264j
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_481FE:              ; CODE XREF: movefun+168j
                    ; movefun+188j ...
        add si, 78h ; 'x'
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_48205:              ; CODE XREF: movefun+192j
        add si, 78h ; 'x'
        mov bx, si
        shr bx, 1
        mov byte ptr [bx+2434h], 6

locret_48211:               ; CODE XREF: movefun+21Bj
                    ; movefun+220j ...
        retn
// ; ---------------------------------------------------------------------------

loc_48212:              ; CODE XREF: movefun+B9j
        and bl, 7
        shl bl, 1
        xor bh, bh
        mov di, [si+6155h]
        push    si
        mov si, 1294h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax

loc_48228:
        mov cx, 10h

loc_4822B:              ; CODE XREF: movefun+2A5j
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_4822B
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 24h ; '$'
        jnz short loc_4824A
        mov word ptr [si+1836h], 0AAAAh

loc_4824A:              ; CODE XREF: movefun+2B2j
        cmp bl, 26h ; '&'
        jnz short loc_4825D
        mov [si+1835h], bl
        add si, 2
        call    sub_488DC
        sub si, 2
        retn
// ; ---------------------------------------------------------------------------

loc_4825D:              ; CODE XREF: movefun+2BDj
        cmp bl, 28h ; '('
        jge short loc_48267
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_48267:              ; CODE XREF: movefun+2D0j
        mov word ptr leveldata[si], 0FFFFh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 1001h
        retn
// ; ---------------------------------------------------------------------------

loc_48277:              ; CODE XREF: movefun+C0j
        and bl, 7
        shl bl, 1
        xor bh, bh
        mov di, [si+6153h]
        push    si
        mov si, 12A4h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax

loc_4828D:
        mov cx, 10h

loc_48290:              ; CODE XREF: movefun+30Aj
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_48290
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 34h ; '4'
        jnz short loc_482AF
        mov word ptr [si+1832h], 0AAAAh

loc_482AF:              ; CODE XREF: movefun+317j
        cmp bl, 36h ; '6'
        jnz short loc_482C2
        mov [si+1835h], bl
        sub si, 2
        call    sub_488DC
        add si, 2
        retn
// ; ---------------------------------------------------------------------------

loc_482C2:              ; CODE XREF: movefun+322j
        cmp bl, 38h ; '8'
        jge short loc_482CC
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_482CC:              ; CODE XREF: movefun+335j
        mov word ptr leveldata[si], 0FFFFh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 1001h
        retn
// ; ---------------------------------------------------------------------------

loc_482DC:              ; CODE XREF: movefun+CFj
        inc bl
        cmp bl, 42h ; 'B'
        jge short loc_482E8
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_482E8:              ; CODE XREF: movefun+351j
        cmp word ptr [si+18ACh], 0
        jz  short loc_482F6
        dec bl
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_482F6:              ; CODE XREF: movefun+35Dj
        mov word ptr leveldata[si], 0FFFFh
        add si, 78h ; 'x'
        mov byte ptr leveldata[si], 1
        mov byte ptr [si+1835h], 10h
        retn
// ; ---------------------------------------------------------------------------

loc_4830A:              ; CODE XREF: movefun+D6j
        xor bh, bh
        and bl, 0Fh
        shl bl, 1
        mov di, [si+6153h]
        push    si
        mov si, 1294h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_48323:              ; CODE XREF: movefun+39Dj
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_48323
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 52h ; 'R'
        jge short loc_48341
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_48341:              ; CODE XREF: movefun+3AAj
        cmp word ptr [si+18AAh], 0
        jnz short loc_48371
        cmp word ptr [si+1832h], 0
        jz  short loc_48357
        cmp word ptr [si+1832h], 8888h
        jnz short loc_48371

loc_48357:              ; CODE XREF: movefun+3BDj
        mov leveldata[si], 0FFFFh
        sub si, 2
        mov byte ptr leveldata[si], 1
        mov byte ptr [si+1835h], 22h ; '"'
        mov word ptr [si+18ACh], 0FFFFh
        retn
// ; ---------------------------------------------------------------------------

loc_48371:              ; CODE XREF: movefun+3B6j
                    ; movefun+3C5j
        dec bl
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_48378:              ; CODE XREF: movefun+DDj
        xor bh, bh
        and bl, 7
        shl bl, 1
        mov di, [si+6155h]
        push    si
        mov si, 12A4h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_48391:              ; CODE XREF: movefun+40Bj
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_48391
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 62h ; 'b'
        jge short loc_483AF
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_483AF:              ; CODE XREF: movefun+418j
        cmp word ptr [si+18AEh], 0
        jnz short loc_483DF
        cmp word ptr [si+1836h], 0
        jz  short loc_483C5
        cmp word ptr [si+1836h], 8888h
        jnz short loc_483DF

loc_483C5:              ; CODE XREF: movefun+42Bj
        mov word ptr leveldata[si], 0FFFFh
        add si, 2
        mov byte ptr leveldata[si], 1
        mov byte ptr [si+1835h], 32h ; '2'

loc_483D8:
        mov word ptr [si+18ACh], 0FFFFh

locret_483DE:
        retn
// ; ---------------------------------------------------------------------------

loc_483DF:              ; CODE XREF: movefun+424j
                    ; movefun+433j
        dec bl
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_483E6:              ; CODE XREF: movefun+E4j
        cmp word ptr [si+18ACh], 0
        jz  short loc_483F6
        cmp word ptr [si+18ACh], 9999h
        jz  short loc_483F6
        retn
// ; ---------------------------------------------------------------------------

loc_483F6:              ; CODE XREF: movefun+45Bj
                    ; movefun+463j
        mov word ptr leveldata[si], 0FFFFh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 1001h
        jmp loc_48078
movefun   endp


; =============== S U B R O U T I N E =======================================


movefun2  proc near       ; DATA XREF: data:1612o
        cmp byte ptr leveldata[si], 4
        jz  short loc_48410
        retn
// ; ---------------------------------------------------------------------------

loc_48410:              ; CODE XREF: movefun2+5j
        mov ax, [si+1834h]
        cmp ax, 4
        jz  short loc_4841B
        jmp short loc_48495
// ; ---------------------------------------------------------------------------

loc_4841B:              ; CODE XREF: movefun2+Fj
        mov ax, [si+18ACh]
        cmp ax, 0
        jz  short loc_48463
        cmp ax, 1
        jz  short loc_48434
        cmp ax, 4
        jz  short loc_48434
        cmp ax, 5
        jz  short loc_48434
        retn
// ; ---------------------------------------------------------------------------

loc_48434:              ; CODE XREF: movefun2+1Fj
                    ; movefun2+24j ...
        cmp word ptr [si+18AAh], 0
        jz  short loc_4846A
        cmp word ptr [si+18AAh], 8888h
        jz  short loc_4846A
        cmp word ptr [si+18AAh], 0AAAAh
        jz  short loc_4846A

loc_4844B:              ; CODE XREF: movefun2+69j
                    ; movefun2+1C7j
        cmp word ptr [si+18AEh], 0
        jz  short loc_48480

loc_48452:
        cmp word ptr [si+18AEh], 8888h
        jz  short loc_48480
        cmp word ptr [si+18AEh], 0AAAAh
        jz  short loc_48480
        retn
// ; ---------------------------------------------------------------------------

loc_48463:              ; CODE XREF: movefun2+1Aj
        mov byte ptr [si+1835h], 40h ; '@'
        jmp short loc_48495
// ; ---------------------------------------------------------------------------

loc_4846A:              ; CODE XREF: movefun2+31j
                    ; movefun2+39j ...
        cmp word ptr [si+1832h], 0
        jz  short loc_48473
        jmp short loc_4844B
// ; ---------------------------------------------------------------------------

loc_48473:              ; CODE XREF: movefun2+67j
        mov byte ptr [si+1835h], 50h ; 'P'
        mov word ptr [si+1832h], 8888h
        jmp short loc_48495
// ; ---------------------------------------------------------------------------

loc_48480:              ; CODE XREF: movefun2+48j
                    ; movefun2+50j ...
        cmp word ptr [si+1836h], 0
        jz  short loc_48488
        retn
// ; ---------------------------------------------------------------------------

loc_48488:              ; CODE XREF: movefun2+7Dj
        mov byte ptr [si+1835h], 60h ; '`'
        mov word ptr [si+1836h], 8888h
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_48495:              ; CODE XREF: movefun2+11j
                    ; movefun2+60j ...
        mov bl, [si+1835h]
        xor bh, bh
        al = bl
        and al, 0F0h
        cmp al, 10h
        jnz short loc_484A5
        jmp short loc_484D0
// ; ---------------------------------------------------------------------------

loc_484A5:              ; CODE XREF: movefun2+99j
        cmp al, 20h ; ' '
        jnz short loc_484AC
        jmp loc_4861B
// ; ---------------------------------------------------------------------------

loc_484AC:              ; CODE XREF: movefun2+9Fj
        cmp al, 30h ; '0'
        jnz short loc_484B3
        jmp loc_48677
// ; ---------------------------------------------------------------------------

loc_484B3:              ; CODE XREF: movefun2+A6j
        cmp al, 40h ; '@'
        jnz short loc_484BA
        jmp loc_486D2
// ; ---------------------------------------------------------------------------

loc_484BA:              ; CODE XREF: movefun2+ADj
        cmp al, 50h ; 'P'
        jnz short loc_484C1
        jmp loc_48700
// ; ---------------------------------------------------------------------------

loc_484C1:              ; CODE XREF: movefun2+B4j
        cmp al, 60h ; '`'
        jnz short loc_484C8
        jmp loc_4876E
// ; ---------------------------------------------------------------------------

loc_484C8:              ; CODE XREF: movefun2+BBj
        cmp al, 70h ; 'p'

loc_484CA:
        jnz short locret_484CF

loc_484CC:
        jmp loc_487DC
// ; ---------------------------------------------------------------------------

locret_484CF:               ; CODE XREF: movefun2:loc_484CAj
        retn
// ; ---------------------------------------------------------------------------

loc_484D0:              ; CODE XREF: movefun2+9Bj
                    ; movefun2+3F3j
        xor bh, bh
        shl bl, 1
        shl bl, 1
        and bl, 1Fh
        mov di, [si+60DDh]
        add di, [bx+6C95h]
        push    si
        mov si, word_515C4
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 12h

loc_484EE:              ; CODE XREF: movefun2+EEj
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_484EE
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 16h
        jnz short loc_48513
        mov [si+1835h], bl
        sub si, 78h ; 'x'
        call    sub_48957
        add si, 78h ; 'x'
        retn
// ; ---------------------------------------------------------------------------

loc_48513:              ; CODE XREF: movefun2+FBj
        cmp bl, 18h
        jge short loc_4851D
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4851D:              ; CODE XREF: movefun2+10Ej
        mov byte ptr [si+1835h], 0
        cmp word ptr [si+18ACh], 0
        jnz short loc_4852C
        jmp loc_485BB
// ; ---------------------------------------------------------------------------

loc_4852C:              ; CODE XREF: movefun2+11Fj
        cmp word ptr [si+18ACh], 9999h
        jnz short loc_48537
        jmp loc_485BB
// ; ---------------------------------------------------------------------------

loc_48537:              ; CODE XREF: movefun2+12Aj
        cmp byte ptr [si+18ACh], 3
        jnz short loc_48541
        jmp loc_485F2
// ; ---------------------------------------------------------------------------

loc_48541:              ; CODE XREF: movefun2+134j
        cmp word ptr [si+18ACh], 14h
        jnz short loc_4854B
        jmp loc_48614
// ; ---------------------------------------------------------------------------

loc_4854B:              ; CODE XREF: movefun2+13Ej
        cmp byte ptr [si+18ACh], 11h
        jnz short loc_48555
        jmp loc_48614
// ; ---------------------------------------------------------------------------

loc_48555:              ; CODE XREF: movefun2+148j
        cmp byte ptr [si+18ACh], 18h
        jnz short loc_4855F
        jmp loc_48614
// ; ---------------------------------------------------------------------------

loc_4855F:              ; CODE XREF: movefun2+152j
        cmp word ptr [si+18ACh], 12h
        jnz short loc_48569
        jmp loc_48614
// ; ---------------------------------------------------------------------------

loc_48569:              ; CODE XREF: movefun2+15Cj
        cmp word ptr [si+18ACh], 8
        jnz short loc_48573
        jmp loc_48614
// ; ---------------------------------------------------------------------------

loc_48573:              ; CODE XREF: movefun2+166j
        call    sound?7
        cmp word ptr [si+18ACh], 1
        jz  short loc_4858C
        cmp word ptr [si+18ACh], 4
        jz  short loc_4858C
        cmp word ptr [si+18ACh], 5
        jz  short loc_4858C
        retn
// ; ---------------------------------------------------------------------------

loc_4858C:              ; CODE XREF: movefun2+173j
                    ; movefun2+17Aj ...
        cmp word ptr [si+18AAh], 0
        jz  short loc_485C8
        cmp word ptr [si+18AAh], 8888h
        jz  short loc_485C8
        cmp word ptr [si+18AAh], 0AAAAh
        jz  short loc_485C8
        cmp word ptr [si+18AEh], 0
        jz  short loc_485DE
        cmp word ptr [si+18AEh], 8888h
        jz  short loc_485DE
        cmp word ptr [si+18AEh], 0AAAAh
        jz  short loc_485DE
        retn
// ; ---------------------------------------------------------------------------

loc_485BB:              ; CODE XREF: movefun2+121j
                    ; movefun2+12Cj
        mov word ptr leveldata[si], 7004h
        mov word ptr [si+18ACh], 9999h
        retn
// ; ---------------------------------------------------------------------------

loc_485C8:              ; CODE XREF: movefun2+189j
                    ; movefun2+191j ...
        cmp word ptr [si+1832h], 0
        jz  short loc_485D2
        jmp loc_4844B
// ; ---------------------------------------------------------------------------

loc_485D2:              ; CODE XREF: movefun2+1C5j
        mov byte ptr [si+1835h], 50h ; 'P'
        mov word ptr [si+1832h], 8888h
        retn
// ; ---------------------------------------------------------------------------

loc_485DE:              ; CODE XREF: movefun2+1A0j
                    ; movefun2+1A8j ...
        cmp word ptr [si+1836h], 0
        jz  short loc_485E6
        retn
// ; ---------------------------------------------------------------------------

loc_485E6:              ; CODE XREF: movefun2+1DBj
        mov byte ptr [si+1835h], 60h ; '`'
        mov word ptr [si+1836h], 8888h
        retn
// ; ---------------------------------------------------------------------------

loc_485F2:              ; CODE XREF: movefun2+136j
        mov bl, [si+18ADh]
        cmp bl, 0Eh
        jz  short locret_4861A
        cmp bl, 0Fh
        jz  short locret_4861A
        cmp bl, 28h ; '('
        jz  short locret_4861A
        cmp bl, 29h ; ')'
        jz  short locret_4861A
        cmp bl, 25h ; '%'
        jz  short locret_4861A
        cmp bl, 26h ; '&'
        jz  short locret_4861A

loc_48614:              ; CODE XREF: movefun2+140j
                    ; movefun2+14Aj ...
        add si, 78h ; 'x'
        call    sub_4A61F

locret_4861A:               ; CODE XREF: movefun2+1F1j
                    ; movefun2+1F6j ...
        retn
// ; ---------------------------------------------------------------------------

loc_4861B:              ; CODE XREF: movefun2+A1j
        and bl, 7
        shl bl, 1
        xor bh, bh
        mov di, [si+6155h]
        push    si
        mov si, 12B6h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_48634:              ; CODE XREF: movefun2+236j
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_48634
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 24h ; '$'
        jnz short loc_48653
        mov word ptr [si+1836h], 0AAAAh

loc_48653:              ; CODE XREF: movefun2+243j
        cmp bl, 26h ; '&'
        jnz short loc_48666
        mov [si+1835h], bl
        add si, 2
        call    sub_48957
        sub si, 2
        retn
// ; ---------------------------------------------------------------------------

loc_48666:              ; CODE XREF: movefun2+24Ej
        cmp bl, 28h ; '('
        jge short loc_48670
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_48670:              ; CODE XREF: movefun2+261j
        mov word ptr leveldata[si], 7004h
        retn
// ; ---------------------------------------------------------------------------

loc_48677:              ; CODE XREF: movefun2+A8j
        and bl, 7
        shl bl, 1
        xor bh, bh
        mov di, [si+6153h]
        push    si
        mov si, 12C6h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_48690:              ; CODE XREF: movefun2+292j
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_48690
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 34h ; '4'
        jnz short loc_486AF
        mov word ptr [si+1832h], 0AAAAh

loc_486AF:              ; CODE XREF: movefun2+29Fj
        cmp bl, 36h ; '6'
        jnz short loc_486C1
        mov [si+1835h], bl
        sub si, 2
        call    sub_48957
        add si, 2

loc_486C1:              ; CODE XREF: movefun2+2AAj
        cmp bl, 38h ; '8'
        jge short loc_486CB
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_486CB:              ; CODE XREF: movefun2+2BCj
        mov word ptr leveldata[si], 7004h
        retn
// ; ---------------------------------------------------------------------------

loc_486D2:              ; CODE XREF: movefun2+AFj
        inc bl
        cmp bl, 42h ; 'B'
        jge short loc_486DE
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_486DE:              ; CODE XREF: movefun2+2CFj
        cmp word ptr [si+18ACh], 0
        jz  short loc_486EC
        dec bl
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_486EC:              ; CODE XREF: movefun2+2DBj
        mov word ptr leveldata[si], 0FFFFh
        add si, 78h ; 'x'
        mov byte ptr leveldata[si], 4
        mov byte ptr [si+1835h], 10h
        retn
// ; ---------------------------------------------------------------------------

loc_48700:              ; CODE XREF: movefun2+B6j
        xor bh, bh
        and bl, 0Fh
        shl bl, 1
        mov di, [si+6153h]
        push    si
        mov si, 12B6h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_48719:              ; CODE XREF: movefun2+31Bj
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_48719
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 52h ; 'R'
        jge short loc_48737
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_48737:              ; CODE XREF: movefun2+328j
        cmp word ptr [si+18AAh], 0
        jnz short loc_48767
        cmp word ptr [si+1832h], 0
        jz  short loc_4874D
        cmp word ptr [si+1832h], 8888h
        jnz short loc_48767

loc_4874D:              ; CODE XREF: movefun2+33Bj
        mov word ptr leveldata[si], 0FFFFh
        sub si, 2
        mov byte ptr leveldata[si], 4
        mov byte ptr [si+1835h], 22h ; '"'
        mov word ptr [si+18ACh], 9999h
        retn
// ; ---------------------------------------------------------------------------

loc_48767:              ; CODE XREF: movefun2+334j
                    ; movefun2+343j
        dec bl
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4876E:              ; CODE XREF: movefun2+BDj
        xor bh, bh
        and bl, 7
        shl bl, 1
        mov di, [si+6155h]
        push    si
        mov si, 12C6h
        mov si, [bx+si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_48787:              ; CODE XREF: movefun2+389j
        movsb
        movsb
        movsb
        movsb
        add si, 76h ; 'v'
        add di, 76h ; 'v'
        loop    loc_48787
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        cmp bl, 62h ; 'b'
        jge short loc_487A5
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_487A5:              ; CODE XREF: movefun2+396j
        cmp word ptr [si+18AEh], 0
        jnz short loc_487D5
        cmp word ptr [si+1836h], 0
        jz  short loc_487BB
        cmp word ptr [si+1836h], 8888h
        jnz short loc_487D5

loc_487BB:              ; CODE XREF: movefun2+3A9j
        mov word ptr leveldata[si], 0FFFFh
        add si, 2
        mov byte ptr leveldata[si], 4
        mov byte ptr [si+1835h], 32h ; '2'
        mov word ptr [si+18ACh], 9999h
        retn
// ; ---------------------------------------------------------------------------

loc_487D5:              ; CODE XREF: movefun2+3A2j
                    ; movefun2+3B1j
        dec bl
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_487DC:              ; CODE XREF: movefun2:loc_484CCj
        cmp word ptr [si+18ACh], 0
        jz  short loc_487EC
        cmp word ptr [si+18ACh], 9999h
        jz  short loc_487EC
        retn
// ; ---------------------------------------------------------------------------

loc_487EC:              ; CODE XREF: movefun2+3D9j
                    ; movefun2+3E1j
        mov word ptr leveldata[si], 0FFFFh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 1004h
        jmp loc_484D0
movefun2  endp


; =============== S U B R O U T I N E =======================================


sub_487FE   proc near       ; CODE XREF: update?+E0Cp update?+E2Ap ...
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4880B
        mov word ptr leveldata[si], 0

loc_4880B:              ; CODE XREF: sub_487FE+5j
        cmp word ptr [si+17BCh], 0
        jz  short loc_48835
        cmp word ptr [si+17BCh], 9999h
        jz  short loc_48835
        cmp word ptr [si+17BCh], 1
        jz  short loc_48829
        cmp word ptr [si+17BCh], 4
        jz  short loc_4882F
        retn
// ; ---------------------------------------------------------------------------

loc_48829:              ; CODE XREF: sub_487FE+21j
        mov byte ptr [si+17BDh], 40h ; '@'
        retn
// ; ---------------------------------------------------------------------------

loc_4882F:              ; CODE XREF: sub_487FE+28j
        mov byte ptr [si+17BDh], 40h ; '@'
        retn
// ; ---------------------------------------------------------------------------

loc_48835:              ; CODE XREF: sub_487FE+12j
                    ; sub_487FE+1Aj
        cmp word ptr [si+17BAh], 1
        jz  short loc_48852
        cmp word ptr [si+17BAh], 4
        jz  short loc_48897

loc_48843:              ; CODE XREF: sub_487FE+69j
                    ; sub_487FE+AEj
        cmp word ptr [si+17BEh], 1
        jz  short loc_48875
        cmp word ptr [si+17BEh], 4
        jz  short loc_488BA
        retn
// ; ---------------------------------------------------------------------------

loc_48852:              ; CODE XREF: sub_487FE+3Cj
        cmp word ptr [si+1832h], 1
        jz  short loc_48869
        cmp word ptr [si+1832h], 4
        jz  short loc_48869
        cmp word ptr [si+1832h], 5
        jz  short loc_48869
        jmp short loc_48843
// ; ---------------------------------------------------------------------------

loc_48869:              ; CODE XREF: sub_487FE+59j
                    ; sub_487FE+60j ...
        mov byte ptr [si+17BBh], 60h ; '`'
        mov word ptr [si+17BCh], 8888h
        retn
// ; ---------------------------------------------------------------------------

loc_48875:              ; CODE XREF: sub_487FE+4Aj
        cmp word ptr [si+1836h], 1
        jz  short loc_4888B
        cmp word ptr [si+1836h], 4
        jz  short loc_4888B
        cmp word ptr [si+1836h], 5
        jz  short loc_4888B
        retn
// ; ---------------------------------------------------------------------------

loc_4888B:              ; CODE XREF: sub_487FE+7Cj
                    ; sub_487FE+83j ...
        mov byte ptr [si+17BFh], 50h ; 'P'
        mov word ptr [si+17BCh], 8888h
        retn
// ; ---------------------------------------------------------------------------

loc_48897:              ; CODE XREF: sub_487FE+43j
        cmp word ptr [si+1832h], 1
        jz  short loc_488AE
        cmp word ptr [si+1832h], 4
        jz  short loc_488AE
        cmp word ptr [si+1832h], 5
        jz  short loc_488AE
        jmp short loc_48843
// ; ---------------------------------------------------------------------------

loc_488AE:              ; CODE XREF: sub_487FE+9Ej
                    ; sub_487FE+A5j ...
        mov byte ptr [si+17BBh], 60h ; '`'
        mov word ptr [si+17BCh], 8888h
        retn
// ; ---------------------------------------------------------------------------

loc_488BA:              ; CODE XREF: sub_487FE+51j
        cmp word ptr [si+1836h], 1
        jz  short loc_488D0
        cmp word ptr [si+1836h], 4
        jz  short loc_488D0
        cmp word ptr [si+1836h], 5
        jz  short loc_488D0
        retn
// ; ---------------------------------------------------------------------------

loc_488D0:              ; CODE XREF: sub_487FE+C1j
                    ; sub_487FE+C8j ...
        mov byte ptr [si+17BFh], 50h ; 'P'
        mov word ptr [si+17BCh], 8888h
        retn
sub_487FE   endp


; =============== S U B R O U T I N E =======================================


sub_488DC   proc near       ; CODE XREF: movefun+124p
                    ; movefun+2C6p ...
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_488E9
        mov word ptr leveldata[si], 0

loc_488E9:              ; CODE XREF: sub_488DC+5j
        cmp word ptr [si+17BCh], 0
        jz  short loc_48901
        cmp word ptr [si+17BCh], 9999h
        jz  short loc_488F9
        retn
// ; ---------------------------------------------------------------------------

loc_488F9:              ; CODE XREF: sub_488DC+1Aj
        cmp byte ptr [si+1744h], 4
        jz  short loc_48901
        retn
// ; ---------------------------------------------------------------------------

loc_48901:              ; CODE XREF: sub_488DC+12j
                    ; sub_488DC+22j
        cmp word ptr [si+17BAh], 1
        jz  short loc_48910

loc_48908:              ; CODE XREF: sub_488DC+49j
        cmp word ptr [si+17BEh], 1
        jz  short loc_48934
        retn
// ; ---------------------------------------------------------------------------

loc_48910:              ; CODE XREF: sub_488DC+2Aj
        cmp word ptr [si+1832h], 1
        jz  short loc_48927
        cmp word ptr [si+1832h], 4
        jz  short loc_48927
        cmp word ptr [si+1832h], 5
        jz  short loc_48927
        jmp short loc_48908
// ; ---------------------------------------------------------------------------

loc_48927:              ; CODE XREF: sub_488DC+39j
                    ; sub_488DC+40j ...
        mov word ptr [si+17BAh], 6001h
        mov word ptr [si+17BCh], 8888h
        retn
// ; ---------------------------------------------------------------------------

loc_48934:              ; CODE XREF: sub_488DC+31j
        cmp word ptr [si+1836h], 1
        jz  short loc_4894A
        cmp word ptr [si+1836h], 4
        jz  short loc_4894A
        cmp word ptr [si+1836h], 5
        jz  short loc_4894A
        retn
// ; ---------------------------------------------------------------------------

loc_4894A:              ; CODE XREF: sub_488DC+5Dj
                    ; sub_488DC+64j ...
        mov word ptr [si+17BEh], 5001h
        mov word ptr [si+17BCh], 8888h
        retn
sub_488DC   endp


; =============== S U B R O U T I N E =======================================


sub_48957   proc near       ; CODE XREF: movefun2+104p
                    ; movefun2+257p ...
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_48964
        mov word ptr leveldata[si], 0

loc_48964:              ; CODE XREF: sub_48957+5j
        cmp word ptr [si+17BCh], 0
        jz  short loc_4897C
        cmp word ptr [si+17BCh], 9999h
        jz  short loc_48974
        retn
// ; ---------------------------------------------------------------------------

loc_48974:              ; CODE XREF: sub_48957+1Aj
        cmp byte ptr [si+1744h], 1
        jz  short loc_4897C
        retn
// ; ---------------------------------------------------------------------------

loc_4897C:              ; CODE XREF: sub_48957+12j
                    ; sub_48957+22j
        cmp word ptr [si+17BAh], 4
        jz  short loc_4898B

loc_48983:              ; CODE XREF: sub_48957+49j
        cmp word ptr [si+17BEh], 4
        jz  short loc_489AF
        retn
// ; ---------------------------------------------------------------------------

loc_4898B:              ; CODE XREF: sub_48957+2Aj
        cmp word ptr [si+1832h], 1
        jz  short loc_489A2
        cmp word ptr [si+1832h], 4
        jz  short loc_489A2
        cmp word ptr [si+1832h], 5
        jz  short loc_489A2
        jmp short loc_48983
// ; ---------------------------------------------------------------------------

loc_489A2:              ; CODE XREF: sub_48957+39j
                    ; sub_48957+40j ...
        mov word ptr [si+17BAh], 6004h
        mov word ptr [si+17BCh], 8888h
        retn
// ; ---------------------------------------------------------------------------

loc_489AF:              ; CODE XREF: sub_48957+31j
        cmp word ptr [si+1836h], 1
        jz  short loc_489C5
        cmp word ptr [si+1836h], 4
        jz  short loc_489C5
        cmp word ptr [si+1836h], 5
        jz  short loc_489C5
        retn
// ; ---------------------------------------------------------------------------

loc_489C5:              ; CODE XREF: sub_48957+5Dj
                    ; sub_48957+64j ...
        mov word ptr [si+17BEh], 5004h
        mov word ptr [si+17BCh], 8888h
        retn
sub_48957   endp

// ; ---------------------------------------------------------------------------
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  8Bh ; ?
        db 0C0h ; +

// ; =============== S U B R O U T I N E =======================================

// ; ax contains a status code if a DOS function fails.
void writeexitmessage() //    proc near       ; CODE XREF: loadScreen2-7CFp
{
        push    es
        mov bx, ax
        mov ax, seg doserrors
        mov es, ax
        assume es:doserrors
        mov di, 0
        al = 0

        ; find message #bx
keeplooking:              ; CODE XREF: writeexitmessage+13j
        mov cx, 1000
        repne scasb             ; look for zero term
        dec bx
        jnz short keeplooking

writechar:              ; CODE XREF: writeexitmessage+23j
        mov ah, 0Eh
        al = es:[di]
        cmp al, 0
        jz  short write_crlf
        inc di
        mov bh, 0
        int 10h     ; - VIDEO - WRITE CHARACTER AND ADVANCE CURSOR (TTY WRITE)
                    ; AL = character, BH = display page (alpha modes)
                    ; BL = foreground color (graphics modes)
        jmp short writechar
// ; ---------------------------------------------------------------------------

write_crlf:              ; CODE XREF: writeexitmessage+1Cj
        mov ah, 0Eh
        al = 0Ah
        mov bh, 0
        int 10h     ; - VIDEO - WRITE CHARACTER AND ADVANCE CURSOR (TTY WRITE)
                    ; AL = character, BH = display page (alpha modes)
                    ; BL = foreground color (graphics modes)
        mov ah, 0Eh
        al = 0Dh
        mov bh, 0
        int 10h     ; - VIDEO - WRITE CHARACTER AND ADVANCE CURSOR (TTY WRITE)
                    ; AL = character, BH = display page (alpha modes)
                    ; BL = foreground color (graphics modes)
        pop es
        assume es:nothing
        retn
}

// ; ---------------------------------------------------------------------------
//         db  2Eh ; .
//         db  8Bh ; ?
//         db 0C0h ; +
//         db  2Eh ; .
//         db  8Bh ; ?
//         db 0C0h ; +
//         db  2Eh ; .
//         db  8Bh ; ?
//         db 0C0h ; +

// ; =============== S U B R O U T I N E =======================================


void sub_48A20() //   proc near       ; CODE XREF: start+32Fp
{
                    ; runLevel:notFunctionKeyp ...
        mov ax, word_510C3
        mov word_510BC, ax
        mov ax, word_510C5
        mov word_510BE, ax
        xor ax, ax
        mov word_510CB, ax
        mov word_510D1, ax
        mov word_51974, ax
        mov word_51978, ax
        mov byte_5195C, 0
        mov byte_5197C, 0
        mov word_510CD, 0
        mov byte ptr word_510B7, 0FFh
        mov byte ptr word_510B7+1, 0FFh
        mov byte_510B9, 0FFh
        mov byte_510AE, 1
        mov byte_510AF, 0
        mov byte_510B0, 0
        mov byte_510B1, 0
        mov byte_510B2, 0
        mov byte_510B4, 0
        mov byte_510B5, 0
        mov byte_510B6, 0
        mov byte_510C0, 0
        mov byte_5196A, 7Fh ; ''
        mov byte_5196B, 0
        mov word_5195D, 0
        mov byte ptr word_510C1, 1
        mov byte ptr word_510C1+1, 0
        mov byte_510D7, 0
        mov byte_510A6, 0
        mov byte ptr word_510D9, 0
        mov byte_510DB, 0
        mov word_510DC, 0
        retn
}


// ; =============== S U B R O U T I N E =======================================


runLevel    proc near       ; CODE XREF: start+35Cp
        cmp byte_510DE, 0
        jz  short loc_48ACE
        mov byte_5A19C, 1
        mov byte_510BA, 0
        jmp short loc_48AD8
// ; ---------------------------------------------------------------------------

loc_48ACE:              ; CODE XREF: runLevel+5j
        mov byte_5A19C, 0
        mov byte_510BA, 1

loc_48AD8:              ; CODE XREF: runLevel+11j
        cmp byte_5A2F8, 1
        jnz short loc_48B09

loc_48ADF:              ; CODE XREF: runLevel+BAj
        mov byte_5A2F8, 0
        call    sub_4FDFD

isFunctionKey:              ; CODE XREF: runLevel+35j
        al = keyPressed
        cmp al, 3Bh ; ';'   ; F1
        jb  short notFunctionKey
        cmp al, 44h ; 'D'   ; F10
        jbe short isFunctionKey

notFunctionKey:             ; CODE XREF: runLevel+31j
        call    sub_48A20
        cmp byte_59886, 0
        jnz short loc_48AFF
        call    sound?3

loc_48AFF:              ; CODE XREF: runLevel+3Fj
        mov byte_5A19C, 0
        mov byte_510BA, 1

loc_48B09:              ; CODE XREF: runLevel+22j
        mov byte_510DB, 0
        mov byte_5A323, 0
        mov word_510A2, 1

gamelooprep:                ; CODE XREF: runLevel+33Cj
                    ; runLevel+345j
        cmp byte_510DE, 0
        jnz short loc_48B23
        call    sub_48E59

loc_48B23:              ; CODE XREF: runLevel+63j
        call    getMouseStatus
        cmp bx, 2
        jnz short loc_48B38
        cmp word_51978, 0
        jg  short loc_48B38
        mov word_510D1, 1

loc_48B38:              ; CODE XREF: runLevel+6Ej runLevel+75j
        cmp word_51970, 0
        jz  short loc_48B6B
        cmp byte_59B7A, 0
        jz  short loc_48B4A
        dec byte_59B7A

loc_48B4A:              ; CODE XREF: runLevel+89j
        cmp byte_51999, 0
        jnz short loc_48B6B
        cmp bx, 1
        jnz short loc_48B6B
        cmp byte_59B7A, 0
        jnz short loc_48B6B
        mov byte_59B7A, 0Ah
        call    levelScanThing
        call    sub_48F6D
        call    sub_4A2E6

loc_48B6B:              ; CODE XREF: runLevel+82j runLevel+94j ...
        call    sub_4955B
        cmp byte_5A2F8, 1
        jnz short loc_48B78
        jmp loc_48ADF
// ; ---------------------------------------------------------------------------

loc_48B78:              ; CODE XREF: runLevel+B8j
        cmp word ptr flashingbackgroundon, 0
        jz  short noFlashing
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        al = 35h ; '5'
        out dx, al
        out dx, al
        out dx, al

noFlashing:              ; CODE XREF: runLevel+C2j
        call    gameloop?
        cmp word ptr flashingbackgroundon, 0
        jz  short noFlashing2
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        al = 21h ; '!'
        out dx, al
        out dx, al
        out dx, al

noFlashing2:              ; CODE XREF: runLevel+D8j
        call    sub_4FDFD
        call    sub_4FD65
        cmp word ptr flashingbackgroundon, 0
        jz  short noFlashing3
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        al = 2Dh ; '-'
        out dx, al
        al = 21h ; '!'
        out dx, al
        al = 0Fh
        out dx, al

noFlashing3:              ; CODE XREF: runLevel+F1j
        call    sub_4A910
        call    sub_4A5E0
        call    sub_49EBE
        mov ax, word_5195F
        and al, 7
        mov byte_510A6, al
        test    speed?3, 40h
        jnz short loc_48BED
        test    speed?3, 80h
        jz  short loc_48BED
        al = gameSpeed
        xchg    al, speed?3
        cmp al, 0BFh ; '?'
        jz  short loc_48BED
        and al, 0Fh
        mov gameSpeed, al

loc_48BED:              ; CODE XREF: runLevel+119j
                    ; runLevel+120j ...
        cmp speed?, 0
        jnz short loc_48BF7
        jmp loc_48C86
// ; ---------------------------------------------------------------------------

loc_48BF7:              ; CODE XREF: runLevel+137j
        inc byte_59B94
        cmp byte_59B96, 0Ah
        jnb short loc_48C05
        jmp loc_48C86
// ; ---------------------------------------------------------------------------

loc_48C05:              ; CODE XREF: runLevel+145j
        mov byte_59B96, 0
        cmp byte_59B94, 8
        ja  short loc_48C1A
        cmp byte_59B94, 6
        jb  short loc_48C1A
        cmp al, al

loc_48C1A:              ; CODE XREF: runLevel+154j
                    ; runLevel+15Bj
        mov byte_59B94, 0
        ja  short loc_48C79
        jb  short loc_48C5F
        dec speed?
        cmp speed?, 0FEh ; '?'
        ja  short loc_48C86
        and speed?3, 0BFh
        mov speed?, 0
        cmp gameSpeed, 4
        jz  short loc_48C4F
        cmp gameSpeed, 6
        jz  short loc_48C6C
        cmp gameSpeed, 9
        jz  short loc_48C4F
        jmp short loc_48C86
// ; ---------------------------------------------------------------------------

loc_48C4F:              ; CODE XREF: runLevel+182j
                    ; runLevel+190j
        cmp gameSpeed, 0Ah
        jb  short loc_48C59
        jmp loc_48D59
// ; ---------------------------------------------------------------------------

loc_48C59:              ; CODE XREF: runLevel+199j
                    ; runLevel+1A9j
        inc gameSpeed
        jmp short loc_48C86
// ; ---------------------------------------------------------------------------

loc_48C5F:              ; CODE XREF: runLevel+166j
        cmp gameSpeed, 0Ah
        jb  short loc_48C59
        dec speed?
        jmp short loc_48C86
// ; ---------------------------------------------------------------------------

loc_48C6C:              ; CODE XREF: runLevel+189j
        cmp gameSpeed, 0
        jz  short loc_48C86

loc_48C73:              ; CODE XREF: runLevel+1C3j
        dec gameSpeed
        jmp short loc_48C86
// ; ---------------------------------------------------------------------------

loc_48C79:              ; CODE XREF: runLevel+164j
        cmp gameSpeed, 0
        ja  short loc_48C73
        dec speed?
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_48C86:              ; CODE XREF: runLevel+139j
                    ; runLevel+147j ...
        cmp fastMode, 1
        jnz short isNotFastMode4
        jmp loc_48D59
// ; ---------------------------------------------------------------------------

isNotFastMode4:              ; CODE XREF: runLevel+1D0j
        cmp gameSpeed, 0Ah
        jb  short loc_48C9A
        jmp loc_48D59
// ; ---------------------------------------------------------------------------

loc_48C9A:              ; CODE XREF: runLevel+1DAj
        mov cx, 6
        sub cl, gameSpeed
        jg  short loc_48CBD
        al = gameSpeed
        inc byte_59B95
        sub al, 5
        cmp al, byte_59B95
        jb  short loc_48CB5
        jmp loc_48D59
// ; ---------------------------------------------------------------------------

loc_48CB5:              ; CODE XREF: runLevel+1F5j
        mov byte_59B95, 0
        mov cx, 1

loc_48CBD:              ; CODE XREF: runLevel+1E6j
        push    bx

loc_48CBE:              ; CODE XREF: runLevel+29Aj
        mov dx, word_5195F
        sub dx, word_59B90
        cmp dx, 10h
        jg  short loc_48D38
        cmp dx, 0FFF0h
        jl  short loc_48D38
        mov ax, word_51961
        sub ax, word_59B92
        cmp ax, 10h
        jg  short loc_48D38
        cmp ax, 0FFF0h
        jl  short loc_48D38
        push(cx);
        inc cx
        idiv    cl
        shl ah, 1
        jnb short loc_48CF3
        neg ah
        cmp ah, cl
        jb  short loc_48CF9
        dec al
        jmp short loc_48CF9
// ; ---------------------------------------------------------------------------

loc_48CF3:              ; CODE XREF: runLevel+22Cj
        cmp ah, cl
        jb  short loc_48CF9
        al++;

loc_48CF9:              ; CODE XREF: runLevel+232j
                    ; runLevel+236j ...
        cbw
        add word_59B92, ax
        mov ax, dx
        idiv    cl
        shl ah, 1
        jnb short loc_48D10
        neg ah
        cmp ah, cl
        jb  short loc_48D16
        dec al
        jmp short loc_48D16
// ; ---------------------------------------------------------------------------

loc_48D10:              ; CODE XREF: runLevel+249j
        cmp ah, cl
        jb  short loc_48D16
        al++;

loc_48D16:              ; CODE XREF: runLevel+24Fj
                    ; runLevel+253j ...
        cbw
        add word_59B90, ax
        mov ax, word_59B90
        mov bx, ax
        and al, 7
        mov byte_510A6, al
        mov cl, 3
        shr bx, cl
        mov ax, word_59B92
        mov cx, 7Ah ; 'z'
        mul cx
        add bx, ax
        add bx, 4D34h
        pop(cx);

loc_48D38:              ; CODE XREF: runLevel+20Ej
                    ; runLevel+213j ...
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        call    videoloop
        call    sub_4D457
        dec cx
        jz  short loc_48D58
        jmp loc_48CBE
// ; ---------------------------------------------------------------------------

loc_48D58:              ; CODE XREF: runLevel+298j
        pop bx

loc_48D59:              ; CODE XREF: runLevel+19Bj
                    ; runLevel+1D2j ...
        mov ax, word_51961
        mov word_59B92, ax
        mov ax, word_5195F
        mov word_59B90, ax
        and al, 7
        mov byte_510A6, al
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        mov cx, word_5195F
        mov ah, cl
        and ah, 7
        cmp word ptr flashingbackgroundon, 0
        jz  short noFlashing4
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        al = 3Fh ; '?'
        out dx, al
        out dx, al
        out dx, al

noFlashing4:              ; CODE XREF: runLevel+2D1j
        mov byte_510A6, ah
        cmp fastMode, 1
        jz  short isFastMode2
        call    videoloop

isFastMode2:              ; CODE XREF: runLevel+2E8j
        cmp word_51A07, 1
        jbe short loc_48DB2
        call    sound?9

loc_48DB2:              ; CODE XREF: runLevel+2F2j
        mov cx, word_51A07

loc_48DB6:              ; CODE XREF: runLevel+310j
        dec cx
        jz  short loc_48DCD
        cmp fastMode, 1
        jz  short isFastMode3
        call    sub_4D457
        call    videoloop

isFastMode3:              ; CODE XREF: runLevel+303j
        push(cx);
        call    sub_4955B
        pop(cx);
        jmp short loc_48DB6
// ; ---------------------------------------------------------------------------

loc_48DCD:              ; CODE XREF: runLevel+2FCj
        cmp word ptr flashingbackgroundon, 0
        jz  short noFlashing5
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        out dx, al
        out dx, al
        out dx, al

noFlashing5:              ; CODE XREF: runLevel+317j
        cmp word_5197A, 0
        jnz short loc_48E03
        inc word_5195D
        cmp word_51974, 1
        jz  short loc_48E03
        cmp word_51978, 0
        jnz short loc_48DFA
        jmp gamelooprep
// ; ---------------------------------------------------------------------------

loc_48DFA:              ; CODE XREF: runLevel+33Aj
        dec word_51978
        jz  short loc_48E03
        jmp gamelooprep
// ; ---------------------------------------------------------------------------

loc_48E03:              ; CODE XREF: runLevel+328j
                    ; runLevel+333j ...
        mov word_510A2, 0
        cmp byte_510E3, 0
        jz  short loc_48E13
        call    somethingspsig

loc_48E13:              ; CODE XREF: runLevel+353j
        cmp byte_5A2F9, 0
        mov byte_5A2F9, 0
        jnz short loc_48E30
        cmp byte_510B3, 0
        jz  short loc_48E30
        cmp byte_5A323, 0
        jnz short loc_48E30
        call    sub_4A95F

loc_48E30:              ; CODE XREF: runLevel+362j
                    ; runLevel+369j ...
        mov word_51A01, 0
        mov word_51963, 0
        mov word_51965, 0
        mov word ptr flashingbackgroundon, 0
        mov word_51A07, 1
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        out dx, al
        out dx, al
        out dx, al
        retn
runLevel    endp


; =============== S U B R O U T I N E =======================================


sub_48E59   proc near       ; CODE XREF: sub_47E98:loc_47EB8p
                    ; sub_47E98+4Dp ...
        xor ax, ax
        xor bl, bl
        cmp byte_50940, 0
        jnz short loc_48E67
        jmp loc_48F68
// ; ---------------------------------------------------------------------------

loc_48E67:              ; CODE XREF: sub_48E59+9j
        mov word_50942, ax
        mov word_50944, ax
        mov word_5094F, ax
        mov word_50951, ax
        mov byte_50953, al
        mov byte_50954, al
        al = byte_50946
        test    al, al
        jnz short loc_48E83
        jmp loc_48F68
// ; ---------------------------------------------------------------------------

loc_48E83:              ; CODE XREF: sub_48E59+25j
        mov ah, 1
        call    sub_49FED
        jnb short loc_48E8D
        jmp loc_48F68
// ; ---------------------------------------------------------------------------

loc_48E8D:              ; CODE XREF: sub_48E59+2Fj
        mov word_50942, ax
        mov dx, word_5094B
        mul dx
        shl ax, 1
        adc dx, 0
        mov ax, dx
        cmp ax, 10h
        jbe short loc_48EA5
        mov ax, 10h

loc_48EA5:              ; CODE XREF: sub_48E59+47j
        cmp byte_519F7, 0
        jz  short loc_48EB1
        mov ax, 1
        jmp short loc_48EBB
// ; ---------------------------------------------------------------------------

loc_48EB1:              ; CODE XREF: sub_48E59+51j
        cmp byte_519F6, 0
        jz  short loc_48EBB
        mov ax, 10h

loc_48EBB:              ; CODE XREF: sub_48E59+56j
                    ; sub_48E59+5Dj
        mov word_5094F, ax
        mov ah, 2
        call    sub_49FED
        jnb short loc_48EC8
        jmp loc_48F68
// ; ---------------------------------------------------------------------------

loc_48EC8:              ; CODE XREF: sub_48E59+6Aj
        mov word_50944, ax
        mov dx, word_5094D
        mul dx
        shl ax, 1
        adc dx, 0
        mov ax, dx
        cmp ax, 10h
        jbe short loc_48EE0
        mov ax, 10h

loc_48EE0:              ; CODE XREF: sub_48E59+82j
        cmp byte_519F9, 0
        jz  short loc_48EEC
        mov ax, 1
        jmp short loc_48EF6
// ; ---------------------------------------------------------------------------

loc_48EEC:              ; CODE XREF: sub_48E59+8Cj
        cmp byte_519F8, 0
        jz  short loc_48EF6
        mov ax, 10h

loc_48EF6:              ; CODE XREF: sub_48E59+91j
                    ; sub_48E59+98j
        mov word_50951, ax
        mov ax, word_50951
        mov cx, 11h
        mul cx
        mov si, word_5094F
        add si, ax
        add si, 645h
        mov bl, [si]
        cmp bl, 0
        jz  short loc_48F40
        mov dx, 201h
        in  al, dx      ; Game I/O port
                    ; bits 0-3: Coordinates (resistive, time-dependent inputs)
                    ; bits 4-7: Buttons/Triggers (digital inputs)
        test    al, 10h
        jnz short loc_48F1F
        add bl, 4
        jmp short loc_48F68
// ; ---------------------------------------------------------------------------

loc_48F1F:              ; CODE XREF: sub_48E59+BFj
        test    al, 20h
        jnz short loc_48F28
        add bl, 4
        jmp short loc_48F68
// ; ---------------------------------------------------------------------------

loc_48F28:              ; CODE XREF: sub_48E59+C8j
        cmp byte_519F5, 0
        jz  short loc_48F34
        add bl, 4
        jmp short loc_48F3E
// ; ---------------------------------------------------------------------------

loc_48F34:              ; CODE XREF: sub_48E59+D4j
        cmp byte_519F4, 0
        jz  short loc_48F3E
        add bl, 4

loc_48F3E:              ; CODE XREF: sub_48E59+D9j
                    ; sub_48E59+E0j
        jmp short loc_48F68
// ; ---------------------------------------------------------------------------

loc_48F40:              ; CODE XREF: sub_48E59+B7j
        mov dx, 201h
        in  al, dx      ; Game I/O port
                    ; bits 0-3: Coordinates (resistive, time-dependent inputs)
                    ; bits 4-7: Buttons/Triggers (digital inputs)
        test    al, 10h
        jnz short loc_48F4C
        mov bl, 9
        jmp short loc_48F68
// ; ---------------------------------------------------------------------------

loc_48F4C:              ; CODE XREF: sub_48E59+EDj
        test    al, 20h
        jnz short loc_48F54
        mov bl, 9
        jmp short loc_48F68
// ; ---------------------------------------------------------------------------

loc_48F54:              ; CODE XREF: sub_48E59+F5j
        cmp byte_519F5, 0
        jz  short loc_48F5F
        mov bl, 9
        jmp short loc_48F68
// ; ---------------------------------------------------------------------------

loc_48F5F:              ; CODE XREF: sub_48E59+100j
        cmp byte_519F4, 0
        jz  short loc_48F68
        mov bl, 9

loc_48F68:              ; CODE XREF: sub_48E59+Bj
                    ; sub_48E59+27j ...
        mov byte_50941, bl
        retn
sub_48E59   endp


; =============== S U B R O U T I N E =======================================


sub_48F6D   proc near       ; CODE XREF: start+335p runLevel+AAp ...
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        push    ds
        mov si, word_5184A
        mov ax, es
        mov ds, ax
        mov di, 4D35h
        mov cx, 8

loc_48F86:              ; CODE XREF: sub_48F6D+20j
        movsb
        add si, 79h ; 'y'
        add di, 79h ; 'y'
        loop    loc_48F86
        sub di, 3CFh
        pop ds
        mov si, word_51856
        push    ds
        mov ax, es
        mov ds, ax
        mov dx, 7

loc_48FA0:              ; CODE XREF: sub_48F6D+41j
        mov cx, 74h ; 't'

loc_48FA3:              ; CODE XREF: sub_48F6D+38j
        movsb
        dec si
        loop    loc_48FA3
        add si, 7Ah ; 'z'
        add di, 6
        dec dx
        jnz short loc_48FA0
        mov cx, 74h ; 't'

loc_48FB3:              ; CODE XREF: sub_48F6D+48j
        movsb
        dec si
        loop    loc_48FB3
        sub di, 356h
        pop ds
        mov si, word_5184C
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 8

loc_48FC8:              ; CODE XREF: sub_48F6D+62j
        movsb
        add si, 79h ; 'y'
        add di, 79h ; 'y'
        loop    loc_48FC8
        pop ds
        mov si, word_51854
        push    ds
        mov ax, es
        mov ds, ax
        mov dx, 2Ch ; ','

loc_48FDE:              ; CODE XREF: sub_48F6D+82j
        mov cx, 8

loc_48FE1:              ; CODE XREF: sub_48F6D+7Bj
        movsb
        add si, 79h ; 'y'
        add di, 79h ; 'y'
        loop    loc_48FE1
        sub si, 3D0h
        dec dx
        jnz short loc_48FDE
        pop ds
        mov si, word_51850
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 8

loc_48FFE:              ; CODE XREF: sub_48F6D+98j
        movsb
        add si, 79h ; 'y'
        add di, 79h ; 'y'
        loop    loc_48FFE
        sub si, 3CEh
        pop ds
        mov si, word_51858
        sub di, 444h
        push    ds
        mov ax, es
        mov ds, ax
        mov dx, 8

loc_4901C:              ; CODE XREF: sub_48F6D+BDj
        mov cx, 74h ; 't'

loc_4901F:              ; CODE XREF: sub_48F6D+B4j
        movsb
        dec si
        loop    loc_4901F
        add si, 7Ah ; 'z'
        add di, 6
        dec dx
        jnz short loc_4901C
        mov cx, 74h ; 't'

loc_4902F:              ; CODE XREF: sub_48F6D+C4j
        movsb
        dec si
        loop    loc_4902F
        sub di, 356h
        pop ds
        mov si, word_51852
        mov di, 5105h
        push    ds
        mov ax, es
        mov ds, ax
        mov dx, 2Ch ; ','

loc_49047:              ; CODE XREF: sub_48F6D+EBj
        mov cx, 8

loc_4904A:              ; CODE XREF: sub_48F6D+E4j
        movsb
        add si, 79h ; 'y'
        add di, 79h ; 'y'
        loop    loc_4904A
        sub si, 3D0h
        dec dx
        jnz short loc_49047
        pop ds
        mov si, word_5184E
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 8

loc_49067:              ; CODE XREF: sub_48F6D+101j
        movsb
        add si, 79h ; 'y'
        add di, 79h ; 'y'
        loop    loc_49067
        pop ds
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        mov bx, offset leveldata
        mov di, 4D34h
        mov cx, 16h
        add bx, 76h ; 'v'
        add di, 3CEh

loc_4909F:              ; CODE XREF: sub_48F6D+18Ej
        push(cx);
        mov cx, 3Ah ; ':'
        add bx, 4
        add di, 4

loc_490A9:              ; CODE XREF: sub_48F6D+184j
        al = [bx]
        cmp al, 28h ; '('
        jbe short loc_490B1
        xor al, al

loc_490B1:              ; CODE XREF: sub_48F6D+140j
        add bx, 2
        xor ah, ah
        shl ax, 1
        mov si, ax
        add si, 3815h
        push(cx);
        mov cx, 10h

loc_490C2:              ; CODE XREF: sub_48F6D+17Aj
        mov ah, 1

loc_490C4:              ; CODE XREF: sub_48F6D+175j
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = ah
        out dx, al      ; EGA port: sequencer data register
        al = [si]
        mov es:[di], al
        inc si
        al = [si]
        mov es:[di+1], al
        shl ah, 1
        add si, 4Fh ; 'O'
        test    ah, 0Fh
        jnz short loc_490C4
        add di, 7Ah ; 'z'
        loop    loc_490C2
        pop(cx);
        sub di, 79Eh
        dec cx
        jz  short loc_490F3
        jmp short loc_490A9
// ; ---------------------------------------------------------------------------

loc_490F3:              ; CODE XREF: sub_48F6D+182j
        pop(cx);
        add di, 728h
        dec cx
        jz  short loc_490FD
        jmp short loc_4909F
// ; ---------------------------------------------------------------------------

loc_490FD:              ; CODE XREF: sub_48F6D+18Cj
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: sequencer data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov bx, word_5195F
        mov cl, 3
        shr bx, cl
        mov ax, word_51961
        mov cx, 7Ah ; 'z'
        mul cx
        add bx, ax
        add bx, 4D34h
        mov word_51967, bx
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        retn
sub_48F6D   endp


; =============== S U B R O U T I N E =======================================


sub_4914A   proc near       ; CODE XREF: sub_4955B+7p
        cmp byte_519C8, 0
        jz  short loc_49159
        dec word_51963
        dec word_51963

loc_49159:              ; CODE XREF: sub_4914A+5j
        cmp byte_519CA, 0
        jz  short loc_49168
        inc word_51963
        inc word_51963

loc_49168:              ; CODE XREF: sub_4914A+14j
        cmp byte_519C5, 0
        jz  short loc_49177
        dec word_51965
        dec word_51965

loc_49177:              ; CODE XREF: sub_4914A+23j
        cmp byte_519CD, 0
        jz  short loc_49186
        inc word_51965
        inc word_51965

loc_49186:              ; CODE XREF: sub_4914A+32j
        cmp byte_519C9, 0
        jz  short loc_49199
        mov word_51963, 0
        mov word_51965, 0

loc_49199:              ; CODE XREF: sub_4914A+41j
        mov bx, 8
        sub bx, word_59B88
        mov ax, word_59B8A
        neg ax
        cmp byte_519CF, 0
        jz  short loc_491B3
        mov word_51963, bx
        mov word_51965, ax

loc_491B3:              ; CODE XREF: sub_4914A+60j
        add bx, 138h
        cmp byte_519C4, 0
        jz  short loc_491C5
        mov word_51963, bx
        mov word_51965, ax

loc_491C5:              ; CODE XREF: sub_4914A+72j
        add bx, 138h
        cmp byte_519C6, 0
        jz  short loc_491D7
        mov word_51963, bx
        mov word_51965, ax

loc_491D7:              ; CODE XREF: sub_4914A+84j
        sub bx, 270h
        add ax, 0A8h ; '?'
        cmp byte ptr word_510C1, 0
        jz  short loc_491E8
        add ax, 18h

loc_491E8:              ; CODE XREF: sub_4914A+99j
        cmp byte_519D0, 0
        jz  short loc_491F6
        mov word_51963, bx
        mov word_51965, ax

loc_491F6:              ; CODE XREF: sub_4914A+A3j
        add bx, 138h
        cmp byte_519CC, 0
        jz  short loc_49208
        mov word_51963, bx
        mov word_51965, ax

loc_49208:              ; CODE XREF: sub_4914A+B5j
        add bx, 138h
        cmp byte_519CE, 0
        jz  short locret_4921A
        mov word_51963, bx
        mov word_51965, ax

locret_4921A:               ; CODE XREF: sub_4914A+C7j
        retn
sub_4914A   endp


; =============== S U B R O U T I N E =======================================


sub_4921B   proc near       ; CODE XREF: readConfig+8Cp
                    ; sub_4955B+31p ...
        push    bp
        xor ax, ax
        mov byte_50946, al
        mov word_50947, ax
        mov word_50949, ax
        mov word_5094B, ax
        mov word_5094D, ax
        xor bp, bp
        mov ah, 1
        call    sub_49FED
        jb  short loc_492A6
        add bp, ax
        mov ah, 1
        call    sub_49FED
        jb  short loc_492A6
        add bp, ax
        mov ah, 1
        call    sub_49FED
        jb  short loc_492A6
        add bp, ax
        mov ah, 1
        call    sub_49FED
        jb  short loc_492A6
        add bp, ax
        shr bp, 1
        mov word_50947, bp
        mov dx, 10h
        xor ax, ax
        cmp bp, dx
        jbe short loc_492A6
        div bp
        mov word_5094B, ax
        xor bp, bp
        mov ah, 2
        call    sub_49FED
        jb  short loc_492A6
        add bp, ax
        mov ah, 2
        call    sub_49FED
        jb  short loc_492A6
        add bp, ax
        mov ah, 2
        call    sub_49FED
        jb  short loc_492A6
        add bp, ax
        mov ah, 2
        call    sub_49FED
        jb  short loc_492A6
        add bp, ax
        shr bp, 1
        mov word_50949, bp
        mov dx, 10h
        xor ax, ax
        cmp bp, dx
        jbe short loc_492A6
        div bp
        mov word_5094D, ax
        al = 1
        mov byte_50946, al

loc_492A6:              ; CODE XREF: sub_4921B+19j
                    ; sub_4921B+22j ...
        pop bp
        retn
sub_4921B   endp


; =============== S U B R O U T I N E =======================================


sub_492A8   proc near       ; CODE XREF: sub_4955B+27p
                    ; sub_4A3E9+76p
        al = byte_510E2
        dec al
        jz  short loc_492B3
        mov byte_510E2, al
        retn
// ; ---------------------------------------------------------------------------

loc_492B3:              ; CODE XREF: sub_492A8+5j
        push    es
        mov ax, seg demoseg
        mov es, ax
        assume es:demoseg
        mov bx, word_510DF
        al = es:[bx]
        inc bx
        pop es
        assume es:nothing
        cmp al, 0FFh
        jz  short loc_492E3
        mov word_510DF, bx

loc_492CA:              ; CODE XREF: sub_492A8+47j
        mov ah, al
        and ah, 0Fh
        mov byte_50941, ah
        and al, 0F0h
        shr al, 1
        shr al, 1
        shr al, 1
        shr al, 1
        al++;
        mov byte_510E2, al
        retn
// ; ---------------------------------------------------------------------------

loc_492E3:              ; CODE XREF: sub_492A8+1Cj
        mov word_51978, 64h ; 'd'
        mov word_51974, 1
        jmp short loc_492CA
sub_492A8   endp


; =============== S U B R O U T I N E =======================================


sub_492F1   proc near       ; CODE XREF: sub_4955B+1Dp
        inc byte_510E2
        mov bl, byte_50941
        cmp byte_510E2, 0FFh
        jnz short loc_49311
        mov byte_510E1, bl
        mov ax, timeOfDay
        mov word_5A199, ax
        mov byte_59B5F, ah
        mov byte_59B5C, al

loc_49311:              ; CODE XREF: sub_492F1+Dj
        cmp byte_510E1, bl
        jnz short loc_4931E
        cmp byte_510E2, 0Fh
        jnz short locret_49359

loc_4931E:              ; CODE XREF: sub_492F1+24j
        al = byte_510E1
        mov ah, byte_510E2
        shl ah, 1
        shl ah, 1
        shl ah, 1
        shl ah, 1
        or  al, ah
        mov byte_510E1, al
        al = byte_510E2
        add byte_59B5C, al
        inc byte_59B5C
        mov ax, 4000h
        mov bx, word_510E4
        mov cx, 1
        mov dx, 0DD1h
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        mov byte_510E2, 0FFh
        mov bl, byte_50941
        mov byte_510E1, bl

locret_49359:               ; CODE XREF: sub_492F1+2Bj
        retn
sub_492F1   endp


; =============== S U B R O U T I N E =======================================


somethingspsig  proc near       ; CODE XREF: runLevel+355p
                    ; sub_4945D+30p ...
        al = speed?2
        xor al, byte_59B5F
        mov byte_59B5D, al
        xor al, byte_59B5C
        mov byte_59B5E, al
        mov ah, 42h ; 'B'
        mov bx, word_510E4
        xor cx, cx
        xor al, al
        mov dx, 5FCh
        int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
                    ; AL = method: offset from beginning of file
        mov ax, 4000h
        mov bx, word_510E4
        mov cx, 2
        mov dx, 984Dh
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        mov ax, 4000h
        mov bx, word_510E4
        mov cx, 2
        mov dx, 9E89h
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        mov ah, 42h ; 'B'
        mov bx, word_510E4
        xor cx, cx
        xor dx, dx
        al = 2
        int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
                    ; AL = method: offset from end of file
        mov byte_510E1, 0FFh
        mov ax, 4000h
        mov bx, word_510E4
        mov cx, 1
        mov dx, 0DD1h
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        cmp byte_5A19B, 0
        jz  short loc_49435
        mov ax, 3D00h
        mov dx, offset aMyspsig_txt ; "MYSPSIG.TXT"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jb  short loc_49435
        mov bx, ax
        push    bx
        mov ax, 4202h
        xor cx, cx
        mov dx, cx
        int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
                    ; AL = method: offset from end of file
        pop bx
        jb  short loc_49430
        mov cx, 1FFh
        or  dx, dx
        jnz short loc_493EB
        or  ax, ax
        jz  short loc_49430
        cmp ax, cx
        ja  short loc_493EB
        mov cx, ax

loc_493EB:              ; CODE XREF: somethingspsig+85j
                    ; somethingspsig+8Dj
        push    bx
        push(cx);
        mov ax, 4200h
        xor cx, cx
        mov dx, cx
        int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
                    ; AL = method: offset from beginning of file
        pop(cx);
        pop bx
        jb  short loc_49430
        push    bx
        push(cx);
        mov ax, 3F00h
        mov dx, offset fileLevelData
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        pop(cx);
        pop bx
        jb  short loc_49430
        push    bx
        push    es
        push    ds
        pop es
        assume es:data
        push(di);
        push(cx);
        mov di, offset fileLevelData
        al = 0FFh
        cld
        repne scasb
        jz  short loc_4941C
        dec cx
        mov byte ptr [di], 0FFh

loc_4941C:              ; CODE XREF: somethingspsig+BCj
        pop ax
        pop(di);
        pop es
        assume es:nothing
        sub cx, ax
        neg cx
        mov ax, 4000h
        mov bx, word_510E4
        mov dx, offset fileLevelData
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        pop bx

loc_49430:              ; CODE XREF: somethingspsig+7Ej
                    ; somethingspsig+89j ...
        mov ax, 3E00h
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle

loc_49435:              ; CODE XREF: somethingspsig+65j
                    ; somethingspsig+6Fj
        mov ax, 3E00h
        mov bx, word_510E4
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        mov byte_510E3, 0
        cmp byte_5A33E, 0
        jz  short loc_4944F
        mov byte_510DE, 1

loc_4944F:              ; CODE XREF: somethingspsig+EEj
        call    sub_4FC20
        mov byte_5A33F, 1
        mov byte_510DE, 0
        retn
somethingspsig  endp


; =============== S U B R O U T I N E =======================================


sub_4945D   proc near       ; CODE XREF: sub_4955B+294p
                    ; sub_4955B+2A4p ...
        mov word_51A01, 0
        mov word_51963, 0
        mov word_51965, 0
        mov word ptr flashingbackgroundon, 0
        mov word_51A07, 1
        push    ax
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        out dx, al
        out dx, al
        out dx, al
        cmp byte_510E3, 0
        jz  short loc_49490
        call    somethingspsig

loc_49490:              ; CODE XREF: sub_4945D+2Ej
        pop ax
        mov bx, 380Ah
        add al, 30h ; '0'
        mov [bx+4], al
        cmp byte ptr word_59B6E, 0
        jnz short loc_494A6
        mov bx, 0A014h
        mov [bx+7], al

loc_494A6:              ; CODE XREF: sub_4945D+41j
        mov byte ptr aRecordingDemo0+12h, al ; "0 ---"
        mov cx, 0
        mov dx, bx
        mov ax, 3C00h
        int 21h     ; DOS - 2+ - CREATE A FILE WITH HANDLE (CREAT)
                    ; CX = attributes for file
                    ; DS:DX -> ASCIZ filename (may include drive and path)
        jnb short loc_494B8
        jmp locret_49543
// ; ---------------------------------------------------------------------------

loc_494B8:              ; CODE XREF: sub_4945D+56j
        mov word_510E4, ax
        mov byte_5A140, 83h ; '?'
        mov bl, speed?3
        mov cl, 4
        shl bl, cl
        or  bl, gameSpeed
        mov speed?2, bl
        mov bx, word_510E4
        mov ax, 4000h
        mov cx, levelDataLength
        mov dx, offset levelBuffer
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        jb  short locret_49543
        mov ax, word_51ABC
        or  al, 80h
        mov byte_510E2, al
        mov ax, 4000h
        mov bx, word_510E4
        mov cx, 1
        mov dx, 0DD2h
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        jb  short locret_49543
        mov byte_510E1, 0
        mov byte_5A2F8, 1
        mov byte_510DE, 0
        mov byte_510E2, 0FEh ; '?'
        mov word_51A07, 1
        cmp byte_599D4, 0
        jnz short loc_4952A
        mov ax, word_58AB8
        mov word_58AEA, ax
        mov ax, word_58ABA
        and ax, 0FFh
        mov word_58AEC, ax

loc_4952A:              ; CODE XREF: sub_4945D+BCj
        mov byte_510E3, 1
        cmp byte_5A33E, 0
        jz  short loc_4953B
        mov byte_510DE, 1

loc_4953B:              ; CODE XREF: sub_4945D+D7j
        call    sub_4A463
        mov byte_510DE, 0

locret_49543:               ; CODE XREF: sub_4945D+58j
                    ; sub_4945D+82j ...
        retn
sub_4945D   endp


; =============== S U B R O U T I N E =======================================


sub_49544   proc near       ; CODE XREF: start+3A1p
                    ; sub_4B375:loc_4B40Fp ...
        mov ax, word ptr aLevels_dat_0+8 ; "AT"
        cmp ax, 3030h
        jnz short loc_4954F
        mov ax, 2D2Dh

loc_4954F:              ; CODE XREF: sub_49544+6j
        cmp ax, 5441h
        jnz short loc_49557
        mov ax, 3030h

loc_49557:              ; CODE XREF: sub_49544+Ej
        mov word ptr a00s0010_sp, ax ; "00S001$0.SP"
        retn
sub_49544   endp


; =============== S U B R O U T I N E =======================================


sub_4955B   proc near       ; CODE XREF: runLevel:loc_48B6Bp
                    ; runLevel+30Cp

; FUNCTION CHUNK AT 2DA8 SIZE 0000038B BYTES

        cmp word_51A01, 0
        jz  short loc_49567
        call    sub_4914A
        jmp short loc_4957B
// ; ---------------------------------------------------------------------------

loc_49567:              ; CODE XREF: sub_4955B+5j
        cmp byte_510DE, 0
        jnz short loc_4957B
        call    sub_4A1BF
        cmp byte_510E3, 0
        jz  short loc_4957B
        call    sub_492F1

loc_4957B:              ; CODE XREF: sub_4955B+Aj
                    ; sub_4955B+11j ...
        cmp byte_510DE, 0
        jz  short loc_49585
        call    sub_492A8

loc_49585:              ; CODE XREF: sub_4955B+25j
        cmp byte_519A1, 0
        jz  short loc_4958F
        call    sub_4921B

loc_4958F:              ; CODE XREF: sub_4955B+2Fj
        cmp byte ptr word_510C1+1, 0
        jz  short loc_4959A
        dec byte ptr word_510C1+1

loc_4959A:              ; CODE XREF: sub_4955B+39j
        cmp byte_51999, 0
        jnz short loc_495A9
        mov byte ptr word_510C1+1, 0
        jmp loc_49635
// ; ---------------------------------------------------------------------------

loc_495A9:              ; CODE XREF: sub_4955B+44j
        cmp byte ptr word_510C1+1, 0
        jz  short loc_495B3
        jmp loc_49635
// ; ---------------------------------------------------------------------------

loc_495B3:              ; CODE XREF: sub_4955B+53j
        mov byte ptr word_510C1+1, 20h ; ' '
        cmp byte ptr word_510C1, 0
        jz  short loc_495FB
        mov byte ptr word_510C1, 0
        cmp videoStatusUnk, 1
        jnz short loc_495ED
        mov cl, 90h ; '?'
        mov dx, 3D4h
        al = 18h
        out dx, al      ; Video: CRT cntrlr addr
                    ; line compare (scan line). Used for split screen operations.
        inc dx
        al = cl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 7
        out dx, al      ; Video: CRT cntrlr addr
                    ; bit 8 for certain CRTC regs. Data bits:
                    ; 0: vertical total (Reg 06)
                    ; 1: vert disp'd enable end (Reg 12H)
                    ; 2: vert retrace start (Reg 10H)
                    ; 3: start vert blanking (Reg 15H)
                    ; 4: line compare (Reg 18H)
                    ; 5: cursor location (Reg 0aH)
        inc dx
        al = 3Fh ; '?'
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 9
        out dx, al      ; Video: CRT cntrlr addr
                    ; maximum scan line
        inc dx
        al = 80h ; '?'
        out dx, al      ; Video: CRT controller internal registers
        jmp short loc_49635
// ; ---------------------------------------------------------------------------

loc_495ED:              ; CODE XREF: sub_4955B+6Ej
        mov cl, 0C8h ; '?'
        mov dx, 3D4h
        al = 18h
        out dx, al      ; Video: CRT cntrlr addr
                    ; line compare (scan line). Used for split screen operations.
        inc dx
        al = cl
        out dx, al      ; Video: CRT controller internal registers
        jmp short loc_49635
// ; ---------------------------------------------------------------------------

loc_495FB:              ; CODE XREF: sub_4955B+62j
        mov byte ptr word_510C1, 1
        cmp videoStatusUnk, 1
        jnz short loc_49629
        mov cl, 5Fh ; '_'
        mov dx, 3D4h
        al = 18h
        out dx, al      ; Video: CRT cntrlr addr
                    ; line compare (scan line). Used for split screen operations.
        inc dx
        al = cl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 7
        out dx, al      ; Video: CRT cntrlr addr
                    ; bit 8 for certain CRTC regs. Data bits:
                    ; 0: vertical total (Reg 06)
                    ; 1: vert disp'd enable end (Reg 12H)
                    ; 2: vert retrace start (Reg 10H)
                    ; 3: start vert blanking (Reg 15H)
                    ; 4: line compare (Reg 18H)
                    ; 5: cursor location (Reg 0aH)
        inc dx
        al = 3Fh ; '?'
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 9
        out dx, al      ; Video: CRT cntrlr addr
                    ; maximum scan line
        inc dx
        al = 80h ; '?'
        out dx, al      ; Video: CRT controller internal registers
        jmp short loc_49635
// ; ---------------------------------------------------------------------------

loc_49629:              ; CODE XREF: sub_4955B+AAj
        mov cl, 0B0h ; '?'
        mov dx, 3D4h
        al = 18h
        out dx, al      ; Video: CRT cntrlr addr
                    ; line compare (scan line). Used for split screen operations.
        inc dx
        al = cl
        out dx, al      ; Video: CRT controller internal registers

loc_49635:              ; CODE XREF: sub_4955B+4Bj
                    ; sub_4955B+55j ...
        cmp word_51970, 1
        jz  short loc_4963F
        jmp loc_49949
// ; ---------------------------------------------------------------------------

loc_4963F:              ; CODE XREF: sub_4955B+DFj
        cmp byte_510E3, 0
        jz  short loc_49649
        jmp loc_49742
// ; ---------------------------------------------------------------------------

loc_49649:              ; CODE XREF: sub_4955B+E9j
        cmp byte ptr dword_519AF, 0
        jz  short loc_49656
        mov word_51A01, 1

loc_49656:              ; CODE XREF: sub_4955B+F3j
        cmp byte_5199D, 0
        jz  short loc_49663
        mov word ptr flashingbackgroundon, 1

loc_49663:              ; CODE XREF: sub_4955B+100j
        cmp byte ptr word_519A9, 0
        jz  short loc_4966F
        mov ah, 1
        call    sub_4A23C

loc_4966F:              ; CODE XREF: sub_4955B+10Dj
        cmp byte ptr word_519AD, 0
        jz  short loc_4967B
        mov ah, 2
        call    sub_4A23C

loc_4967B:              ; CODE XREF: sub_4955B+119j
        cmp byte_519A0, 0
        jz  short loc_49687
        mov ah, 6
        call    sub_4A23C

loc_49687:              ; CODE XREF: sub_4955B+125j
        cmp byte ptr word_519AB, 0
        jz  short loc_49693
        mov ah, 5
        call    sub_4A23C

loc_49693:              ; CODE XREF: sub_4955B+131j
        cmp byte_5199C, 0
        jz  short loc_4969F
        mov ah, 11h
        call    sub_4A23C

loc_4969F:              ; CODE XREF: sub_4955B+13Dj
        cmp byte_51990, 0
        jz  short loc_496AC
        call    videoloop
        call    sub_4A3E9

loc_496AC:              ; CODE XREF: sub_4955B+149j
        cmp byte_510DE, 0
        jz  short loc_496B6
        jmp loc_49742
// ; ---------------------------------------------------------------------------

loc_496B6:              ; CODE XREF: sub_4955B+156j
        cmp speed?3, 0
        jge short loc_496C0
        jmp loc_49742
// ; ---------------------------------------------------------------------------

loc_496C0:              ; CODE XREF: sub_4955B+160j
        cmp byte_5197F, 0
        jz  short loc_496CD
        mov word_51A07, 1

loc_496CD:              ; CODE XREF: sub_4955B+16Aj
        cmp byte_51980, 0
        jz  short loc_496DA
        mov word_51A07, 2

loc_496DA:              ; CODE XREF: sub_4955B+177j
        cmp byte_51981, 0
        jz  short loc_496E7
        mov word_51A07, 3

loc_496E7:              ; CODE XREF: sub_4955B+184j
        cmp byte_51982, 0
        jz  short loc_496F4
        mov word_51A07, 4

loc_496F4:              ; CODE XREF: sub_4955B+191j
        cmp byte_51983, 0
        jz  short loc_49701
        mov word_51A07, 6

loc_49701:              ; CODE XREF: sub_4955B+19Ej
        cmp byte_51984, 0
        jz  short loc_4970E
        mov word_51A07, 8

loc_4970E:              ; CODE XREF: sub_4955B+1ABj
        cmp byte_51985, 0
        jz  short loc_4971B
        mov word_51A07, 0Ch

loc_4971B:              ; CODE XREF: sub_4955B+1B8j
        cmp byte_51986, 0
        jz  short loc_49728
        mov word_51A07, 10h

loc_49728:              ; CODE XREF: sub_4955B+1C5j
        cmp byte_51987, 0
        jz  short loc_49735
        mov word_51A07, 18h

loc_49735:              ; CODE XREF: sub_4955B+1D2j
        cmp byte_51988, 0
        jz  short loc_49742
        mov word_51A07, 20h ; ' '

loc_49742:              ; CODE XREF: sub_4955B+EBj
                    ; sub_4955B+158j ...
        cmp byte_5199A, 1
        jnz short loc_4974C
        jmp loc_497D1
// ; ---------------------------------------------------------------------------

loc_4974C:              ; CODE XREF: sub_4955B+1ECj
        mov ax, word_5195D
        and ax, 7
        jz  short loc_49757
        jmp loc_4988E
// ; ---------------------------------------------------------------------------

loc_49757:              ; CODE XREF: sub_4955B+1F7j
        cmp byte_510E3, 0
        jz  short loc_49761
        jmp loc_4988E
// ; ---------------------------------------------------------------------------

loc_49761:              ; CODE XREF: sub_4955B+201j
        cmp byte_519B8, 0
        jnz short loc_4976F
        mov byte_59B7C, 0
        jmp short loc_49786
// ; ---------------------------------------------------------------------------

loc_4976F:              ; CODE XREF: sub_4955B+20Bj
        cmp byte_59B7C, 0
        jnz short loc_49786
        dec byte_59B7C
        and byte_5101C, 1
        xor byte_5101C, 1
        jmp short loc_497CE
// ; ---------------------------------------------------------------------------

loc_49786:              ; CODE XREF: sub_4955B+212j
                    ; sub_4955B+219j
        cmp byte_519B9, 0
        jnz short loc_49794
        mov byte_59B7D, 0
        jmp short loc_497AB
// ; ---------------------------------------------------------------------------

loc_49794:              ; CODE XREF: sub_4955B+230j
        cmp byte_59B7D, 0
        jnz short loc_497AB
        dec byte_59B7D
        and byte_51035, 2
        xor byte_51035, 2
        jmp short loc_497CE
// ; ---------------------------------------------------------------------------

loc_497AB:              ; CODE XREF: sub_4955B+237j
                    ; sub_4955B+23Ej
        cmp byte_519BA, 0
        jnz short loc_497B9
        mov byte_59B7E, 0
        jmp short loc_497CE
// ; ---------------------------------------------------------------------------

loc_497B9:              ; CODE XREF: sub_4955B+255j
        cmp byte_59B7E, 0
        jnz short loc_497CE
        dec byte_59B7E
        and byte_510D7, 1
        xor byte_510D7, 1

loc_497CE:              ; CODE XREF: sub_4955B+229j
                    ; sub_4955B+24Ej ...
        jmp loc_4988E
// ; ---------------------------------------------------------------------------

loc_497D1:              ; CODE XREF: sub_4955B+1EEj
        cmp byte_510DE, 0
        jz  short loc_497DB
        jmp loc_4988E
// ; ---------------------------------------------------------------------------

loc_497DB:              ; CODE XREF: sub_4955B+27Bj
        cmp speed?3, 0
        jge short loc_497E5
        jmp loc_4988E
// ; ---------------------------------------------------------------------------

loc_497E5:              ; CODE XREF: sub_4955B+285j
        cmp byte_519B8, 1
        jnz short loc_497F5
        mov ax, 0
        call    sub_4945D
        jmp loc_4988E
// ; ---------------------------------------------------------------------------

loc_497F5:              ; CODE XREF: sub_4955B+28Fj
        cmp byte_519B9, 1
        jnz short loc_49805
        mov ax, 1
        call    sub_4945D
        jmp loc_4988E
// ; ---------------------------------------------------------------------------

loc_49805:              ; CODE XREF: sub_4955B+29Fj
        cmp byte_519BA, 1
        jnz short loc_49814
        mov ax, 2
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_49814:              ; CODE XREF: sub_4955B+2AFj
        cmp byte_519BB, 1
        jnz short loc_49823
        mov ax, 3
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_49823:              ; CODE XREF: sub_4955B+2BEj
        cmp byte_519BC, 1
        jnz short loc_49832
        mov ax, 4
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_49832:              ; CODE XREF: sub_4955B+2CDj
        cmp byte_519BD, 1
        jnz short loc_49841
        mov ax, 5
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_49841:              ; CODE XREF: sub_4955B+2DCj
        cmp byte_519BE, 1
        jnz short loc_49850
        mov ax, 6
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_49850:              ; CODE XREF: sub_4955B+2EBj
        cmp byte_519BF, 1
        jnz short loc_4985F
        mov ax, 7
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_4985F:              ; CODE XREF: sub_4955B+2FAj
        cmp byte_519C0, 1
        jnz short loc_4986E
        mov ax, 8
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_4986E:              ; CODE XREF: sub_4955B+309j
        cmp byte_519C1, 1
        jnz short loc_4987D
        mov ax, 9
        call    sub_4945D
        jmp short loc_4988E
// ; ---------------------------------------------------------------------------

loc_4987D:              ; CODE XREF: sub_4955B+318j
        cmp byte_519D5, 1
        jnz short loc_4988E
        cmp byte_510E3, 0
        jz  short loc_4988E
        call    somethingspsig

loc_4988E:              ; CODE XREF: sub_4955B+1F9j
                    ; sub_4955B+203j ...
        cmp byte_510E3, 0
        jz  short loc_49898
        jmp loc_49949
// ; ---------------------------------------------------------------------------

loc_49898:              ; CODE XREF: sub_4955B+338j
        cmp byte_510DE, 0
        jz  short loc_498A2
        jmp loc_49949
// ; ---------------------------------------------------------------------------

loc_498A2:              ; CODE XREF: sub_4955B+342j
        cmp byte_51989, 0
        jnz short loc_498B5
        mov byte_59B7F, 0
        mov byte_59B80, 5
        jmp short loc_498FC
// ; ---------------------------------------------------------------------------

loc_498B5:              ; CODE XREF: sub_4955B+34Cj
        cmp byte_59B7F, 0
        jz  short loc_498C2
        dec byte_59B7F
        jmp short loc_498FC
// ; ---------------------------------------------------------------------------

loc_498C2:              ; CODE XREF: sub_4955B+35Fj
        cmp byte_59B80, 0
        jz  short loc_498D2
        dec byte_59B80
        mov byte_59B7F, 10h

loc_498D2:              ; CODE XREF: sub_4955B+36Cj
        cmp word_51ABC, 1
        ja  short loc_498DF
        mov word_51ABC, 2

loc_498DF:              ; CODE XREF: sub_4955B+37Cj
        dec word_51ABC
        cmp word_51ABC, 6Fh ; 'o'
        jbe short loc_498F0
        mov word_51ABC, 6Fh ; 'o'

loc_498F0:              ; CODE XREF: sub_4955B+38Dj
        mov ax, word_51ABC
        call    sub_4BF4A
        call    sub_4C141
        call    sub_4A3D2

loc_498FC:              ; CODE XREF: sub_4955B+358j
                    ; sub_4955B+365j
        cmp byte_5198A, 0
        jnz short loc_4990F
        mov byte_59B81, 0
        mov byte_59B82, 5
        jmp short loc_49949
// ; ---------------------------------------------------------------------------

loc_4990F:              ; CODE XREF: sub_4955B+3A6j
        cmp byte_59B81, 0
        jz  short loc_4991C
        dec byte_59B81
        jmp short loc_49949
// ; ---------------------------------------------------------------------------

loc_4991C:              ; CODE XREF: sub_4955B+3B9j
        cmp byte_59B82, 0
        jz  short loc_4992C
        dec byte_59B82
        mov byte_59B81, 10h

loc_4992C:              ; CODE XREF: sub_4955B+3C6j
        cmp word_51ABC, 6Fh ; 'o'
        jb  short loc_49939
        mov word_51ABC, 6Eh ; 'n'

loc_49939:              ; CODE XREF: sub_4955B+3D6j
        inc word_51ABC
        mov ax, word_51ABC
        call    sub_4BF4A
        call    sub_4C141
        call    sub_4A3D2

loc_49949:              ; CODE XREF: sub_4955B+E1j
                    ; sub_4955B+33Aj ...
        cmp byte_59B6B, 0
        mov byte_59B6B, 0
        jz  short loc_49958
        jmp loc_49A89
// ; ---------------------------------------------------------------------------

loc_49958:              ; CODE XREF: sub_4955B+3F8j
        cmp byte_5199A, 1
        jz  short loc_49962
        jmp loc_49C41
// ; ---------------------------------------------------------------------------

loc_49962:              ; CODE XREF: sub_4955B+402j
        cmp byte_519D5, 1
        jnz short loc_49984
        cmp byte_510DE, 0
        jz  short loc_49984
        mov byte_510DE, 0
        mov byte_510B3, 0
        mov byte_5A2F9, 1
        mov byte_5A33E, 1

loc_49984:              ; CODE XREF: sub_4955B+40Cj
                    ; sub_4955B+413j
        cmp byte_519C3, 1
        jnz short loc_499AA
        mov word_51970, 1
        cmp videoStatusUnk, 1
        jnz short loc_499AA
        mov di, 6D2h
        mov ah, 6
        push    si
        mov si, 0A00Ah
        call    sub_500F3
        pop si
        mov byte_5197C, 46h ; 'F'

loc_499AA:              ; CODE XREF: sub_4955B+42Ej
                    ; sub_4955B+43Bj
        cmp byte_5198E, 1
        jz  short loc_499C8
        jmp loc_49A7F
sub_4955B   endp


; =============== S U B R O U T I N E =======================================


readFromFh1 proc near       ; CODE XREF: sub_4955B+54Ep
                    ; sub_4955B+57Cp ...
        mov ax, 3F00h
        mov bx, fh1
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        retn
readFromFh1 endp


; =============== S U B R O U T I N E =======================================


writeToFh1  proc near       ; CODE XREF: sub_4955B+486p
                    ; sub_4955B+494p ...
        mov ax, 4000h
        mov bx, fh1
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        retn
writeToFh1  endp

// ; ---------------------------------------------------------------------------
; START OF FUNCTION CHUNK FOR sub_4955B

loc_499C8:              ; CODE XREF: sub_4955B+454j
        mov cx, 0
        mov dx, offset aSavegame_sav ; "SAVEGAME.SAV"
        mov ax, 3C00h
        int 21h     ; DOS - 2+ - CREATE A FILE WITH HANDLE (CREAT)
                    ; CX = attributes for file
                    ; DS:DX -> ASCIZ filename (may include drive and path)
        jnb short loc_499D8
        jmp loc_49C28
// ; ---------------------------------------------------------------------------

loc_499D8:              ; CODE XREF: sub_4955B+478j
        mov fh1, ax
        mov cx, 4
        mov dx, 9FF9h
        call    writeToFh1
        jnb short loc_499E9
        jmp loc_49C1F
// ; ---------------------------------------------------------------------------

loc_499E9:              ; CODE XREF: sub_4955B+489j
        mov cx, 1238h
        mov dx, offset leveldata
        call    writeToFh1
        jnb short loc_499F7
        jmp loc_49C1F
// ; ---------------------------------------------------------------------------

loc_499F7:              ; CODE XREF: sub_4955B+497j
        cmp byte_510DE, 0
        jnz short loc_49A03
        mov dx, 87A8h
        jmp short loc_49A06
// ; ---------------------------------------------------------------------------

loc_49A03:              ; CODE XREF: sub_4955B+4A1j
        mov dx, 87DAh

loc_49A06:              ; CODE XREF: sub_4955B+4A6j
        mov cx, 1Ch
        push    dx
        call    writeToFh1
        pop bx
        jnb short loc_49A13
        jmp loc_49C1F
// ; ---------------------------------------------------------------------------

loc_49A13:              ; CODE XREF: sub_4955B+4B3j
        mov cx, 6
        mov byte_5988D, 63h ; 'c'
        mov ax, word ptr aLevels_dat_0+8 ; "AT"
        mov word_5988E, ax
        al = [bx]
        cmp byte_510DE, 0
        jz  short loc_49A2C
        or  al, 80h

loc_49A2C:              ; CODE XREF: sub_4955B+4CDj
        mov byte_59890, al
        mov ax, [bx+1]
        mov word_59891, ax
        mov dx, 957Dh
        call    writeToFh1
        jnb short loc_49A40
        jmp loc_49C1F
// ; ---------------------------------------------------------------------------

loc_49A40:              ; CODE XREF: sub_4955B+4E0j
        mov cx, 0E6h ; '?'
        mov dx, 0D08h
        call    writeToFh1
        jnb short loc_49A4E
        jmp loc_49C1F
// ; ---------------------------------------------------------------------------

loc_49A4E:              ; CODE XREF: sub_4955B+4EEj
        mov cx, 23h ; '#'
        mov dx, 164Ah
        call    writeToFh1
        jnb short loc_49A5C
        jmp loc_49C1F
// ; ---------------------------------------------------------------------------

loc_49A5C:              ; CODE XREF: sub_4955B+4FCj
        mov cx, levelDataLength
        mov dx, offset levelBuffer
        call    writeToFh1
        jnb short loc_49A6A
        jmp loc_49C1F
// ; ---------------------------------------------------------------------------

loc_49A6A:              ; CODE XREF: sub_4955B+50Aj
        mov ax, 3E00h
        mov bx, fh1
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short loc_49A78
        jmp loc_49C28
// ; ---------------------------------------------------------------------------

loc_49A78:              ; CODE XREF: sub_4955B+518j
        push    si
        mov si, 0A001h
        jmp loc_49C2C
// ; ---------------------------------------------------------------------------

loc_49A7F:              ; CODE XREF: sub_4955B+456j
        cmp byte ptr dword_519A3, 1
        jz  short loc_49A89
        jmp loc_49C41
// ; ---------------------------------------------------------------------------

loc_49A89:              ; CODE XREF: sub_4955B+3FAj
                    ; sub_4955B+529j
        mov ax, 3D00h
        mov dx, offset aSavegame_sav ; "SAVEGAME.SAV"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_49A96
        jmp loc_49C28
// ; ---------------------------------------------------------------------------

loc_49A96:              ; CODE XREF: sub_4955B+536j
        mov fh1, ax
        cmp byte_510E3, 0
        jz  short loc_49AA3
        call    somethingspsig

loc_49AA3:              ; CODE XREF: sub_4955B+543j
        mov cx, 4
        mov dx, 9FFDh
        call    readFromFh1
        jnb short loc_49AB1
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49AB1:              ; CODE XREF: sub_4955B+551j
        mov cx, word_5A30D
        cmp cx, word_5A309
        jnz short loc_49AC5
        mov cx, word_5A30F
        cmp cx, word_5A30B
        jz  short loc_49AD1

loc_49AC5:              ; CODE XREF: sub_4955B+55Ej
        mov ax, 3E00h
        mov bx, fh1
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jmp loc_49C28
// ; ---------------------------------------------------------------------------

loc_49AD1:              ; CODE XREF: sub_4955B+568j
        mov cx, 1238h
        mov dx, offset leveldata
        call    readFromFh1
        jnb short loc_49ADF
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49ADF:              ; CODE XREF: sub_4955B+57Fj
        mov cx, 1Ch
        mov dx, 87A8h
        call    readFromFh1
        jnb short loc_49AED
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49AED:              ; CODE XREF: sub_4955B+58Dj
        mov cx, 6
        mov dx, 957Dh
        call    readFromFh1
        jnb short loc_49AFB
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49AFB:              ; CODE XREF: sub_4955B+59Bj
        mov cx, 84h ; '?'
        mov dx, 0D08h
        call    readFromFh1
        jnb short loc_49B09
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49B09:              ; CODE XREF: sub_4955B+5A9j
        mov cx, 4
        mov dx, 0D9Bh
        call    readFromFh1
        jnb short loc_49B17
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49B17:              ; CODE XREF: sub_4955B+5B7j
        mov cx, 7
        mov dx, 0D90h
        call    readFromFh1
        jnb short loc_49B25
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49B25:              ; CODE XREF: sub_4955B+5C5j
        mov cx, 4
        mov dx, 0D9Bh
        call    readFromFh1
        jnb short loc_49B33
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49B33:              ; CODE XREF: sub_4955B+5D3j
        mov cx, 53h ; 'S'
        mov dx, 0D9Bh
        push    word_510C1
        call    readFromFh1
        pop word_510C1
        jnb short loc_49B49
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49B49:              ; CODE XREF: sub_4955B+5E9j
        mov cx, 23h ; '#'
        mov dx, 164Ah
        push    word_51961
        push    word_51967
        push    word_51970
        call    readFromFh1
        pop word_51970
        pop word_51967
        pop word_51961
        jnb short loc_49B6F
        jmp loc_49C1A
// ; ---------------------------------------------------------------------------

loc_49B6F:              ; CODE XREF: sub_4955B+60Fj
        cmp byte_5988D, 0
        jz  short loc_49B84
        mov cx, levelDataLength
        mov dx, offset levelBuffer
        call    readFromFh1
        jb  short loc_49B84
        call    sub_49D53

loc_49B84:              ; CODE XREF: sub_4955B+619j
                    ; sub_4955B+624j
        mov ax, 3E00h
        mov bx, fh1
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        mov byte_510DE, 0
        mov byte_510E3, 0
        mov byte_50941, 0
        mov word_51A01, 0
        mov word_51963, 0
        mov word_51965, 0
        mov word ptr flashingbackgroundon, 0
        mov word_51A07, 1
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        out dx, al
        out dx, al
        out dx, al
        call    getTime
        call    sub_4A1AE
        mov si, 60D5h
        call    sub_4D836
        call    sub_48F6D
        call    sub_501C0
        call    sub_4A2E6
        mov si, murphyloc
        mov ax, si
        shr ax, 1
        call    sub_4A291
        mov ax, 0FFFFh
        mov word_510B7, ax
        mov byte_510B9, al
        call    sub_4FDFD
        mov byte_5A2F9, 1
        mov byte_510E3, 0
        push    si
        mov si, 0A004h
        cmp videoStatusUnk, 1
        jnz short loc_49C12
        mov di, 6D2h
        mov ah, 6
        call    sub_500F3
        mov byte_5197C, 46h ; 'F'

loc_49C12:              ; CODE XREF: sub_4955B+6A8j
        mov si, 6015h
        call    fade
        jmp short loc_49C40
// ; ---------------------------------------------------------------------------

loc_49C1A:              ; CODE XREF: sub_4955B+553j
                    ; sub_4955B+581j ...
        mov byte_510E3, 0

loc_49C1F:              ; CODE XREF: sub_4955B+48Bj
                    ; sub_4955B+499j ...
        mov ax, 3E00h
        mov bx, fh1
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle

loc_49C28:              ; CODE XREF: sub_4955B+47Aj
                    ; sub_4955B+51Aj ...
        push    si
        mov si, 0A007h

loc_49C2C:              ; CODE XREF: sub_4955B+521j
        cmp videoStatusUnk, 1
        jnz short loc_49C40
        mov di, 6D2h
        mov ah, 6
        call    sub_500F3
        mov byte_5197C, 46h ; 'F'

loc_49C40:              ; CODE XREF: sub_4955B+6BDj
                    ; sub_4955B+6D6j
        pop si

loc_49C41:              ; CODE XREF: sub_4955B+404j
                    ; sub_4955B+52Bj
        cmp byte_519B5, 1
        jnz short loc_49C96
        cmp byte_519C3, 1
        jnz short loc_49C96
        mov word_51970, 0
        mov word_51A01, 0
        mov word_51963, 0
        mov word_51965, 0
        mov word ptr flashingbackgroundon, 0
        mov word_51A07, 1
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        out dx, al
        out dx, al
        out dx, al
        cmp videoStatusUnk, 1
        jnz short loc_49C96
        mov di, 6D2h
        mov ah, 6
        push    si
        mov si, 0A00Dh
        call    sub_500F3
        pop si
        mov byte_5197C, 46h ; 'F'

loc_49C96:              ; CODE XREF: sub_4955B+6EBj
                    ; sub_4955B+6F2j ...
        cmp byte_51996, 0
        jz  short loc_49CC8
        mov byte_510AE, 0
        mov si, 6095h
        call    fade

loc_49CA8:              ; CODE XREF: sub_4955B+752j
        cmp byte_51996, 1
        jz  short loc_49CA8

loc_49CAF:              ; CODE XREF: sub_4955B+759j
        cmp byte_51996, 0
        jz  short loc_49CAF

loc_49CB6:              ; CODE XREF: sub_4955B+760j
        cmp byte_51996, 1
        jz  short loc_49CB6
        mov si, 6015h
        call    fade
        mov byte_510AE, 1

loc_49CC8:              ; CODE XREF: sub_4955B+740j
        cmp byte_519C2, 0
        jz  short loc_49D15
        mov byte_510AE, 0
        mov si, 6095h
        call    fade

loc_49CDA:              ; CODE XREF: sub_4955B+784j
        cmp byte_519C2, 1
        jz  short loc_49CDA
        mov si, 179Ah

loc_49CE4:              ; CODE XREF: sub_4955B+7A6j
        mov bx, [si]
        cmp bx, 0FFFFh
        jnz short loc_49CF3
        mov word_51970, 0
        jmp short loc_49D03
// ; ---------------------------------------------------------------------------

loc_49CF3:              ; CODE XREF: sub_4955B+78Ej
        cmp byte ptr [bx+166Dh], 0
        jz  short loc_49CFC
        inc si
        inc si

loc_49CFC:              ; CODE XREF: sub_4955B+79Dj
        cmp byte_519C2, 0
        jz  short loc_49CE4

loc_49D03:              ; CODE XREF: sub_4955B+796j
                    ; sub_4955B+7ADj
        cmp byte_519C2, 1
        jz  short loc_49D03
        mov si, 6015h
        call    fade
        mov byte_510AE, 1

loc_49D15:              ; CODE XREF: sub_4955B+772j
        cmp byte_5197E, 0
        jz  short loc_49D29
        cmp word_51978, 0
        jg  short loc_49D29
        mov word_510D1, 1

loc_49D29:              ; CODE XREF: sub_4955B+7BFj
                    ; sub_4955B+7C6j
        cmp byte_5198D, 0
        jz  short loc_49D48
        mov word_51A01, 0
        mov word_51963, 0
        mov word_51965, 0
        mov word ptr flashingbackgroundon, 0

loc_49D48:              ; CODE XREF: sub_4955B+7D3j
        cmp byte ptr word_519B3, 0
        jz  short locret_49D52
        call    sub_4FDCE

locret_49D52:               ; CODE XREF: sub_4955B+7F2j
        retn
; END OF FUNCTION CHUNK FOR sub_4955B

; =============== S U B R O U T I N E =======================================


sub_49D53   proc near       ; CODE XREF: sub_4955B+626p
                    ; sub_4A23C+21p
        mov byte_59B7B, 0
sub_49D53   endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================


levelScanThing   proc near       ; CODE XREF: runLevel+A7p
        push    es
        push    ds
        pop es
        assume es:data
        cld
        mov cx, 5A0h
        mov di, offset leveldata
        mov ax, 0F11Fh

loc_49D65:              ; CODE XREF: levelScanThing+18j
        cmp es:[di], al
        jnz short loc_49D6D
        mov es:[di], ah

loc_49D6D:              ; CODE XREF: levelScanThing+10j
        inc di
        inc di
        dec cx
        jnz short loc_49D65
        cmp byte_59B7B, 0
        mov byte_59B7B, 0
        jnz short loc_49DDE
        mov cx, 5A0h
        mov di, offset leveldata

loc_49D84:              ; CODE XREF: levelScanThing+4Cj
        mov ax, 6
        repne scasw
        jnz short loc_49DA6
        mov bx, 59Fh
        sub bx, cx
        al = [bx-6775h]
        cmp ax, 1Ch
        jb  short loc_49DA2
        cmp ax, 25h ; '%'
        ja  short loc_49DA2
        mov es:[di-2], ax

loc_49DA2:              ; CODE XREF: levelScanThing+3Fj
                    ; levelScanThing+44j
        or  cx, cx
        jnz short loc_49D84

loc_49DA6:              ; CODE XREF: levelScanThing+31j
        mov cx, 5A0h
        mov di, offset leveldata

loc_49DAC:              ; CODE XREF: levelScanThing+7Fj
        mov ax, 5
        repne scasw
        jnz short loc_49DD9
        mov bx, 59Fh
        sub bx, cx
        al = [bx-6775h]
        cmp ax, 1Ah
        jb  short loc_49DD5
        cmp ax, 27h ; '''
        ja  short loc_49DD5
        sub ax, 1Ch
        cmp ax, 0Ah
        jb  short loc_49DD5
        add ax, 1Ch
        mov es:[di-2], ax

loc_49DD5:              ; CODE XREF: levelScanThing+67j
                    ; levelScanThing+6Cj ...
        or  cx, cx
        jnz short loc_49DAC

loc_49DD9:              ; CODE XREF: levelScanThing+59j
        mov byte_59B7B, 1

loc_49DDE:              ; CODE XREF: levelScanThing+24j
        pop es
        assume es:nothing
        retn
levelScanThing   endp


; =============== S U B R O U T I N E =======================================


gameloop?   proc near       ; CODE XREF: runLevel:noFlashingp

        ; set graphics write mode = 1
        mov dx, 3CEh
        al = 5
        out dx, al
        inc dx
        al = 1
        out dx, al

        mov si, murphyloc
        call    update?
        mov murphyloc, si

        cmp word ptr flashingbackgroundon, 0
        jz  short loc_49E0A

        ; flashes background
        ; set palette ix 0 = #3f2121 (maroon)
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        al = 3Fh
        out dx, al
        out dx, al
        al = 21h
        out dx, al

loc_49E0A:
        cmp byte ptr word_510C1, 0
        jz  short loc_49E14
        call    sub_5024B

loc_49E14:
        ; set graphics write mode = 1
        mov dx, 3CEh
        al = 5
        out dx, al
        inc dx
        al = 1
        out dx, al

        cmp word ptr flashingbackgroundon, 0
        jz  short loc_49E33
        
        ; flashes background
        ; set palette ix 0 = #3f2121 (maroon)
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        al = 3Fh
        out dx, al
        al = 21h
        out dx, al
        out dx, al

loc_49E33:
        mov di, offset movingObjects?
        mov si, 7Ah ; level width in words + 2 (index of first gamefiled cell)
        mov cx, 526h ; unsure count
        xor dx, dx
        xor bh, bh

checkCellForMovingObject:              ; CODE XREF: gameloop?+84j
        mov bl, byte ptr leveldata[si]

        test    bl, 0Dh
        jz  short moveToNextCell

        cmp bl, 20h
        jnb short moveToNextCell

        ; multiply by 2 to go from byte to word
        shl bx, 1
        mov ax, movingFunctions[bx]
        or  ax, ax
        jz  short moveToNextCell

        ; log moving object for call
        ; each index has 2 bytes for index and 2 bytes as fn ptr to call
        inc dx
        mov [di], si
        mov [di+2], ax
        add di, 4

moveToNextCell:

        add si, 2
        loop    checkCellForMovingObject

        cmp dx, 0
        jz  short doneWithGameLoop ; not a single moving object
        mov cx, dx ; cx = number of moving objects
        mov di, offset movingObjects?

; call moving functions one by one
callNextMovingFunction:
        push(di);
        push(cx);
        mov si, [di]    ; the cell for the moving object
        mov ax, [di+2]  ; the function to call
        call    ax
        pop(cx);
        pop(di);
        add di, 4
        loop    callNextMovingFunction

doneWithGameLoop:
        ; set graphics write mode = 0
        mov dx, 3CEh
        al = 5
        out dx, al
        inc dx
        al = 0
        out dx, al

        cmp word_510D1, 1
        jz  short loc_49E99
        cmp word_510CF, 0
        jz  short loc_49E99
        retn
// ; ---------------------------------------------------------------------------

loc_49E99:              ; CODE XREF: gameloop?+AFj
                    ; gameloop?+B6j
        cmp word_51978, 0
        jnz short loc_49EB3
        mov word_510D1, 0
        mov si, word_510C7
        call    sub_4A61F
        mov word_51978, 40h ; '@'

loc_49EB3:
        ; set graphics write mode = 0
        mov dx, 3CEh
        al = 5
        out dx, al
        inc dx
        al = 1
        out dx, al

        retn
gameloop?   endp


; =============== S U B R O U T I N E =======================================


sub_49EBE   proc near       ; CODE XREF: runLevel+109p
                    ; sub_4A291+29p
        xor dx, dx
        cmp byte_510C0, 0
        jz  short loc_49ECC
        call    sub_4A1AE
        mov dx, ax

loc_49ECC:              ; CODE XREF: sub_49EBE+7j
        mov bx, word_510E8
        mov ax, word_510EA
        sub bx, 98h ; '?'
        cmp bx, 8
        jge short loc_49EDF
        mov bx, 8

loc_49EDF:              ; CODE XREF: sub_49EBE+1Cj
        cmp bx, 278h
        jbe short loc_49EE8
        mov bx, 278h

loc_49EE8:              ; CODE XREF: sub_49EBE+25j
        cmp byte ptr word_510C1, 0
        jz  short loc_49EF4
        sub ax, 58h ; 'X'
        jmp short loc_49EF7
// ; ---------------------------------------------------------------------------

loc_49EF4:              ; CODE XREF: sub_49EBE+2Fj
        sub ax, 64h ; 'd'

loc_49EF7:              ; CODE XREF: sub_49EBE+34j
        cmp ax, 0
        jge short loc_49EFE
        xor ax, ax

loc_49EFE:              ; CODE XREF: sub_49EBE+3Cj
        cmp byte ptr word_510C1, 0
        jz  short loc_49F0F
        cmp ax, 0C0h ; '?'
        jle short loc_49F0D
        mov ax, 0C0h ; '?'

loc_49F0D:              ; CODE XREF: sub_49EBE+4Aj
        jmp short loc_49F17
// ; ---------------------------------------------------------------------------

loc_49F0F:              ; CODE XREF: sub_49EBE+45j
        cmp ax, 0A8h ; '?'
        jle short loc_49F17
        mov ax, 0A8h ; '?'

loc_49F17:              ; CODE XREF: sub_49EBE:loc_49F0Dj
                    ; sub_49EBE+54j
        cmp word_51A01, 0
        jz  short loc_49F25
        cmp byte_519C9, 0
        jz  short loc_49F2E

loc_49F25:              ; CODE XREF: sub_49EBE+5Ej
        mov word_59B88, bx
        mov word_59B8A, ax
        jmp short loc_49FA9
// ; ---------------------------------------------------------------------------

loc_49F2E:              ; CODE XREF: sub_49EBE+65j
        mov bx, word_59B88
        mov ax, word_59B8A
        mov cx, bx
        add bx, word_51963
        cmp bx, 8
        jge short loc_49F56
        cmp byte_59B6C, 0
        jnz short loc_49F4C
        mov bx, 8
        jmp short loc_49F66
// ; ---------------------------------------------------------------------------

loc_49F4C:              ; CODE XREF: sub_49EBE+87j
        add bx, 3D0h
        dec word_51965
        jmp short loc_49F66
// ; ---------------------------------------------------------------------------

loc_49F56:              ; CODE XREF: sub_49EBE+80j
        cmp byte_59B6C, 0
        jnz short loc_49F6E
        cmp bx, 278h
        jbe short loc_49F6E
        mov bx, 278h

loc_49F66:              ; CODE XREF: sub_49EBE+8Cj
                    ; sub_49EBE+96j
        sub cx, bx
        neg cx
        mov word_51963, cx

loc_49F6E:              ; CODE XREF: sub_49EBE+9Dj
                    ; sub_49EBE+A3j
        mov cx, ax
        add ax, word_51965
        jge short loc_49F81
        cmp byte_59B6C, 0
        jnz short loc_49FA9
        xor ax, ax
        jmp short loc_49FA1
// ; ---------------------------------------------------------------------------

loc_49F81:              ; CODE XREF: sub_49EBE+B6j
        cmp byte_59B6C, 0
        jnz short loc_49FA9
        cmp byte ptr word_510C1, 0
        jz  short loc_49F99
        cmp ax, 0C0h ; '?'
        jbe short loc_49FA9
        mov ax, 0C0h ; '?'
        jmp short loc_49FA1
// ; ---------------------------------------------------------------------------

loc_49F99:              ; CODE XREF: sub_49EBE+CFj
        cmp ax, 0A8h ; '?'
        jbe short loc_49FA9
        mov ax, 0A8h ; '?'

loc_49FA1:              ; CODE XREF: sub_49EBE+C1j
                    ; sub_49EBE+D9j
        sub cx, ax
        neg cx
        mov word_51965, cx

loc_49FA9:              ; CODE XREF: sub_49EBE+6Ej
                    ; sub_49EBE+BDj ...
        cmp byte_59B72, 0
        jnz short loc_49FBE
        cmp byte_59B6D, 0
        jnz short loc_49FD2
        cmp byte ptr word_51978, 0
        jz  short loc_49FD2

loc_49FBE:              ; CODE XREF: sub_49EBE+F0j
        xor cx, cx
        and dx, 101h
        xchg    cl, dh
        add ax, cx
        cmp bx, 13Ch
        jbe short loc_49FD0
        neg dx

loc_49FD0:              ; CODE XREF: sub_49EBE+10Ej
        add bx, dx

loc_49FD2:              ; CODE XREF: sub_49EBE+F7j
                    ; sub_49EBE+FEj
        mov word_5195F, bx
        mov word_51961, ax
        mov cl, 3
        shr bx, cl
        mov cx, 7Ah ; 'z'
        mul cx
        add bx, ax
        add bx, 4D34h
        mov word_51967, bx
        retn
sub_49EBE   endp


; =============== S U B R O U T I N E =======================================


sub_49FED   proc near       ; CODE XREF: sub_48E59+2Cp
                    ; sub_48E59+67p ...
        al = byte_50940
        cmp al, 0
        jnz short loc_49FF8
        xor ax, ax
        stc
        retn
// ; ---------------------------------------------------------------------------

loc_49FF8:              ; CODE XREF: sub_49FED+5j
        mov dx, 201h
        cli
        mov cx, 0

loc_49FFF:              ; CODE XREF: sub_49FED+21j
        in  al, dx      ; Game I/O port
                    ; bits 0-3: Coordinates (resistive, time-dependent inputs)
                    ; bits 4-7: Buttons/Triggers (digital inputs)
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        test    al, ah
        jz  short loc_4A013
        loop    loc_49FFF
        stc
        jmp short loc_4A03B
// ; ---------------------------------------------------------------------------

loc_4A013:              ; CODE XREF: sub_49FED+1Fj
        out dx, al      ; Game I/O port
                    ; bits 0-3: Coordinates (resistive, time-dependent inputs)
                    ; bits 4-7: Buttons/Triggers (digital inputs)
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        mov cx, 0

loc_4A021:              ; CODE XREF: sub_49FED+44j
        nop
        in  al, dx      ; Game I/O port
                    ; bits 0-3: Coordinates (resistive, time-dependent inputs)
                    ; bits 4-7: Buttons/Triggers (digital inputs)
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        test    al, ah
        jz  short loc_4A036
        loop    loc_4A021
        stc
        jmp short loc_4A03B
// ; ---------------------------------------------------------------------------

loc_4A036:              ; CODE XREF: sub_49FED+42j
        neg cx
        mov ax, cx
        clc

loc_4A03B:              ; CODE XREF: sub_49FED+24j
                    ; sub_49FED+47j
        sti
        retn
sub_49FED   endp


; =============== S U B R O U T I N E =======================================


movefun7  proc near       ; DATA XREF: data:163Co
        cmp byte ptr leveldata[si], 19h
        jz  short loc_4A045
        retn
// ; ---------------------------------------------------------------------------

loc_4A045:              ; CODE XREF: movefun7+5j
        mov ax, word_5195D
        and ax, 3
        cmp ax, 0
        jz  short loc_4A051
        retn
// ; ---------------------------------------------------------------------------

loc_4A051:              ; CODE XREF: movefun7+11j
        mov bl, [si+1835h]
        inc bl
        cmp bl, 0Eh
        jl  short loc_4A067
        call    sub_4A1AE
        and al, 3Fh
        add al, 20h ; ' '
        neg al
        mov bl, al

loc_4A067:              ; CODE XREF: movefun7+1Dj
        mov [si+1835h], bl
        cmp bl, 0
        jge short loc_4A071
        retn
// ; ---------------------------------------------------------------------------

loc_4A071:              ; CODE XREF: movefun7+31j
        cmp byte ptr [si+17BAh], 3
        jz  short loc_4A0AB
        cmp byte ptr [si+17BCh], 3
        jz  short loc_4A0AB
        cmp byte ptr [si+17BEh], 3
        jz  short loc_4A0AB
        cmp byte ptr [si+1832h], 3
        jz  short loc_4A0AB
        cmp byte ptr [si+1836h], 3
        jz  short loc_4A0AB
        cmp byte ptr [si+18AAh], 3
        jz  short loc_4A0AB
        cmp byte ptr [si+18ACh], 3
        jz  short loc_4A0AB
        cmp byte ptr [si+18AEh], 3
        jz  short loc_4A0AB
        jmp short loc_4A0AE
// ; ---------------------------------------------------------------------------

loc_4A0AB:              ; CODE XREF: movefun7+39j
                    ; movefun7+40j ...
        call    sound?8

loc_4A0AE:              ; CODE XREF: movefun7+6Cj
        xor bh, bh
        mov di, [si+6155h]
        mov si, bx
        shl si, 1
        add si, 1272h
        mov si, [si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4A0C6:              ; CODE XREF: movefun7+91j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4A0C6
        pop ds
        retn
movefun7  endp


; =============== S U B R O U T I N E =======================================


movefun5  proc near       ; DATA XREF: data:1630o
        cmp byte ptr leveldata[si], 13h
        jz  short loc_4A0DA
        retn
// ; ---------------------------------------------------------------------------

loc_4A0DA:              ; CODE XREF: movefun5+5j
        mov bl, [si+1835h]
        inc bl
        cmp bl, 0
        jg  short loc_4A0EA
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4A0EA:              ; CODE XREF: movefun5+11j
        call    sub_4A1AE
        and al, byte_5196A
        neg al
        mov bl, al
        mov [si+1835h], bl
        mov di, [si+6155h]
        mov si, di
        add di, 4C4h
        add si, 0F4h ; '?'
        push    ds
        mov ax, es
        mov ds, ax
        push    si
        push(di);
        movsb
        add di, 0FC2Fh
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        pop(di);
        pop si
        inc si
        inc di
        movsb
        add di, 0FC2Fh
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        movsb
        add di, 79h ; 'y'
        add si, 79h ; 'y'
        pop ds
        retn
movefun5  endp


; =============== S U B R O U T I N E =======================================


getTime     proc near       ; CODE XREF: start:doesNotHaveCommandLinep
                    ; sub_4955B+669p ...
        mov ax, 0
        int 1Ah     ; CLOCK - GET TIME OF DAY
                    ; Return: CX:DX = clock count
                    ; AL = 00h if clock was read or written (via AH=0,1) since the previous
                    ; midnight
                    ; Otherwise, AL > 0
        xor cx, dx
        mov timeOfDay, cx
        retn
getTime     endp


; =============== S U B R O U T I N E =======================================


sub_4A1AE   proc near       ; CODE XREF: sub_4955B+66Cp
                    ; sub_49EBE+9p ...
        mov ax, timeOfDay
        mov bx, 5E5h
        mul bx
        add ax, 31h ; '1'
        mov timeOfDay, ax
        shr ax, 1
        retn
sub_4A1AE   endp


; =============== S U B R O U T I N E =======================================


sub_4A1BF   proc near       ; CODE XREF: sub_4955B+13p
                    ; runMainMenu+BDp ...
        xor ah, ah
        cmp byte_519C5, 0
        jnz short loc_4A1CF
        cmp byte_519F9, 0
        jz  short loc_4A1D6

loc_4A1CF:              ; CODE XREF: sub_4A1BF+7j
        mov byte_50941, 1
        mov ah, 1

loc_4A1D6:              ; CODE XREF: sub_4A1BF+Ej
        cmp byte_519C8, 0
        jnz short loc_4A1E4
        cmp byte_519F7, 0
        jz  short loc_4A1EB

loc_4A1E4:              ; CODE XREF: sub_4A1BF+1Cj
        mov byte_50941, 2
        mov ah, 1

loc_4A1EB:              ; CODE XREF: sub_4A1BF+23j
        cmp byte_519CD, 0
        jnz short loc_4A1F9
        cmp byte_519F8, 0
        jz  short loc_4A200

loc_4A1F9:              ; CODE XREF: sub_4A1BF+31j
        mov byte_50941, 3
        mov ah, 1

loc_4A200:              ; CODE XREF: sub_4A1BF+38j
        cmp byte_519CA, 0
        jnz short loc_4A20E
        cmp byte_519F6, 0
        jz  short loc_4A215

loc_4A20E:              ; CODE XREF: sub_4A1BF+46j
        mov byte_50941, 4
        mov ah, 1

loc_4A215:              ; CODE XREF: sub_4A1BF+4Dj
        cmp byte_519B6, 0
        jnz short loc_4A22A
        cmp byte_519F5, 0
        jnz short loc_4A22A
        cmp byte_519F4, 0
        jz  short locret_4A23B

loc_4A22A:              ; CODE XREF: sub_4A1BF+5Bj
                    ; sub_4A1BF+62j
        cmp ah, 1
        jnz short loc_4A236
        add byte_50941, 4
        jmp short locret_4A23B
// ; ---------------------------------------------------------------------------

loc_4A236:              ; CODE XREF: sub_4A1BF+6Ej
        mov byte_50941, 9

locret_4A23B:               ; CODE XREF: sub_4A1BF+69j
                    ; sub_4A1BF+75j
        retn
sub_4A1BF   endp


; =============== S U B R O U T I N E =======================================


sub_4A23C   proc near       ; CODE XREF: sub_4955B+111p
                    ; sub_4955B+11Dp ...
        mov si, 0
        mov cx, 5A0h

loc_4A242:              ; CODE XREF: sub_4A23C+1Fj
        al = [si+1834h]
        cmp al, ah
        jz  short loc_4A253
        cmp ah, 11h
        jnz short loc_4A259
        cmp al, 0BBh ; '?'
        jnz short loc_4A259

loc_4A253:              ; CODE XREF: sub_4A23C+Cj
        mov word ptr leveldata[si], 0

loc_4A259:              ; CODE XREF: sub_4A23C+11j
                    ; sub_4A23C+15j
        inc si
        inc si
        loop    loc_4A242
        call    sub_49D53
        call    sub_48F6D
        call    sub_4A2E6
        mov byte_510B3, 0
        mov byte_5A2F9, 1
        retn
sub_4A23C   endp


; =============== S U B R O U T I N E =======================================


findMurphy   proc near       ; CODE XREF: start+344p sub_4A463+22p
        push    es
        mov ax, ds
        mov es, ax
        assume es:data
        mov di, offset leveldata
        mov cx, levelcells
        mov ax, murphy
        repne scasw
        pop es
        assume es:nothing
        mov ax, levelcells-1
        sub ax, cx
        mov cx, ax
        shl cx, 1
        mov murphyloc, cx
        mov si, cx
findMurphy   endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================


sub_4A291   proc near       ; CODE XREF: sub_4955B+686p
        mov bl, 3Ch ; '<'
        div bl
        mov bl, ah
        xor bh, bh
        mov word_510C3, bx
        xor ah, ah
        mov word_510C5, ax
        mov cl, 4
        shl ax, cl
        shl bx, cl
        mov word_510E8, bx
        mov word_510EA, ax
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        call    sub_49EBE
        mov ax, word_51961
        mov word_59B92, ax
        mov ax, word_5195F
        mov word_59B90, ax
        and al, 7
        mov byte_510A6, al
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        call    videoloop
        retn
sub_4A291   endp


; =============== S U B R O U T I N E =======================================


sub_4A2E6   proc near       ; CODE XREF: start+33Bp runLevel+ADp ...
        xor bx, bx
        xor dx, dx
        mov cx, 5A0h
        mov si, offset leveldata

loc_4A2F0:              ; CODE XREF: sub_4A2E6+D1j
        mov ax, [si]
        cmp al, 0F1h ; '?'
        jnz short loc_4A2FC
        mov byte ptr [si], 1Fh
        jmp loc_4A3B0
// ; ---------------------------------------------------------------------------

loc_4A2FC:              ; CODE XREF: sub_4A2E6+Ej
        cmp byte_5A33F, 1
        jz  short loc_4A312
        cmp ax, 4
        jz  short loc_4A33C
        cmp ax, 11h
        jz  short loc_4A34B
        cmp ax, 18h
        jz  short loc_4A379

loc_4A312:              ; CODE XREF: sub_4A2E6+1Bj
        cmp ax, 1Ah
        jz  short loc_4A33F
        cmp ax, 1Bh
        jz  short loc_4A33F
        cmp ax, 26h ; '&'
        jz  short loc_4A33F
        cmp ax, 27h ; '''
        jz  short loc_4A33F
        cmp ax, 1Ch
        jl  short loc_4A330
        cmp ax, 26h ; '&'
        jl  short loc_4A345

loc_4A330:              ; CODE XREF: sub_4A2E6+43j
        cmp ax, 0Dh
        jl  short loc_4A33A
        cmp ax, 11h
        jl  short loc_4A3A7

loc_4A33A:              ; CODE XREF: sub_4A2E6+4Dj
        jmp short loc_4A3B0
// ; ---------------------------------------------------------------------------

loc_4A33C:              ; CODE XREF: sub_4A2E6+20j
        inc dx
        jmp short loc_4A3B0
// ; ---------------------------------------------------------------------------

loc_4A33F:              ; CODE XREF: sub_4A2E6+2Fj
                    ; sub_4A2E6+34j ...
        mov word ptr [si], 5
        jmp short loc_4A3B0
// ; ---------------------------------------------------------------------------

loc_4A345:              ; CODE XREF: sub_4A2E6+48j
        mov word ptr [si], 6
        jmp short loc_4A3B0
// ; ---------------------------------------------------------------------------

loc_4A34B:              ; CODE XREF: sub_4A2E6+25j
        cmp word ptr [si-2], 0
        jnz short loc_4A357
        mov byte ptr [si+1], 1
        jmp short loc_4A3B0
// ; ---------------------------------------------------------------------------

loc_4A357:              ; CODE XREF: sub_4A2E6+69j
        cmp word ptr [si-78h], 0
        jnz short loc_4A368
        mov word ptr [si-78h], 1011h
        mov word ptr [si], 0FFFFh
        jmp short loc_4A3B0
// ; ---------------------------------------------------------------------------

loc_4A368:              ; CODE XREF: sub_4A2E6+75j
        cmp word ptr [si+2], 0
        jnz short loc_4A3B0
        mov word ptr [si+2], 2811h
        mov word ptr [si], 0FFFFh
        jmp short loc_4A3B0
// ; ---------------------------------------------------------------------------

loc_4A379:              ; CODE XREF: sub_4A2E6+2Aj
        cmp word ptr [si-2], 0
        jnz short loc_4A385
        mov byte ptr [si+1], 1
        jmp short loc_4A3B0
// ; ---------------------------------------------------------------------------

loc_4A385:              ; CODE XREF: sub_4A2E6+97j
        cmp word ptr [si-78h], 0
        jnz short loc_4A396
        mov word ptr [si-78h], 1018h
        mov word ptr [si], 0FFFFh
        jmp short loc_4A3B0
// ; ---------------------------------------------------------------------------

loc_4A396:              ; CODE XREF: sub_4A2E6+A3j
        cmp word ptr [si+2], 0
        jnz short loc_4A3B0
        mov word ptr [si+2], 2818h
        mov word ptr [si], 0FFFFh
        jmp short loc_4A3B0
// ; ---------------------------------------------------------------------------

loc_4A3A7:              ; CODE XREF: sub_4A2E6+52j
        sub byte ptr [si], 4
        mov byte ptr [si+1], 1
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A3B0:              ; CODE XREF: sub_4A2E6+13j
                    ; sub_4A2E6:loc_4A33Aj ...
        add si, 2
        inc bx
        dec cx
        jz  short locret_4A3BA
        jmp loc_4A2F0
// ; ---------------------------------------------------------------------------

locret_4A3BA:               ; CODE XREF: sub_4A2E6+CFj
        retn
sub_4A2E6   endp


; =============== S U B R O U T I N E =======================================


sub_4A3BB   proc near       ; CODE XREF: start+33Ep sub_4A463+17p
        cmp byte_51036, 0
        jz  short loc_4A3C6
        mov dl, byte_51036

loc_4A3C6:              ; CODE XREF: sub_4A3BB+5j
        mov byte_5195A, dl
        mov byte_5195B, dl
        call    sub_4FD21
        retn
sub_4A3BB   endp


; =============== S U B R O U T I N E =======================================


sub_4A3D2   proc near       ; CODE XREF: sub_4955B+39Ep
                    ; sub_4955B+3EBp
        mov byte_599D4, 0
        mov word_599D8, 0
        cmp byte_5A33E, 0
        mov byte_5A33E, 0
        jnz short $+12
sub_4A3D2   endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================


sub_4A3E9   proc near       ; CODE XREF: sub_4955B+14Ep
        cmp byte_5A33E, 0
        jnz short loc_4A3F3
        call    sub_4A95F

loc_4A3F3:              ; CODE XREF: sub_4A3D2+15j
                    ; sub_4A3E9+5j
        mov word_51A01, 0
        mov word_51963, 0
        mov word_51965, 0
        mov word ptr flashingbackgroundon, 0
        mov word_51A07, 1
        mov dx, 3C8h
        xor al, al
        out dx, al
        inc dx
        out dx, al
        out dx, al
        out dx, al
        cmp byte_5A33E, 0
        jz  short loc_4A427
        mov byte_510DE, 1

loc_4A427:              ; CODE XREF: sub_4A3E9+37j
        mov byte_5A33F, 0
        call    sub_4A463
        mov byte_5A33F, 1
        cmp byte_5A33E, 1
        jb  short loc_4A446
        mov byte_510DE, 0
        jnz short loc_4A446
        inc byte_5A33E

loc_4A446:              ; CODE XREF: sub_4A3E9+50j
                    ; sub_4A3E9+57j
        mov byte_50941, 0
        cmp byte_510DE, 0
        jz  short locret_4A462
        mov bx, word_5A33C
        mov word_510DF, bx
        mov byte_510E2, 1
        call    sub_492A8

locret_4A462:               ; CODE XREF: sub_4A3E9+67j
        retn
sub_4A3E9   endp


; =============== S U B R O U T I N E =======================================


sub_4A463   proc near       ; CODE XREF: sub_4945D:loc_4953Bp
                    ; sub_4A3E9+43p
        call    readLevels
        call    sub_4D464
        call    sub_48F6D
        call    sub_501C0
        neg byte_5A33F
        call    sub_4A2E6
        neg byte_5A33F
        call    sub_4A3BB
        mov byte_59B7B, 1
        call    sub_48A20
        call    findMurphy
        retn
sub_4A463   endp


; =============== S U B R O U T I N E =======================================


movefun3  proc near       ; DATA XREF: data:161Ao
        cmp byte ptr leveldata[si], 8
        jz  short loc_4A491
        retn
// ; ---------------------------------------------------------------------------

loc_4A491:              ; CODE XREF: movefun3+5j
        mov ax, [si+1834h]
        cmp ax, 3008h
        jge short loc_4A4D4
        cmp ax, 2008h
        jge short loc_4A4B4
        mov ax, [si+18ACh]
        cmp ax, 0
        jz  short loc_4A4A9
        retn
// ; ---------------------------------------------------------------------------

loc_4A4A9:              ; CODE XREF: movefun3+1Dj
        mov byte ptr [si+1835h], 20h ; ' '
        mov byte ptr [si+18ADh], 8
        retn
// ; ---------------------------------------------------------------------------

loc_4A4B4:              ; CODE XREF: movefun3+14j
        cmp word ptr [si+18ACh], 0
        jnz short loc_4A4C2
        mov word ptr leveldata[si], 8
        retn
// ; ---------------------------------------------------------------------------

loc_4A4C2:              ; CODE XREF: movefun3+30j
        mov bl, [si+1835h]
        inc bl
        cmp bl, 22h ; '"'
        jnz short loc_4A4CF
        mov bl, 30h ; '0'

loc_4A4CF:              ; CODE XREF: movefun3+42j
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4A4D4:              ; CODE XREF: movefun3+Fj
        push    si
        mov bl, [si+1835h]
        xor bh, bh
        al = bl
        shl bx, 1
        ;and bx, byte ptr 0Fh
        db 83h, 0E3h, 0Fh
        mov di, [si+6155h]
        shl bx, 1
        add di, [bx+6C95h]
        mov si, 12F6h
        mov si, [si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 12h

loc_4A4F9:              ; CODE XREF: movefun3+78j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4A4F9
        pop ds
        pop si
        mov bl, [si+1835h]
        inc bl
        al = bl
        and al, 7
        jz  short loc_4A516
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4A516:              ; CODE XREF: movefun3+86j
        mov word ptr leveldata[si], 0
        mov word ptr [si+18ACh], 8
        add si, 78h ; 'x'
        cmp word ptr [si+18ACh], 0
        jnz short loc_4A537
        mov byte ptr [si+1835h], 30h ; '0'
        mov byte ptr [si+18ADh], 8
        retn
// ; ---------------------------------------------------------------------------

loc_4A537:              ; CODE XREF: movefun3+A1j
        cmp byte ptr [si+18ACh], 1Fh
        jnz short loc_4A53F
        retn
// ; ---------------------------------------------------------------------------

loc_4A53F:              ; CODE XREF: movefun3+B3j
        call    sub_4A61F
        retn
movefun3  endp

// ; ---------------------------------------------------------------------------

loc_4A543:              ; DATA XREF: data:1648o
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4A54B
        retn
// ; ---------------------------------------------------------------------------

loc_4A54B:              ; CODE XREF: code:3928j
        mov ax, word_5195D
        and ax, 3
        cmp ax, 0
        jz  short loc_4A557
        retn
// ; ---------------------------------------------------------------------------

loc_4A557:              ; CODE XREF: code:3934j
        mov bl, [si+1835h]
        test    bl, 80h
        jnz short loc_4A5A0
        inc bl
        mov [si+1835h], bl
        push    si
        push    bx
        xor bh, bh
        dec bl
        mov di, [si+6155h]
        mov si, bx
        shl si, 1
        add si, 12D6h
        mov si, [si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4A582:              ; CODE XREF: code:396Aj
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4A582
        pop ds
        pop bx
        pop si
        cmp bl, 8
        jnz short locret_4A59F
        mov word ptr leveldata[si], 0
        mov byte_510C0, 0

locret_4A59F:               ; CODE XREF: code:3972j
        retn
// ; ---------------------------------------------------------------------------

loc_4A5A0:              ; CODE XREF: code:393Ej
        inc bl
        cmp bl, 89h ; '?'
        jnz short loc_4A5B3
        mov word ptr leveldata[si], 4
        mov byte_510C0, 0
        retn
// ; ---------------------------------------------------------------------------

loc_4A5B3:              ; CODE XREF: code:3985j
        mov [si+1835h], bl
        xor bh, bh
        dec bl
        and bl, 0Fh
        mov di, [si+6155h]
        mov si, bx
        shl si, 1
        add si, 12E6h
        mov si, [si]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4A5D4:              ; CODE XREF: code:39BCj
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4A5D4
        pop ds
        retn

; =============== S U B R O U T I N E =======================================


sub_4A5E0   proc near       ; CODE XREF: runLevel+106p
        mov si, 0
        mov bx, 0
        mov cx, 5A0h

loc_4A5E9:              ; CODE XREF: sub_4A5E0+25j
        cmp byte ptr [bx+2434h], 0
        jz  short loc_4A601
        jl  short loc_4A608
        dec byte ptr [bx+2434h]
        jnz short loc_4A601
        push    si
        push(cx);
        push    bx
        call    sub_4A61F
        pop bx
        pop(cx);
        pop si

loc_4A601:              ; CODE XREF: sub_4A5E0+Ej
                    ; sub_4A5E0+16j ...
        add si, 2
        inc bx
        loop    loc_4A5E9
        retn
// ; ---------------------------------------------------------------------------

loc_4A608:              ; CODE XREF: sub_4A5E0+10j
        inc byte ptr [bx+2434h]
        jnz short loc_4A601
        push    si
        push(cx);
        push    bx
        mov word ptr leveldata[si], 0FF18h
        call    sub_4A61F
        pop bx
        pop(cx);
        pop si
        jmp short loc_4A601
sub_4A5E0   endp


; =============== S U B R O U T I N E =======================================


sub_4A61F   proc near       ; CODE XREF: movefun+271p
                    ; movefun2+20Fp ...
        cmp byte ptr leveldata[si], 6
        jnz short loc_4A627
        retn
// ; ---------------------------------------------------------------------------

loc_4A627:              ; CODE XREF: sub_4A61F+5j
        mov byte_510C0, 1
        cmp byte ptr leveldata[si], 3
        jnz short loc_4A639
        mov word_510D1, 1

loc_4A639:              ; CODE XREF: sub_4A61F+12j
        cmp byte ptr leveldata[si], 18h
        jnz short loc_4A647
        mov cx, 801Fh
        mov dl, 0F3h ; '?'
        jmp short loc_4A64C
// ; ---------------------------------------------------------------------------

loc_4A647:              ; CODE XREF: sub_4A61F+1Fj
        mov cx, 1Fh
        mov dl, 0Dh

loc_4A64C:              ; CODE XREF: sub_4A61F+26j
        mov bx, si
        shr bx, 1
        mov dh, dl
        push(cx);
        mov ax, leveldata[si-78h-2h]
        cmp al, 8
        jz  short loc_4A680
        cmp al, 12h
        jz  short loc_4A680
        cmp al, 11h
        jz  short loc_4A680
        cmp al, 1
        jz  short loc_4A69C
        cmp al, 4
        jz  short loc_4A692
        cmp al, 18h
        jnz short loc_4A676
        neg dh
        mov cx, 801Fh
        jmp short loc_4A680
// ; ---------------------------------------------------------------------------

loc_4A676:              ; CODE XREF: sub_4A61F+4Ej
        cmp al, 3
        jnz short loc_4A688
        mov word_510D1, 1

loc_4A680:              ; CODE XREF: sub_4A61F+3Aj
                    ; sub_4A61F+3Ej ...
        cmp al, 6
        jz  short loc_4A688
        mov [bx+23F7h], dh

loc_4A688:              ; CODE XREF: sub_4A61F+59j
                    ; sub_4A61F+63j
        cmp al, 6
        jz  short loc_4A690
        mov [si+17BAh], cx

loc_4A690:              ; CODE XREF: sub_4A61F+6Bj
        jmp short loc_4A6A6
// ; ---------------------------------------------------------------------------

loc_4A692:              ; CODE XREF: sub_4A61F+4Aj
        push    si
        add si, 0FF86h
        call    sub_4AA34
        pop si
        jmp short loc_4A6A6
// ; ---------------------------------------------------------------------------

loc_4A69C:              ; CODE XREF: sub_4A61F+46j
        push    si
        add si, 0FF86h
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A6A6:              ; CODE XREF: sub_4A61F:loc_4A690j
                    ; sub_4A61F+7Bj ...
        pop(cx);
        mov dh, dl
        push(cx);
        mov ax, [si+17BCh]
        cmp al, 8
        jz  short loc_4A6D7
        cmp al, 12h
        jz  short loc_4A6D7
        cmp al, 11h
        jz  short loc_4A6D7
        cmp al, 1
        jz  short loc_4A6F3
        cmp al, 4
        jz  short loc_4A6E9
        cmp al, 18h
        jnz short loc_4A6CD
        neg dh
        mov cx, 801Fh
        jmp short loc_4A6D7
// ; ---------------------------------------------------------------------------

loc_4A6CD:              ; CODE XREF: sub_4A61F+A5j
        cmp al, 3
        jnz short loc_4A6DF
        mov word_510D1, 1

loc_4A6D7:              ; CODE XREF: sub_4A61F+91j
                    ; sub_4A61F+95j ...
        cmp al, 6
        jz  short loc_4A6DF
        mov [bx+23F8h], dh

loc_4A6DF:              ; CODE XREF: sub_4A61F+B0j
                    ; sub_4A61F+BAj
        cmp al, 6
        jz  short loc_4A6E7
        mov [si+17BCh], cx

loc_4A6E7:              ; CODE XREF: sub_4A61F+C2j
        jmp short loc_4A6FD
// ; ---------------------------------------------------------------------------

loc_4A6E9:              ; CODE XREF: sub_4A61F+A1j
        push    si
        add si, 0FF88h
        call    sub_4AA34
        pop si
        jmp short loc_4A6FD
// ; ---------------------------------------------------------------------------

loc_4A6F3:              ; CODE XREF: sub_4A61F+9Dj
        push    si
        add si, 0FF88h
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A6FD:              ; CODE XREF: sub_4A61F:loc_4A6E7j
                    ; sub_4A61F+D2j ...
        pop(cx);
        mov dh, dl
        push(cx);
        mov ax, [si+17BEh]
        cmp al, 8
        jz  short loc_4A72E
        cmp al, 12h
        jz  short loc_4A72E
        cmp al, 11h
        jz  short loc_4A72E
        cmp al, 1
        jz  short loc_4A74A
        cmp al, 4
        jz  short loc_4A740
        cmp al, 18h
        jnz short loc_4A724
        neg dh
        mov cx, 801Fh
        jmp short loc_4A72E
// ; ---------------------------------------------------------------------------

loc_4A724:              ; CODE XREF: sub_4A61F+FCj
        cmp al, 3
        jnz short loc_4A736
        mov word_510D1, 1

loc_4A72E:              ; CODE XREF: sub_4A61F+E8j
                    ; sub_4A61F+ECj ...
        cmp al, 6
        jz  short loc_4A736
        mov [bx+23F9h], dh

loc_4A736:              ; CODE XREF: sub_4A61F+107j
                    ; sub_4A61F+111j
        cmp al, 6
        jz  short loc_4A73E
        mov [si+17BEh], cx

loc_4A73E:              ; CODE XREF: sub_4A61F+119j
        jmp short loc_4A754
// ; ---------------------------------------------------------------------------

loc_4A740:              ; CODE XREF: sub_4A61F+F8j
        push    si
        add si, 0FF8Ah
        call    sub_4AA34
        pop si
        jmp short loc_4A754
// ; ---------------------------------------------------------------------------

loc_4A74A:              ; CODE XREF: sub_4A61F+F4j
        push    si
        add si, 0FF8Ah
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A754:              ; CODE XREF: sub_4A61F:loc_4A73Ej
                    ; sub_4A61F+129j ...
        pop(cx);
        mov dh, dl
        push(cx);
        mov ax, [si+1832h]
        cmp al, 8
        jz  short loc_4A785
        cmp al, 12h
        jz  short loc_4A785
        cmp al, 11h
        jz  short loc_4A785
        cmp al, 1
        jz  short loc_4A7A1
        cmp al, 4
        jz  short loc_4A797
        cmp al, 18h
        jnz short loc_4A77B
        neg dh
        mov cx, 801Fh
        jmp short loc_4A785
// ; ---------------------------------------------------------------------------

loc_4A77B:              ; CODE XREF: sub_4A61F+153j
        cmp al, 3
        jnz short loc_4A78D
        mov word_510D1, 1

loc_4A785:              ; CODE XREF: sub_4A61F+13Fj
                    ; sub_4A61F+143j ...
        cmp al, 6
        jz  short loc_4A78D
        mov [bx+2433h], dh

loc_4A78D:              ; CODE XREF: sub_4A61F+15Ej
                    ; sub_4A61F+168j
        cmp al, 6
        jz  short loc_4A795
        mov [si+1832h], cx

loc_4A795:              ; CODE XREF: sub_4A61F+170j
        jmp short loc_4A7AB
// ; ---------------------------------------------------------------------------

loc_4A797:              ; CODE XREF: sub_4A61F+14Fj
        push    si
        add si, 0FFFEh
        call    sub_4AA34
        pop si
        jmp short loc_4A7AB
// ; ---------------------------------------------------------------------------

loc_4A7A1:              ; CODE XREF: sub_4A61F+14Bj
        push    si
        add si, 0FFFEh
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A7AB:              ; CODE XREF: sub_4A61F:loc_4A795j
                    ; sub_4A61F+180j ...
        pop(cx);
        mov [si+1834h], cx
        mov dh, dl
        push(cx);
        mov ax, [si+1836h]
        cmp al, 8
        jz  short loc_4A7E0
        cmp al, 12h
        jz  short loc_4A7E0
        cmp al, 11h
        jz  short loc_4A7E0
        cmp al, 1
        jz  short loc_4A7FC
        cmp al, 4
        jz  short loc_4A7F2
        cmp al, 18h
        jnz short loc_4A7D6
        neg dh
        mov cx, 801Fh
        jmp short loc_4A7E0
// ; ---------------------------------------------------------------------------

loc_4A7D6:              ; CODE XREF: sub_4A61F+1AEj
        cmp al, 3
        jnz short loc_4A7E8
        mov word_510D1, 1

loc_4A7E0:              ; CODE XREF: sub_4A61F+19Aj
                    ; sub_4A61F+19Ej ...
        cmp al, 6
        jz  short loc_4A7E8
        mov [bx+2435h], dh

loc_4A7E8:              ; CODE XREF: sub_4A61F+1B9j
                    ; sub_4A61F+1C3j
        cmp al, 6
        jz  short loc_4A7F0
        mov [si+1836h], cx

loc_4A7F0:              ; CODE XREF: sub_4A61F+1CBj
        jmp short loc_4A806
// ; ---------------------------------------------------------------------------

loc_4A7F2:              ; CODE XREF: sub_4A61F+1AAj
        push    si
        add si, 2
        call    sub_4AA34
        pop si
        jmp short loc_4A806
// ; ---------------------------------------------------------------------------

loc_4A7FC:              ; CODE XREF: sub_4A61F+1A6j
        push    si
        add si, 2
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A806:              ; CODE XREF: sub_4A61F:loc_4A7F0j
                    ; sub_4A61F+1DBj ...
        pop(cx);
        mov dh, dl
        push(cx);
        mov ax, [si+18AAh]
        cmp al, 8
        jz  short loc_4A837
        cmp al, 12h
        jz  short loc_4A837
        cmp al, 11h
        jz  short loc_4A837
        cmp al, 1
        jz  short loc_4A853
        cmp al, 4
        jz  short loc_4A849
        cmp al, 18h
        jnz short loc_4A82D
        neg dh
        mov cx, 801Fh
        jmp short loc_4A837
// ; ---------------------------------------------------------------------------

loc_4A82D:              ; CODE XREF: sub_4A61F+205j
        cmp al, 3
        jnz short loc_4A83F
        mov word_510D1, 1

loc_4A837:              ; CODE XREF: sub_4A61F+1F1j
                    ; sub_4A61F+1F5j ...
        cmp al, 6
        jz  short loc_4A83F
        mov [bx+246Fh], dh

loc_4A83F:              ; CODE XREF: sub_4A61F+210j
                    ; sub_4A61F+21Aj
        cmp al, 6
        jz  short loc_4A847
        mov [si+18AAh], cx

loc_4A847:              ; CODE XREF: sub_4A61F+222j
        jmp short loc_4A85D
// ; ---------------------------------------------------------------------------

loc_4A849:              ; CODE XREF: sub_4A61F+201j
        push    si
        add si, 76h ; 'v'
        call    sub_4AA34
        pop si
        jmp short loc_4A85D
// ; ---------------------------------------------------------------------------

loc_4A853:              ; CODE XREF: sub_4A61F+1FDj
        push    si
        add si, 76h ; 'v'
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A85D:              ; CODE XREF: sub_4A61F:loc_4A847j
                    ; sub_4A61F+232j ...
        pop(cx);
        mov dh, dl
        push(cx);
        mov ax, [si+18ACh]
        cmp al, 8
        jz  short loc_4A88E
        cmp al, 12h
        jz  short loc_4A88E
        cmp al, 11h
        jz  short loc_4A88E
        cmp al, 1
        jz  short loc_4A8AA
        cmp al, 4
        jz  short loc_4A8A0
        cmp al, 18h
        jnz short loc_4A884
        neg dh
        mov cx, 801Fh
        jmp short loc_4A88E
// ; ---------------------------------------------------------------------------

loc_4A884:              ; CODE XREF: sub_4A61F+25Cj
        cmp al, 3
        jnz short loc_4A896
        mov word_510D1, 1

loc_4A88E:              ; CODE XREF: sub_4A61F+248j
                    ; sub_4A61F+24Cj ...
        cmp al, 6
        jz  short loc_4A896
        mov [bx+2470h], dh

loc_4A896:              ; CODE XREF: sub_4A61F+267j
                    ; sub_4A61F+271j
        cmp al, 6
        jz  short loc_4A89E
        mov [si+18ACh], cx

loc_4A89E:              ; CODE XREF: sub_4A61F+279j
        jmp short loc_4A8B4
// ; ---------------------------------------------------------------------------

loc_4A8A0:              ; CODE XREF: sub_4A61F+258j
        push    si
        add si, 78h ; 'x'
        call    sub_4AA34
        pop si
        jmp short loc_4A8B4
// ; ---------------------------------------------------------------------------

loc_4A8AA:              ; CODE XREF: sub_4A61F+254j
        push    si
        add si, 78h ; 'x'
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A8B4:              ; CODE XREF: sub_4A61F:loc_4A89Ej
                    ; sub_4A61F+289j ...
        pop(cx);
        mov dh, dl
        push(cx);
        mov ax, [si+18AEh]
        cmp al, 8
        jz  short loc_4A8E5
        cmp al, 12h
        jz  short loc_4A8E5
        cmp al, 11h
        jz  short loc_4A8E5
        cmp al, 1
        jz  short loc_4A901
        cmp al, 4
        jz  short loc_4A8F7
        cmp al, 18h
        jnz short loc_4A8DB
        neg dh
        mov cx, 801Fh
        jmp short loc_4A8E5
// ; ---------------------------------------------------------------------------

loc_4A8DB:              ; CODE XREF: sub_4A61F+2B3j
        cmp al, 3
        jnz short loc_4A8ED
        mov word_510D1, 1

loc_4A8E5:              ; CODE XREF: sub_4A61F+29Fj
                    ; sub_4A61F+2A3j ...
        cmp al, 6
        jz  short loc_4A8ED
        mov [bx+2471h], dh

loc_4A8ED:              ; CODE XREF: sub_4A61F+2BEj
                    ; sub_4A61F+2C8j
        cmp al, 6
        jz  short loc_4A8F5
        mov [si+18AEh], cx

loc_4A8F5:              ; CODE XREF: sub_4A61F+2D0j
        jmp short loc_4A90B
// ; ---------------------------------------------------------------------------

loc_4A8F7:              ; CODE XREF: sub_4A61F+2AFj
        push    si
        add si, 7Ah ; 'z'
        call    sub_4AA34
        pop si
        jmp short loc_4A90B
// ; ---------------------------------------------------------------------------

loc_4A901:              ; CODE XREF: sub_4A61F+2ABj
        push    si
        add si, 7Ah ; 'z'
        call    sub_4A9C4
        pop si
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4A90B:              ; CODE XREF: sub_4A61F:loc_4A8F5j
                    ; sub_4A61F+2E0j ...
        pop(cx);
        call    sound?4
        retn
sub_4A61F   endp


; =============== S U B R O U T I N E =======================================


sub_4A910   proc near       ; CODE XREF: runLevel:noFlashing3p
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        al = byte_510DB
        cmp al, 1
        jle short loc_4A954
        mov si, word_510DC
        cmp word ptr leveldata[si], 0
        jnz short loc_4A932
        mov word ptr leveldata[si], 14h

loc_4A932:              ; CODE XREF: sub_4A910+1Aj
        mov di, [si+6155h]
        mov si, word_5177E
        call    sub_4F200
        inc byte_510DB
        cmp byte_510DB, 28h ; '('
        jl  short loc_4A954
        mov si, word_510DC
        call    sub_4A61F
        mov byte_510DB, 0

loc_4A954:              ; CODE XREF: sub_4A910+Fj
                    ; sub_4A910+36j
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        retn
sub_4A910   endp


; =============== S U B R O U T I N E =======================================


sub_4A95F   proc near       ; CODE XREF: runLevel+372p
                    ; sub_4A3E9+7p ...
        al = byte_510B0
        mov byte_510B4, al
        al = byte_510B1
        mov byte_510B5, al
        al = byte_510B2
        mov byte_510B6, al
        cmp byte_510DE, 0
        jnz short locret_4A97F
        cmp byte_599D4, 0
        jz  short loc_4A980

locret_4A97F:               ; CODE XREF: sub_4A95F+17j
        retn
// ; ---------------------------------------------------------------------------

loc_4A980:              ; CODE XREF: sub_4A95F+1Ej
        mov bh, byte_5981F
        xor bl, bl
        shr bx, 1
        mov si, bx
        add si, 8A9Ch
        al = byte_510B4
        add al, [si+0Bh]

loc_4A994:              ; CODE XREF: sub_4A95F+3Ej
        cmp al, 3Ch ; '<'
        jl  short loc_4A99F
        sub al, 3Ch ; '<'
        inc byte ptr [si+0Ah]
        jmp short loc_4A994
// ; ---------------------------------------------------------------------------

loc_4A99F:              ; CODE XREF: sub_4A95F+37j
        mov [si+0Bh], al
        al = byte_510B5
        add al, [si+0Ah]

loc_4A9A8:              ; CODE XREF: sub_4A95F+52j
        cmp al, 3Ch ; '<'
        jl  short loc_4A9B3
        sub al, 3Ch ; '<'
        inc byte ptr [si+9]
        jmp short loc_4A9A8
// ; ---------------------------------------------------------------------------

loc_4A9B3:              ; CODE XREF: sub_4A95F+4Bj
        mov [si+0Ah], al
        al = byte_510B6
        add al, [si+9]
        jnb short loc_4A9C0
        al = 0FFh

loc_4A9C0:              ; CODE XREF: sub_4A95F+5Dj
        mov [si+9], al
        retn
sub_4A95F   endp


; =============== S U B R O U T I N E =======================================


sub_4A9C4   proc near       ; CODE XREF: sub_4A61F+81p
                    ; sub_4A61F+D8p ...
        mov [si+1834h], cx
        and ah, 0F0h
        cmp ah, 10h
        jz  short loc_4A9EF
        cmp ah, 70h ; 'p'
        jz  short loc_4A9EF
        cmp ah, 20h ; ' '
        jz  short loc_4AA05
        cmp ah, 30h ; '0'
        jz  short loc_4AA12
        cmp ah, 50h ; 'P'
        jz  short loc_4AA1F
        cmp ah, 60h ; '`'
        jz  short loc_4AA26
        cmp ah, 70h ; 'p'
        jz  short loc_4AA2D
        retn
// ; ---------------------------------------------------------------------------

loc_4A9EF:              ; CODE XREF: sub_4A9C4+Aj sub_4A9C4+Fj
        sub si, 78h ; 'x'
        call    sub_4AAB4
        add si, 0F0h ; '?'
        cmp word ptr leveldata[si], 9999h
        jnz short locret_4AA04
        call    sub_4AAB4

locret_4AA04:               ; CODE XREF: sub_4A9C4+3Bj
        retn
// ; ---------------------------------------------------------------------------

loc_4AA05:              ; CODE XREF: sub_4A9C4+14j
        add si, 2
        call    sub_4AAB4
        add si, 76h ; 'v'
        call    sub_4AAB4
        retn
// ; ---------------------------------------------------------------------------

loc_4AA12:              ; CODE XREF: sub_4A9C4+19j
        sub si, 2
        call    sub_4AAB4
        add si, 7Ah ; 'z'
        call    sub_4AAB4
        retn
// ; ---------------------------------------------------------------------------

loc_4AA1F:              ; CODE XREF: sub_4A9C4+1Ej
        sub si, 2
        call    sub_4AAB4
        retn
// ; ---------------------------------------------------------------------------

loc_4AA26:              ; CODE XREF: sub_4A9C4+23j
        add si, 2
        call    sub_4AAB4
        retn
// ; ---------------------------------------------------------------------------

loc_4AA2D:              ; CODE XREF: sub_4A9C4+28j
        add si, 78h ; 'x'
        call    sub_4AAB4
        retn
sub_4A9C4   endp


; =============== S U B R O U T I N E =======================================


sub_4AA34   proc near       ; CODE XREF: sub_4A61F+77p
                    ; sub_4A61F+CEp ...
        mov leveldata[si], cx
        and ah, 0F0h
        cmp ah, 10h
        jz  short loc_4AA5F
        cmp ah, 70h ; 'p'
        jz  short loc_4AA5F
        cmp ah, 20h ; ' '
        jz  short loc_4AA75
        cmp ah, 30h ; '0'
        jz  short loc_4AA8A
        cmp ah, 50h ; 'P'
        jz  short loc_4AA9F
        cmp ah, 60h ; '`'
        jz  short loc_4AAA6
        cmp ah, 70h ; 'p'
        jz  short loc_4AAAD
        retn
// ; ---------------------------------------------------------------------------

loc_4AA5F:              ; CODE XREF: sub_4AA34+Aj sub_4AA34+Fj
        sub si, 78h ; 'x'
        call    sub_4AAB4
        add si, 0F0h ; '?'
        cmp leveldata[si], 9999h
        jnz short locret_4AA74
        call    sub_4AAB4

locret_4AA74:               ; CODE XREF: sub_4AA34+3Bj
        retn
// ; ---------------------------------------------------------------------------

loc_4AA75:              ; CODE XREF: sub_4AA34+14j
        add si, 2
        call    sub_4AAB4
        add si, 76h ; 'v'
        cmp leveldata[si], 9999h
        jnz short locret_4AA89
        call    sub_4AAB4

locret_4AA89:               ; CODE XREF: sub_4AA34+50j
        retn
// ; ---------------------------------------------------------------------------

loc_4AA8A:              ; CODE XREF: sub_4AA34+19j
        sub si, 2
        call    sub_4AAB4
        add si, 7Ah ; 'z'
        cmp word ptr leveldata[si], 9999h
        jnz short locret_4AA9E
        call    sub_4AAB4

locret_4AA9E:               ; CODE XREF: sub_4AA34+65j
        retn
// ; ---------------------------------------------------------------------------

loc_4AA9F:              ; CODE XREF: sub_4AA34+1Ej
        sub si, 2
        call    sub_4AAB4
        retn
// ; ---------------------------------------------------------------------------

loc_4AAA6:              ; CODE XREF: sub_4AA34+23j
        add si, 2
        call    sub_4AAB4
        retn
// ; ---------------------------------------------------------------------------

loc_4AAAD:              ; CODE XREF: sub_4AA34+28j
        add si, 78h ; 'x'
        call    sub_4AAB4
        retn
sub_4AA34   endp


; =============== S U B R O U T I N E =======================================


sub_4AAB4   proc near       ; CODE XREF: sub_4A9C4+2Ep
                    ; sub_4A9C4+3Dp ...
        cmp byte ptr leveldata[si], 1Fh
        jnz short loc_4AABC
        retn
// ; ---------------------------------------------------------------------------

loc_4AABC:              ; CODE XREF: sub_4AAB4+5j
        mov word ptr leveldata[si], 0
        push    si
        push    ds
        mov di, [si+6155h]
        mov si, word_51580
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4AAD3:              ; CODE XREF: sub_4AAB4+27j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4AAD3
        pop ds
        pop si
        retn
sub_4AAB4   endp


; =============== S U B R O U T I N E =======================================


readMenuDat proc near       ; CODE XREF: readEverything+9p
        mov ax, 3D00h
        mov dx, offset aMenu_dat ; "MENU.DAT"
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_4AAED
        jmp exit
// ; ---------------------------------------------------------------------------

loc_4AAED:              ; CODE XREF: readMenuDat+8j
        mov lastFileHandle, ax
        mov bx, lastFileHandle
        push    ds
        mov ax, seg menuseg
        mov ds, ax
        assume ds:nothing
        mov ax, 3F00h
        mov cx, 7D00h
        mov dx, 0
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_4AB0B
        pop ds
        assume ds:data
        jmp exit
// ; ---------------------------------------------------------------------------

loc_4AB0B:              ; CODE XREF: readMenuDat+25j
        pop ds
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short locret_4AB1A
        jmp exit
// ; ---------------------------------------------------------------------------

locret_4AB1A:               ; CODE XREF: readMenuDat+35j
        retn
readMenuDat endp


; =============== S U B R O U T I N E =======================================


sub_4AB1B   proc near       ; CODE XREF: runMainMenu+28Fp
                    ; DATA XREF: data:off_50318o
        cmp byte_59B85, 0
        jnz short loc_4AB4A
        mov si, 8A9Ch
        mov ax, 2D2Dh
        mov bl, 0
        mov cx, 14h

loc_4AB2D:              ; CODE XREF: sub_4AB1B+2Dj
        cmp [si], ax
        jnz short loc_4AB42
        cmp [si+2], ax
        jnz short loc_4AB42
        cmp [si+4], ax
        jnz short loc_4AB42
        cmp [si+6], ax
        jnz short loc_4AB42
        jmp short loc_4AB56
// ; ---------------------------------------------------------------------------

loc_4AB42:              ; CODE XREF: sub_4AB1B+14j
                    ; sub_4AB1B+19j ...
        add si, 80h ; '?'
        inc bl
        loop    loc_4AB2D

loc_4AB4A:              ; CODE XREF: sub_4AB1B+5j
        mov si, 81DBh
        mov di, 89F7h
        mov ah, 6
        call    sub_4BA5F
        retn
// ; ---------------------------------------------------------------------------

loc_4AB56:              ; CODE XREF: sub_4AB1B+25j
        mov byte_59820, bl
        call    sub_4B899
        mov si, 8193h
        mov di, 89F7h
        mov ah, 4
        call    sub_4BA5F
        mov ax, 2020h
        mov si, 820Bh
        mov [si], ax
        mov [si+2], ax
        mov [si+4], ax
        mov [si+6], ax
        mov word_58475, 0

loc_4AB7F:              ; CODE XREF: sub_4AB1B+6Aj
        call    getMouseStatus
        cmp bx, 0
        jnz short loc_4AB7F

noKeyPressed:               ; CODE XREF: sub_4AB1B+79j
                    ; sub_4AB1B+8Aj ...
        call    getMouseStatus
        cmp bx, 0
        jnz short loc_4ABEB
        al = keyPressed
        cmp al, 0
        jz  short noKeyPressed
        mov keyPressed, 0
        mov bl, al
        xor bh, bh
        al = [bx+16FAh]
        cmp al, 0
        jz  short noKeyPressed
        cmp al, 0Ah
        jz  short loc_4ABEB
        cmp al, 8
        jz  short loc_4ABCC
        mov si, 820Bh
        mov bx, word_58475
        cmp bx, 8
        jge short noKeyPressed
        mov [bx+si], al
        inc bx
        mov word_58475, bx
        mov di, 89FFh
        mov ah, 6
        call    sub_4BA5F
        jmp short noKeyPressed
// ; ---------------------------------------------------------------------------

loc_4ABCC:              ; CODE XREF: sub_4AB1B+92j
        mov bx, word_58475
        cmp bx, 0
        jz  short noKeyPressed
        dec bx
        mov si, 820Bh
        al = 20h ; ' '
        mov [bx+si], al
        mov word_58475, bx
        mov di, 89FFh
        mov ah, 6
        call    sub_4BA5F
        jmp short noKeyPressed
// ; ---------------------------------------------------------------------------

loc_4ABEB:              ; CODE XREF: sub_4AB1B+72j
                    ; sub_4AB1B+8Ej ...
        call    getMouseStatus
        cmp bx, 0
        jnz short loc_4ABEB
        mov ax, 2020h
        mov si, 820Bh
        cmp [si], ax
        jnz short loc_4AC1E
        cmp [si+2], ax
        jnz short loc_4AC1E
        cmp [si+4], ax
        jnz short loc_4AC1E
        cmp [si+6], ax
        jnz short loc_4AC1E
        mov si, 8226h
        mov di, 89F7h
        mov ah, 8
        call    sub_4BA5F
        call    sub_4B85C
        call    sub_4B8BE
        retn
// ; ---------------------------------------------------------------------------

loc_4AC1E:              ; CODE XREF: sub_4AB1B+E0j
                    ; sub_4AB1B+E5j ...
        mov ax, 2D2Dh
        cmp [si], ax
        jnz short loc_4AC46
        cmp [si+2], ax
        jnz short loc_4AC46
        cmp [si+4], ax
        jnz short loc_4AC46
        cmp [si+6], ax
        jnz short loc_4AC46
        mov si, 81ABh
        mov di, 89F7h
        mov ah, 6
        call    sub_4BA5F
        call    sub_4B85C
        call    sub_4B8BE
        retn
// ; ---------------------------------------------------------------------------

loc_4AC46:              ; CODE XREF: sub_4AB1B+108j
                    ; sub_4AB1B+10Dj ...
        push    es
        mov ax, ds
        mov es, ax
        assume es:data

loc_4AC4B:              ; CODE XREF: sub_4AB1B+14Cj
        mov si, 820Bh
        cmp byte ptr [si+7], 20h ; ' '
        jnz short loc_4AC69
        mov cx, 7
        add si, 6
        mov di, si
        inc di
        std
        rep movsb
        cld
        mov si, 820Bh
        mov byte ptr [si], 20h ; ' '
        jmp short loc_4AC4B
// ; ---------------------------------------------------------------------------

loc_4AC69:              ; CODE XREF: sub_4AB1B+137j
        pop es
        assume es:nothing
        mov di, 8A9Ch
        mov si, 820Bh
        mov cx, 14h

loc_4AC73:              ; CODE XREF: sub_4AB1B+18Cj
        mov ax, [si]
        cmp ax, [di]
        jnz short loc_4ACA3
        mov ax, [si+2]
        cmp ax, [di+2]
        jnz short loc_4ACA3
        mov ax, [si+4]
        cmp ax, [di+4]
        jnz short loc_4ACA3
        mov ax, [si+6]
        cmp ax, [di+6]
        jnz short loc_4ACA3
        mov si, 81C3h
        mov di, 89F7h
        mov ah, 6
        call    sub_4BA5F
        call    sub_4B85C
        call    sub_4B8BE
        retn
// ; ---------------------------------------------------------------------------

loc_4ACA3:              ; CODE XREF: sub_4AB1B+15Cj
                    ; sub_4AB1B+164j ...
        add di, 80h ; '?'
        loop    loc_4AC73
        mov bh, byte_59820
        mov byte_5981F, bh
        xor bl, bl
        shr bx, 1
        mov di, 8A9Ch
        add di, bx
        mov si, 820Bh
        mov ax, [si]
        mov [di], ax
        add si, 2
        add di, 2
        mov ax, [si]
        mov [di], ax
        add si, 2
        add di, 2
        mov ax, [si]
        mov [di], ax
        add si, 2
        add di, 2
        mov ax, [si]
        mov [di], ax
        add si, 2
        add di, 2
        mov si, 8226h
        mov di, 89F7h
        mov ah, 8
        call    sub_4BA5F
        call    sub_4CFB2
        call    sub_4CFDB
        mov byte_51ABE, 1
        call    sub_4C34A
        call    sub_4C293
        call    sub_4C141
        call    sub_4C0DD
        call    sub_4B85C
        call    sub_4B8BE
        retn
sub_4AB1B   endp


; =============== S U B R O U T I N E =======================================


sub_4AD0E   proc near
        cmp byte_59B85, 0
        jnz short loc_4AD3C
        mov bh, byte_5981F
        xor bl, bl
        shr bx, 1
        mov si, 8A9Ch
        add si, bx
        mov word ptr dword_58477, si
        mov ax, 2D2Dh
        cmp ax, [si]
        jnz short loc_4AD48
        cmp ax, [si+2]
        jnz short loc_4AD48
        cmp ax, [si+4]
        jnz short loc_4AD48
        cmp ax, [si+6]
        jnz short loc_4AD48

loc_4AD3C:              ; CODE XREF: sub_4AD0E+5j
        mov si, 82B6h
        mov di, 89F7h
        mov ah, 8
        call    sub_4BA5F
        retn
// ; ---------------------------------------------------------------------------

loc_4AD48:              ; CODE XREF: sub_4AD0E+1Dj
                    ; sub_4AD0E+22j ...
        mov di, 828Eh
        mov ax, [si]
        mov [di], ax
        mov ax, [si+2]
        mov [di+2], ax
        mov ax, [si+4]
        mov [di+4], ax
        mov ax, [si+6]
        mov [di+6], ax
        mov si, 8286h
        mov di, 89F7h
        mov ah, 8
        call    sub_4BA5F

loc_4AD6C:              ; CODE XREF: sub_4AD0E+64j
        call    getMouseStatus
        cmp bx, 0
        jnz short loc_4AD6C

loc_4AD74:              ; CODE XREF: sub_4AD0E+88j
        call    videoloop
        call    getMouseStatus
        mov word_5847D, bx
        mov mousex, cx
        mov mousey, dx
        call    sub_4B899
        call    sub_4B85C
        call    sub_4B8BE
        mov bx, word_5847D
        cmp bx, 0
        jz  short loc_4AD74
        mov cx, mousex
        mov dx, mousey
        mov si, 5Ah ; 'Z'
        cmp [si], cx
        jg  short loc_4ADCE
        cmp [si+2], dx
        jg  short loc_4ADCE
        cmp [si+4], cx
        jl  short loc_4ADCE
        cmp [si+6], dx
        jl  short loc_4ADCE
        mov di, word ptr dword_58477
        push    es
        mov ax, ds
        mov es, ax
        assume es:data
        mov cx, 8
        al = 2Dh ; '-'
        rep stosb
        al = 0
        mov cx, 78h ; 'x'
        rep stosb
        pop es
        assume es:nothing

loc_4ADCE:              ; CODE XREF: sub_4AD0E+97j
                    ; sub_4AD0E+9Cj ...
        call    sub_4B899
        mov si, 8226h
        mov di, 89F7h
        mov ah, 8
        call    sub_4BA5F
        call    sub_4CFB2
        call    sub_4CFDB
        mov byte_51ABE, 1
        call    sub_4C34A
        call    sub_4C293
        call    sub_4C141
        call    sub_4C0DD

loc_4ADF3:              ; CODE XREF: sub_4AD0E+EBj
        call    getMouseStatus
        cmp bx, 0
        jnz short loc_4ADF3
        call    sub_4B85C
        retn
sub_4AD0E   endp


; =============== S U B R O U T I N E =======================================


sub_4ADFF   proc near
        mov bh, byte_5981F
        xor bl, bl
        shr bx, 1
        mov si, 8A9Ch
        add si, bx
        mov ax, 2D2Dh
        cmp ax, [si]
        jnz short loc_4AE2E
        cmp ax, [si+2]
        jnz short loc_4AE2E
        cmp ax, [si+4]
        jnz short loc_4AE2E
        cmp ax, [si+6]
        jnz short loc_4AE2E
        mov si, 82B6h
        mov di, 89F7h
        mov ah, 8
        call    sub_4BA5F
        retn
// ; ---------------------------------------------------------------------------

loc_4AE2E:              ; CODE XREF: sub_4ADFF+12j
                    ; sub_4ADFF+17j ...
        add si, 0Ch
        mov cx, 6Fh ; 'o'
        al = 2
        mov bl, 0

loc_4AE38:              ; CODE XREF: sub_4ADFF+40j
        cmp al, [si]
        jnz short loc_4AE3E
        inc bl

loc_4AE3E:              ; CODE XREF: sub_4ADFF+3Bj
        inc si
        loop    loc_4AE38
        cmp word_51970, 0
        jz  short loc_4AE4A
        jmp short loc_4AE5B
// ; ---------------------------------------------------------------------------

loc_4AE4A:              ; CODE XREF: sub_4ADFF+47j
        cmp bl, 3
        jl  short loc_4AE5B
        mov si, 826Eh
        mov di, 89F7h
        mov ah, 6
        call    sub_4BA5F
        retn
// ; ---------------------------------------------------------------------------

loc_4AE5B:              ; CODE XREF: sub_4ADFF+49j
                    ; sub_4ADFF+4Ej
        mov ax, word_51ABC
        dec ax
        mov si, 949Eh
        add si, ax
        cmp byte ptr [si], 2
        jz  short loc_4AE75
        mov si, 82CEh
        mov di, 89F7h
        mov ah, 4
        call    sub_4BA5F
        retn
// ; ---------------------------------------------------------------------------

loc_4AE75:              ; CODE XREF: sub_4ADFF+68j
        mov si, 82A9h
        mov ax, word_51ABC
        call    sub_4BF4D
        mov si, 829Eh
        mov di, 89F7h
        mov ah, 8
        call    sub_4BA5F

loc_4AE89:              ; CODE XREF: sub_4ADFF+90j
        call    getMouseStatus
        cmp bx, 0
        jnz short loc_4AE89

loc_4AE91:              ; CODE XREF: sub_4ADFF+B4j
        call    videoloop
        call    getMouseStatus
        mov word_5847D, bx
        mov mousex, cx
        mov mousey, dx
        call    sub_4B899
        call    sub_4B85C
        call    sub_4B8BE
        mov bx, word_5847D
        cmp bx, 0
        jz  short loc_4AE91
        mov cx, mousex
        mov dx, mousey
        mov si, 5Ah ; 'Z'
        cmp [si], cx
        jg  short loc_4AEE9
        cmp [si+2], dx
        jg  short loc_4AEE9
        cmp [si+4], cx
        jl  short loc_4AEE9
        cmp [si+6], dx
        jl  short loc_4AEE9
        mov byte_510BB, 2
        call    sub_4D24D
        call    sub_4CFB2
        call    sub_4CFDB
        mov byte_51ABE, 0
        call    sub_4C34A

loc_4AEE9:              ; CODE XREF: sub_4ADFF+C3j
                    ; sub_4ADFF+C8j ...
        call    sub_4B899
        mov si, 8226h
        mov di, 89F7h
        mov ah, 8
        call    sub_4BA5F
        call    sub_4C293
        call    sub_4C141
        call    sub_4C0DD

loc_4AF00:              ; CODE XREF: sub_4ADFF+107j
        call    getMouseStatus
        cmp bx, 0
        jnz short loc_4AF00
        call    sub_4B85C
        retn
sub_4ADFF   endp


; =============== S U B R O U T I N E =======================================


sub_4AF0C   proc near
        mov bh, byte_5981F
        xor bl, bl
        shr bx, 1
        add bx, 8A9Ch
        mov bp, bx
        mov si, bx
        mov ax, 2D2Dh
        cmp ax, [si]
        jnz short loc_4AF3E
        cmp ax, [si+2]
        jnz short loc_4AF3E
        cmp ax, [si+4]
        jnz short loc_4AF3E
        cmp ax, [si+6]
        jnz short loc_4AF3E
        mov si, 82B6h
        mov di, 89F7h
        mov ah, 8
        call    sub_4BA5F
        retn
// ; ---------------------------------------------------------------------------

loc_4AF3E:              ; CODE XREF: sub_4AF0C+15j
                    ; sub_4AF0C+1Aj ...
        mov ax, 5F5Fh
        mov word_586FB, ax
        mov word_586FD, ax
        mov word_586FF, ax
        mov word_58701, ax
        mov word_58703, ax
        mov word_5870D, ax
        mov byte_5870F, al
        mov word_58710, ax
        mov word_58712, ax
        mov word_58714, ax
        cmp word_51970, 0
        jz  short loc_4AFE3
        al = 0Ah
        sub al, gameSpeed
        aam
        or  ax, 3030h
        xchg    al, ah
        mov word_586FB, 3C20h
        mov word_586FD, 4C44h
        mov word_586FF, 3A59h
        mov word_58701, ax
        mov word_58703, 203Eh
        mov word_5870D, 3C20h
        mov word_58712, 5A48h
        mov word_58714, 203Eh
        mov byte_59B94, 0
        mov byte_59B96, 0

loc_4AFAA:              ; CODE XREF: sub_4AF0C+A3j
        test    byte_59B96, 0FFh
        jz  short loc_4AFAA
        mov byte_59B96, 0

loc_4AFB6:              ; CODE XREF: sub_4AF0C+B9j
        call    videoloop
        call    sub_4D457
        inc byte_59B94
        cmp byte_59B96, 32h ; '2'
        jb  short loc_4AFB6
        al = byte_59B94
        xor ah, ah
        push(cx);
        mov cl, 64h ; 'd'
        div cl
        pop(cx);
        or  al, 30h
        mov byte_5870F, al
        al = ah
        aam
        or  ax, 3030h
        xchg    al, ah
        mov word_58710, ax

loc_4AFE3:              ; CODE XREF: sub_4AF0C+58j
        mov si, 60D5h
        call    fade
        call    vgaloadbackseg
        mov byte_5091A, 0
        mov si, 83A4h
        mov di, 5716h
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, 83BFh
        mov di, 6560h
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, 83E0h
        mov di, 6A1Eh
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, 8411h
        mov di, 73AEh
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, bp
        mov di, 843Eh
        mov cx, 8

loc_4B025:              ; CODE XREF: sub_4AF0C+11Fj
        al = [si]
        mov [di], al
        inc si
        inc di
        loop    loc_4B025
        mov si, 842Ch
        mov di, 7D36h
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, bp
        al = [si+7Eh]
        cmp al, 71h ; 'q'
        jnz short loc_4B046
        mov byte_5091A, 1

loc_4B046:              ; CODE XREF: sub_4AF0C+133j
        mov si, 845Eh
        call    sub_4BF4D
        mov si, 8447h
        mov di, 81FAh
        mov ah, 0Fh
        call    sub_4BDF0
        mov di, bp
        al = [di+0Bh]
        mov si, 8479h
        call    sub_4BF4D
        mov si, 8476h
        mov byte ptr [si+3], 3Ah ; ':'
        al = [di+0Ah]
        call    sub_4BF4D
        mov si, 8473h
        mov byte ptr [si+3], 3Ah ; ':'
        al = [di+9]
        mov ah, 20h ; ' '
        call    sub_4BF4F
        mov si, 8462h
        mov di, 86BEh
        mov ah, 0Fh
        call    sub_4BDF0
        mov di, bp
        al = [di+9]
        mov bl, 3Ch ; '<'
        mul bl
        add al, [di+0Ah]
        adc ah, 0
        mov bl, [di+0Bh]
        cmp bl, 1Eh
        jl  short loc_4B0A1
        inc ax

loc_4B0A1:              ; CODE XREF: sub_4AF0C+192j
        mov bl, [di+7Eh]
        div bl
        push    ax
        al = ah
        mov bl, 0Ah
        mul bl
        mov bl, [di+7Eh]
        div bl
        mov si, 849Ch
        call    sub_4BF4D
        pop ax
        cmp al, 0
        jnz short loc_4B0C2
        mov byte_5091A, 2

loc_4B0C2:              ; CODE XREF: sub_4AF0C+1AFj
        mov si, 849Ah
        mov byte ptr [si+3], 2Eh ; '.'
        mov ah, 20h ; ' '
        call    sub_4BF4F
        cmp byte_5091A, 1
        jnz short loc_4B0E2
        mov si, 84CFh
        mov di, 903Fh
        mov ah, 0Fh
        call    sub_4BDF0
        jmp short loc_4B105
// ; ---------------------------------------------------------------------------

loc_4B0E2:              ; CODE XREF: sub_4AF0C+1C7j
        cmp byte_5091A, 2
        jnz short loc_4B0F6
        mov si, 84A8h
        mov di, 9041h
        mov ah, 0Fh
        call    sub_4BDF0
        jmp short loc_4B105
// ; ---------------------------------------------------------------------------

loc_4B0F6:              ; CODE XREF: sub_4AF0C+1DBj
        mov si, 847Dh
        mov byte ptr [si+1Ch], 20h ; ' '
        mov di, 9040h
        mov ah, 0Fh
        call    sub_4BDF0

loc_4B105:              ; CODE XREF: sub_4AF0C+1D4j
                    ; sub_4AF0C+1E8j
        mov bx, 4D84h
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        mov si, 5FD5h
        call    fade
        call    sub_47E98
        mov si, 60D5h
        call    fade
        mov bx, 4D5Ch
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        mov si, 6015h
        call    fade
        retn
sub_4AF0C   endp


; =============== S U B R O U T I N E =======================================


sub_4B149   proc near
        call    vgaloadgfxseg
        call    sub_4C5AF
        call    sub_47E98
        call    loc_4C44F
        call    sub_4C2F2
        retn
sub_4B149   endp


; =============== S U B R O U T I N E =======================================


sub_4B159   proc near       ; CODE XREF: runMainMenu+6Fp
        call    readDemo
        or  cx, cx
        jnz short loc_4B163
        jmp locret_4B1F1
// ; ---------------------------------------------------------------------------

loc_4B163:              ; CODE XREF: sub_4B159+5j
        mov word_5196C, 1
        mov byte_510DE, 1
        push    es
        mov ax, seg demoseg
        mov es, ax
        assume es:demoseg
        mov bx, 0
        mov cx, 0

loc_4B17A:              ; CODE XREF: sub_4B159+2Dj
        mov ax, es:[bx]
        add bx, 2
        cmp ax, 0FFFFh
        jz  short loc_4B188
        inc cx
        jmp short loc_4B17A
// ; ---------------------------------------------------------------------------

loc_4B188:              ; CODE XREF: sub_4B159+2Aj
        push(cx);
        call    getTime
        call    sub_4A1AE
        xor dx, dx
        pop(cx);
        div cx
        mov bx, 0
        shl dx, 1
        add bx, dx
        mov bx, es:[bx]
        cmp bx, 0FFFFh
        jnz short loc_4B1AE
        mov word_5196C, 0
        mov byte_510DE, 0

loc_4B1AE:              ; CODE XREF: sub_4B159+48j
        al = es:[bx]
        xor ah, ah
        push    bx
        mov bx, dx
        shr dx, 1
        mov word_599D6, dx
        mov word_599D8, 0
        cmp al, 6Fh ; 'o'
        ja  short loc_4B1CF
        or  al, al
        jz  short loc_4B1CF
        mov byte ptr word_599D8, al
        mov dl, al

loc_4B1CF:              ; CODE XREF: sub_4B159+6Bj
                    ; sub_4B159+6Fj
        al = dl
        mov bx, [bx-67CAh]
        mov timeOfDay, bx
        pop bx
        mov word_510E6, ax
        inc bx
        pop es
        assume es:nothing
        mov word_510DF, bx
        mov word_5A33C, bx
        mov byte_510E1, 0
        mov byte_510E2, 1

locret_4B1F1:               ; CODE XREF: sub_4B159+7j
        retn
sub_4B159   endp


; =============== S U B R O U T I N E =======================================


demoSomething?  proc near       ; CODE XREF: start+3BAp
                    ; runMainMenu+12Ep ...
        push    ax
        call    readDemo
        pop ax
        push    ax
        mov bx, ax
        shl bx, 1
        mov ax, [bx-67CAh]
        mov timeOfDay, ax
        pop ax
        mov word_5196C, 1
        mov byte_510DE, 1
        push    es
        mov dx, seg demoseg
        mov es, dx
        assume es:demoseg
        mov bx, 0
        shl ax, 1
        add bx, ax
        mov bx, es:[bx]
        cmp bx, 0FFFFh
        jnz short loc_4B22F
        mov word_5196C, 0
        mov byte_510DE, 0

loc_4B22F:              ; CODE XREF: demoSomething?+30j
        mov word_599D8, 0
        shr al, 1
        mov ah, es:[bx]
        cmp ah, 6Fh ; 'o'
        ja  short loc_4B248
        or  ah, ah
        jz  short loc_4B248
        al = ah
        mov byte ptr word_599D8, al

loc_4B248:              ; CODE XREF: demoSomething?+4Bj
                    ; demoSomething?+4Fj
        xor ah, ah
        mov word_510E6, ax
        inc bx
        pop es
        assume es:nothing
        mov word_510DF, bx
        mov word_5A33C, bx
        mov byte_510E1, 0
        mov byte_510E2, 1
        retn
demoSomething?  endp

// ; ---------------------------------------------------------------------------
loc_4B262:
        mov byte_50915, 1
        mov byte_50913, 0
        mov byte_50914, 1
        mov ax, word_5195D
        sub ax, word_58471
        cmp ax, word_58473
        jnb short loc_4B27F
        retn
// ; ---------------------------------------------------------------------------

loc_4B27F:              ; CODE XREF: code:465Cj
        call    sub_4B899
        mov ax, word_5195D
        mov word_58473, ax
        cmp word_58471, 1
        jbe short loc_4B293
        dec word_58471

loc_4B293:              ; CODE XREF: code:466Dj
        cmp byte_59B85, 0
        jnz short loc_4B2A5
        cmp byte_58D46, 0
        jbe short loc_4B2A5
        dec byte_58D46

loc_4B2A5:              ; CODE XREF: code:4678j code:467Fj
        call    sub_4C0DD
        call    sub_4B85C
        call    sub_4B8BE
        retn
// ; ---------------------------------------------------------------------------
loc_4B2AF:
        mov byte_50915, 1
        mov byte_50913, 1
        mov byte_50914, 0
        mov ax, word_5195D
        sub ax, word_58471
        cmp ax, word_58473
        jnb short loc_4B2CC
        retn
// ; ---------------------------------------------------------------------------

loc_4B2CC:              ; CODE XREF: code:46A9j
        call    sub_4B899
        mov ax, word_5195D
        mov word_58473, ax
        cmp word_58471, 1
        jbe short loc_4B2E0
        dec word_58471

loc_4B2E0:              ; CODE XREF: code:46BAj
        cmp byte_59B85, 0
        jnz short loc_4B2F2
        cmp byte_58D46, 13h
        jnb short loc_4B2F2
        inc byte_58D46

loc_4B2F2:              ; CODE XREF: code:46C5j code:46CCj
        call    sub_4C0DD
        call    sub_4B85C
        call    sub_4B8BE
        retn

; =============== S U B R O U T I N E =======================================


sub_4B2FC   proc near       ; CODE XREF: sub_4B375+56p
        mov si, 60D5h
        call    fade
        call    vgaloadbackseg
        mov si, 8718h
        mov ah, 0Fh
        mov di, 5BDFh
        call    sub_4BDF0
        mov si, 8728h
        mov ah, 0Fh
        mov di, 6EE3h
        call    sub_4BDF0
        mov si, 8756h
        mov ah, 0Fh
        mov di, 760Eh
        call    sub_4BDF0
        mov si, 8777h
        mov ah, 0Fh
        mov di, 7D31h
        call    sub_4BDF0
        mov bx, 4D84h
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        mov si, 5FD5h
        call    fade
        call    sub_47E98
        mov si, 60D5h
        call    fade
        mov bx, 4D5Ch
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        mov si, 6015h
        call    fade
        retn
sub_4B2FC   endp


; =============== S U B R O U T I N E =======================================


sub_4B375   proc near       ; CODE XREF: runMainMenu+11Ep
        mov bh, byte_5981F
        xor bl, bl
        shr bx, 1
        mov si, 8A9Ch
        add si, bx
        mov ax, 2D2Dh
        cmp [si], ax
        jnz short loc_4B3A4
        cmp [si+2], ax
        jnz short loc_4B3A4
        cmp [si+4], ax
        jnz short loc_4B3A4
        cmp [si+6], ax
        jnz short loc_4B3A4
        mov si, 82B6h
        mov di, 89F7h
        mov ah, 8
        call    sub_4BA5F
        retn
// ; ---------------------------------------------------------------------------

loc_4B3A4:              ; CODE XREF: sub_4B375+12j
                    ; sub_4B375+17j ...
        mov ax, word_51ABC
        cmp ax, 6Fh ; 'o'
        jle short loc_4B3DB
        cmp word_51ABC, 70h ; 'p'
        jnz short loc_4B3B4
        retn
// ; ---------------------------------------------------------------------------

loc_4B3B4:              ; CODE XREF: sub_4B375+3Cj
        add si, 0Ch
        mov cx, 6Fh ; 'o'
        mov bl, 0

loc_4B3BC:              ; CODE XREF: sub_4B375+4Fj
        cmp byte ptr [si], 1
        jnz short loc_4B3C3
        inc bl

loc_4B3C3:              ; CODE XREF: sub_4B375+4Aj
        inc si
        loop    loc_4B3BC
        cmp bl, 6Fh ; 'o'
        jnz short loc_4B3CF
        call    sub_4B2FC
        retn
// ; ---------------------------------------------------------------------------

loc_4B3CF:              ; CODE XREF: sub_4B375+54j
        mov si, 82CEh
        mov di, 89F7h
        mov ah, 2
        call    sub_4BA5F
        retn
// ; ---------------------------------------------------------------------------

loc_4B3DB:              ; CODE XREF: sub_4B375+35j
        dec ax
        mov si, 949Eh
        add si, ax
        al = [si]
        cmp al, 6
        jz  short loc_4B404
        mov word_5196C, 1
        mov byte_510DE, 0
        cmp al, 4
        jnz short loc_4B3FD
        mov byte_510B3, 0
        jmp short loc_4B40F
// ; ---------------------------------------------------------------------------

loc_4B3FD:              ; CODE XREF: sub_4B375+7Fj
        mov byte_510B3, 1
        jmp short loc_4B40F
// ; ---------------------------------------------------------------------------

loc_4B404:              ; CODE XREF: sub_4B375+70j
        mov si, 82CEh
        mov di, 89F7h
        mov ah, 8
        call    sub_4BA5F

loc_4B40F:              ; CODE XREF: sub_4B375+86j
                    ; sub_4B375+8Dj
        call    sub_49544
        mov ax, word_51ABC
        call    sub_4BF4A
        retn
sub_4B375   endp


; =============== S U B R O U T I N E =======================================


sub_4B419   proc near
        xor al, al
        cmp byte_519B5, al
        jz  short loc_4B423
        not al

loc_4B423:              ; CODE XREF: sub_4B419+6j
        cmp byte ptr word_59B65, 0
        jz  short loc_4B42C
        not al

loc_4B42C:              ; CODE XREF: sub_4B419+Fj
        al++;
        jnz short loc_4B433
        jmp loc_4B583
// ; ---------------------------------------------------------------------------

loc_4B433:              ; CODE XREF: sub_4B419+15j
        mov ax, word_5195D
        sub ax, word_59B8C
        cmp ax, word_59B8E
        jnb short loc_4B443
        jmp locret_4B582
// ; ---------------------------------------------------------------------------

loc_4B443:              ; CODE XREF: sub_4B419+25j
        mov ax, word_5195D
        mov word_59B8E, ax
        cmp word_59B8C, 1
        jbe short loc_4B454
        dec word_59B8C

loc_4B454:              ; CODE XREF: sub_4B419+35j
                    ; sub_4B419+9Aj
        mov ax, word ptr aLevels_dat_0+8 ; "AT"
        mov dl, byte ptr word_519B3
        or  dl, byte ptr word_519A7
        jnz short loc_4B482
        cmp ax, 5441h
        jnz short loc_4B46B
        mov ax, 3030h
        jmp short loc_4B4A3
// ; ---------------------------------------------------------------------------

loc_4B46B:              ; CODE XREF: sub_4B419+4Bj
        cmp ax, 3939h
        jnz short loc_4B475
        mov ax, 5441h
        jmp short loc_4B4A3
// ; ---------------------------------------------------------------------------

loc_4B475:              ; CODE XREF: sub_4B419+55j
        inc ah
        cmp ah, 39h ; '9'
        jbe short loc_4B4A3
        mov ah, 30h ; '0'
        al++;
        jmp short loc_4B4A3
// ; ---------------------------------------------------------------------------

loc_4B482:              ; CODE XREF: sub_4B419+46j
        cmp ax, 5441h
        jnz short loc_4B48C
        mov ax, 3939h
        jmp short loc_4B4A3
// ; ---------------------------------------------------------------------------

loc_4B48C:              ; CODE XREF: sub_4B419+6Cj
        cmp ax, 3030h
        jnz short loc_4B496
        mov ax, 5441h
        jmp short loc_4B4A3
// ; ---------------------------------------------------------------------------

loc_4B496:              ; CODE XREF: sub_4B419+76j
        dec ah
        cmp ah, 30h ; '0'
        jnb short loc_4B4A3
        mov ah, 39h ; '9'
        dec al
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4B4A3:              ; CODE XREF: sub_4B419+50j
                    ; sub_4B419+5Aj ...
        mov word ptr aLevels_dat_0+8, ax ; "AT"
        mov ax, 3D00h
        mov dx, 17AFh
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_4B4B8
        cmp ax, 2
        jz  short loc_4B454
        jmp exit
// ; ---------------------------------------------------------------------------

loc_4B4B8:              ; CODE XREF: sub_4B419+95j
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short loc_4B4C6
        jmp exit
// ; ---------------------------------------------------------------------------

loc_4B4C6:              ; CODE XREF: sub_4B419+A8j
        mov ax, word ptr aLevels_dat_0+8 ; "AT"
        mov word ptr aLevelSet??+0Fh, ax ; "??  "
        cmp ah, 54h ; 'T'
        jnz short loc_4B4D3
        al = 53h ; 'S'

loc_4B4D3:              ; CODE XREF: sub_4B419+B6j
        mov word ptr aLevel_lst+7, ax ; "ST"
        mov word ptr aPlayer_lst+8, ax ; "ST"
        mov word ptr aHallfame_lst+0Ah, ax ; "ST"
        cmp ah, 54h ; 'T'
        jnz short loc_4B4E4
        mov ax, 4E49h

loc_4B4E4:              ; CODE XREF: sub_4B419+C6j
        mov word ptr aDemo0_bin+7, ax ; "IN"
        cmp ah, 4Eh ; 'N'
        jnz short loc_4B4EF
        mov ax, 5641h

loc_4B4EF:              ; CODE XREF: sub_4B419+D1j
        cmp byte ptr dword_59B76, 0
        jnz short loc_4B4F9
        mov word ptr aSavegame_sav+0Ah, ax ; "AV"

loc_4B4F9:              ; CODE XREF: sub_4B419+DBj
        mov si, 9EEDh
        cmp ah, 56h ; 'V'
        jnz short loc_4B504
        mov si, 9F05h

loc_4B504:              ; CODE XREF: sub_4B419+E6j
        mov di, 89F7h
        mov ah, 4
        call    sub_4BA5F
        call    readLevelsLst
        call    readDemo
        push    es
        push    ds
        pop es
        assume es:data
        cmp byte_59B85, 0
        jz  short loc_4B52A
        lea di, byte_58DB4+4
        mov cx, 6Fh ; 'o'
        al = 2
        rep stosb
        pop es
        assume es:nothing
        jmp short loc_4B565
// ; ---------------------------------------------------------------------------

loc_4B52A:              ; CODE XREF: sub_4B419+101j
        lea di, word_58DAC
        mov cx, 14h

loc_4B531:              ; CODE XREF: sub_4B419+129j
        push(cx);
        mov ax, 2D2Dh
        mov cx, 4
        rep stosw
        xor ax, ax
        mov cx, 3Ch ; '<'
        rep stosw
        pop(cx);
        loop    loc_4B531
        lea di, asc_59824   ; "    "
        mov cx, 3

loc_4B54B:              ; CODE XREF: sub_4B419+143j
        push(cx);
        mov ax, 2020h
        mov cx, 4
        rep stosw
        xor ax, ax
        mov cx, 2
        rep stosw
        pop(cx);
        loop    loc_4B54B
        pop es
        call    readHallfameLst
        call    readPlayersLst

loc_4B565:              ; CODE XREF: sub_4B419+10Fj
        mov byte_51ABE, 1
        call    sub_4C34A
        call    sub_4C293
        call    sub_4C141
        call    sub_4C1A9
        call    sub_4C0DD
        call    sub_4B899
        call    sub_4B85C
        call    sub_4B8BE

locret_4B582:               ; CODE XREF: sub_4B419+27j
        retn
// ; ---------------------------------------------------------------------------

loc_4B583:              ; CODE XREF: sub_4B419+17j
        mov si, 60D5h
        call    fade
        call    vgaloadbackseg
        mov si, 84FFh
        mov di, 786Dh
        mov ah, 0Fh
        call    sub_4BDF0
        mov ah, 19h
        int 21h     ; DOS - GET DEFAULT DISK NUMBER
        mov byte_59B9A, al
        cmp al, 0
        jz  short loc_4B5B3
        cmp al, 1
        jz  short loc_4B5B3
        mov si, 851Bh
        mov di, 81F5h
        mov ah, 0Fh
        call    sub_4BDF0
        jmp short loc_4B5BE
// ; ---------------------------------------------------------------------------

loc_4B5B3:              ; CODE XREF: sub_4B419+187j
                    ; sub_4B419+18Bj
        mov si, 853Ah
        mov di, 81F5h
        mov ah, 0Fh
        call    sub_4BDF0

loc_4B5BE:              ; CODE XREF: sub_4B419+198j
        mov si, 8562h
        mov di, 0A81Ah
        mov ah, 0Fh
        call    sub_4BDF0
        mov bx, 4D84h
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        mov si, 5FD5h
        call    fade

loc_4B5E6:              ; CODE XREF: sub_4B419+1D3j
        call    getMouseStatus
        cmp bx, 0
        jnz short loc_4B5E6

loc_4B5EE:              ; CODE XREF: sub_4B419+20Fj
        cmp word_5197A, 1
        jz  short loc_4B63D
        al = keyPressed
        xor dl, dl
        cmp al, 1Eh     ; A
        jz  short loc_4B62A
        inc dl
        cmp al, 30h ; '0'   ; B
        jz  short loc_4B62A
        inc dl
        cmp al, 2Eh ; '.'   ; C
        jz  short loc_4B62A
        inc dl
        cmp al, 20h ; ' '   ; D
        jz  short loc_4B62A
        inc dl
        cmp al, 12h     ; E
        jz  short loc_4B62A
        inc dl
        cmp al, 21h ; '!'   ; F
        jz  short loc_4B62A
        mov dl, byte_59B9A
        cmp al, 1Ch     ; Enter
        jz  short loc_4B62A
        cmp al, 1       ; Esc
        jz  short loc_4B63D
        jmp short loc_4B5EE
// ; ---------------------------------------------------------------------------

loc_4B62A:              ; CODE XREF: sub_4B419+1E3j
                    ; sub_4B419+1E9j ...
        mov ah, 0Eh
        int 21h     ; DOS - SELECT DISK
                    ; DL = new default drive number (0 = A, 1 = B, etc.)
                    ; Return: AL = number of logical drives
        mov byte_59B86, 0
        call    readMoving
        cmp byte_59B86, 0FFh
        jnz short loc_4B647

loc_4B63D:              ; CODE XREF: sub_4B419+1DAj
                    ; sub_4B419+20Dj
        mov dl, byte_59B9A
        mov ah, 0Eh
        int 21h     ; DOS - SELECT DISK
                    ; DL = new default drive number (0 = A, 1 = B, etc.)
                    ; Return: AL = number of logical drives
        jmp short loc_4B64D
// ; ---------------------------------------------------------------------------

loc_4B647:              ; CODE XREF: sub_4B419+222j
        call    readEverything
        call    vgaloadbackseg

loc_4B64D:              ; CODE XREF: sub_4B419+22Cj
        mov si, 60D5h
        call    fade
        mov bx, 4D5Ch
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        mov si, 6015h
        call    fade
        retn
sub_4B419   endp


; =============== S U B R O U T I N E =======================================


sub_4B671   proc near
        mov byte_50912, 1
        mov byte_50910, 1
        mov byte_50911, 0
        mov ax, word_5195D
        sub ax, word_5846D
        cmp ax, word_5846F
        jnb short loc_4B68E
        retn
// ; ---------------------------------------------------------------------------

loc_4B68E:              ; CODE XREF: sub_4B671+1Aj
        mov ax, word_5195D
        mov word_5846F, ax
        cmp word_5846D, 1
        jbe short loc_4B69F
        dec word_5846D

loc_4B69F:              ; CODE XREF: sub_4B671+28j
        cmp byte_59B85, 0
        jnz short loc_4B6B1
        cmp byte_5981F, 13h
        jnb short loc_4B6B1
        inc byte_5981F

loc_4B6B1:              ; CODE XREF: sub_4B671+33j
                    ; sub_4B671+3Aj
        call    sub_4B899
        mov byte_51ABE, 1
        call    sub_4C34A
        call    sub_4C293
        call    sub_4C141
        call    sub_4B85C
        call    sub_4B8BE
        retn
sub_4B671   endp


; =============== S U B R O U T I N E =======================================


sub_4B6C9   proc near
        mov byte_50912, 1
        mov byte_50910, 0
        mov byte_50911, 1
        mov ax, word_5195D
        sub ax, word_5846D
        cmp ax, word_5846F
        jnb short loc_4B6E6
        retn
// ; ---------------------------------------------------------------------------

loc_4B6E6:              ; CODE XREF: sub_4B6C9+1Aj
        mov ax, word_5195D
        mov word_5846F, ax
        cmp word_5846D, 1
        jbe short loc_4B6F7
        dec word_5846D

loc_4B6F7:              ; CODE XREF: sub_4B6C9+28j
        cmp byte_59B85, 0
        jnz short loc_4B709
        cmp byte_5981F, 0
        jbe short loc_4B709
        dec byte_5981F

loc_4B709:              ; CODE XREF: sub_4B6C9+33j
                    ; sub_4B6C9+3Aj
        call    sub_4B899
        mov byte_51ABE, 1
        call    sub_4C34A
        call    sub_4C293
        call    sub_4C141
        call    sub_4B85C
        call    sub_4B8BE
        retn
sub_4B6C9   endp


; =============== S U B R O U T I N E =======================================


sub_4B721   proc near
        al = byte_58D47
        mov byte_58D46, al
        call    sub_4C0DD
        retn
sub_4B721   endp


; =============== S U B R O U T I N E =======================================


sub_4B72B   proc near
        mov byte_50918, 1
        mov byte_50916, 1
        mov byte_50917, 0
        mov ax, word_5195D
        sub ax, word_58469
        cmp ax, word_5846B
        jnb short loc_4B748
        retn
// ; ---------------------------------------------------------------------------

loc_4B748:              ; CODE XREF: sub_4B72B+1Aj
        mov ax, word_5195D
        mov word_5846B, ax
        cmp word_58469, 1
        jbe short loc_4B759
        dec word_58469

loc_4B759:              ; CODE XREF: sub_4B72B+28j
        cmp word_51ABC, 71h ; 'q'
        jnb short locret_4B770
        inc word_51ABC
        call    sub_4B899
        call    sub_4C141
        call    sub_4B85C
        call    sub_4B8BE

locret_4B770:               ; CODE XREF: sub_4B72B+33j
        retn
sub_4B72B   endp


; =============== S U B R O U T I N E =======================================


sub_4B771   proc near
        mov byte_50918, 1
        mov byte_50916, 0
        mov byte_50917, 1
        mov ax, word_5195D
        sub ax, word_58469
        cmp ax, word_5846B
        jnb short loc_4B78E
        retn
// ; ---------------------------------------------------------------------------

loc_4B78E:              ; CODE XREF: sub_4B771+1Aj
        mov ax, word_5195D
        mov word_5846B, ax
        cmp word_58469, 1
        jbe short loc_4B79F
        dec word_58469

loc_4B79F:              ; CODE XREF: sub_4B771+28j
        cmp word_51ABC, 1
        jbe short locret_4B7B6
        dec word_51ABC
        call    sub_4B899
        call    sub_4C141
        call    sub_4B85C
        call    sub_4B8BE

locret_4B7B6:               ; CODE XREF: sub_4B771+33j
        retn
sub_4B771   endp


; =============== S U B R O U T I N E =======================================


sub_4B7B7   proc near
        mov si, 60D5h
        call    fade
        mov bx, 4D84h
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        call    vgaloadbackseg
        mov si, 861Dh
        mov di, 5252h
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, 8638h
        mov di, 609Bh
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, 865Ch
        mov di, 6563h
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, 866Eh
        mov di, 786Fh
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, 8691h
        mov di, 7D34h
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, 86B2h
        mov di, 81F7h
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, 86D5h
        mov di, 9E90h
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, 86F7h
        mov di, 0A818h
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, 5FD5h
        call    fade
        call    sub_47E98
        mov si, 60D5h
        call    fade
        mov bx, 4D5Ch
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        mov si, 6015h
        call    fade
        retn
sub_4B7B7   endp


; =============== S U B R O U T I N E =======================================


sub_4B85C   proc near       ; CODE XREF: sub_4AB1B+FCp
                    ; sub_4AB1B+124p ...
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        mov ax, mousey
        mov bx, 7Ah ; 'z'
        mul bx
        add ax, 4D5Ch
        mov si, ax
        mov ax, mousex
        shr ax, 1
        shr ax, 1
        shr ax, 1
        add si, ax
        mov word_5847B, si
        mov di, 4D34h
        mov cx, 10h
        push    ds
        mov ax, es
        mov ds, ax

loc_4B88D:              ; CODE XREF: sub_4B85C+39j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4B88D
        pop ds
        retn
sub_4B85C   endp


; =============== S U B R O U T I N E =======================================


sub_4B899   proc near       ; CODE XREF: start+417p sub_4AB1B+3Fp ...
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        mov si, 4D34h
        mov di, word_5847B
        mov cx, 10h
        push    ds
        mov ax, es
        mov ds, ax

loc_4B8B2:              ; CODE XREF: sub_4B899+21j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4B8B2
        pop ds
        retn
sub_4B899   endp


; =============== S U B R O U T I N E =======================================


sub_4B8BE   proc near       ; CODE XREF: sub_4AB1B+FFp
                    ; sub_4AB1B+127p ...
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 8
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 2
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; color compare.
                    ; Data bits 0-3 select color for read mode 01
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 7
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; color masking disable
                    ; bits 0-3 disable planes from compare logic in read mode 01
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov si, 1520h
        mov bx, word_5195D
        shr bx, 1
        shr bx, 1
        ;and bx, 0Eh
        db  83h, 0E3h, 0Eh
        mov si, [bx+si]
        mov di, word_5847B
        mov bx, mousex
        and bl, 7
        mov bh, 8
        sub bh, bl
        mov cx, 8
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        push    ds
        mov ax, es
        mov ds, ax
        push    si
        push(di);
        mov cx, 8

loc_4B926:              ; CODE XREF: sub_4B8BE+85j
        push(cx);
        al = [si]
        mov ah, al
        mov cl, bl
        shr al, cl
        out dx, al      ; EGA port: graphics controller data register
        xor [di], al
        al = ah
        mov cl, bh
        shl al, cl
        out dx, al      ; EGA port: graphics controller data register
        xor [di+1], al
        add si, 7Ah ; 'z'
        add di, 7Ah ; 'z'
        pop(cx);
        loop    loc_4B926
        mov dx, 3CEh
        al = 2
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; color compare.
                    ; Data bits 0-3 select color for read mode 01
        inc dx
        al = 5
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 5
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        pop(di);
        pop si
        push    si
        push(di);
        mov cx, 8

loc_4B96A:              ; CODE XREF: sub_4B8BE+C9j
        push(cx);
        al = [si]
        mov ah, al
        mov cl, bl
        shr al, cl
        out dx, al      ; EGA port: graphics controller data register
        xor [di], al
        al = ah
        mov cl, bh
        shl al, cl
        out dx, al      ; EGA port: graphics controller data register
        xor [di+1], al
        add si, 7Ah ; 'z'
        add di, 7Ah ; 'z'
        pop(cx);
        loop    loc_4B96A
        mov dx, 3CEh
        al = 2
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; color compare.
                    ; Data bits 0-3 select color for read mode 01
        inc dx
        al = 8
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 8
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        pop(di);
        pop si
        push    si
        push(di);
        mov cx, 8

loc_4B9AE:              ; CODE XREF: sub_4B8BE+10Dj
        push(cx);
        al = [si]
        mov ah, al
        mov cl, bl
        shr al, cl
        out dx, al      ; EGA port: graphics controller data register
        xor [di], al
        al = ah
        mov cl, bh
        shl al, cl
        out dx, al      ; EGA port: graphics controller data register
        xor [di+1], al
        add si, 7Ah ; 'z'
        add di, 7Ah ; 'z'
        pop(cx);
        loop    loc_4B9AE
        mov dx, 3CEh
        al = 2
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; color compare.
                    ; Data bits 0-3 select color for read mode 01
        inc dx
        al = 9
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 9
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        pop(di);
        pop si
        push    si
        push(di);
        mov cx, 8

loc_4B9F2:              ; CODE XREF: sub_4B8BE+151j
        push(cx);
        al = [si]
        mov ah, al
        mov cl, bl
        shr al, cl
        out dx, al      ; EGA port: graphics controller data register
        xor [di], al
        al = ah
        mov cl, bh
        shl al, cl
        out dx, al      ; EGA port: graphics controller data register
        xor [di+1], al
        add si, 7Ah ; 'z'
        add di, 7Ah ; 'z'
        pop(cx);
        loop    loc_4B9F2
        mov dx, 3CEh
        al = 2
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; color compare.
                    ; Data bits 0-3 select color for read mode 01
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        pop(di);
        pop si
        mov cx, 8

loc_4BA34:              ; CODE XREF: sub_4B8BE+193j
        push(cx);
        al = [si]
        mov ah, al
        mov cl, bl
        shr al, cl
        out dx, al      ; EGA port: graphics controller data register
        xor [di], al
        al = ah
        mov cl, bh
        shl al, cl
        out dx, al      ; EGA port: graphics controller data register
        xor [di+1], al
        add si, 7Ah ; 'z'
        add di, 7Ah ; 'z'
        pop(cx);
        loop    loc_4BA34
        pop ds
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        retn
sub_4B8BE   endp


; =============== S U B R O U T I N E =======================================


sub_4BA5F   proc near       ; CODE XREF: sub_4AB1B+37p
                    ; sub_4AB1B+4Ap ...
        cmp byte_5A33F, 1
        jnz short loc_4BA69
        jmp locret_4BDEF
// ; ---------------------------------------------------------------------------

loc_4BA69:              ; CODE XREF: sub_4BA5F+5j
        mov byte_51969, ah
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        mov cl, 0

loc_4BA8D:              ; CODE XREF: sub_4BA5F:loc_4BDECj
        mov bl, [si]
        cmp bl, 0
        jnz short loc_4BA97
        jmp locret_4BDEF
// ; ---------------------------------------------------------------------------

loc_4BA97:              ; CODE XREF: sub_4BA5F+33j
        cmp bl, 0Ah
        jnz short loc_4BA9F
        jmp locret_4BDEF
// ; ---------------------------------------------------------------------------

loc_4BA9F:              ; CODE XREF: sub_4BA5F+3Bj
        inc si
        sub bl, 20h ; ' '
        xor bh, bh
        add bx, 5B15h
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov ah, 0FCh ; '?'
        shr ah, cl
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov ah, 0FCh ; '?'
        mov ch, cl
        al = 8
        sub al, cl
        mov cl, al
        shl ah, cl
        mov cl, ch
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = byte_51969
        out dx, al      ; EGA port: graphics controller data register
        mov ah, [bx]
        shr ah, cl
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov ah, [bx]
        mov ch, cl
        al = 8
        sub al, cl
        mov cl, al
        shl ah, cl
        mov cl, ch
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov ah, 0FCh ; '?'
        shr ah, cl
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov ah, 0FCh ; '?'
        mov ch, cl
        al = 8
        sub al, cl
        mov cl, al
        shl ah, cl
        mov cl, ch
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = byte_51969
        out dx, al      ; EGA port: graphics controller data register
        mov ah, [bx]
        shr ah, cl
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov ah, [bx]
        mov ch, cl
        al = 8
        sub al, cl
        mov cl, al
        shl ah, cl
        mov cl, ch
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov ah, 0FCh ; '?'
        shr ah, cl
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov ah, 0FCh ; '?'
        mov ch, cl
        al = 8
        sub al, cl
        mov cl, al
        shl ah, cl
        mov cl, ch
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = byte_51969
        out dx, al      ; EGA port: graphics controller data register
        mov ah, [bx]
        shr ah, cl
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov ah, [bx]
        mov ch, cl
        al = 8
        sub al, cl
        mov cl, al
        shl ah, cl
        mov cl, ch
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov ah, 0FCh ; '?'
        shr ah, cl
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov ah, 0FCh ; '?'
        mov ch, cl
        al = 8
        sub al, cl
        mov cl, al
        shl ah, cl
        mov cl, ch
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = byte_51969
        out dx, al      ; EGA port: graphics controller data register
        mov ah, [bx]
        shr ah, cl
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov ah, [bx]
        mov ch, cl
        al = 8
        sub al, cl
        mov cl, al
        shl ah, cl
        mov cl, ch
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov ah, 0FCh ; '?'
        shr ah, cl
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov ah, 0FCh ; '?'
        mov ch, cl
        al = 8
        sub al, cl
        mov cl, al
        shl ah, cl
        mov cl, ch
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = byte_51969
        out dx, al      ; EGA port: graphics controller data register
        mov ah, [bx]
        shr ah, cl
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov ah, [bx]
        mov ch, cl
        al = 8
        sub al, cl
        mov cl, al
        shl ah, cl
        mov cl, ch
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov ah, 0FCh ; '?'
        shr ah, cl
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov ah, 0FCh ; '?'
        mov ch, cl
        al = 8
        sub al, cl
        mov cl, al
        shl ah, cl
        mov cl, ch
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = byte_51969
        out dx, al      ; EGA port: graphics controller data register
        mov ah, [bx]
        shr ah, cl
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov ah, [bx]
        mov ch, cl
        al = 8
        sub al, cl
        mov cl, al
        shl ah, cl
        mov cl, ch
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov ah, 0FCh ; '?'
        shr ah, cl
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov ah, 0FCh ; '?'
        mov ch, cl
        al = 8
        sub al, cl
        mov cl, al
        shl ah, cl
        mov cl, ch
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = byte_51969
        out dx, al      ; EGA port: graphics controller data register
        mov ah, [bx]
        shr ah, cl
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov ah, [bx]
        mov ch, cl
        al = 8
        sub al, cl
        mov cl, al
        shl ah, cl
        mov cl, ch
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        sub di, 356h
        add cl, 6
        cmp cl, 8
        jl  short loc_4BDEC
        sub cl, 8
        inc di

loc_4BDEC:              ; CODE XREF: sub_4BA5F+387j
        jmp loc_4BA8D
// ; ---------------------------------------------------------------------------

locret_4BDEF:               ; CODE XREF: sub_4BA5F+7j
                    ; sub_4BA5F+35j ...
        retn
sub_4BA5F   endp


; =============== S U B R O U T I N E =======================================


sub_4BDF0   proc near       ; CODE XREF: sub_47F39+2Ap
                    ; sub_4AF0C+EDp ...
        cmp byte_5A33F, 1
        jnz short loc_4BDFA
        jmp locret_4BF49
// ; ---------------------------------------------------------------------------

loc_4BDFA:              ; CODE XREF: sub_4BDF0+5j
        mov byte_51969, ah
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        mov cl, 0

loc_4BE1E:              ; CODE XREF: sub_4BDF0:loc_4BF46j
        mov bl, [si]
        cmp bl, 0
        jnz short loc_4BE28
        jmp locret_4BF49
// ; ---------------------------------------------------------------------------

loc_4BE28:              ; CODE XREF: sub_4BDF0+33j
        cmp bl, 0Ah
        jnz short loc_4BE30
        jmp locret_4BF49
// ; ---------------------------------------------------------------------------

loc_4BE30:              ; CODE XREF: sub_4BDF0+3Bj
        inc si
        sub bl, 20h ; ' '
        xor bh, bh
        add bx, 5B15h
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = byte_51969
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        al = [bx]
        shr al, cl
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        al = [bx]
        mov ch, cl
        mov ah, 8
        sub ah, cl
        mov cl, ah
        shl al, cl
        mov cl, ch
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        al = [bx]
        shr al, cl
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        al = [bx]
        mov ch, cl
        mov ah, 8
        sub ah, cl
        mov cl, ah
        shl al, cl
        mov cl, ch
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        al = [bx]
        shr al, cl
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        al = [bx]
        mov ch, cl
        mov ah, 8
        sub ah, cl
        mov cl, ah
        shl al, cl
        mov cl, ch
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        al = [bx]
        shr al, cl
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        al = [bx]
        mov ch, cl
        mov ah, 8
        sub ah, cl
        mov cl, ah
        shl al, cl
        mov cl, ch
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        al = [bx]
        shr al, cl
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        al = [bx]
        mov ch, cl
        mov ah, 8
        sub ah, cl
        mov cl, ah
        shl al, cl
        mov cl, ch
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        al = [bx]
        shr al, cl
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        al = [bx]
        mov ch, cl
        mov ah, 8
        sub ah, cl
        mov cl, ah
        shl al, cl
        mov cl, ch
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        al = [bx]
        shr al, cl
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        al = [bx]
        mov ch, cl
        mov ah, 8
        sub ah, cl
        mov cl, ah
        shl al, cl
        mov cl, ch
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di+1], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        sub di, 356h
        add cl, 6
        cmp cl, 8
        jl  short loc_4BF46
        sub cl, 8
        inc di

loc_4BF46:              ; CODE XREF: sub_4BDF0+150j
        jmp loc_4BE1E
// ; ---------------------------------------------------------------------------

locret_4BF49:               ; CODE XREF: sub_4BDF0+7j
                    ; sub_4BDF0+35j ...
        retn
sub_4BDF0   endp


; =============== S U B R O U T I N E =======================================


sub_4BF4A   proc near       ; CODE XREF: start+3F7p sub_4955B+398p ...
        mov si, 0A017h
sub_4BF4A   endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================


sub_4BF4D   proc near       ; CODE XREF: sub_4ADFF+7Cp
                    ; sub_4AF0C+13Dp ...
        mov ah, 30h ; '0'
sub_4BF4D   endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================


sub_4BF4F   proc near       ; CODE XREF: sub_4AF0C+16Fp
                    ; sub_4AF0C+1BFp ...
        push    ax
        mov ah, 30h ; '0'
        mov [si], ah
        mov [si+1], ah
        mov [si+2], ah

loc_4BF5A:              ; CODE XREF: sub_4BF4F+11j
        sub al, 64h ; 'd'
        jb  short loc_4BF62
        inc byte ptr [si]
        jmp short loc_4BF5A
// ; ---------------------------------------------------------------------------

loc_4BF62:              ; CODE XREF: sub_4BF4F+Dj
        inc si
        add al, 64h ; 'd'

loc_4BF65:              ; CODE XREF: sub_4BF4F+1Cj
        sub al, 0Ah
        jb  short loc_4BF6D
        inc byte ptr [si]
        jmp short loc_4BF65
// ; ---------------------------------------------------------------------------

loc_4BF6D:              ; CODE XREF: sub_4BF4F+18j
        inc si
        add al, 0Ah
        add [si], al
        pop ax
        cmp ah, 20h ; ' '
        jnz short locret_4BF8C
        dec si
        dec si
        cmp byte ptr [si], 30h ; '0'
        jnz short loc_4BF8B
        mov byte ptr [si], 20h ; ' '
        inc si
        cmp byte ptr [si], 30h ; '0'
        jnz short loc_4BF8B
        mov byte ptr [si], 20h ; ' '

loc_4BF8B:              ; CODE XREF: sub_4BF4F+2Ej
                    ; sub_4BF4F+37j
        inc si

locret_4BF8C:               ; CODE XREF: sub_4BF4F+27j
        retn
sub_4BF4F   endp


; =============== S U B R O U T I N E =======================================


sub_4BF8D   proc near       ; CODE XREF: sub_4C0DDp
        cmp byte_59B85, 0
        jz  short loc_4BF97
        jmp locret_4C0DC
// ; ---------------------------------------------------------------------------

loc_4BF97:              ; CODE XREF: sub_4BF8D+5j
        mov cx, 14h
        mov si, 8A9Ch
        mov di, 8A38h
        mov dl, 0

loc_4BFA2:              ; CODE XREF: sub_4BF8D+38j
        mov [di], dl
        al = [si+7Eh]
        mov [di+1], al
        al = [si+9]
        mov [di+2], al
        al = [si+0Ah]
        mov [di+3], al
        al = [si+0Bh]
        mov [di+4], al
        inc dl
        add si, 80h ; '?'
        add di, 5
        loop    loc_4BFA2

loc_4BFC7:              ; CODE XREF: sub_4BF8D+B4j
        mov dx, 0
        mov cx, 13h
        mov si, 8A38h
        mov di, 8A3Dh

loc_4BFD3:              ; CODE XREF: sub_4BF8D+AFj
        al = [si+1]
        cmp [di+1], al
        jl  short loc_4C036
        jg  short loc_4BFFD
        al = [si+2]
        cmp [di+2], al
        jg  short loc_4C036
        jl  short loc_4BFFD
        al = [si+3]
        cmp [di+3], al
        jg  short loc_4C036
        jl  short loc_4BFFD
        al = [si+4]
        cmp [di+4], al
        jg  short loc_4C036
        jl  short loc_4BFFD
        jmp short loc_4C036
// ; ---------------------------------------------------------------------------

loc_4BFFD:              ; CODE XREF: sub_4BF8D+4Ej
                    ; sub_4BF8D+58j ...
        al = [si]
        mov ah, [di]
        mov [si], ah
        mov [di], al
        al = [si+1]
        mov ah, [di+1]
        mov [si+1], ah
        mov [di+1], al
        al = [si+2]
        mov ah, [di+2]
        mov [si+2], ah
        mov [di+2], al
        al = [si+3]
        mov ah, [di+3]
        mov [si+3], ah
        mov [di+3], al
        al = [si+4]
        mov ah, [di+4]
        mov [si+4], ah
        mov [di+4], al
        inc dx

loc_4C036:              ; CODE XREF: sub_4BF8D+4Cj
                    ; sub_4BF8D+56j ...
        add si, 5
        add di, 5
        loop    loc_4BFD3
        cmp dx, 0
        jnz short loc_4BFC7
        mov cx, 14h
        mov dl, 0
        mov si, 8A38h

loc_4C04B:              ; CODE XREF: sub_4BF8D+CFj
        al = [si]
        cmp al, byte_5981F
        jnz short loc_4C057
        mov byte_58D47, dl

loc_4C057:              ; CODE XREF: sub_4BF8D+C4j
        add si, 5
        inc dl
        loop    loc_4C04B
        mov cx, 14h

loc_4C061:
        mov si, 8A38h
        mov di, 883Ch

loc_4C067:              ; CODE XREF: sub_4BF8D+14Dj
        al = [si+1]
        cmp al, 71h ; 'q'
        jnz short loc_4C078
        mov word ptr [di], 3939h

loc_4C072:
        mov byte ptr [di+2], 39h ; '9'
        jmp short loc_4C07F
// ; ---------------------------------------------------------------------------

loc_4C078:              ; CODE XREF: sub_4BF8D+DFj
        push    si
        mov si, di
        call    sub_4BF4F
        pop si

loc_4C07F:              ; CODE XREF: sub_4BF8D+E9j
        add di, 4
        push    si
        mov ah, [si]
        xor al, al
        shr ax, 1
        mov si, 8A9Ch
        add si, ax
        mov bx, 8

loc_4C091:              ; CODE XREF: sub_4BF8D+10Bj
        al = [si]
        mov [di], al
        inc si
        inc di
        dec bx
        jnz short loc_4C091
        pop si
        push(di);
        push    si
        al = [si+4]
        mov si, di
        add si, 7
        call    sub_4BF4D
        mov si, di
        add si, 7
        mov byte ptr [si], 3Ah ; ':'
        pop si
        push    si
        al = [si+3]
        mov si, di
        add si, 4
        call    sub_4BF4D
        mov si, di
        add si, 4
        mov byte ptr [si], 3Ah ; ':'
        pop si
        push    si
        al = [si+2]
        mov si, di
        add si, 1
        call    sub_4BF4D
        pop si
        pop(di);
        add di, 0Bh
        add si, 5
        loop    loc_4C067

locret_4C0DC:               ; CODE XREF: sub_4BF8D+7j
        retn
sub_4BF8D   endp


; =============== S U B R O U T I N E =======================================


sub_4C0DD   proc near       ; CODE XREF: sub_4AB1B+1E9p
                    ; sub_4AD0E+E2p ...
        call    sub_4BF8D
        mov si, 880Eh
        al = byte_58D46
        xor ah, ah
        mov bl, 17h
        mul bl
        add si, ax
        push    si
        mov ah, 8
        mov di, 7935h
        call    sub_4BA5F
        pop si
        add si, 17h
        push    si
        mov ah, 8
        mov di, 7D7Fh
        call    sub_4BA5F
        pop si
        add si, 17h
        push    si
        mov ah, 6
        mov di, 81C9h
        call    sub_4BA5F
        pop si
        add si, 17h
        push    si
        mov ah, 8
        mov di, 8613h
        call    sub_4BA5F
        pop si
        add si, 17h
        mov ah, 8
        mov di, 8A5Dh
        call    sub_4BA5F
        mov si, 8359h
        al = byte_58D46
        al++;
        call    sub_4BF4D
        mov si, 835Ah
        mov di, 81DAh
        mov ah, 6
        call    sub_4BA5F
        retn
sub_4C0DD   endp


; =============== S U B R O U T I N E =======================================


sub_4C141   proc near       ; CODE XREF: start+41Ap sub_4955B+39Bp ...
        mov si, 949Eh
        mov bx, word_51ABC
        sub bx, 2
        add si, bx
        al = [si]
        mov byte_59821, al
        al = [si+1]
        mov byte_59822, al
        al = [si+2]
        mov byte_59823, al
        mov ax, word_51ABC
        dec ax
        mov bx, 1Ch
        mul bx
        mov si, ax
        add si, 2A6Ch
        sub si, 1Ch
        push    si
        mov di, 974Ch
        mov ah, byte_59821
        call    sub_4BA5F
        pop si
        add si, 1Ch
        push    si
        mov di, 9B96h
        mov ah, byte_59822
        call    sub_4BA5F
        mov cx, 1Ch
        pop si
        push    si
        mov di, 87A8h
        push    es
        mov ax, ds
        mov es, ax
        assume es:data
        rep movsb
        pop es
        assume es:nothing
        pop si
        add si, 1Ch
        mov di, 9FE0h
        mov ah, byte_59823
        call    sub_4BA5F
        retn
sub_4C141   endp


; =============== S U B R O U T I N E =======================================


sub_4C1A9   proc near       ; CODE XREF: sub_4B419+15Ap
                    ; sub_4C2F2+11p
        push    es
        mov ax, ds
        mov es, ax
        assume es:data
        mov si, 9514h
        mov di, 836Bh
        mov cx, 3

loc_4C1B7:              ; CODE XREF: sub_4C1A9+56j
        push(cx);
        push    si
        al = [si+0Bh]
        mov si, di
        add si, 0Fh
        call    sub_4BF4D
        mov si, di
        add si, 0Fh
        mov byte ptr [si], 3Ah ; ':'
        pop si
        push    si
        al = [si+0Ah]
        mov si, di
        add si, 0Ch
        call    sub_4BF4D
        mov si, di
        add si, 0Ch
        mov byte ptr [si], 3Ah ; ':'
        pop si
        push    si
        al = [si+9]
        mov si, di
        add si, 9
        mov ah, 20h ; ' '
        call    sub_4BF4F
        pop si
        push    si
        mov cx, 8
        rep movsb
        pop si
        add si, 0Ch
        add di, 0Bh
        pop(cx);
        loop    loc_4C1B7
        pop es
        assume es:nothing
        mov si, 836Bh
        mov ah, 8
        mov di, 5ACBh
        call    sub_4BA5F
        mov si, 837Eh
        mov ah, 8
        mov di, 5F15h
        call    sub_4BA5F
        mov si, 8391h
        mov ah, 8
        mov di, 635Fh
        call    sub_4BA5F
        retn
sub_4C1A9   endp


; =============== S U B R O U T I N E =======================================


sub_4C224   proc near       ; CODE XREF: sub_4C293+5Bp
        mov bh, byte_5981F
        xor bl, bl
        shr bx, 1
        mov si, bx
        add si, 8A9Ch
        mov bp, si
        mov di, 79C3h
        mov ah, 8
        call    sub_4BA5F
        mov di, bp
        xor ah, ah
        al = [di+0Bh]
        mov si, 8363h
        call    sub_4BF4D
        mov si, 8363h
        mov byte ptr [si], 3Ah ; ':'
        mov di, bp
        xor ah, ah
        al = [di+0Ah]
        sub si, 3
        call    sub_4BF4D
        mov si, 8360h
        mov byte ptr [si], 3Ah ; ':'
        mov di, bp
        xor ah, ah
        al = [di+9]
        sub si, 3
        call    sub_4BF4D
        mov si, 835Dh
        mov di, 79CAh
        mov ah, 8
        call    sub_4BA5F
        mov di, bp
        xor ah, ah
        al = [di+7Eh]
        mov si, 8367h
        call    sub_4BF4D
        mov si, 8367h
        mov di, 79D2h
        mov ah, 8
        call    sub_4BA5F
        retn
sub_4C224   endp


; =============== S U B R O U T I N E =======================================


sub_4C293   proc near       ; CODE XREF: start+32Cp start+407p ...
        mov bh, byte_5981F
        xor bl, bl
        shr bx, 1
        mov si, bx
        add si, 8A9Ch
        push    si
        mov di, 879Fh
        mov cx, 8
        push    es
        mov ax, ds
        mov es, ax
        assume es:data
        rep movsb
        pop es
        assume es:nothing
        sub si, 8
        mov di, 9B86h
        mov ah, 6
        call    sub_4BA5F
        pop si
        push    si
        cmp byte_5981F, 0
        jbe short loc_4C2CA
        sub si, 80h ; '?'
        jmp short loc_4C2CD
// ; ---------------------------------------------------------------------------

loc_4C2CA:              ; CODE XREF: sub_4C293+2Fj
        mov si, 821Dh

loc_4C2CD:              ; CODE XREF: sub_4C293+35j
        mov di, 973Ch
        mov ah, 8
        call    sub_4BA5F
        pop si
        cmp byte_5981F, 13h
        jnb short loc_4C2E3
        add si, 80h ; '?'
        jmp short loc_4C2E6
// ; ---------------------------------------------------------------------------

loc_4C2E3:              ; CODE XREF: sub_4C293+48j
        mov si, 821Dh

loc_4C2E6:              ; CODE XREF: sub_4C293+4Ej
        mov di, 9FD0h
        mov ah, 8
        call    sub_4BA5F
        call    sub_4C224
        retn
sub_4C293   endp


; =============== S U B R O U T I N E =======================================


sub_4C2F2   proc near       ; CODE XREF: sub_4B149+Cp
                    ; sub_4C407+1Fp ...
        mov si, 817Bh
        mov di, 89F7h
        mov ah, 4
        call    sub_4BA5F
        call    sub_4C293
        call    sub_4C141
        call    sub_4C1A9
        call    sub_4C0DD
        cmp byte_59B83, 0
        jz  short locret_4C349
        mov byte_59B83, 0
        cmp byte_5A19B, 0
        jz  short loc_4C32D
        cmp byte_5A19C, 0
        jz  short loc_4C328
        mov si, 9E8Dh
        jmp short loc_4C33C
// ; ---------------------------------------------------------------------------

loc_4C328:              ; CODE XREF: sub_4C2F2+2Fj
        mov si, 9EBDh
        jmp short loc_4C33C
// ; ---------------------------------------------------------------------------

loc_4C32D:              ; CODE XREF: sub_4C2F2+28j
        cmp byte_5A19C, 0
        jz  short loc_4C339
        mov si, 9EA5h
        jmp short loc_4C33C
// ; ---------------------------------------------------------------------------

loc_4C339:              ; CODE XREF: sub_4C2F2+40j
        mov si, 9ED5h

loc_4C33C:              ; CODE XREF: sub_4C2F2+34j
                    ; sub_4C2F2+39j ...
        mov di, 89F7h
        mov ah, 4
        call    sub_4BA5F
        mov byte_5A19B, 0

locret_4C349:               ; CODE XREF: sub_4C2F2+1Cj
        retn
sub_4C2F2   endp


; =============== S U B R O U T I N E =======================================


sub_4C34A   proc near       ; CODE XREF: start+404p sub_4AB1B+1E0p ...
        mov bh, byte_5981F
        xor bl, bl
        shr bx, 1
        mov si, 8AA8h
        add si, bx
        mov di, 8A9Ch
        add di, bx
        push(di);
        mov di, 949Eh
        mov cx, 6Fh ; 'o'
        al = 6

loc_4C365:              ; CODE XREF: sub_4C34A+1Ej
        mov [di], al
        inc di
        loop    loc_4C365
        push    si
        al = 1
        mov cx, 6Fh ; 'o'
        mov di, 949Eh

loc_4C373:              ; CODE XREF: sub_4C34A+53j
        mov ah, [si]
        cmp ah, 2
        jnz short loc_4C37F
        mov byte ptr [di], 8
        jmp short loc_4C39B
// ; ---------------------------------------------------------------------------

loc_4C37F:              ; CODE XREF: sub_4C34A+2Ej
        cmp ah, 1
        jnz short loc_4C389
        mov byte ptr [di], 4
        jmp short loc_4C39B
// ; ---------------------------------------------------------------------------

loc_4C389:              ; CODE XREF: sub_4C34A+38j
        or  ah, ah
        jnz short loc_4C39B
        cmp al, 1
        jnz short loc_4C396
        mov byte ptr [di], 2
        jmp short loc_4C399
// ; ---------------------------------------------------------------------------

loc_4C396:              ; CODE XREF: sub_4C34A+45j
        mov byte ptr [di], 6

loc_4C399:              ; CODE XREF: sub_4C34A+4Aj
        al = ah

loc_4C39B:              ; CODE XREF: sub_4C34A+33j
                    ; sub_4C34A+3Dj ...
        inc si
        inc di
        loop    loc_4C373
        pop si
        pop(di);
        mov cx, 6Fh ; 'o'
        mov bx, 1

loc_4C3A7:              ; CODE XREF: sub_4C34A+65j
        al = [si]
        cmp al, 0
        jz  short loc_4C3D6
        inc si
        inc bx
        loop    loc_4C3A7
        sub si, 6Fh ; 'o'
        mov bx, 1
        mov cx, 6Fh ; 'o'

loc_4C3BA:              ; CODE XREF: sub_4C34A+78j
        al = [si]
        cmp al, 2
        jz  short loc_4C3D6
        inc si
        inc bx
        loop    loc_4C3BA
        cmp byte_51ABE, 0
        jz  short loc_4C3D1
        mov word_51ABC, 71h ; 'q'

loc_4C3D1:              ; CODE XREF: sub_4C34A+7Fj
        mov byte ptr [di+7Eh], 71h ; 'q'
        retn
// ; ---------------------------------------------------------------------------

loc_4C3D6:              ; CODE XREF: sub_4C34A+61j
                    ; sub_4C34A+74j
        cmp byte_51ABE, 0
        jz  short loc_4C3E1
        mov word_51ABC, bx

loc_4C3E1:              ; CODE XREF: sub_4C34A+91j
        cmp bl, 1
        jnz short loc_4C403
        cmp word ptr [di], 2D2Dh
        jnz short loc_4C403
        cmp word ptr [di+2], 2D2Dh
        jnz short loc_4C403
        cmp word ptr [di+4], 2D2Dh
        jnz short loc_4C403
        cmp word ptr [di+6], 2D2Dh
        jnz short loc_4C403
        mov bl, 0

loc_4C403:              ; CODE XREF: sub_4C34A+9Aj
                    ; sub_4C34A+A0j ...
        mov [di+7Eh], bl
        retn
sub_4C34A   endp


; =============== S U B R O U T I N E =======================================


sub_4C407   proc near       ; CODE XREF: runMainMenu+5Dp
        mov byte_510A6, 0
        cmp byte_510BA, 0
        jz  short loc_4C431
        mov byte_510BA, 0
        call    sub_4C4F9
        call    sub_4C611
        mov byte_51ABE, 0
        call    sub_4C34A
        call    sub_4C2F2
        mov si, 6015h
        call    fade
        jmp short loc_4C499
// ; ---------------------------------------------------------------------------

loc_4C431:              ; CODE XREF: sub_4C407+Aj
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        cmp word_5195F, 140h
        jb  short loc_4C449
        mov di, 4D34h
        call    sub_4C4BD

loc_4C449:              ; CODE XREF: sub_4C407+3Aj
        mov di, 4D84h
        call    sub_4C4BD

;ploc_4C44F::
loc_4C44F:              ; CODE XREF: sub_4B149+9p
        call    sub_4C611
        mov byte_51ABE, 0
        call    sub_4C34A
        call    sub_4C2F2
        call    videoloop
        call    sub_4D457
        mov bx, 4D83h

loc_4C466:              ; CODE XREF: sub_4C407+90j
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        mov byte_510A6, 7
        call    videoloop
        call    sub_4D457
        mov byte_510A6, 3
        call    videoloop
        call    sub_4D457
        sub bx, 1
        cmp bx, 4D5Ch
        jnb short loc_4C466

loc_4C499:              ; CODE XREF: sub_4C407+28j
        mov bx, 4D5Ch
        mov word_51967, bx
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        mov byte_510A6, 0
        call    videoloop
        retn
sub_4C407   endp


; =============== S U B R O U T I N E =======================================


sub_4C4BD   proc near       ; CODE XREF: sub_4C407+3Fp
                    ; sub_4C407+45p
        mov si, word_51967
        mov word_51967, di
        mov bx, di
        mov dx, 0C8h ; '?'
        push    ds
        mov ax, es
        mov ds, ax

loc_4C4CF:              ; CODE XREF: sub_4C4BD+1Ej
        mov cx, 28h ; '('
        rep movsb
        add si, 52h ; 'R'
        add di, 52h ; 'R'
        dec dx
        jnz short loc_4C4CF
        pop ds
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        call    videoloop
        call    sub_4D457
        retn
sub_4C4BD   endp


; =============== S U B R O U T I N E =======================================


sub_4C4F9   proc near       ; CODE XREF: sub_4C407+11p
        mov si, 60D5h
        call    sub_4D836
        call    vgaloadbackseg
        mov si, 8577h
        mov di, 6A2Ch
        mov ah, 0Fh
        call    sub_4BDF0
        cmp byte_5195A, 0
        jnz short loc_4C52C
        mov si, 8582h
        mov di, 73A9h
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, 85ACh
        mov di, 7D35h
        mov ah, 0Fh
        call    sub_4BDF0
        jmp short loc_4C55C
// ; ---------------------------------------------------------------------------

loc_4C52C:              ; CODE XREF: sub_4C4F9+19j
        mov si, 85DCh
        al = byte_5195B
        sub al, byte_5195A
        mov ah, 20h ; ' '
        call    sub_4BF4F
        mov si, 85EBh
        al = byte_5195B
        mov ah, 20h ; ' '
        call    sub_4BF4F
        mov si, 85C9h
        mov di, 73A9h
        mov ah, 0Fh
        call    sub_4BDF0
        mov si, 85EFh
        mov di, 7D39h
        mov ah, 0Fh
        call    sub_4BDF0

loc_4C55C:              ; CODE XREF: sub_4C4F9+31j
        mov si, 8600h
        mov di, 86BDh
        mov ah, 0Fh
        call    sub_4BDF0
        mov bx, 4D84h
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        call    videoloop
        mov si, 5FD5h
        call    sub_4D836
        cmp word_5197A, 1
        jz  short loc_4C591
        call    sub_47E98

loc_4C591:              ; CODE XREF: sub_4C4F9+93j
        mov si, 60D5h
        call    sub_4D836
        mov bx, 4D5Ch
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        retn
sub_4C4F9   endp


; =============== S U B R O U T I N E =======================================


sub_4C5AF   proc near       ; CODE XREF: sub_4B149+3p
        mov byte_510A6, 0
        call    videoloop
        mov bx, 4D5Ch

loc_4C5BA:              ; CODE XREF: sub_4C5AF+3Cj
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        mov byte_510A6, 1
        call    videoloop
        call    sub_4D457
        mov byte_510A6, 5
        call    videoloop
        call    sub_4D457
        add bx, 1
        cmp bx, 4D84h
        jb  short loc_4C5BA
        mov bx, 4D84h
        mov word_51967, bx
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        mov byte_510A6, 0
        call    videoloop
        retn
sub_4C5AF   endp


; =============== S U B R O U T I N E =======================================


sub_4C611   proc near       ; CODE XREF: sub_4C407+14p
                    ; sub_4C407:loc_4C44Fp ...
        push    ds
        mov ax, seg menuseg
        mov ds, ax
        assume ds:nothing
        mov si, 0
        mov di, 4D5Ch
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        mov cx, 0C8h ; '?'

loc_4C63E:              ; CODE XREF: sub_4C611+4Dj
        push(cx);
        mov ah, 1

loc_4C641:              ; CODE XREF: sub_4C611+47j
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = ah
        out dx, al      ; EGA port: sequencer data register
        mov cx, 28h ; '('
        rep movsb
        sub di, 28h ; '('
        shl ah, 1
        test    ah, 0Fh
        jnz short loc_4C641
        add di, 7Ah ; 'z'
        pop(cx);
        loop    loc_4C63E
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: sequencer data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        pop ds
        assume ds:data
        retn
sub_4C611   endp


; =============== S U B R O U T I N E =======================================


vgaloadgfxseg:
        push    ds
        mov ax, seg gfxseg

; START OF FUNCTION CHUNK FOR vgaloadbackseg

vgaloadseginax:              ; CODE XREF: vgaloadbackseg+4j code:5AD9j
        mov ds, ax
        assume ds:nothing
        mov si, 0
        mov di, 4D84h
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        mov cx, 0C8h ; '?'

loc_4C6AD:              ; CODE XREF: vgaloadbackseg-22j
        push(cx);
        mov ah, 1

loc_4C6B0:              ; CODE XREF: vgaloadbackseg-28j
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = ah
        out dx, al      ; EGA port: sequencer data register
        mov cx, 28h ; '('
        rep movsb
        sub di, 28h ; '('
        shl ah, 1
        test    ah, 0Fh
        jnz short loc_4C6B0
        add di, 7Ah ; 'z'
        pop(cx);
        loop    loc_4C6AD
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: sequencer data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        pop ds
        assume ds:data
        retn
; END OF FUNCTION CHUNK FOR vgaloadbackseg

; =============== S U B R O U T I N E =======================================


vgaloadbackseg:
        push    ds
        mov ax, backseg
        jmp short vgaloadseginax

// ; ---------------------------------------------------------------------------
vgaloadcontrolsseg:
        push ds
        mov ax, controlsseg
        jmp short vgaloadseginax
// ; ---------------------------------------------------------------------------
loc_4C6FB:
        call    sub_4C705
        call    sound?4
        call    sub_4CAFC
        retn

; =============== S U B R O U T I N E =======================================


sub_4C705   proc near       ; CODE XREF: code:5ADBp
        call    loadBeep
        call    sound?4
        call    sub_4CAFC
        retn
sub_4C705   endp


; =============== S U B R O U T I N E =======================================


sub_4C70F   proc near
        call    loadBeep2
        call    sound?4
        call    sub_4CAFC
        retn
sub_4C70F   endp


; =============== S U B R O U T I N E =======================================


sub_4C719   proc near
        call    loadBlaster
        call    sound?4
        call    sub_4CAFC
        retn
sub_4C719   endp


; =============== S U B R O U T I N E =======================================


sub_4C723   proc near
        call    loadAdlib
        call    sound?4
        call    sub_4CAFC
        retn
sub_4C723   endp


; =============== S U B R O U T I N E =======================================


sub_4C72D   proc near
        call    loadRoland
        call    sound?4
        call    sub_4CAFC
        retn
sub_4C72D   endp


; =============== S U B R O U T I N E =======================================


sub_4C737   proc near
        call    loadCombined
        call    sound?4
        call    sub_4CAFC
        retn
sub_4C737   endp


; =============== S U B R O U T I N E =======================================


sub_4C741   proc near
        cmp byte_59886, 1
        jnz short loc_4C752
        call    sound?3
        mov byte_59886, 0
        jmp short loc_4C75A
// ; ---------------------------------------------------------------------------

loc_4C752:              ; CODE XREF: sub_4C741+5j
        mov byte_59886, 1
        call    sound?2

loc_4C75A:              ; CODE XREF: sub_4C741+Fj
        call    sub_4CC7C
        retn
sub_4C741   endp

// ; ---------------------------------------------------------------------------
loc_4C75E:
        cmp byte_59885, 1
        jnz short loc_4C76C
        mov byte_59885, 0
        jmp short loc_4C774
// ; ---------------------------------------------------------------------------

loc_4C76C:              ; CODE XREF: code:5B43j
        mov byte_59885, 1
        call    sound?4

loc_4C774:              ; CODE XREF: code:5B4Aj
        call    sub_4CC7C
        retn
// ; ---------------------------------------------------------------------------
loc_4C778:
        mov byte_50940, 0
        call    sub_4CCDF
        retn
// ; ---------------------------------------------------------------------------
loc_4C781:
        mov byte_50940, 1
        call    sub_4921B
        call    sub_4CCDF
        retn
// ; ---------------------------------------------------------------------------
loc_4C78D:
        mov word_58463, 1
        retn

; =============== S U B R O U T I N E =======================================


runMainMenu proc near       ; CODE XREF: start+43Ap
        mov byte_5A33E, 0
        mov word_599D8, 0
        mov byte_599D4, 0
        mov word_58465, 0EF98h
        cmp word_58467, 0
        jz  short loc_4C7EC
        call    sub_4C611
        mov byte_51ABE, 1
        call    sub_4C34A
        call    sub_4C2F2
        mov bx, 4D5Ch
        mov dx, 3D4h
        al = 0Dh
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (low)
        inc dx
        al = bl
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 0Ch
        out dx, al      ; Video: CRT cntrlr addr
                    ; regen start address (high)
        inc dx
        al = bh
        out dx, al      ; Video: CRT controller internal registers
        mov byte_510A6, 0
        call    videoloop
        mov si, 6015h
        call    fade
        mov word_58467, 0
        jmp short loc_4C7F4
// ; ---------------------------------------------------------------------------

loc_4C7EC:              ; CODE XREF: runMainMenu+1Bj
        mov byte_59B83, 1
        call    sub_4C407

loc_4C7F4:              ; CODE XREF: runMainMenu+56j
        call    sound?2
        call    sub_4B85C
        call    sub_4B8BE

loc_4C7FD:              ; CODE XREF: runMainMenu+121j
                    ; runMainMenu+219j ...
        inc word_58465
        jnz short loc_4C806
        call    sub_4B159

loc_4C806:              ; CODE XREF: runMainMenu+6Dj
        cmp word_5196C, 0
        jz  short loc_4C81A
        mov word_5196C, 0
        call    sub_4CFB2
        call    sub_4CFDB
        retn
// ; ---------------------------------------------------------------------------

loc_4C81A:              ; CODE XREF: runMainMenu+77j
        call    videoloop
        inc word_5195D
        call    getMouseStatus
        mov word_5847D, bx
        cmp mousex, cx
        jnz short loc_4C834
        cmp mousey, dx
        jz  short loc_4C83A

loc_4C834:              ; CODE XREF: runMainMenu+98j
        mov word_58465, 0EF98h

loc_4C83A:              ; CODE XREF: runMainMenu+9Ej
        mov mousex, cx
        mov mousey, dx
        call    sub_4B899
        call    sub_4B85C
        call    sub_4B8BE
        call    sub_4D0AD
        call    sub_48E59
        call    sub_4A1BF
        cmp byte_50910, 0
        jnz short loc_4C862
        cmp byte_50911, 0
        jz  short loc_4C867

loc_4C862:              ; CODE XREF: runMainMenu+C5j
        mov byte_50912, 1

loc_4C867:              ; CODE XREF: runMainMenu+CCj
        mov byte_50910, 0
        mov byte_50911, 0
        cmp byte_50913, 0
        jnz short loc_4C87F
        cmp byte_50914, 0
        jz  short loc_4C884

loc_4C87F:              ; CODE XREF: runMainMenu+E2j
        mov byte_50915, 1

loc_4C884:              ; CODE XREF: runMainMenu+E9j
        mov byte_50913, 0
        mov byte_50914, 0
        cmp byte_50916, 0
        jnz short loc_4C89C
        cmp byte_50917, 0
        jz  short loc_4C8A1

loc_4C89C:              ; CODE XREF: runMainMenu+FFj
        mov byte_50918, 1

loc_4C8A1:              ; CODE XREF: runMainMenu+106j
        mov byte_50916, 0
        mov byte_50917, 0
        cmp byte_50941, 4
        jle short loc_4C8B8
        call    sub_4B375
        jmp loc_4C7FD
// ; ---------------------------------------------------------------------------

loc_4C8B8:              ; CODE XREF: runMainMenu+11Cj
        cmp byte_519B8, 1
        jnz short loc_4C8C8
        mov ax, 0
        call    demoSomething?
        jmp loc_4C9B0
// ; ---------------------------------------------------------------------------

loc_4C8C8:              ; CODE XREF: runMainMenu+129j
        cmp byte_519B9, 1
        jnz short loc_4C8D8
        mov ax, 1
        call    demoSomething?
        jmp loc_4C9B0
// ; ---------------------------------------------------------------------------

loc_4C8D8:              ; CODE XREF: runMainMenu+139j
        cmp byte_519BA, 1
        jnz short loc_4C8E8
        mov ax, 2
        call    demoSomething?
        jmp loc_4C9B0
// ; ---------------------------------------------------------------------------

loc_4C8E8:              ; CODE XREF: runMainMenu+149j
        cmp byte_519BB, 1
        jnz short loc_4C8F8
        mov ax, 3
        call    demoSomething?
        jmp loc_4C9B0
// ; ---------------------------------------------------------------------------

loc_4C8F8:              ; CODE XREF: runMainMenu+159j
        cmp byte_519BC, 1
        jnz short loc_4C908
        mov ax, 4
        call    demoSomething?
        jmp loc_4C9B0
// ; ---------------------------------------------------------------------------

loc_4C908:              ; CODE XREF: runMainMenu+169j
        cmp byte_519BD, 1
        jnz short loc_4C918
        mov ax, 5
        call    demoSomething?
        jmp loc_4C9B0
// ; ---------------------------------------------------------------------------

loc_4C918:              ; CODE XREF: runMainMenu+179j
        cmp byte_519BE, 1
        jnz short loc_4C928
        mov ax, 6
        call    demoSomething?
        jmp loc_4C9B0
// ; ---------------------------------------------------------------------------

loc_4C928:              ; CODE XREF: runMainMenu+189j
        cmp byte_519BF, 1
        jnz short loc_4C937
        mov ax, 7
        call    demoSomething?
        jmp short loc_4C9B0
// ; ---------------------------------------------------------------------------

loc_4C937:              ; CODE XREF: runMainMenu+199j
        cmp byte_519C0, 1
        jnz short loc_4C946
        mov ax, 8
        call    demoSomething?
        jmp short loc_4C9B0
// ; ---------------------------------------------------------------------------

loc_4C946:              ; CODE XREF: runMainMenu+1A8j
        cmp byte_519C1, 1
        jnz short loc_4C955
        mov ax, 9
        call    demoSomething?
        jmp short loc_4C9B0
// ; ---------------------------------------------------------------------------

loc_4C955:              ; CODE XREF: runMainMenu+1B7j
        cmp byte_519D4, 1
        jnz short loc_4C977
        cmp demoFileName, 0
        jz  short loc_4C977
        cmp byte_599D5, 1
        jnz short loc_4C977
        mov byte_599D4, 1
        mov ax, 0
        call    demoSomething?
        jmp short loc_4C9B0
// ; ---------------------------------------------------------------------------

loc_4C977:              ; CODE XREF: runMainMenu+1C6j
                    ; runMainMenu+1CDj ...
        cmp byte_519D5, 1
        jnz short loc_4C9B0
        cmp demoFileName, 0
        jz  short loc_4C9B0
        mov byte_599D4, 1
        mov word_5196C, 1
        mov byte_510DE, 0
        mov byte_510B3, 0
        mov byte_5A2F9, 1
        call    sub_49544
        mov byte ptr a00s0010_sp+3, 2Dh ; '-' ; "001$0.SP"
        mov word ptr a00s0010_sp+4, 2D2Dh ; "01$0.SP"
        jmp loc_4C7FD
// ; ---------------------------------------------------------------------------

loc_4C9B0:              ; CODE XREF: runMainMenu+131j
                    ; runMainMenu+141j ...
        mov bx, word_5847D
        cmp bx, 2
        jz  short loc_4CA34
        cmp byte_5197E, 1
        jz  short loc_4CA34
        cmp word_5197A, 1
        jz  short loc_4CA34
        cmp bx, 1
        jz  short loc_4C9FF
        mov word_58469, 10h
        mov word_5846B, 0
        mov word_5846D, 10h
        mov word_5846F, 0
        mov word_58471, 10h
        mov word_58473, 0
        mov word_59B8C, 10h
        mov word_59B8E, 0
        jmp loc_4C7FD
// ; ---------------------------------------------------------------------------

loc_4C9FF:              ; CODE XREF: runMainMenu+236j
        mov word_58465, 0EF98h
        mov cx, mousex
        mov dx, mousey
        mov si, offset menudata

checkmousecoords:              ; CODE XREF: runMainMenu+29Bj
        cmp [si], cx
        jg  short nomousehit
        cmp [si+2], dx
        jg  short nomousehit
        cmp [si+4], cx
        jl  short nomousehit
        cmp [si+6], dx
        jl  short nomousehit
        call [si+8]
        jmp loc_4C7FD
// ; ---------------------------------------------------------------------------

nomousehit:              ; CODE XREF: runMainMenu+27Ej
                    ; runMainMenu+283j ...
        add si, 0Ah
        cmp word ptr [si], 0FFFFh
        jnz short checkmousecoords
        jmp loc_4C7FD
// ; ---------------------------------------------------------------------------

loc_4CA34:              ; CODE XREF: runMainMenu+223j
                    ; runMainMenu+22Aj ...
        mov word_5197A, 1
        call    sub_4CFB2
        call    sub_4CFDB
        retn
runMainMenu endp

// ; ---------------------------------------------------------------------------

showControls:                              ; DATA XREF: data:0044o
                mov     byte_50919, 0FFh
                call    vgaloadcontrolsseg
                call    sub_4CAFC
                call    sub_4CC7C
                call    sub_4CCDF
                mov     si, 6055h
                call    sub_4D836
                call    sub_4C5AF  ; DO SLIDE
                mov     word_58463, 0
                call    sub_4B85C
                call    sub_4B8BE

loc_4CA67:                              ; CODE XREF: code:5E89j
                                        ; code:5EBFj ...
                call    videoloop
                call    sub_4CD3C
                inc     word_5195D
                call    getMouseStatus
                mov     word_5847D, bx
                add     cx, 140h
                mov     mousex, cx
                mov     mousey, dx
                call    sub_4B899
                call    sub_4B85C
                call    sub_4B8BE
                mov     bx, word_5847D
                cmp     bx, 2
                jz      short loc_4CAEC
                cmp     byte_5197E, 1
                jz      short loc_4CAEC
                cmp     word_58463, 1
                jz      short loc_4CAEC
                cmp     bx, 1
                jz      short loc_4CAAB
                jmp     short loc_4CA67
// ; ---------------------------------------------------------------------------

loc_4CAAB:                              ; CODE XREF: code:5E87j
                mov     cx, mousex
                sub     cx, 140h
                mov     dx, mousey
                mov     si, offset controlsbuttons ; 0ACh

loc_4CABA:                              ; CODE XREF: code:5EC7j
                cmp     [si], cx
                jg      short loc_4CAE1
                cmp     [si+2], dx
                jg      short loc_4CAE1
                cmp     [si+4], cx
                jl      short loc_4CAE1
                cmp     [si+6], dx
                jl      short loc_4CAE1
                call    word ptr [si+8]

loc_4CAD0:                              ; CODE XREF: code:5EBDj
                call    videoloop
                inc     word_5195D
                call    getMouseStatus
                cmp     bx, 0
                jnz     short loc_4CAD0
                jmp     short loc_4CA67
// ; ---------------------------------------------------------------------------

loc_4CAE1:                              ; CODE XREF: code:5E9Cj
                                        ; code:5EA1j ...
                add     si, 0Ah
                cmp     word ptr [si], 0FFFFh
                jnz     short loc_4CABA
                jmp     loc_4CA67
// ; ---------------------------------------------------------------------------

loc_4CAEC:                              ; CODE XREF: code:5E74j
                                        ; code:5E7Bj ...
                call    sub_4755A
                call    loc_4C44F
                call    sub_4C2F2
                mov     si, 6015h
                call    sub_4D836
                retn

; =============== S U B R O U T I N E =======================================


sub_4CAFC   proc near       ; CODE XREF: code:5AE1p sub_4C705+6p ...
        mov si, 578Bh
        mov cx, 5
        mov dx, 8
        call    sub_4CE9C
        mov ah, 4
        mov si, 130h
        call    sub_4CF13
        mov ah, 4
        mov si, 161h
        call    sub_4CF13
        mov si, 68B1h
        mov cx, 9
        mov dx, 8
        call    sub_4CE9C
        mov ah, 4
        mov si, 1A7h
        call    sub_4CF13
        mov ah, 4
        mov si, 1CAh
        call    sub_4CF13
        mov si, 79DAh
        mov cx, 7
        mov dx, 8
        call    sub_4CE9C
        mov ah, 4
        mov si, 1F4h
        call    sub_4CF13
        mov ah, 4
        mov si, 225h
        call    sub_4CF13
        mov si, 8B01h
        mov cx, 8
        mov dx, 8
        call    sub_4CE9C
        mov si, 5629h
        mov cx, 9
        mov dx, 8
        call    sub_4CE9C
        mov ah, 4
        mov si, 2E2h
        call    sub_4CF13
        mov si, 6380h
        mov cx, 5
        mov dx, 5
        call    sub_4CE9C
        mov ah, 4
        mov si, 328h
        call    sub_4CF13
        mov si, 6386h
        mov cx, 5
        mov dx, 5
        call    sub_4CE9C
        mov ah, 4
        mov si, 36Eh
        call    sub_4CF13
        mov ah, 4
        mov si, 28Eh
        call    sub_4CF13
        cmp sndType, 3
        jnz short loc_4CBC6
        mov si, 578Bh
        mov cx, 5
        mov dx, 8
        call    sub_4CE11
        mov ah, 6
        mov si, 130h
        call    sub_4CF13
        mov ah, 6
        mov si, 161h
        call    sub_4CF13
        jmp locret_4CC7B
// ; ---------------------------------------------------------------------------

loc_4CBC6:              ; CODE XREF: sub_4CAFC+A9j
        cmp sndType, 4
        jnz short loc_4CC11
        mov ah, 6
        mov si, 1CAh
        call    sub_4CF13
        cmp musType, 3
        jnz short loc_4CBF3
        mov si, 68B1h
        mov cx, 9
        mov dx, 8
        call    sub_4CE11
        mov ah, 6
        mov si, 1A7h
        call    sub_4CF13
        jmp locret_4CC7B
// ; ---------------------------------------------------------------------------

loc_4CBF3:              ; CODE XREF: sub_4CAFC+DEj
        mov si, 8B01h
        mov cx, 8
        mov dx, 8
        call    sub_4CE11
        mov ah, 6
        mov si, 1F4h
        call    sub_4CF13
        mov ah, 6
        mov si, 28Eh
        call    sub_4CF13
        jmp short locret_4CC7B
// ; ---------------------------------------------------------------------------

loc_4CC11:              ; CODE XREF: sub_4CAFC+CFj
        cmp sndType, 5
        jnz short loc_4CC36
        mov si, 79DAh
        mov cx, 7
        mov dx, 8
        call    sub_4CE11
        mov ah, 6
        mov si, 1F4h
        call    sub_4CF13
        mov ah, 6
        mov si, 225h
        call    sub_4CF13
        jmp short locret_4CC7B
// ; ---------------------------------------------------------------------------

loc_4CC36:              ; CODE XREF: sub_4CAFC+11Aj
        mov si, 5629h
        mov cx, 9
        mov dx, 8
        call    sub_4CE11
        mov ah, 6
        mov si, 2E2h
        call    sub_4CF13
        cmp sndType, 1
        jnz short loc_4CC67
        mov si, 6380h
        mov cx, 5
        mov dx, 5
        call    sub_4CE11
        mov ah, 6
        mov si, 328h
        call    sub_4CF13
        jmp short locret_4CC7B
// ; ---------------------------------------------------------------------------

loc_4CC67:              ; CODE XREF: sub_4CAFC+153j
        mov si, 6386h
        mov cx, 5
        mov dx, 5
        call    sub_4CE11
        mov ah, 6
        mov si, 36Eh
        call    sub_4CF13

locret_4CC7B:               ; CODE XREF: sub_4CAFC+C7j
                    ; sub_4CAFC+F4j ...
        retn
sub_4CAFC   endp


; =============== S U B R O U T I N E =======================================


sub_4CC7C   proc near       ; CODE XREF: sub_4C741:loc_4C75Ap
                    ; code:loc_4C774p
        cmp byte_59886, 1
        jnz short loc_4CC99
        mov si, 7CC3h
        mov cx, 5
        mov dx, 8
        call    sub_4CE11
        mov ah, 6
        mov si, 3B4h
        call    sub_4CF13
        jmp short loc_4CCAD
// ; ---------------------------------------------------------------------------

loc_4CC99:              ; CODE XREF: sub_4CC7C+5j
        mov si, 7CC3h
        mov cx, 5
        mov dx, 8
        call    sub_4CE9C
        mov ah, 4
        mov si, 3B4h
        call    sub_4CF13

loc_4CCAD:              ; CODE XREF: sub_4CC7C+1Bj
        cmp byte_59885, 1
        jnz short loc_4CCCA
        mov si, 8F59h
        mov cx, 3
        mov dx, 8
        call    sub_4CE11
        mov ah, 6
        mov si, 3DEh
        call    sub_4CF13
        jmp short locret_4CCDE
// ; ---------------------------------------------------------------------------

loc_4CCCA:              ; CODE XREF: sub_4CC7C+36j
        mov si, 8F59h
        mov cx, 3
        mov dx, 8
        call    sub_4CE9C
        mov ah, 4
        mov si, 3DEh
        call    sub_4CF13

locret_4CCDE:               ; CODE XREF: sub_4CC7C+4Cj
        retn
sub_4CC7C   endp


; =============== S U B R O U T I N E =======================================


sub_4CCDF   proc near       ; CODE XREF: code:5B5Dp code:5B69p
        cmp byte_50940, 0
        jnz short loc_4CD10
        mov si, 7714h
        mov cx, 1
        mov dx, 3Eh ; '>'
        call    sub_4CE11
        mov si, 7792h
        mov cx, 1
        mov dx, 3Ah ; ':'
        call    sub_4CE9C
        mov ah, 4
        mov si, 478h
        call    sub_4CF13
        mov ah, 6
        mov si, 4E1h
        call    sub_4CF13
        jmp short loc_4CD38
// ; ---------------------------------------------------------------------------

loc_4CD10:              ; CODE XREF: sub_4CCDF+5j
        mov si, 7714h
        mov cx, 1
        mov dx, 3Eh ; '>'
        call    sub_4CE9C
        mov si, 7792h
        mov cx, 1
        mov dx, 3Ah ; ':'
        call    sub_4CE11
        mov ah, 6
        mov si, 478h
        call    sub_4CF13
        mov ah, 4
        mov si, 4E1h
        call    sub_4CF13

loc_4CD38:              ; CODE XREF: sub_4CCDF+2Fj
        call    sub_4CD3C
        retn
sub_4CCDF   endp


; =============== S U B R O U T I N E =======================================


sub_4CD3C   proc near       ; CODE XREF: sub_4CCDF:loc_4CD38p
        call    sub_48E59
        call    sub_4A1BF
        mov bl, byte_50941
        cmp bl, byte_50919
        jnz short loc_4CD4D
        retn
// ; ---------------------------------------------------------------------------

loc_4CD4D:              ; CODE XREF: sub_4CD3C+Ej
        mov byte_50919, bl
        cmp byte_50941, 0
        jnz short loc_4CD6A
        mov ah, 6
        mov si, 424h
        call    sub_4CF13
        mov ah, 4
        mov si, 46Ah
        call    sub_4CF13
        jmp short loc_4CDAE
// ; ---------------------------------------------------------------------------

loc_4CD6A:              ; CODE XREF: sub_4CD3C+1Aj
        cmp byte_50941, 4
        jbe short loc_4CD9E
        mov ah, 6
        mov si, 46Ah
        call    sub_4CF13
        cmp byte_50941, 9
        jnz short loc_4CD8F
        mov byte_50941, 0
        mov ah, 6
        mov si, 424h
        call    sub_4CF13
        jmp short loc_4CDAE
// ; ---------------------------------------------------------------------------

loc_4CD8F:              ; CODE XREF: sub_4CD3C+42j
        sub byte_50941, 4
        mov ah, 4
        mov si, 424h
        call    sub_4CF13
        jmp short loc_4CDAE
// ; ---------------------------------------------------------------------------

loc_4CD9E:              ; CODE XREF: sub_4CD3C+33j
        mov ah, 4
        mov si, 424h
        call    sub_4CF13
        mov ah, 4
        mov si, 46Ah
        call    sub_4CF13

loc_4CDAE:              ; CODE XREF: sub_4CD3C+2Cj
                    ; sub_4CD3C+51j ...
        mov ah, 4
        mov si, 432h
        call    sub_4CF13
        mov ah, 4
        mov si, 440h
        call    sub_4CF13
        mov ah, 4
        mov si, 44Eh
        call    sub_4CF13
        mov ah, 4
        mov si, 45Ch
        call    sub_4CF13
        cmp byte_50941, 1
        jnz short loc_4CDDF
        mov ah, 6
        mov si, 432h
        call    sub_4CF13
        jmp short locret_4CE10
// ; ---------------------------------------------------------------------------

loc_4CDDF:              ; CODE XREF: sub_4CD3C+97j
        cmp byte_50941, 2
        jnz short loc_4CDF0
        mov ah, 6
        mov si, 440h
        call    sub_4CF13
        jmp short locret_4CE10
// ; ---------------------------------------------------------------------------

loc_4CDF0:              ; CODE XREF: sub_4CD3C+A8j
        cmp byte_50941, 3
        jnz short loc_4CE01
        mov ah, 6
        mov si, 44Eh
        call    sub_4CF13
        jmp short locret_4CE10
// ; ---------------------------------------------------------------------------

loc_4CE01:              ; CODE XREF: sub_4CD3C+B9j
        cmp byte_50941, 4
        jnz short locret_4CE10
        mov ah, 6
        mov si, 45Ch
        call    sub_4CF13

locret_4CE10:               ; CODE XREF: sub_4CD3C+A1j
                    ; sub_4CD3C+B2j ...
        retn
sub_4CD3C   endp


; =============== S U B R O U T I N E =======================================

; Attributes: bp-based frame

sub_4CE11   proc near       ; CODE XREF: sub_4CAFC+B4p
                    ; sub_4CAFC+E9p ...

var_2       = word ptr -2

        push    bp
        mov bp, sp
        add sp, 0FFFEh
        push    si
        push(cx);
        push    dx
        call    sub_4B899
        pop dx
        pop(cx);
        pop si
        mov bx, dx
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 8
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 2
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; color compare.
                    ; Data bits 0-3 select color for read mode 01
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 7
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; color masking disable
                    ; bits 0-3 disable planes from compare logic in read mode 01
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov di, si
        push    ds
        mov ax, es
        mov ds, ax
        mov [bp+var_2], cx
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register

loc_4CE68:              ; CODE XREF: sub_4CE11+5Aj
                    ; sub_4CE11+6Aj
        lodsb
        out dx, al      ; EGA port: graphics controller data register
        stosb
        loop    loc_4CE68
        mov cx, [bp+var_2]
        add si, 7Ah ; 'z'
        sub si, cx
        add di, 7Ah ; 'z'
        sub di, cx
        dec bx
        jnz short loc_4CE68
        pop ds
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        call    sub_4B85C
        call    sub_4B8BE
        mov sp, bp
        pop bp
        retn
sub_4CE11   endp


; =============== S U B R O U T I N E =======================================

; Attributes: bp-based frame

sub_4CE9C   proc near       ; CODE XREF: sub_4CAFC+9p
                    ; sub_4CAFC+25p ...

var_2       = word ptr -2

        push    bp
        mov bp, sp
        add sp, 0FFFEh
        push    si
        push(cx);
        push    dx
        call    sub_4B899
        pop dx
        pop(cx);
        pop si
        mov bx, dx
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 8
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 2
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; color compare.
                    ; Data bits 0-3 select color for read mode 01
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 7
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; color masking disable
                    ; bits 0-3 disable planes from compare logic in read mode 01
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov di, si
        push    ds
        mov ax, es
        mov ds, ax
        mov [bp+var_2], cx
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register

loc_4CEF3:              ; CODE XREF: sub_4CE9C+5Aj
                    ; sub_4CE9C+6Aj
        lodsb
        out dx, al      ; EGA port: graphics controller data register
        stosb
        loop    loc_4CEF3
        mov cx, [bp+var_2]
        add si, 7Ah ; 'z'
        sub si, cx
        add di, 7Ah ; 'z'
        sub di, cx
        dec bx
        jnz short loc_4CEF3
        pop ds
        call    sub_4B85C
        call    sub_4B8BE
        mov sp, bp
        pop bp
        retn
sub_4CE9C   endp


; =============== S U B R O U T I N E =======================================


sub_4CF13   proc near       ; CODE XREF: sub_4CAFC+11p
                    ; sub_4CAFC+19p ...
        push    si
        push    ax
        call    sub_4B899
        pop ax
        pop si
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register

loc_4CF38:              ; CODE XREF: sub_4CF13+96j
        mov bl, [si]
        cmp bl, 0FFh
        jz  short loc_4CFAB
        mov di, 4D84h
        mov ax, [si+3]
        mov cx, 7Ah ; 'z'
        mul cx
        add di, ax
        mov dx, [si+1]
        mov cx, dx
        shr dx, 1
        shr dx, 1
        shr dx, 1
        add di, dx
        and cl, 7
        mov ah, 80h ; '?'
        shr ah, cl
        mov cx, [si+5]
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        al = ah

loc_4CF6F:              ; CODE XREF: sub_4CF13:loc_4CFA4j
        out dx, al      ; EGA port: graphics controller data register
        xor es:[di], al
        or  bl, bl
        jnz short loc_4CF7E
        ror al, 1
        jnb short loc_4CF7C
        inc di

loc_4CF7C:              ; CODE XREF: sub_4CF13+66j
        jmp short loc_4CFA4
// ; ---------------------------------------------------------------------------

loc_4CF7E:              ; CODE XREF: sub_4CF13+62j
        cmp bl, 1
        jnz short loc_4CF88
        sub di, 7Ah ; 'z'
        jmp short loc_4CFA4
// ; ---------------------------------------------------------------------------

loc_4CF88:              ; CODE XREF: sub_4CF13+6Ej
        cmp bl, 2
        jnz short loc_4CF97
        sub di, 7Ah ; 'z'
        ror al, 1
        jnb short loc_4CF95
        inc di

loc_4CF95:              ; CODE XREF: sub_4CF13+7Fj
        jmp short loc_4CFA4
// ; ---------------------------------------------------------------------------

loc_4CF97:              ; CODE XREF: sub_4CF13+78j
        cmp bl, 3
        jnz short loc_4CFA4
        add di, 7Ah ; 'z'
        ror al, 1
        jnb short loc_4CFA4
        inc di

loc_4CFA4:              ; CODE XREF: sub_4CF13:loc_4CF7Cj
                    ; sub_4CF13+73j ...
        loop    loc_4CF6F
        add si, 7
        jmp short loc_4CF38
// ; ---------------------------------------------------------------------------

loc_4CFAB:              ; CODE XREF: sub_4CF13+2Aj
        call    sub_4B85C
        call    sub_4B8BE
        retn
sub_4CF13   endp


; =============== S U B R O U T I N E =======================================


sub_4CFB2   proc near       ; CODE XREF: sub_4AB1B+1D5p
                    ; sub_4AD0E+CEp ...
        cmp byte_59B85, 0
        jnz short locret_4CFDA
        mov ax, 3C00h
        mov cx, 2
        mov dx, 37E5h
        int 21h     ; DOS - 2+ - CREATE A FILE WITH HANDLE (CREAT)
                    ; CX = attributes for file
                    ; DS:DX -> ASCIZ filename (may include drive and path)
        jb  short locret_4CFDA
        mov bx, ax
        mov ax, 4000h
        mov cx, 0A00h
        mov dx, 8A9Ch
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        jb  short locret_4CFDA
        mov ax, 3E00h
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle

locret_4CFDA:               ; CODE XREF: sub_4CFB2+5j
                    ; sub_4CFB2+12j ...
        retn
sub_4CFB2   endp


; =============== S U B R O U T I N E =======================================


sub_4CFDB   proc near       ; CODE XREF: sub_4AB1B+1D8p
                    ; sub_4AD0E+D1p ...
        cmp byte_59B85, 0
        jnz short locret_4D003
        mov ax, 3C00h
        mov cx, 2
        mov dx, 37FDh
        int 21h     ; DOS - 2+ - CREATE A FILE WITH HANDLE (CREAT)
                    ; CX = attributes for file
                    ; DS:DX -> ASCIZ filename (may include drive and path)
        jb  short locret_4D003
        mov bx, ax
        mov ax, 4000h
        mov cx, 24h ; '$'
        mov dx, 9514h
        int 21h     ; DOS - 2+ - WRITE TO FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to write, DS:DX -> buffer
        jb  short locret_4D003
        mov ax, 3E00h
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle

locret_4D003:               ; CODE XREF: sub_4CFDB+5j
                    ; sub_4CFDB+12j ...
        retn
sub_4CFDB   endp


; =============== S U B R O U T I N E =======================================


sub_4D004   proc near       ; CODE XREF: sub_4D0AD+17p
                    ; sub_4D0AD+2Ap ...
        push    si
        push    ax
        call    sub_4B899
        pop ax
        pop si
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register

loc_4D029:              ; CODE XREF: sub_4D004+96j
        mov bl, [si]
        cmp bl, 0FFh
        jz  short loc_4D09C
        mov di, 4D5Ch
        mov ax, [si+3]
        mov cx, 7Ah ; 'z'
        mul cx
        add di, ax
        mov dx, [si+1]
        mov cx, dx
        shr dx, 1
        shr dx, 1
        shr dx, 1
        add di, dx
        and cl, 7
        mov ah, 80h ; '?'
        shr ah, cl
        mov cx, [si+5]
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        al = ah

loc_4D060:              ; CODE XREF: sub_4D004:loc_4D095j
        out dx, al      ; EGA port: graphics controller data register
        xor es:[di], al
        or  bl, bl
        jnz short loc_4D06F
        ror al, 1
        jnb short loc_4D06D
        inc di

loc_4D06D:              ; CODE XREF: sub_4D004+66j
        jmp short loc_4D095
// ; ---------------------------------------------------------------------------

loc_4D06F:              ; CODE XREF: sub_4D004+62j
        cmp bl, 1
        jnz short loc_4D079
        sub di, 7Ah ; 'z'
        jmp short loc_4D095
// ; ---------------------------------------------------------------------------

loc_4D079:              ; CODE XREF: sub_4D004+6Ej
        cmp bl, 2
        jnz short loc_4D088
        sub di, 7Ah ; 'z'
        ror al, 1
        jnb short loc_4D086
        inc di

loc_4D086:              ; CODE XREF: sub_4D004+7Fj
        jmp short loc_4D095
// ; ---------------------------------------------------------------------------

loc_4D088:              ; CODE XREF: sub_4D004+78j
        cmp bl, 3
        jnz short loc_4D095
        add di, 7Ah ; 'z'
        ror al, 1
        jnb short loc_4D095
        inc di

loc_4D095:              ; CODE XREF: sub_4D004:loc_4D06Dj
                    ; sub_4D004+73j ...
        loop    loc_4D060
        add si, 7
        jmp short loc_4D029
// ; ---------------------------------------------------------------------------

loc_4D09C:              ; CODE XREF: sub_4D004+2Aj
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        call    sub_4B85C
        call    sub_4B8BE
        retn
sub_4D004   endp


; =============== S U B R O U T I N E =======================================


sub_4D0AD   proc near       ; CODE XREF: runMainMenu+B7p
        cmp byte_50912, 0
        jz  short loc_4D105
        cmp byte_50911, 0
        jz  short loc_4D0BF
        mov ah, 0Dh
        jmp short loc_4D0C1
// ; ---------------------------------------------------------------------------

loc_4D0BF:              ; CODE XREF: sub_4D0AD+Cj
        mov ah, 7

loc_4D0C1:              ; CODE XREF: sub_4D0AD+10j
        mov si, 504h
        call    sub_4D004
        cmp byte_50911, 0
        jz  short loc_4D0D2
        mov ah, 7
        jmp short loc_4D0D4
// ; ---------------------------------------------------------------------------

loc_4D0D2:              ; CODE XREF: sub_4D0AD+1Fj
        mov ah, 0Dh

loc_4D0D4:              ; CODE XREF: sub_4D0AD+23j
        mov si, 519h
        call    sub_4D004
        cmp byte_50910, 0
        jz  short loc_4D0E5
        mov ah, 0Dh
        jmp short loc_4D0E7
// ; ---------------------------------------------------------------------------

loc_4D0E5:              ; CODE XREF: sub_4D0AD+32j
        mov ah, 7

loc_4D0E7:              ; CODE XREF: sub_4D0AD+36j
        mov si, 52Eh
        call    sub_4D004
        cmp byte_50910, 0
        jz  short loc_4D0F8
        mov ah, 7
        jmp short loc_4D0FA
// ; ---------------------------------------------------------------------------

loc_4D0F8:              ; CODE XREF: sub_4D0AD+45j
        mov ah, 0Dh

loc_4D0FA:              ; CODE XREF: sub_4D0AD+49j
        mov si, 543h
        call    sub_4D004
        mov byte_50912, 0

loc_4D105:              ; CODE XREF: sub_4D0AD+5j
        cmp byte_50915, 0
        jz  short loc_4D15D
        cmp byte_50914, 0
        jz  short loc_4D117
        mov ah, 0Dh
        jmp short loc_4D119
// ; ---------------------------------------------------------------------------

loc_4D117:              ; CODE XREF: sub_4D0AD+64j
        mov ah, 7

loc_4D119:              ; CODE XREF: sub_4D0AD+68j
        mov si, 558h
        call    sub_4D004
        cmp byte_50914, 0
        jz  short loc_4D12A
        mov ah, 7
        jmp short loc_4D12C
// ; ---------------------------------------------------------------------------

loc_4D12A:              ; CODE XREF: sub_4D0AD+77j
        mov ah, 0Dh

loc_4D12C:              ; CODE XREF: sub_4D0AD+7Bj
        mov si, 56Dh
        call    sub_4D004
        cmp byte_50913, 0
        jz  short loc_4D13D
        mov ah, 0Dh
        jmp short loc_4D13F
// ; ---------------------------------------------------------------------------

loc_4D13D:              ; CODE XREF: sub_4D0AD+8Aj
        mov ah, 7

loc_4D13F:              ; CODE XREF: sub_4D0AD+8Ej
        mov si, 582h
        call    sub_4D004
        cmp byte_50913, 0
        jz  short loc_4D150
        mov ah, 7
        jmp short loc_4D152
// ; ---------------------------------------------------------------------------

loc_4D150:              ; CODE XREF: sub_4D0AD+9Dj
        mov ah, 0Dh

loc_4D152:              ; CODE XREF: sub_4D0AD+A1j
        mov si, 597h
        call    sub_4D004
        mov byte_50915, 0

loc_4D15D:              ; CODE XREF: sub_4D0AD+5Dj
        cmp byte_50918, 0
        jz  short locret_4D1B5
        cmp byte_50917, 0
        jz  short loc_4D16F
        mov ah, 0Dh
        jmp short loc_4D171
// ; ---------------------------------------------------------------------------

loc_4D16F:              ; CODE XREF: sub_4D0AD+BCj
        mov ah, 7

loc_4D171:              ; CODE XREF: sub_4D0AD+C0j
        mov si, 5ACh
        call    sub_4D004
        cmp byte_50917, 0
        jz  short loc_4D182
        mov ah, 7
        jmp short loc_4D184
// ; ---------------------------------------------------------------------------

loc_4D182:              ; CODE XREF: sub_4D0AD+CFj
        mov ah, 0Dh

loc_4D184:              ; CODE XREF: sub_4D0AD+D3j
        mov si, 5C1h
        call    sub_4D004
        cmp byte_50916, 0
        jz  short loc_4D195
        mov ah, 0Dh
        jmp short loc_4D197
// ; ---------------------------------------------------------------------------

loc_4D195:              ; CODE XREF: sub_4D0AD+E2j
        mov ah, 7

loc_4D197:              ; CODE XREF: sub_4D0AD+E6j
        mov si, 5D6h
        call    sub_4D004
        cmp byte_50916, 0
        jz  short loc_4D1A8
        mov ah, 7
        jmp short loc_4D1AA
// ; ---------------------------------------------------------------------------

loc_4D1A8:              ; CODE XREF: sub_4D0AD+F5j
        mov ah, 0Dh

loc_4D1AA:              ; CODE XREF: sub_4D0AD+F9j
        mov si, 5EBh
        call    sub_4D004
        mov byte_50918, 0

locret_4D1B5:               ; CODE XREF: sub_4D0AD+B5j
        retn
sub_4D0AD   endp


; =============== S U B R O U T I N E =======================================


sub_4D1B6   proc near       ; CODE XREF: sub_4D24D+2Ep
        cmp byte_510DE, 0
        jz  short loc_4D1BE
        retn
// ; ---------------------------------------------------------------------------

loc_4D1BE:              ; CODE XREF: sub_4D1B6+5j
        mov bh, byte_5981F
        xor bl, bl
        shr bx, 1
        mov si, 8A9Ch
        add si, bx
        cmp byte ptr [si+7Fh], 0
        jz  short loc_4D1D2
        retn
// ; ---------------------------------------------------------------------------

loc_4D1D2:              ; CODE XREF: sub_4D1B6+19j
        push    es
        mov ax, ds
        mov es, ax
        assume es:data
        mov cx, 6Fh ; 'o'
        mov bx, 0
        mov di, si
        add di, 0Ch

loc_4D1E2:              ; CODE XREF: sub_4D1B6+33j
        cmp byte ptr [di], 1
        jnz short loc_4D1E8
        inc bx

loc_4D1E8:              ; CODE XREF: sub_4D1B6+2Fj
        inc di
        loop    loc_4D1E2
        cmp bx, 6Fh ; 'o'
        jnz short loc_4D24B
        mov byte ptr [si+7Fh], 1
        mov cx, 3
        mov di, 9514h

loc_4D1FA:              ; CODE XREF: sub_4D1B6+78j
        cmp byte ptr [di+9], 0
        jnz short loc_4D20E
        cmp byte ptr [di+0Ah], 0
        jnz short loc_4D20E
        cmp byte ptr [di+0Bh], 0
        jnz short loc_4D20E
        jmp short loc_4D232
// ; ---------------------------------------------------------------------------

loc_4D20E:              ; CODE XREF: sub_4D1B6+48j
                    ; sub_4D1B6+4Ej ...
        al = [si+9]
        cmp al, [di+9]
        jl  short loc_4D232
        jnz short loc_4D22A
        al = [si+0Ah]
        cmp al, [di+0Ah]
        jl  short loc_4D232
        jnz short loc_4D22A
        al = [si+0Bh]
        cmp al, [di+0Bh]
        jl  short loc_4D232

loc_4D22A:              ; CODE XREF: sub_4D1B6+60j
                    ; sub_4D1B6+6Aj
        add di, 0Ch
        dec cx
        jnz short loc_4D1FA
        jmp short loc_4D24B
// ; ---------------------------------------------------------------------------

loc_4D232:              ; CODE XREF: sub_4D1B6+56j
                    ; sub_4D1B6+5Ej ...
        push    si
        add di, 2Fh ; '/'
        mov si, di
        sub si, 0Ch
        std
        mov cx, 24h ; '$'
        rep movsb
        cld
        pop si
        sub di, 0Bh
        mov cx, 0Ch
        rep movsb

loc_4D24B:              ; CODE XREF: sub_4D1B6+38j
                    ; sub_4D1B6+7Aj
        pop es
        assume es:nothing
        retn
sub_4D1B6   endp


; =============== S U B R O U T I N E =======================================


sub_4D24D   proc near       ; CODE XREF: sub_4ADFF+D9p
                    ; update?:loc_4E6A4p
        cmp byte_510DE, 0
        jnz short locret_4D27E
        cmp byte_5A2F9, 0
        jnz short locret_4D27E
        al = byte_510BB
        mov byte_510BB, 0
        mov bh, byte_5981F
        xor bl, bl
        shr bx, 1
        mov si, 8AA8h
        add si, bx
        mov bx, word_51ABC
        dec bx
        mov [bx+si], al
        inc word_51ABC
        call    sub_4D1B6

locret_4D27E:               ; CODE XREF: sub_4D24D+5j sub_4D24D+Cj
        retn
sub_4D24D   endp

// ; ---------------------------------------------------------------------------
        nop

// ; =============== S U B R O U T I N E =======================================


void checkVideo() //  proc near       ; CODE XREF: start+282p
{
    ah = 0x0F;
        // mov ah, 0Fh
        // int 10h     ; - VIDEO - GET CURRENT VIDEO MODE
        //             ; Return: AH = number of columns on screen
        //             ; AL = current video mode
        //             ; BH = current active display page
    currVideoMode = al;
    ax = 0x0D;
        // int 10h     ; - VIDEO - SET VIDEO MODE
        //             ; AL = mode
    bx = 0;
    cx = 0x100;

videoCheckStart:            // ; CODE XREF: checkVideo:checkAgainj
    dx = 0x3CE;
    al = 8;
        // out dx, al      ; EGA: graph 1 and 2 addr reg:
        //             ; bit mask
        //             ; Bits 0-7 select bits to be masked in all planes
    dx++;
    al = cl;
        // out dx, al      ; EGA port: graphics controller data register
        // in  al, dx      ; EGA port: graphics controller data register
    if (al == cl)
    {
        goto checkAgain;
    }
    bx++;

checkAgain:             //; CODE XREF: checkVideo+29j
    cx--;
    if (cx > 0)
    {
        goto videoCheckStart;
    }
    
    if (bx != 0)
    {
        goto setStatusBit2;
    }
    
    videoStatusUnk = 1;
    goto returnout;
// ; ---------------------------------------------------------------------------

setStatusBit2:              // ; CODE XREF: checkVideo+30j
    videoStatusUnk = 2;

returnout:                  // ; CODE XREF: checkVideo+37j
        //retn
// checkVideo  endp
}


; =============== S U B R O U T I N E =======================================


sub_4D2BF   proc near       ; CODE XREF: start+296p
        cmp videoStatusUnk, 1
        jnz short loc_4D2DA
        mov cx, 10h

loc_4D2C9:              ; CODE XREF: sub_4D2BF+17j
        push(cx);
        mov ax, 1000h
        mov bl, cl
        dec bl
        mov bh, bl
        int 10h     ; - VIDEO - SET PALETTE REGISTER (Jr, PS, TANDY 1000, EGA, VGA)
                    ; BL = palette register to set
                    ; BH = color value to store
        pop(cx);
        loop    loc_4D2C9
        jmp short $+2
// ; ---------------------------------------------------------------------------

loc_4D2DA:              ; CODE XREF: sub_4D2BF+5j
                    ; sub_4D2BF+19j
        mov si, 60D5h
        call    fade
        retn
sub_4D2BF   endp


; =============== S U B R O U T I N E =======================================


sub_4D2E1   proc near       ; CODE XREF: start+450p
                    ; loadScreen2-7D7p
        mov ah, 0
        al = currVideoMode
        int 10h     ; - VIDEO - SET VIDEO MODE
                    ; AL = mode
        retn
sub_4D2E1   endp


; =============== S U B R O U T I N E =======================================


sub_4D2E9   proc near       ; CODE XREF: start+299p
        mov byte_58487, 0
        push    es
        mov ah, 35h ; '5'
        al = 33h ; '3'
        int 21h     ; DOS - 2+ - GET INTERRUPT VECTOR
                    ; AL = interrupt number
                    ; Return: ES:BX = value of interrupt vector
        mov ax, es
        or  ax, bx
        jz  short loc_4D33B
        cmp byte ptr es:[bx], 0CFh ; '?'
        jz  short loc_4D33B
        mov ax, 0
        int 33h     ; - MS MOUSE - RESET DRIVER AND READ STATUS
                    ; Return: AX = status
                    ; BX = number of buttons
        cmp bx, 2
        jz  short loc_4D310
        cmp bx, 3
        jnz short loc_4D33B

loc_4D310:              ; CODE XREF: sub_4D2E9+20j
        mov byte_58487, 1
        mov ax, 2
        int 33h     ; - MS MOUSE - HIDE MOUSE CURSOR
                    ; SeeAlso: AX=0001h, INT 16/AX=FFFFh
        mov ax, 7
        mov cx, 20h ; ' '
        mov dx, 260h
        int 33h     ; - MS MOUSE - DEFINE HORIZONTAL CURSOR RANGE
                    ; CX = minimum column, DX = maximum column
        mov ax, 8
        mov cx, 8
        mov dx, 0C0h ; '?'
        int 33h     ; - MS MOUSE - DEFINE VERTICAL CURSOR RANGE
                    ; CX = minimum row, DX = maximum row
        mov ax, 4
        mov cx, 140h
        mov dx, 64h ; 'd'
        int 33h     ; - MS MOUSE - POSITION MOUSE CURSOR
                    ; CX = column, DX = row

loc_4D33B:              ; CODE XREF: sub_4D2E9+10j
                    ; sub_4D2E9+16j ...
        mov word_58481, 0A0h ; '?'
        mov word_58485, 64h ; 'd'
        mov word ptr dword_58488, 1
        pop es
        retn
sub_4D2E9   endp


; =============== S U B R O U T I N E =======================================


getMouseStatus   proc near       ; CODE XREF: sub_47E98:mouseIsClickedp
                    ; sub_47E98+3Ep ...
        cmp byte_58487, 0
        jz  short loc_4D360
        mov ax, 3
        int 33h     ; - MS MOUSE - RETURN POSITION AND BUTTON STATUS
                    ; Return: BX = button status, CX = column, DX = row
        shr cx, 1
        jmp locret_4D40E
// ; ---------------------------------------------------------------------------

loc_4D360:              ; CODE XREF: getMouseStatus+5j
        xor bx, bx
        cmp byte_5197E, 1
        jnz short loc_4D36C
        mov bx, 2

loc_4D36C:              ; CODE XREF: getMouseStatus+18j
        cmp byte_51999, 1
        jnz short loc_4D376
        mov bx, 1

loc_4D376:              ; CODE XREF: getMouseStatus+22j
        mov cx, 1
        mov ax, word ptr dword_58488
        shr ax, 1
        shr ax, 1
        shr ax, 1
        inc ax
        cmp byte_519C5, 1
        jnz short loc_4D390
        sub word_58485, ax
        xor cx, cx

loc_4D390:              ; CODE XREF: getMouseStatus+39j
        cmp byte_519CD, 1
        jnz short loc_4D39D
        add word_58485, ax
        xor cx, cx

loc_4D39D:              ; CODE XREF: getMouseStatus+46j
        cmp byte_519C8, 1
        jnz short loc_4D3AA
        sub word_58481, ax
        xor cx, cx

loc_4D3AA:              ; CODE XREF: getMouseStatus+53j
        cmp byte_519CA, 1
        jnz short loc_4D3B7
        add word_58481, ax
        xor cx, cx

loc_4D3B7:              ; CODE XREF: getMouseStatus+60j
        or  cx, cx
        jnz short loc_4D3C1
        inc word ptr dword_58488
        jmp short loc_4D3C7
// ; ---------------------------------------------------------------------------

loc_4D3C1:              ; CODE XREF: getMouseStatus+6Aj
        mov word ptr dword_58488, 1

loc_4D3C7:              ; CODE XREF: getMouseStatus+70j
        cmp word ptr dword_58488, 40h ; '@'
        jbe short loc_4D3D4
        mov word ptr dword_58488, 40h ; '@'

loc_4D3D4:              ; CODE XREF: getMouseStatus+7Dj
        mov cx, word_58481
        mov dx, word_58485
        cmp cx, 10h
        jg  short loc_4D3E8
        mov cx, 10h
        mov word_58481, cx

loc_4D3E8:              ; CODE XREF: getMouseStatus+90j
        cmp cx, 130h
        jl  short loc_4D3F5
        mov cx, 130h
        mov word_58481, cx

loc_4D3F5:              ; CODE XREF: getMouseStatus+9Dj
        cmp dx, 8
        jg  short loc_4D401
        mov dx, 8
        mov word_58485, dx

loc_4D401:              ; CODE XREF: getMouseStatus+A9j
        cmp dx, 0C0h ; '?'
        jl  short locret_4D40E
        mov dx, 0C0h ; '?'
        mov word_58485, dx

locret_4D40E:               ; CODE XREF: getMouseStatus+Ej
                    ; getMouseStatus+B6j
        retn
getMouseStatus   endp


; =============== S U B R O U T I N E =======================================


videoloop   proc near       ; CODE XREF: crt?2+52p crt?1+3Ep ...
        push    dx
        push    ax
        cmp byte ptr dword_59B67, 0
        jnz short waitForVsync
        mov dx, 3DAh
        cli
        in  al, dx      ; Video status bits:
                    ; 0: retrace.  1=display is in vert or horiz retrace.
                    ; 1: 1=light pen is triggered; 0=armed
                    ; 2: 1=light pen switch is open; 0=closed
                    ; 3: 1=vertical sync pulse is occurring.
        mov ah, al
        mov dx, 3C0h
        al = 33h ; '3'
        out dx, al      ; EGA: horizontal pixel panning:
                    ; Number of dots to shift data left.
                    ; Bits 0-3 valid (0-0fH)
        al = byte_510A6
        out dx, al      ; EGA: palette register: select colors for attribute AL:
                    ; 0: RED
                    ; 1: GREEN
                    ; 2: BLUE
                    ; 3: blue
                    ; 4: green
                    ; 5: red
        test    ah, 8
        jnz short loc_4D453

waitForVsync:              ; CODE XREF: videoloop+7j
                    ; videoloop+31j
        sti
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        mov dx, 3DAh
        cli
        in  al, dx      ; Video status bits:
                    ; 0: retrace.  1=display is in vert or horiz retrace.
                    ; 1: 1=light pen is triggered; 0=armed
                    ; 2: 1=light pen switch is open; 0=closed
                    ; 3: 1=vertical sync pulse is occurring.
        test    al, 1000b
        jz  short waitForVsync
        cmp byte ptr dword_59B67, 0
        jz  short loc_4D453
        mov dx, 3C0h
        al = 33h ; '3'
        out dx, al      ; EGA: horizontal pixel panning:
                    ; Number of dots to shift data left.
                    ; Bits 0-3 valid (0-0fH)
        al = byte_510A6
        out dx, al      ; EGA: palette register: select colors for attribute AL:
                    ; 0: RED
                    ; 1: GREEN
                    ; 2: BLUE
                    ; 3: blue
                    ; 4: green
                    ; 5: red

loc_4D453:              ; CODE XREF: videoloop+1Dj
                    ; videoloop+38j
        sti
        pop ax
        pop dx
        retn
videoloop   endp


; =============== S U B R O U T I N E =======================================


sub_4D457   proc near       ; CODE XREF: crt?2+55p crt?1+41p ...
        push    dx
        push    ax

loc_4D459:              ; CODE XREF: sub_4D457+8j
        mov dx, 3DAh
        in  al, dx      ; Video status bits:
                    ; 0: retrace.  1=display is in vert or horiz retrace.
                    ; 1: 1=light pen is triggered; 0=armed
                    ; 2: 1=light pen switch is open; 0=closed
                    ; 3: 1=vertical sync pulse is occurring.
        test    al, 8
        jnz short loc_4D459
        pop ax
        pop dx
        retn
sub_4D457   endp


; =============== S U B R O U T I N E =======================================


sub_4D464   proc near       ; CODE XREF: start+332p sub_4A463+3p
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: sequencer data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3D4h
        al = 13h
        out dx, al      ; Video: CRT cntrlr addr
                    ; vertical displayed adjustment
        inc dx
        al = 3Dh ; '='
        out dx, al      ; Video: CRT controller internal registers
        cmp videoStatusUnk, 1
        jnz short loc_4D4CF
        mov dx, 3D4h
        al = 18h
        out dx, al      ; Video: CRT cntrlr addr
                    ; line compare (scan line). Used for split screen operations.
        inc dx
        al = 5Fh ; '_'
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 7
        out dx, al      ; Video: CRT cntrlr addr
                    ; bit 8 for certain CRTC regs. Data bits:
                    ; 0: vertical total (Reg 06)
                    ; 1: vert disp'd enable end (Reg 12H)
                    ; 2: vert retrace start (Reg 10H)
                    ; 3: start vert blanking (Reg 15H)
                    ; 4: line compare (Reg 18H)
                    ; 5: cursor location (Reg 0aH)
        inc dx
        al = 3Fh ; '?'
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 9
        out dx, al      ; Video: CRT cntrlr addr
                    ; maximum scan line
        inc dx
        al = 80h ; '?'
        out dx, al      ; Video: CRT controller internal registers
        call    videoloop
        mov dx, 3DAh
        cli
        in  al, dx      ; Video status bits:
                    ; 0: retrace.  1=display is in vert or horiz retrace.
                    ; 1: 1=light pen is triggered; 0=armed
                    ; 2: 1=light pen switch is open; 0=closed
                    ; 3: 1=vertical sync pulse is occurring.
        mov dx, 3C0h
        al = 30h ; '0'
        out dx, al      ; EGA: mode control bits:
                    ; 0: 1=graph modes, 0=text
                    ; 1: 1=MDA
                    ; 2: 1=9th dot=8th dot for line/box chars
                    ;    0=use bkgd colr as 9th dot of char
                    ; 3: 1=enable blink, 0=allow 4-bit bkgd
        al = 21h ; '!'
        out dx, al      ; EGA: palette register: select colors for attribute AL:
                    ; 0: RED
                    ; 1: GREEN
                    ; 2: BLUE
                    ; 3: blue
                    ; 4: green
                    ; 5: red
        sti
        jmp short locret_4D4E3
// ; ---------------------------------------------------------------------------

loc_4D4CF:              ; CODE XREF: sub_4D464+37j
        mov dx, 3D4h
        al = 18h
        out dx, al      ; Video: CRT cntrlr addr
                    ; line compare (scan line). Used for split screen operations.
        inc dx
        al = 0B0h ; '?'
        out dx, al      ; Video: CRT controller internal registers
        mov dx, 3D4h
        al = 7
        out dx, al      ; Video: CRT cntrlr addr
                    ; bit 8 for certain CRTC regs. Data bits:
                    ; 0: vertical total (Reg 06)
                    ; 1: vert disp'd enable end (Reg 12H)
                    ; 2: vert retrace start (Reg 10H)
                    ; 3: start vert blanking (Reg 15H)
                    ; 4: line compare (Reg 18H)
                    ; 5: cursor location (Reg 0aH)
        inc dx
        al = 1
        out dx, al      ; Video: CRT controller internal registers

locret_4D4E3:               ; CODE XREF: sub_4D464+69j
        retn
sub_4D464   endp


; =============== S U B R O U T I N E =======================================


sub_4D4E4   proc near       ; CODE XREF: start+2B2p start+2C7p
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: sequencer data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3D4h
        al = 13h
        out dx, al      ; Video: CRT cntrlr addr
                    ; vertical displayed adjustment
        inc dx
        al = 3Dh ; '='
        out dx, al      ; Video: CRT controller internal registers
        retn
sub_4D4E4   endp


; =============== S U B R O U T I N E =======================================


sub_4D517   proc near       ; CODE XREF: start+2AFp
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        mov cx, 0FFFFh
        mov di, 0
        rep stosb
        retn
sub_4D517   endp


; =============== S U B R O U T I N E =======================================


readLevels  proc near       ; CODE XREF: start:loc_46F3Ep
                    ; sub_4A463p
        cmp byte_510DE, 0
        jz  short loc_4D59F
        cmp byte ptr word_599D8, 0
        jnz short loc_4D599
        cmp byte_599D4, 0
        jnz short loc_4D59F
        mov ax, word_510E6
        push    es
        push    ds
        push    ds
        pop es
        assume es:data
        mov si, seg demoseg
        mov ds, si
        assume ds:demoseg
        mov si, 0BE20h
        mov di, offset fileLevelData
        mov cx, ax
        shl cx, 1
        add ax, cx
        mov cl, 9
        shl ax, cl
        add si, ax
        mov cx, 300h
        cld
        rep movsw
        mov di, 87DAh
        mov ax, 532Eh
        stosw
        mov ax, 50h ; 'P'
        stosw
        sub si, 5Ah ; 'Z'
        mov cx, 17h
        rep movsb
        pop ds
        assume ds:data
        pop es
        assume es:nothing
        jmp loc_4D64B
// ; ---------------------------------------------------------------------------

loc_4D599:              ; CODE XREF: readLevels+Cj
        lea dx, aLevels_dat ; "LEVELS.DAT"
        jmp short loc_4D5A3
// ; ---------------------------------------------------------------------------

loc_4D59F:              ; CODE XREF: readLevels+5j
                    ; readLevels+13j
        lea dx, aLevels_dat_0 ; "LEVELS.DAT"

loc_4D5A3:              ; CODE XREF: readLevels+55j
        mov ax, 3D00h
        cmp byte_599D4, 0
        jz  short loc_4D5BB
        mov dx, offset demoFileName
        cmp word_599DA, 0
        jz  short loc_4D5BB
        lea dx, aLevels_dat ; "LEVELS.DAT"

loc_4D5BB:              ; CODE XREF: readLevels+63j
                    ; readLevels+6Dj
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_4D5C2
        jmp exit
// ; ---------------------------------------------------------------------------

loc_4D5C2:              ; CODE XREF: readLevels+75j
        mov lastFileHandle, ax
        cmp byte_510DE, 0
        jz  short loc_4D5D1
        mov ax, word_510E6
        jmp short loc_4D5D4
// ; ---------------------------------------------------------------------------

loc_4D5D1:              ; CODE XREF: readLevels+82j
        mov ax, word_51ABC

loc_4D5D4:              ; CODE XREF: readLevels+87j
        cmp byte_599D4, 0
        jz  short loc_4D5E3
        mov ax, word_599DA
        or  ax, ax
        jnz short loc_4D5E3
        inc ax

loc_4D5E3:              ; CODE XREF: readLevels+91j
                    ; readLevels+98j
        dec ax
        mov bx, ax
        shl bx, 1
        add ax, bx
        shl ax, 1
        mov cl, 8
        mov dx, ax
        shl dx, cl
        shr ax, cl
        mov cx, ax
        mov ax, 4200h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
                    ; AL = method: offset from beginning of file
        jnb short loc_4D604
        jmp exit
// ; ---------------------------------------------------------------------------

loc_4D604:              ; CODE XREF: readLevels+B7j
        mov ax, 3F00h
        mov bx, lastFileHandle
        mov cx, levelDataLength
        mov dx, offset fileLevelData
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_4D618
        jmp exit
// ; ---------------------------------------------------------------------------

loc_4D618:              ; CODE XREF: readLevels+CBj
        cmp byte ptr word_599D8, 0
        jz  short loc_4D64B
        mov byte ptr word_599D8+1, 0FFh
        push    es
        mov ax, demoseg
        mov es, ax
        assume es:demoseg
        lea si, ds:[00768h]
        lea di, ds:[0BE20h]
        mov ax, word_599D6
        mov word_510E6, ax
        mov cx, ax
        shl cx, 1
        add ax, cx
        mov cl, 9
        shl ax, cl
        add di, ax
        mov cx, 300h
        cld
        rep movsw
        pop es
        assume es:nothing

loc_4D64B:              ; CODE XREF: readLevels+4Ej
                    ; readLevels+D5j
        cmp byte_510DE, 0
        jz  short loc_4D65D
        mov ax, word_51076
        mov timeOfDay, ax
        mov di, 87DAh
        jmp short loc_4D660
// ; ---------------------------------------------------------------------------

loc_4D65D:              ; CODE XREF: readLevels+108j
        mov di, 87A8h

loc_4D660:              ; CODE XREF: readLevels+113j
        push    es
        push    ds
        pop es
        assume es:data
        cld
        cmp word_599D8, 0
        jnz short loc_4D679
        cmp byte_599D4, 0
        jz  short loc_4D68C
        cmp word_599DA, 0
        jz  short loc_4D682

loc_4D679:              ; CODE XREF: readLevels+121j
        mov ax, 4942h
        stosw
        mov ax, 4Eh ; 'N'
        jmp short loc_4D689
// ; ---------------------------------------------------------------------------

loc_4D682:              ; CODE XREF: readLevels+12Fj
        mov ax, 532Eh
        stosw
        mov ax, 50h ; 'P'

loc_4D689:              ; CODE XREF: readLevels+138j
        stosw
        jmp short loc_4D68F
// ; ---------------------------------------------------------------------------

loc_4D68C:              ; CODE XREF: readLevels+128j
        add di, 4

loc_4D68F:              ; CODE XREF: readLevels+142j
        mov si, 0D0Eh
        mov cx, 17h
        rep movsb
        pop es
        assume es:nothing
        push    es
        push    ds
        pop es
        assume es:data
        mov si, offset fileLevelData
        mov di, offset levelBuffer
        mov cx, 300h
        cld
        rep movsw
        pop es
        assume es:nothing
        push    es
        mov ax, ds
        mov es, ax
        assume es:data
        mov cx, levelDataLength
        mov si, offset fileLevelData
        mov di, offset leveldata
        xor ah, ah

loc_4D6B8:              ; CODE XREF: readLevels+172j
        lodsb
        stosw
        loop    loc_4D6B8
        mov di, 2434h
        xor al, al
        mov cx, levelbytes
        rep stosb
        pop es
        assume es:nothing
        cmp byte_510DE, 0
        jz  short loc_4D6DC
        cmp byte ptr word_599D8, 0
        jnz short loc_4D6DC
        cmp byte_599D4, 0
        jz  short loc_4D6EA

loc_4D6DC:              ; CODE XREF: readLevels+184j
                    ; readLevels+18Bj
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short loc_4D6EA
        jmp exit
// ; ---------------------------------------------------------------------------

loc_4D6EA:              ; CODE XREF: readLevels+192j
                    ; readLevels+19Dj
        mov byte ptr word_599D8, 0
        retn
readLevels  endp


// ; =============== S U B R O U T I N E =======================================

// ; Attributes: bp-based frame

void fade() //        proc near       ; CODE XREF: start+2C1p start+312p ...
{

var_8       = word ptr -8
var_6       = word ptr -6
var_4       = word ptr -4
var_2       = word ptr -2

        push(bp);
        bp = sp;
        sp += 0x0FFF8;
        push(es);
        if (videoStatusUnk != 2)
        {
            goto loc_4D706;
        }
        
        sub_4D836();
        pop(es);
        sp = bp;
        pop(bp);
        return;
// ; ---------------------------------------------------------------------------

loc_4D706:              //; CODE XREF: fade+Cj
        push(si);
        ax = ds;
        es = ax;
        // assume es:data
        ax = word_510A2;
        [bp+var_8] = ax;
        word_510A2 = 0;
        cx = 0x40; // '@'
        di = &fileLevelData;

loc_4D71D:              //; CODE XREF: fade+33j
        lodsb
        al = al << 1;
        al = al << 1;
        stosb
        cx--;
        if (cx > 0)
        {
            goto loc_4D71D;
        }
        dx = 0x3C7;
        al = 0;
        // out dx, al
        dx = 0x3C9;
        di = 0x6115;
        cx = 0x10;

loc_4D734:              // ; CODE XREF: fade+51j
        // in  al, dx
        *di = al;
        di++;
        // in  al, dx
        *di = al;
        di++;
        // in  al, dx
        *di = al;
        di++;
        di++;
        cx--;
        if (cx > 0)
        {
            goto loc_4D734;
        }
        ax = 0;
        [bp+var_2] = ax;
        ax = 0x3F; // '?'
        [bp+var_4] = ax;

loc_4D74F:              //; CODE XREF: fade+134j
        dx = 0x3C8;
        al = 0;
        // out dx, al
        cx = 0x10;
        si = &fileLevelData;
        di = 0x6115;

loc_4D75E:              //; CODE XREF: fade+115j
        al = [si];
        ah = 0;
        bx = [bp+var_2];
        ax = ax * bx;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        mov [bp+var_6], ax
        al = [di]
        ah = 0;
        bx = [bp+var_4];
        ax = ax * bx;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax += [bp+var_6];
        di++;
        si++;
        dx = 0x3C9;
        // out dx, al
        al = [si];
        ah = 0;
        bx = [bp+var_2];
        ax = ax * bx;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        [bp+var_6] = ax;
        al = [di];
        ah = 0;
        bx = [bp+var_4];
        ax = ax * bx;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        add ax, [bp+var_6]
        inc di
        inc si
        mov dx, 3C9h
        out dx, al
        al = [si]
        xor ah, ah
        mov bx, [bp+var_2]
        ax = ax * bx;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        mov [bp+var_6], ax
        al = [di]
        xor ah, ah
        mov bx, [bp+var_4]
        ax = ax * bx;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax = ax >> 1;
        ax += [bp+var_6];
        di++;
        si++;
        dx = 0x3C9;
        // out dx, al
        si++;
        di++;
        cx--;
        if (cx == 0)
        {
            goto loc_4D808;
        }
        goto loc_4D75E;
// ; ---------------------------------------------------------------------------

loc_4D808:              //; CODE XREF: fade+113j
        if (fastMode == 1)
        {
            goto isFastMode4;
        }
        videoloop();
        sub_4D457();

isFastMode4:              //; CODE XREF: fade+11Dj
        [bp+var_4]--;
        ax = [bp+var_2];
        ax++;
        [bp+var_2] = ax;
        if (ax > 0x3F) // '?'
        {
            goto loc_4D827;
        }
        goto loc_4D74F;
// ; ---------------------------------------------------------------------------

loc_4D827:              //; CODE XREF: fade+132j
        pop(si);
        sub_4D836();
        ax = [bp+var_8];
        word_510A2 = ax;
        pop(es);
        // assume es:nothing
        sp = bp;
        pop(bp);
        // retn
}


; =============== S U B R O U T I N E =======================================

; Attributes: bp-based frame

sub_4D836   proc near       ; CODE XREF: start+2B8p
                    ; loadScreen2+B5p ...

var_2       = word ptr -2

        push    bp
        mov bp, sp
        add sp, 0FFFEh
        mov ax, word_510A2
        mov [bp+var_2], ax
        mov word_510A2, 0
        cmp videoStatusUnk, 1
        jnz short loc_4D872
        mov cx, 10h
        mov dx, 3C8h
        al = 0
        out dx, al
        mov dx, 3C9h

loc_4D85B:              ; CODE XREF: sub_4D836+38j
        lodsb
        shl al, 1
        shl al, 1
        out dx, al
        lodsb
        shl al, 1
        shl al, 1
        out dx, al
        lodsb
        shl al, 1
        shl al, 1
        out dx, al
        lodsb
        loop    loc_4D85B
        jmp short loc_4D89E
// ; ---------------------------------------------------------------------------

loc_4D872:              ; CODE XREF: sub_4D836+17j
        mov cx, 10h
        mov dx, 0
        mov di, 611h

loc_4D87B:              ; CODE XREF: sub_4D836+58j
        lodsb
        lodsb
        lodsb
        lodsb
        mov bh, al
        and bh, 7
        and al, 8
        shl al, 1
        or  bh, al
        mov [di], bh
        inc di
        inc dx
        loop    loc_4D87B
        push    es
        mov ax, ds
        mov es, ax
        assume es:data
        mov ax, 1002h
        mov dx, 611h
        int 10h     ; - VIDEO - SET ALL PALETTE REGISTERS (Jr, PS, TANDY 1000, EGA, VGA)
                    ; ES:DX -> 17-byte palette register list
        pop es
        assume es:nothing

loc_4D89E:              ; CODE XREF: sub_4D836+3Aj
        mov ax, [bp+var_2]
        mov word_510A2, ax
        mov sp, bp
        pop bp
        retn
sub_4D836   endp

// ; ---------------------------------------------------------------------------
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  8Bh ; ?
        db 0C0h ; +

; =============== S U B R O U T I N E =======================================


sub_4D8B0   proc near       ; CODE XREF: start+2A5p
        push    ds
        xor ax, ax
        mov ds, ax
        assume ds:nothing
        mov bx, 200h
        mov [bx], ax
        mov ax, seg soundseg
        mov [bx+2], ax
        xor ax, ax
        mov bx, 204h
        mov [bx], ax
        mov ax, seg sound2seg
        mov [bx+2], ax
        pop ds
        assume ds:data
        mov byte_59885, 0
        mov byte_59886, 0
        mov soundEnabled?, 0
        retn
sub_4D8B0   endp


; =============== S U B R O U T I N E =======================================


soundShutdown?  proc near       ; CODE XREF: start+48Ep
                    ; loadScreen2-7DAp
        mov soundEnabled?, 0
        call    sound?1
        retn
soundShutdown?  endp

// ; ---------------------------------------------------------------------------
        call    sound?1
        mov musType, 0
        mov sndType, 0
        mov soundEnabled?, 0
        retn

; =============== S U B R O U T I N E =======================================


loadBeep    proc near       ; CODE XREF: readConfig:loc_4751Ap
                    ; readConfig:loc_47551p ...
        call    sound?1
        mov dx, offset aBeep_snd ; "BEEP.SND"
        mov cx, 0AC4h
        call    readSound
        mov musType, 1
        mov sndType, 1
        mov soundEnabled?, 1
        call    sound?2
        mov byte_59889, 0
        mov byte_5988A, 64h ; 'd'
        mov byte_5988B, 0
        mov byte_5988C, 0
        retn
loadBeep    endp


; =============== S U B R O U T I N E =======================================


loadBeep2   proc near       ; CODE XREF: readConfig+4Cp sub_4C70Fp
        call    sound?1
        mov dx, offset aBeep_snd ; "BEEP.SND"
        mov cx, 0AC4h
        call    readSound
        mov dx, offset aSample_snd ; "SAMPLE.SND"
        mov cx, 8DACh
        call    readSound2
        mov musType, 1
        mov sndType, 2
        mov soundEnabled?, 1
        call    sound?2
        mov byte_59889, 0
        mov byte_5988A, 64h ; 'd'
        mov byte_5988B, 0
        mov byte_5988C, 0
        retn
loadBeep2   endp


; =============== S U B R O U T I N E =======================================


loadAdlib   proc near       ; CODE XREF: readConfig+56p sub_4C723p
        call    sound?1
        mov dx, offset aAdlib_snd ; "ADLIB.SND"
        mov cx, 14EAh
        call    readSound
        mov musType, 3
        mov sndType, 3
        mov soundEnabled?, 0
        call    sound?2
        mov byte_59889, 0
        mov byte_5988A, 64h ; 'd'
        mov byte_5988B, 0
        mov byte_5988C, 0
        retn
loadAdlib   endp


; =============== S U B R O U T I N E =======================================


loadBlaster  proc near       ; CODE XREF: readConfig+60p sub_4C719p
        call    sound?1
        mov dx, offset aAdlib_snd ; "ADLIB.SND"
        mov cx, 14EAh
        call    readSound
        mov dx, offset aBlaster_snd ; "BLASTER.SND"
        mov cx, 991Bh
        call    readSound2
        mov musType, 3
        mov sndType, 4
        mov soundEnabled?, 0
        call    sound?2
        mov byte_59889, 0
        mov byte_5988A, 64h ; 'd'
        mov byte_5988B, 0
        mov byte_5988C, 0
        retn
loadBlaster  endp


; =============== S U B R O U T I N E =======================================


loadRoland  proc near       ; CODE XREF: readConfig+6Ap sub_4C72Dp
        call    sound?1
        mov dx, offset aRoland_snd ; "ROLAND.SND"
        mov cx, 0F80h
        call    readSound
        mov musType, 5
        mov sndType, 5
        mov soundEnabled?, 0
        call    sound?2
        mov byte_59889, 0
        mov byte_5988A, 64h ; 'd'
        mov byte_5988B, 0
        mov byte_5988C, 0
        retn
loadRoland  endp


; =============== S U B R O U T I N E =======================================


loadCombined proc near       ; CODE XREF: readConfig+74p sub_4C737p
        call    sound?1
        mov dx, offset aRoland_snd ; "ROLAND.SND"
        mov cx, 0F80h
        call    readSound
        mov dx, offset aBlaster_snd ; "BLASTER.SND"
        mov cx, 991Bh
        call    readSound2
        mov musType, 5
        mov sndType, 4
        mov soundEnabled?, 0
        call    sound?2
        mov byte_59889, 0
        mov byte_5988A, 64h ; 'd'
        mov byte_5988B, 0
        mov byte_5988C, 0
        retn
loadCombined endp


; =============== S U B R O U T I N E =======================================


readSound   proc near       ; CODE XREF: loadBeep+9p loadBeep2+9p ...
        mov ax, 3D00h
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_4DA51
        jmp exit
// ; ---------------------------------------------------------------------------

loc_4DA51:              ; CODE XREF: readSound+5j
        mov lastFileHandle, ax
        mov bx, lastFileHandle
        push    ds
        mov ax, seg soundseg
        mov ds, ax
        assume ds:soundseg
        mov ax, 3F00h
        mov dx, 0
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_4DA6C
        pop ds
        assume ds:data
        jmp exit
// ; ---------------------------------------------------------------------------

loc_4DA6C:              ; CODE XREF: readSound+1Fj
        pop ds
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short locret_4DA7B
        jmp exit
// ; ---------------------------------------------------------------------------

locret_4DA7B:               ; CODE XREF: readSound+2Fj
        retn
readSound   endp


; =============== S U B R O U T I N E =======================================


readSound2  proc near       ; CODE XREF: loadBeep2+12p
                    ; loadBlaster+12p ...
        mov ax, 3D00h
        int 21h     ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
                    ; DS:DX -> ASCIZ filename
                    ; AL = access mode
                    ; 0 - read
        jnb short loc_4DA86
        jmp exit
// ; ---------------------------------------------------------------------------

loc_4DA86:              ; CODE XREF: readSound2+5j
        mov lastFileHandle, ax
        mov bx, lastFileHandle
        push    ds
        mov ax, seg sound2seg
        mov ds, ax
        assume ds:sound2seg
        mov ax, 3F00h
        mov dx, 0
        int 21h     ; DOS - 2+ - READ FROM FILE WITH HANDLE
                    ; BX = file handle, CX = number of bytes to read
                    ; DS:DX -> buffer
        jnb short loc_4DAA1
        pop ds
        assume ds:data
        jmp exit
// ; ---------------------------------------------------------------------------

loc_4DAA1:              ; CODE XREF: readSound2+1Fj
        pop ds
        mov ax, 3E00h
        mov bx, lastFileHandle
        int 21h     ; DOS - 2+ - CLOSE A FILE WITH HANDLE
                    ; BX = file handle
        jnb short locret_4DAB0
        jmp exit
// ; ---------------------------------------------------------------------------

locret_4DAB0:               ; CODE XREF: readSound2+2Fj
        retn
readSound2  endp


; =============== S U B R O U T I N E =======================================


sound?1     proc near       ; CODE XREF: soundShutdown?+5p
                    ; code:6CC7p ...
        mov soundEnabled?, 0
        cmp musType, 1
        jnz short loc_4DAC9
        mov ah, 2
        int 80h     ; LINUX -
        in  al, 61h     ; PC/XT PPI port B bits:
                    ; 0: Tmr 2 gate ??? OR 03H=spkr ON
                    ; 1: Tmr 2 data ?  AND  0fcH=spkr OFF
                    ; 3: 1=read high switches
                    ; 4: 0=enable RAM parity checking
                    ; 5: 0=enable I/O channel check
                    ; 6: 0=hold keyboard clock low
                    ; 7: 0=enable kbrd
        and al, 0FCh
        out 61h, al     ; PC/XT PPI port B bits:
                    ; 0: Tmr 2 gate ??? OR 03H=spkr ON
                    ; 1: Tmr 2 data ?  AND  0fcH=spkr OFF
                    ; 3: 1=read high switches
                    ; 4: 0=enable RAM parity checking
                    ; 5: 0=enable I/O channel check
                    ; 6: 0=hold keyboard clock low
                    ; 7: 0=enable kbrd
        jmp short loc_4DAE8
// ; ---------------------------------------------------------------------------

loc_4DAC9:              ; CODE XREF: sound?1+Aj
        cmp musType, 3
        jnz short loc_4DAD9
        mov dx, 388h
        mov ah, 2
        int 80h     ; LINUX -
        jmp short loc_4DAE8
// ; ---------------------------------------------------------------------------

loc_4DAD9:              ; CODE XREF: sound?1+1Dj
        cmp musType, 5
        jnz short loc_4DAE8
        mov ah, 2
        int 80h     ; LINUX -
        mov ah, 10h
        int 80h     ; LINUX -

loc_4DAE8:              ; CODE XREF: sound?1+16j sound?1+26j ...
        mov musType, 0
        cmp sndType, 2
        jnz short loc_4DAFA
        mov ah, 1
        int 81h
        jmp short loc_4DB05
// ; ---------------------------------------------------------------------------

loc_4DAFA:              ; CODE XREF: sound?1+41j
        cmp sndType, 4
        jnz short loc_4DB05
        mov ah, 2
        int 81h

loc_4DB05:              ; CODE XREF: sound?1+47j sound?1+4Ej
        mov sndType, 0
        retn
sound?1     endp


; =============== S U B R O U T I N E =======================================


sound?2     proc near       ; CODE XREF: start+39Bp start+410p ...
        cmp byte_59886, 1
        jz  short loc_4DB13
        retn
// ; ---------------------------------------------------------------------------

loc_4DB13:              ; CODE XREF: sound?2+5j
        cmp musType, 1
        jnz short loc_4DB26
        mov ax, 0
        int 80h     ; LINUX - old_setup_syscall
        mov soundEnabled?, 1
        jmp short locret_4DB4D
// ; ---------------------------------------------------------------------------

loc_4DB26:              ; CODE XREF: sound?2+Dj
        cmp musType, 3
        jnz short loc_4DB3C
        mov dx, 388h
        mov ax, 0
        int 80h     ; LINUX - old_setup_syscall
        mov soundEnabled?, 1
        jmp short locret_4DB4D
// ; ---------------------------------------------------------------------------

loc_4DB3C:              ; CODE XREF: sound?2+20j
        cmp musType, 5
        jnz short locret_4DB4D
        mov ax, 0
        int 80h     ; LINUX - old_setup_syscall
        mov soundEnabled?, 1

locret_4DB4D:               ; CODE XREF: sound?2+19j sound?2+2Fj ...
        retn
sound?2     endp


; =============== S U B R O U T I N E =======================================


sound?3     proc near       ; CODE XREF: start+354p runLevel+41p ...
        cmp musType, 1
        jnz short loc_4DB5B
        mov ah, 2
        int 80h     ; LINUX -
        jmp short locret_4DB76
// ; ---------------------------------------------------------------------------

loc_4DB5B:              ; CODE XREF: sound?3+5j
        cmp musType, 3
        jnz short loc_4DB6B
        mov dx, 388h
        mov ah, 2
        int 80h     ; LINUX -
        jmp short locret_4DB76
// ; ---------------------------------------------------------------------------

loc_4DB6B:              ; CODE XREF: sound?3+12j
        cmp musType, 5
        jnz short locret_4DB76
        mov ah, 2
        int 80h     ; LINUX -

locret_4DB76:               ; CODE XREF: sound?3+Bj sound?3+1Bj ...
        retn
sound?3     endp


; =============== S U B R O U T I N E =======================================


sound?4     proc near       ; CODE XREF: sub_4A61F+2EDp code:5ADEp ...
        cmp byte_59885, 1
        jz  short loc_4DB7F
        retn
// ; ---------------------------------------------------------------------------

loc_4DB7F:              ; CODE XREF: sound?4+5j
        cmp byte_59889, 5
        jl  short loc_4DB87
        retn
// ; ---------------------------------------------------------------------------

loc_4DB87:              ; CODE XREF: sound?4+Dj
        mov byte_5988B, 0Fh
        mov byte_59889, 5
        cmp sndType, 1
        jnz short loc_4DB9F
        mov ax, 400h
        int 80h     ; LINUX -
        jmp short locret_4DBDF
// ; ---------------------------------------------------------------------------

loc_4DB9F:              ; CODE XREF: sound?4+1Fj
        cmp sndType, 2
        jnz short loc_4DBB4
        mov dx, 5D38h
        mov ah, 3
        int 81h
        mov ax, 0
        int 81h
        jmp short locret_4DBDF
// ; ---------------------------------------------------------------------------

loc_4DBB4:              ; CODE XREF: sound?4+2Dj
        cmp sndType, 3
        jnz short loc_4DBC5
        mov ax, 400h
        mov dx, 388h
        int 80h     ; LINUX -
        jmp short locret_4DBDF
// ; ---------------------------------------------------------------------------

loc_4DBC5:              ; CODE XREF: sound?4+42j
        cmp sndType, 4
        jnz short loc_4DBD3
        mov ax, 0
        int 81h
        jmp short locret_4DBDF
// ; ---------------------------------------------------------------------------

loc_4DBD3:              ; CODE XREF: sound?4+53j
        cmp sndType, 5
        jnz short locret_4DBDF
        mov ax, 400h
        int 80h     ; LINUX -

locret_4DBDF:               ; CODE XREF: sound?4+26j sound?4+3Bj ...
        retn
sound?4     endp


; =============== S U B R O U T I N E =======================================


sound?5     proc near       ; CODE XREF: update?:loc_4E55Cp
                    ; update?:loc_4E588p ...
        cmp byte_59885, 1
        jz  short loc_4DBE8
        retn
// ; ---------------------------------------------------------------------------

loc_4DBE8:              ; CODE XREF: sound?5+5j
        cmp byte_59889, 5
        jl  short loc_4DBF0
        retn
// ; ---------------------------------------------------------------------------

loc_4DBF0:              ; CODE XREF: sound?5+Dj
        mov byte_5988B, 0Fh
        mov byte_59889, 4
        cmp sndType, 1
        jnz short loc_4DC08
        mov ax, 401h
        int 80h     ; LINUX -
        jmp short locret_4DC48
// ; ---------------------------------------------------------------------------

loc_4DC08:              ; CODE XREF: sound?5+1Fj
        cmp sndType, 2
        jnz short loc_4DC1D
        mov dx, 5D38h
        mov ah, 3
        int 81h
        mov ax, 1
        int 81h
        jmp short locret_4DC48
// ; ---------------------------------------------------------------------------

loc_4DC1D:              ; CODE XREF: sound?5+2Dj
        cmp sndType, 3
        jnz short loc_4DC2E
        mov ax, 401h
        mov dx, 388h
        int 80h     ; LINUX -
        jmp short locret_4DC48
// ; ---------------------------------------------------------------------------

loc_4DC2E:              ; CODE XREF: sound?5+42j
        cmp sndType, 4
        jnz short loc_4DC3C
        mov ax, 1
        int 81h
        jmp short locret_4DC48
// ; ---------------------------------------------------------------------------

loc_4DC3C:              ; CODE XREF: sound?5+53j
        cmp sndType, 5
        jnz short locret_4DC48
        mov ax, 401h
        int 80h     ; LINUX -

locret_4DC48:               ; CODE XREF: sound?5+26j sound?5+3Bj ...
        retn
sound?5     endp


; =============== S U B R O U T I N E =======================================


sound?6     proc near       ; CODE XREF: update?+B8Bp
                    ; update?+136Cp
        cmp byte_59885, 1
        jz  short loc_4DC51
        retn
// ; ---------------------------------------------------------------------------

loc_4DC51:              ; CODE XREF: sound?6+5j
        cmp byte_59889, 2
        jl  short loc_4DC59
        retn
// ; ---------------------------------------------------------------------------

loc_4DC59:              ; CODE XREF: sound?6+Dj
        mov byte_5988B, 7
        mov byte_59889, 2
        cmp sndType, 1
        jnz short loc_4DC71
        mov ax, 402h
        int 80h     ; LINUX -
        jmp short locret_4DCB1
// ; ---------------------------------------------------------------------------

loc_4DC71:              ; CODE XREF: sound?6+1Fj
        cmp sndType, 2
        jnz short loc_4DC86
        mov dx, 5D38h
        mov ah, 3
        int 81h
        mov ax, 2
        int 81h
        jmp short locret_4DCB1
// ; ---------------------------------------------------------------------------

loc_4DC86:              ; CODE XREF: sound?6+2Dj
        cmp sndType, 3
        jnz short loc_4DC97
        mov ax, 402h
        mov dx, 388h
        int 80h     ; LINUX -
        jmp short locret_4DCB1
// ; ---------------------------------------------------------------------------

loc_4DC97:              ; CODE XREF: sound?6+42j
        cmp sndType, 4
        jnz short loc_4DCA5
        mov ax, 2
        int 81h
        jmp short locret_4DCB1
// ; ---------------------------------------------------------------------------

loc_4DCA5:              ; CODE XREF: sound?6+53j
        cmp sndType, 5
        jnz short locret_4DCB1
        mov ax, 402h
        int 80h     ; LINUX -

locret_4DCB1:               ; CODE XREF: sound?6+26j sound?6+3Bj ...
        retn
sound?6     endp


; =============== S U B R O U T I N E =======================================


sound?7     proc near       ; CODE XREF: movefun:loc_48125p
                    ; movefun2:loc_48573p
        cmp byte_59885, 1
        jz  short loc_4DCBA
        retn
// ; ---------------------------------------------------------------------------

loc_4DCBA:              ; CODE XREF: sound?7+5j
        cmp byte_59889, 2
        jl  short loc_4DCC2
        retn
// ; ---------------------------------------------------------------------------

loc_4DCC2:              ; CODE XREF: sound?7+Dj
        mov byte_5988B, 7
        mov byte_59889, 2
        cmp sndType, 1
        jnz short loc_4DCDA
        mov ax, 403h
        int 80h     ; LINUX -
        jmp short locret_4DD1A
// ; ---------------------------------------------------------------------------

loc_4DCDA:              ; CODE XREF: sound?7+1Fj
        cmp sndType, 2
        jnz short loc_4DCEF
        mov dx, 5D38h
        mov ah, 3
        int 81h
        mov ax, 3
        int 81h
        jmp short locret_4DD1A
// ; ---------------------------------------------------------------------------

loc_4DCEF:              ; CODE XREF: sound?7+2Dj
        cmp sndType, 3
        jnz short loc_4DD00
        mov ax, 403h
        mov dx, 388h
        int 80h     ; LINUX -
        jmp short locret_4DD1A
// ; ---------------------------------------------------------------------------

loc_4DD00:              ; CODE XREF: sound?7+42j
        cmp sndType, 4
        jnz short loc_4DD0E
        mov ax, 3
        int 81h
        jmp short locret_4DD1A
// ; ---------------------------------------------------------------------------

loc_4DD0E:              ; CODE XREF: sound?7+53j
        cmp sndType, 5
        jnz short locret_4DD1A
        mov ax, 403h
        int 80h     ; LINUX -

locret_4DD1A:               ; CODE XREF: sound?7+26j sound?7+3Bj ...
        retn
sound?7     endp


; =============== S U B R O U T I N E =======================================


sound?8     proc near       ; CODE XREF: movefun7:loc_4A0ABp
        cmp byte_59885, 1
        jz  short loc_4DD23
        retn
// ; ---------------------------------------------------------------------------

loc_4DD23:              ; CODE XREF: sound?8+5j
        cmp byte_59889, 3
        jl  short loc_4DD2B
        retn
// ; ---------------------------------------------------------------------------

loc_4DD2B:              ; CODE XREF: sound?8+Dj
        mov byte_5988B, 3
        mov byte_59889, 3
        cmp sndType, 1
        jnz short loc_4DD43
        mov ax, 404h
        int 80h     ; LINUX -
        jmp short locret_4DD83
// ; ---------------------------------------------------------------------------

loc_4DD43:              ; CODE XREF: sound?8+1Fj
        cmp sndType, 2
        jnz short loc_4DD58
        mov dx, 5D38h
        mov ah, 3
        int 81h
        mov ax, 4
        int 81h
        jmp short locret_4DD83
// ; ---------------------------------------------------------------------------

loc_4DD58:              ; CODE XREF: sound?8+2Dj
        cmp sndType, 3
        jnz short loc_4DD69
        mov ax, 404h
        mov dx, 388h
        int 80h     ; LINUX -
        jmp short locret_4DD83
// ; ---------------------------------------------------------------------------

loc_4DD69:              ; CODE XREF: sound?8+42j
        cmp sndType, 4
        jnz short loc_4DD77
        mov ax, 4
        int 81h
        jmp short locret_4DD83
// ; ---------------------------------------------------------------------------

loc_4DD77:              ; CODE XREF: sound?8+53j
        cmp sndType, 5
        jnz short locret_4DD83
        mov ax, 404h
        int 80h     ; LINUX -

locret_4DD83:               ; CODE XREF: sound?8+26j sound?8+3Bj ...
        retn
sound?8     endp


; =============== S U B R O U T I N E =======================================


sound?9     proc near       ; CODE XREF: runLevel+2F4p
                    ; update?:loc_4E3E1p ...
        cmp byte_59885, 1
        jz  short xxxxxxxxdcdc
        retn
// ; ---------------------------------------------------------------------------

xxxxxxxxdcdc:               ; CODE XREF: sound?9+5j
        cmp byte_59889, 1
        jl  short loc_4DD94
        retn
// ; ---------------------------------------------------------------------------

loc_4DD94:              ; CODE XREF: sound?9+Dj
        mov byte_5988B, 3
        mov byte_59889, 1
        cmp sndType, 1
        jnz short loc_4DDAC
        mov ax, 405h
        int 80h     ; LINUX -
        jmp short locret_4DDEC
// ; ---------------------------------------------------------------------------

loc_4DDAC:              ; CODE XREF: sound?9+1Fj
        cmp sndType, 2
        jnz short loc_4DDC1
        mov dx, 5D38h
        mov ah, 3
        int 81h
        mov ax, 5
        int 81h
        jmp short locret_4DDEC
// ; ---------------------------------------------------------------------------

loc_4DDC1:              ; CODE XREF: sound?9+2Dj
        cmp sndType, 3
        jnz short loc_4DDD2
        mov ax, 405h
        mov dx, 388h
        int 80h     ; LINUX -
        jmp short locret_4DDEC
// ; ---------------------------------------------------------------------------

loc_4DDD2:              ; CODE XREF: sound?9+42j
        cmp sndType, 4
        jnz short loc_4DDE0
        mov ax, 5
        int 81h
        jmp short locret_4DDEC
// ; ---------------------------------------------------------------------------

loc_4DDE0:              ; CODE XREF: sound?9+53j
        cmp sndType, 5
        jnz short locret_4DDEC
        mov ax, 405h
        int 80h     ; LINUX -

locret_4DDEC:               ; CODE XREF: sound?9+26j sound?9+3Bj ...
        retn
sound?9     endp


; =============== S U B R O U T I N E =======================================


sound?10    proc near       ; CODE XREF: update?+7EBp
        cmp byte_59885, 1
        jz  short loc_4DDF5
        retn
// ; ---------------------------------------------------------------------------

loc_4DDF5:              ; CODE XREF: sound?10+5j
        mov byte_5988B, 0FAh ; '?'
        mov byte_59889, 0Ah
        call    sound?3
        cmp sndType, 1
        jnz short loc_4DE10
        mov ax, 1
        int 80h     ; LINUX - sys_exit
// ; ---------------------------------------------------------------------------
        jmp short locret_4DE5E
// ; ---------------------------------------------------------------------------

loc_4DE10:              ; CODE XREF: sound?10+1Aj
        cmp sndType, 2
        jnz short loc_4DE25
        mov dx, 5D38h
        mov ah, 3
        int 81h
        mov ax, 6
        int 81h
        jmp short locret_4DE5E
// ; ---------------------------------------------------------------------------

loc_4DE25:              ; CODE XREF: sound?10+28j
        cmp sndType, 3
        jnz short loc_4DE36
        mov ax, 1
        mov dx, 388h
        int 80h     ; LINUX - sys_exit
// ; ---------------------------------------------------------------------------
        jmp short locret_4DE5E
// ; ---------------------------------------------------------------------------

loc_4DE36:              ; CODE XREF: sound?10+3Dj
        cmp sndType, 4
        jnz short loc_4DE52
        cmp musType, 5
        jnz short loc_4DE4B
        mov ax, 1
        int 80h     ; LINUX - sys_exit
// ; ---------------------------------------------------------------------------
        jmp short locret_4DE5E
// ; ---------------------------------------------------------------------------

loc_4DE4B:              ; CODE XREF: sound?10+55j
        mov ax, 6
        int 81h
        jmp short locret_4DE5E
// ; ---------------------------------------------------------------------------

loc_4DE52:              ; CODE XREF: sound?10+4Ej
        cmp sndType, 5
        jnz short locret_4DE5E
        mov ax, 1
        int 80h     ; LINUX - sys_exit
// ; ---------------------------------------------------------------------------

locret_4DE5E:               ; CODE XREF: sound?10+21j sound?10+36j ...
        retn
sound?10    endp


; =============== S U B R O U T I N E =======================================


sound?11    proc near       ; CODE XREF: int8handler+51p
        cmp musType, 1
        jnz short loc_4DE6C
        mov ah, 1
        int 80h     ; LINUX -
        jmp short locret_4DE87
// ; ---------------------------------------------------------------------------

loc_4DE6C:              ; CODE XREF: sound?11+5j
        cmp musType, 3
        jnz short loc_4DE7C
        mov dx, 388h
        mov ah, 1
        int 80h     ; LINUX -
        jmp short locret_4DE87
// ; ---------------------------------------------------------------------------

loc_4DE7C:              ; CODE XREF: sound?11+12j
        cmp musType, 5
        jnz short locret_4DE87
        mov ah, 1
        int 80h     ; LINUX -

locret_4DE87:               ; CODE XREF: sound?11+Bj sound?11+1Bj ...
        retn
sound?11    endp

// ; ---------------------------------------------------------------------------
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  8Bh ; ?
        db 0C0h ; +

; =============== S U B R O U T I N E =======================================


update?     proc near       ; CODE XREF: gameloop?+Ep

        cmp byte ptr leveldata[si], 3
        jz  short hasValidMurphy
        mov word_510CF, 0
        retn
// ; ---------------------------------------------------------------------------

hasValidMurphy:              ; CODE XREF: update?+5j
        mov word_510CF, 1
        mov word_510C7, si
        mov ax, leveldata[si]
        cmp ax, 3
        jz  short loc_4DEB4
        jmp loc_4EA07
// ; ---------------------------------------------------------------------------

loc_4DEB4:              ; CODE XREF: update?+1Fj
        mov byte_510D8, 0
        cmp byte_5101C, 0
        jz  short loc_4DEE1
        cmp byte ptr [si+17BCh], 0Ch
        jz  short loc_4DEE1
        cmp byte ptr [si+17BCh], 15h
        jz  short loc_4DEE1
        cmp byte ptr [si+17BCh], 17h
        jz  short loc_4DEE1
        cmp word ptr [si+18ACh], 0
        jnz short loc_4DEE1
        mov byte_510D8, 1

loc_4DEE1:              ; CODE XREF: update?+2Ej update?+35j ...
        mov bl, byte_50941
        cmp bl, 0
        jz  short loc_4DEED
        jmp loc_4E001
// ; ---------------------------------------------------------------------------

loc_4DEED:              ; CODE XREF: update?+58j
        mov byte_510D3, 1
        cmp byte_510D8, 0
        jz  short loc_4DEFC
        jmp loc_4E38A
// ; ---------------------------------------------------------------------------

loc_4DEFC:              ; CODE XREF: update?+67j
        mov ax, word_5195D
        and ax, 3
        jz  short loc_4DF05
        retn
// ; ---------------------------------------------------------------------------

loc_4DF05:              ; CODE XREF: update?+72j
        inc word_510CD
        cmp word_510CD, 4
        jnz short loc_4DF1E
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4DF1E:              ; CODE XREF: update?+7Ej
        cmp word_510CD, 1F4h
        jg  short loc_4DF27
        retn
// ; ---------------------------------------------------------------------------

loc_4DF27:              ; CODE XREF: update?+94j
        cmp word_510CD, 20Ah
        jg  short loc_4DF4A
        push    si
        mov di, [si+6155h]
        mov si, 14F4h
        mov bx, word_510CD
        sub bx, 1F4h
        shr bx, 1
        shl bx, 1
        mov si, [bx+si]
        call    sub_4F200
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4DF4A:              ; CODE XREF: update?+9Dj
        cmp word_510CD, 3E8h
        jg  short loc_4DF53
        retn
// ; ---------------------------------------------------------------------------

loc_4DF53:              ; CODE XREF: update?+C0j
        cmp word_510CD, 3FEh
        jg  short loc_4DF76
        push    si
        mov di, [si+6155h]
        mov si, 14F4h
        mov bx, word_510CD
        sub bx, 3E8h
        shr bx, 1
        shl bx, 1
        mov si, [bx+si]
        call    sub_4F200
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4DF76:              ; CODE XREF: update?+C9j
        cmp word_510CD, 640h
        jg  short loc_4DF7F
        retn
// ; ---------------------------------------------------------------------------

loc_4DF7F:              ; CODE XREF: update?+ECj
        cmp word_510CD, 656h
        jg  short loc_4DFA2
        push    si
        mov di, [si+6155h]
        mov si, 14F4h
        mov bx, word_510CD
        sub bx, 640h
        shr bx, 1
        shl bx, 1
        mov si, [bx+si]
        call    sub_4F200
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4DFA2:              ; CODE XREF: update?+F5j
        cmp word_510CD, 676h
        jg  short locret_4DFBE
        cmp word ptr [si+1832h], 0
        jnz short loc_4DFBF
        cmp word ptr [si+1836h], 0
        jnz short loc_4DFE0
        mov word_510CD, 24h ; '$'

locret_4DFBE:               ; CODE XREF: update?+118j
        retn
// ; ---------------------------------------------------------------------------

loc_4DFBF:              ; CODE XREF: update?+11Fj
        push    si
        mov di, [si+6155h]
        mov si, 150Eh
        mov bx, word_510CD
        sub bx, 656h
        shr bx, 1
        shr bx, 1
        shr bx, 1
        shr bx, 1
        shl bx, 1
        mov si, [bx+si]
        call    sub_4F200
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4DFE0:              ; CODE XREF: update?+126j
        push    si
        mov di, [si+6155h]
        mov si, 1516h
        mov bx, word_510CD
        sub bx, 656h
        shr bx, 1
        shr bx, 1
        shr bx, 1
        shr bx, 1
        shl bx, 1
        mov si, [bx+si]
        call    sub_4F200
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4E001:              ; CODE XREF: update?+5Aj
        cmp byte_510D8, 0
        jz  short loc_4E035
        cmp word ptr [si+18ACh], 0
        jnz short loc_4E035
        cmp bl, 1
        jnz short loc_4E01B
        cmp word ptr [si+17BCh], 2
        jz  short loc_4E035

loc_4E01B:              ; CODE XREF: update?+182j
        cmp bl, 2
        jnz short loc_4E027
        cmp word ptr [si+1832h], 2
        jz  short loc_4E035

loc_4E027:              ; CODE XREF: update?+18Ej
        cmp bl, 4
        jnz short loc_4E033
        cmp word ptr [si+1836h], 2
        jz  short loc_4E035

loc_4E033:              ; CODE XREF: update?+19Aj
        mov bl, 3

loc_4E035:              ; CODE XREF: update?+176j update?+17Dj ...
        cmp bl, 1
        jnz short loc_4E041
        mov byte_510D3, 0
        jmp short loc_4E0AA
// ; ---------------------------------------------------------------------------

loc_4E041:              ; CODE XREF: update?+1A8j
        cmp bl, 2
        jnz short loc_4E04E
        mov byte_510D3, 0
        jmp loc_4E10C
// ; ---------------------------------------------------------------------------

loc_4E04E:              ; CODE XREF: update?+1B4j
        cmp bl, 3
        jnz short loc_4E05B
        mov byte_510D3, 0
        jmp loc_4E186
// ; ---------------------------------------------------------------------------

loc_4E05B:              ; CODE XREF: update?+1C1j
        cmp bl, 4
        jnz short loc_4E068
        mov byte_510D3, 0
        jmp loc_4E1E8
// ; ---------------------------------------------------------------------------

loc_4E068:              ; CODE XREF: update?+1CEj
        cmp bl, 5
        jnz short loc_4E075
        mov byte_510D3, 0
        jmp loc_4E260
// ; ---------------------------------------------------------------------------

loc_4E075:              ; CODE XREF: update?+1DBj
        cmp bl, 6
        jnz short loc_4E082
        mov byte_510D3, 0
        jmp loc_4E28A
// ; ---------------------------------------------------------------------------

loc_4E082:              ; CODE XREF: update?+1E8j
        cmp bl, 7
        jnz short loc_4E08F
        mov byte_510D3, 0
        jmp loc_4E2BA
// ; ---------------------------------------------------------------------------

loc_4E08F:              ; CODE XREF: update?+1F5j
        cmp bl, 8
        jnz short loc_4E09C
        mov byte_510D3, 0
        jmp loc_4E2E4
// ; ---------------------------------------------------------------------------

loc_4E09C:              ; CODE XREF: update?+202j
        cmp bl, 9
        jnz short loc_4E0A4
        jmp loc_4E314
// ; ---------------------------------------------------------------------------

loc_4E0A4:              ; CODE XREF: update?+20Fj
        mov byte_510D3, 0
        retn
// ; ---------------------------------------------------------------------------

loc_4E0AA:              ; CODE XREF: update?+1AFj update?+279j
        mov ax, [si+17BCh]
        cmp ax, 0
        jnz short loc_4E0B6
        jmp loc_4E344
// ; ---------------------------------------------------------------------------

loc_4E0B6:              ; CODE XREF: update?+221j
        cmp ax, 2
        jnz short loc_4E0BE
        jmp loc_4E3E1
// ; ---------------------------------------------------------------------------

loc_4E0BE:              ; CODE XREF: update?+229j
        cmp al, 19h
        jnz short loc_4E0C5
        jmp loc_4E3D0
// ; ---------------------------------------------------------------------------

loc_4E0C5:              ; CODE XREF: update?+230j
        cmp ax, 4
        jnz short loc_4E0CD
        jmp loc_4E55C
// ; ---------------------------------------------------------------------------

loc_4E0CD:              ; CODE XREF: update?+238j
        cmp ax, 7
        jnz short loc_4E0D5
        jmp loc_4E674
// ; ---------------------------------------------------------------------------

loc_4E0D5:              ; CODE XREF: update?+240j
        cmp al, 13h
        jnz short loc_4E0DC
        jmp loc_4E712
// ; ---------------------------------------------------------------------------

loc_4E0DC:              ; CODE XREF: update?+247j
        cmp al, 0Ch
        jnz short loc_4E0E3
        jmp loc_4E7DE
// ; ---------------------------------------------------------------------------

loc_4E0E3:              ; CODE XREF: update?+24Ej
        cmp al, 15h
        jnz short loc_4E0EA
        jmp loc_4E7DE
// ; ---------------------------------------------------------------------------

loc_4E0EA:              ; CODE XREF: update?+255j
        cmp al, 17h
        jnz short loc_4E0F1
        jmp loc_4E7DE
// ; ---------------------------------------------------------------------------

loc_4E0F1:              ; CODE XREF: update?+25Cj
        cmp al, 14h
        jnz short loc_4E0F8
        jmp loc_4E847
// ; ---------------------------------------------------------------------------

loc_4E0F8:              ; CODE XREF: update?+263j
        cmp al, 12h
        jnz short loc_4E0FF
        jmp loc_4E8F9
// ; ---------------------------------------------------------------------------

loc_4E0FF:              ; CODE XREF: update?+26Aj
        push    si
        sub si, 78h ; 'x'
        call    sub_4F21F
        pop si
        jb  short locret_4E10B
        jmp short loc_4E0AA
// ; ---------------------------------------------------------------------------

locret_4E10B:               ; CODE XREF: update?+277j
        retn
// ; ---------------------------------------------------------------------------

loc_4E10C:              ; CODE XREF: update?+1BBj update?+2F3j
        mov word_510CB, 1
        mov ax, [si+1832h]
        cmp ax, 0
        jnz short loc_4E11E
        jmp loc_4E36D
// ; ---------------------------------------------------------------------------

loc_4E11E:              ; CODE XREF: update?+289j
        cmp ax, 2
        jnz short loc_4E126
        jmp loc_4E41E
// ; ---------------------------------------------------------------------------

loc_4E126:              ; CODE XREF: update?+291j
        cmp al, 19h
        jnz short loc_4E12D
        jmp loc_4E40D
// ; ---------------------------------------------------------------------------

loc_4E12D:              ; CODE XREF: update?+298j
        cmp ax, 4
        jnz short loc_4E135
        jmp loc_4E588
// ; ---------------------------------------------------------------------------

loc_4E135:              ; CODE XREF: update?+2A0j
        cmp ax, 7
        jnz short loc_4E13D
        jmp loc_4E674
// ; ---------------------------------------------------------------------------

loc_4E13D:              ; CODE XREF: update?+2A8j
        cmp ax, 1
        jnz short loc_4E145
        jmp loc_4E6BA
// ; ---------------------------------------------------------------------------

loc_4E145:              ; CODE XREF: update?+2B0j
        cmp al, 13h
        jnz short loc_4E14C
        jmp loc_4E73C
// ; ---------------------------------------------------------------------------

loc_4E14C:              ; CODE XREF: update?+2B7j
        cmp al, 0Bh
        jnz short loc_4E153
        jmp loc_4E7F5
// ; ---------------------------------------------------------------------------

loc_4E153:              ; CODE XREF: update?+2BEj
        cmp al, 16h
        jnz short loc_4E15A
        jmp loc_4E7F5
// ; ---------------------------------------------------------------------------

loc_4E15A:              ; CODE XREF: update?+2C5j
        cmp al, 17h
        jnz short loc_4E161
        jmp loc_4E7F5
// ; ---------------------------------------------------------------------------

loc_4E161:              ; CODE XREF: update?+2CCj
        cmp ax, 14h
        jnz short loc_4E169
        jmp loc_4E863
// ; ---------------------------------------------------------------------------

loc_4E169:              ; CODE XREF: update?+2D4j
        cmp ax, 12h
        jnz short loc_4E171
        jmp loc_4E920
// ; ---------------------------------------------------------------------------

loc_4E171:              ; CODE XREF: update?+2DCj
        cmp ax, 8
        jnz short loc_4E179
        jmp loc_4E993
// ; ---------------------------------------------------------------------------

loc_4E179:              ; CODE XREF: update?+2E4j
        push    si
        sub si, 2
        call    sub_4F21F
        pop si
        jb  short locret_4E185
        jmp short loc_4E10C
// ; ---------------------------------------------------------------------------

locret_4E185:               ; CODE XREF: update?+2F1j
        retn
// ; ---------------------------------------------------------------------------

loc_4E186:              ; CODE XREF: update?+1C8j update?+355j
        mov ax, leveldata[si+78h]
        cmp ax, 0
        jnz short loc_4E192
        jmp loc_4E38A
// ; ---------------------------------------------------------------------------

loc_4E192:              ; CODE XREF: update?+2FDj
        cmp ax, 2
        jnz short loc_4E19A
        jmp loc_4E44F
// ; ---------------------------------------------------------------------------

loc_4E19A:              ; CODE XREF: update?+305j
        cmp al, 19h
        jnz short loc_4E1A1
        jmp loc_4E43E
// ; ---------------------------------------------------------------------------

loc_4E1A1:              ; CODE XREF: update?+30Cj
        cmp ax, 4
        jnz short loc_4E1A9
        jmp loc_4E5A8
// ; ---------------------------------------------------------------------------

loc_4E1A9:              ; CODE XREF: update?+314j
        cmp ax, 7
        jnz short loc_4E1B1
        jmp loc_4E674
// ; ---------------------------------------------------------------------------

loc_4E1B1:              ; CODE XREF: update?+31Cj
        cmp al, 13h
        jnz short loc_4E1B8
        jmp loc_4E766
// ; ---------------------------------------------------------------------------

loc_4E1B8:              ; CODE XREF: update?+323j
        cmp al, 0Ah
        jnz short loc_4E1BF
        jmp loc_4E80C
// ; ---------------------------------------------------------------------------

loc_4E1BF:              ; CODE XREF: update?+32Aj
        cmp al, 15h
        jnz short loc_4E1C6
        jmp loc_4E80C
// ; ---------------------------------------------------------------------------

loc_4E1C6:              ; CODE XREF: update?+331j
        cmp al, 17h
        jnz short loc_4E1CD
        jmp loc_4E80C
// ; ---------------------------------------------------------------------------

loc_4E1CD:              ; CODE XREF: update?+338j
        cmp al, 14h
        jnz short loc_4E1D4
        jmp loc_4E87F
// ; ---------------------------------------------------------------------------

loc_4E1D4:              ; CODE XREF: update?+33Fj
        cmp al, 12h
        jnz short loc_4E1DB
        jmp loc_4E947
// ; ---------------------------------------------------------------------------

loc_4E1DB:              ; CODE XREF: update?+346j
        push    si
        add si, 78h ; 'x'
        call    sub_4F21F
        pop si
        jb  short locret_4E1E7
        jmp short loc_4E186
// ; ---------------------------------------------------------------------------

locret_4E1E7:               ; CODE XREF: update?+353j
        retn
// ; ---------------------------------------------------------------------------

loc_4E1E8:              ; CODE XREF: update?+1D5j update?+3CDj
        mov word_510CB, 0
        mov ax, leveldata[si+2]
        cmp ax, 0
        jnz short loc_4E1FA
        jmp loc_4E3B3
// ; ---------------------------------------------------------------------------

loc_4E1FA:              ; CODE XREF: update?+365j
        cmp ax, 2
        jnz short loc_4E202
        jmp loc_4E48C
// ; ---------------------------------------------------------------------------

loc_4E202:              ; CODE XREF: update?+36Dj
        cmp al, 19h
        jnz short loc_4E209
        jmp loc_4E47B
// ; ---------------------------------------------------------------------------

loc_4E209:              ; CODE XREF: update?+374j
        cmp ax, 4
        jnz short loc_4E211
        jmp loc_4E5D4
// ; ---------------------------------------------------------------------------

loc_4E211:              ; CODE XREF: update?+37Cj
        cmp ax, 7
        jnz short loc_4E219
        jmp loc_4E674
// ; ---------------------------------------------------------------------------

loc_4E219:              ; CODE XREF: update?+384j
        cmp ax, 1
        jnz short loc_4E221
        jmp loc_4E6E1
// ; ---------------------------------------------------------------------------

loc_4E221:              ; CODE XREF: update?+38Cj
        cmp al, 13h
        jnz short loc_4E228
        jmp loc_4E790
// ; ---------------------------------------------------------------------------

loc_4E228:              ; CODE XREF: update?+393j
        cmp al, 9
        jnz short loc_4E22F
        jmp loc_4E823
// ; ---------------------------------------------------------------------------

loc_4E22F:              ; CODE XREF: update?+39Aj
        cmp al, 16h
        jnz short loc_4E236
        jmp loc_4E823
// ; ---------------------------------------------------------------------------

loc_4E236:              ; CODE XREF: update?+3A1j
        cmp al, 17h
        jnz short loc_4E23D
        jmp loc_4E823
// ; ---------------------------------------------------------------------------

loc_4E23D:              ; CODE XREF: update?+3A8j
        cmp al, 14h
        jnz short loc_4E244
        jmp loc_4E89A
// ; ---------------------------------------------------------------------------

loc_4E244:              ; CODE XREF: update?+3AFj
        cmp al, 12h
        jnz short loc_4E24B
        jmp loc_4E96D
// ; ---------------------------------------------------------------------------

loc_4E24B:              ; CODE XREF: update?+3B6j
        cmp ax, 8
        jnz short loc_4E253
        jmp loc_4E9B9
// ; ---------------------------------------------------------------------------

loc_4E253:              ; CODE XREF: update?+3BEj
        push    si
        add si, 2
        call    sub_4F21F
        pop si
        jb  short locret_4E25F
        jmp short loc_4E1E8
// ; ---------------------------------------------------------------------------

locret_4E25F:               ; CODE XREF: update?+3CBj
        retn
// ; ---------------------------------------------------------------------------

loc_4E260:              ; CODE XREF: update?+1E2j
        mov ax, leveldata[si-78h]
        cmp ax, 2
        jnz short loc_4E26C
        jmp loc_4E4BD
// ; ---------------------------------------------------------------------------

loc_4E26C:              ; CODE XREF: update?+3D7j
        cmp al, 19h
        jnz short loc_4E273
        jmp loc_4E4AC
// ; ---------------------------------------------------------------------------

loc_4E273:              ; CODE XREF: update?+3DEj
        cmp ax, 4
        jnz short loc_4E27B
        jmp loc_4E5F4
// ; ---------------------------------------------------------------------------

loc_4E27B:              ; CODE XREF: update?+3E6j
        cmp al, 13h
        jnz short loc_4E282
        jmp loc_4E712
// ; ---------------------------------------------------------------------------

loc_4E282:              ; CODE XREF: update?+3EDj
        cmp al, 14h
        jnz short locret_4E289
        jmp loc_4E8B6
// ; ---------------------------------------------------------------------------

locret_4E289:               ; CODE XREF: update?+3F4j
        retn
// ; ---------------------------------------------------------------------------

loc_4E28A:              ; CODE XREF: update?+1EFj
        mov word_510CB, 1
        mov ax, [si+1832h]
        cmp ax, 2
        jnz short loc_4E29C
        jmp loc_4E4E9
// ; ---------------------------------------------------------------------------

loc_4E29C:              ; CODE XREF: update?+407j
        cmp al, 19h
        jnz short loc_4E2A3
        jmp loc_4E4D8
// ; ---------------------------------------------------------------------------

loc_4E2A3:              ; CODE XREF: update?+40Ej
        cmp ax, 4
        jnz short loc_4E2AB
        jmp loc_4E614
// ; ---------------------------------------------------------------------------

loc_4E2AB:              ; CODE XREF: update?+416j
        cmp al, 13h
        jnz short loc_4E2B2
        jmp loc_4E73C
// ; ---------------------------------------------------------------------------

loc_4E2B2:              ; CODE XREF: update?+41Dj
        cmp al, 14h
        jnz short locret_4E2B9
        jmp loc_4E8C5
// ; ---------------------------------------------------------------------------

locret_4E2B9:               ; CODE XREF: update?+424j
        retn
// ; ---------------------------------------------------------------------------

loc_4E2BA:              ; CODE XREF: update?+1FCj
        mov ax, [si+18ACh]
        cmp ax, 2
        jnz short loc_4E2C6
        jmp loc_4E515
// ; ---------------------------------------------------------------------------

loc_4E2C6:              ; CODE XREF: update?+431j
        cmp al, 19h
        jnz short loc_4E2CD
        jmp loc_4E504
// ; ---------------------------------------------------------------------------

loc_4E2CD:              ; CODE XREF: update?+438j
        cmp ax, 4
        jnz short loc_4E2D5
        jmp loc_4E634
// ; ---------------------------------------------------------------------------

loc_4E2D5:              ; CODE XREF: update?+440j
        cmp al, 13h
        jnz short loc_4E2DC
        jmp loc_4E766
// ; ---------------------------------------------------------------------------

loc_4E2DC:              ; CODE XREF: update?+447j
        cmp al, 14h
        jnz short locret_4E2E3
        jmp loc_4E8D4
// ; ---------------------------------------------------------------------------

locret_4E2E3:               ; CODE XREF: update?+44Ej
        retn
// ; ---------------------------------------------------------------------------

loc_4E2E4:              ; CODE XREF: update?+209j
        mov word_510CB, 0
        mov ax, [si+1836h]
        cmp ax, 2
        jnz short loc_4E2F6
        jmp loc_4E541
// ; ---------------------------------------------------------------------------

loc_4E2F6:              ; CODE XREF: update?+461j
        cmp al, 19h
        jnz short loc_4E2FD
        jmp loc_4E530
// ; ---------------------------------------------------------------------------

loc_4E2FD:              ; CODE XREF: update?+468j
        cmp ax, 4
        jnz short loc_4E305
        jmp loc_4E654
// ; ---------------------------------------------------------------------------

loc_4E305:              ; CODE XREF: update?+470j
        cmp al, 13h
        jnz short loc_4E30C
        jmp loc_4E790
// ; ---------------------------------------------------------------------------

loc_4E30C:              ; CODE XREF: update?+477j
        cmp al, 14h
        jnz short locret_4E313
        jmp loc_4E8E3
// ; ---------------------------------------------------------------------------

locret_4E313:               ; CODE XREF: update?+47Ej
        retn
// ; ---------------------------------------------------------------------------

loc_4E314:              ; CODE XREF: update?+211j
        cmp byte_5195C, 0
        jz  short locret_4E343
        cmp byte_510DB, 0
        jnz short locret_4E343
        cmp byte_510D3, 1
        jnz short locret_4E343
        mov byte ptr [si+1835h], 2Ah ; '*'
        mov word_510EE, 40h ; '@'
        mov dx, 110Eh
        mov byte_510DB, 1
        mov word_510DC, si
        jmp loc_4E9F3
// ; ---------------------------------------------------------------------------

locret_4E343:               ; CODE XREF: update?+489j update?+490j ...
        retn
// ; ---------------------------------------------------------------------------

loc_4E344:              ; CODE XREF: update?+223j
        cmp word_510CB, 0
        jz  short loc_4E350
        mov dx, 0DFEh
        jmp short loc_4E353
// ; ---------------------------------------------------------------------------

loc_4E350:              ; CODE XREF: update?+4B9j
        mov dx, 0E0Eh

loc_4E353:              ; CODE XREF: update?+4BEj
        mov byte ptr [si+17BDh], 1
        mov byte ptr [si+17BCh], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        sub si, 78h ; 'x'
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E36D:              ; CODE XREF: update?+28Bj
        mov dx, 0E1Eh
        mov byte ptr [si+1833h], 2
        mov byte ptr [si+1832h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        sub si, 2
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E38A:              ; CODE XREF: update?+69j update?+2FFj
        cmp word_510CB, 0
        jz  short loc_4E396
        mov dx, 0E2Eh
        jmp short loc_4E399
// ; ---------------------------------------------------------------------------

loc_4E396:              ; CODE XREF: update?+4FFj
        mov dx, 0E3Eh

loc_4E399:              ; CODE XREF: update?+504j
        mov byte ptr [si+18ADh], 3
        mov byte ptr [si+18ACh], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        add si, 78h ; 'x'
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E3B3:              ; CODE XREF: update?+367j
        mov dx, 0E4Eh
        mov byte ptr [si+1837h], 4
        mov byte ptr [si+1836h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        add si, 2
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E3D0:              ; CODE XREF: update?+232j
        cmp byte ptr [si+17BDh], 0
        jl  short loc_4E3DB
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4E3DB:              ; CODE XREF: update?+545j
        mov word ptr [si+17BCh], 2

loc_4E3E1:              ; CODE XREF: update?+22Bj
        call    sound?9
        cmp word_510CB, 0
        jz  short loc_4E3F0
        mov dx, 0E6Eh
        jmp short loc_4E3F3
// ; ---------------------------------------------------------------------------

loc_4E3F0:              ; CODE XREF: update?+559j
        mov dx, 0E7Eh

loc_4E3F3:              ; CODE XREF: update?+55Ej
        mov byte ptr [si+17BDh], 5
        mov byte ptr [si+17BCh], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        sub si, 78h ; 'x'
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E40D:              ; CODE XREF: update?+29Aj
        cmp byte ptr [si+1833h], 0
        jl  short loc_4E418
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4E418:              ; CODE XREF: update?+582j
        mov word ptr [si+1832h], 2

loc_4E41E:              ; CODE XREF: update?+293j
        call    sound?9
        mov dx, 0E8Eh
        mov byte ptr [si+1833h], 2
        mov byte ptr [si+1832h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        sub si, 2
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E43E:              ; CODE XREF: update?+30Ej
        cmp byte ptr [si+18ADh], 0
        jl  short loc_4E449
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4E449:              ; CODE XREF: update?+5B3j
        mov word ptr [si+18ACh], 2

loc_4E44F:              ; CODE XREF: update?+307j
        call    sound?9
        cmp word_510CB, 0
        jz  short loc_4E45E
        mov dx, 0E9Eh
        jmp short loc_4E461
// ; ---------------------------------------------------------------------------

loc_4E45E:              ; CODE XREF: update?+5C7j
        mov dx, 0EAEh

loc_4E461:              ; CODE XREF: update?+5CCj
        mov byte ptr [si+18ADh], 7
        mov byte ptr [si+18ACh], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        add si, 78h ; 'x'
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E47B:              ; CODE XREF: update?+376j
        cmp byte ptr [si+1837h], 0
        jl  short loc_4E486
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4E486:              ; CODE XREF: update?+5F0j
        mov word ptr [si+1836h], 2

loc_4E48C:              ; CODE XREF: update?+36Fj
        call    sound?9
        mov dx, 0EBEh
        mov byte ptr [si+1837h], 8
        mov byte ptr [si+1836h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        add si, 2
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E4AC:              ; CODE XREF: update?+3E0j
        cmp byte ptr [si+17BDh], 0
        jl  short loc_4E4B7
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4E4B7:              ; CODE XREF: update?+621j
        mov word ptr [si+17BCh], 2

loc_4E4BD:              ; CODE XREF: update?+3D9j
        push    si
        mov di, [si+6155h]
        mov si, word_51840
        call    sub_4F200
        pop si
        call    sound?9
        mov dx, 0ECEh
        mov byte ptr [si+1835h], 10h
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E4D8:              ; CODE XREF: update?+410j
        cmp byte ptr [si+1833h], 0
        jl  short loc_4E4E3
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4E4E3:              ; CODE XREF: update?+64Dj
        mov word ptr [si+1832h], 2

loc_4E4E9:              ; CODE XREF: update?+409j
        push    si
        mov di, [si+6155h]
        mov si, word_51842
        call    sub_4F200
        pop si
        call    sound?9
        mov dx, 0EDEh
        mov byte ptr [si+1835h], 11h
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E504:              ; CODE XREF: update?+43Aj
        cmp byte ptr [si+18ADh], 0
        jl  short loc_4E50F
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4E50F:              ; CODE XREF: update?+679j
        mov word ptr [si+18ACh], 2

loc_4E515:              ; CODE XREF: update?+433j
        push    si
        mov di, [si+6155h]
        mov si, word_51844
        call    sub_4F200
        pop si
        call    sound?9
        mov dx, 0EEEh
        mov byte ptr [si+1835h], 12h
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E530:              ; CODE XREF: update?+46Aj
        cmp byte ptr [si+1837h], 0
        jl  short loc_4E53B
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4E53B:              ; CODE XREF: update?+6A5j
        mov word ptr [si+1836h], 2

loc_4E541:              ; CODE XREF: update?+463j
        push    si
        mov di, [si+6155h]
        mov si, word_51846
        call    sub_4F200
        pop si
        call    sound?9
        mov dx, 0EFEh
        mov byte ptr [si+1835h], 13h
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E55C:              ; CODE XREF: update?+23Aj
        call    sound?5
        cmp word_510CB, 0
        jz  short loc_4E56B
        mov dx, 0F0Eh
        jmp short loc_4E56E
// ; ---------------------------------------------------------------------------

loc_4E56B:              ; CODE XREF: update?+6D4j
        mov dx, 0F1Eh

loc_4E56E:              ; CODE XREF: update?+6D9j
        mov byte ptr [si+17BDh], 9
        mov byte ptr [si+17BCh], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        sub si, 78h ; 'x'
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E588:              ; CODE XREF: update?+2A2j
        call    sound?5
        mov dx, 0F2Eh
        mov byte ptr [si+1833h], 0Ah
        mov byte ptr [si+1832h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        sub si, 2
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E5A8:              ; CODE XREF: update?+316j
        call    sound?5
        cmp word_510CB, 0
        jz  short loc_4E5B7
        mov dx, 0F3Eh
        jmp short loc_4E5BA
// ; ---------------------------------------------------------------------------

loc_4E5B7:              ; CODE XREF: update?+720j
        mov dx, 0F4Eh

loc_4E5BA:              ; CODE XREF: update?+725j
        mov byte ptr [si+18ADh], 0Bh
        mov byte ptr [si+18ACh], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        add si, 78h ; 'x'
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E5D4:              ; CODE XREF: update?+37Ej
        call    sound?5
        mov dx, 0F5Eh
        mov byte ptr [si+1837h], 0Ch
        mov byte ptr [si+1836h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        add si, 2
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E5F4:              ; CODE XREF: update?+3E8j
        push    si
        mov di, [si+6155h]
        mov si, word_51840
        call    sub_4F200
        pop si
        call    sound?5
        mov dx, 0F6Eh
        mov byte ptr [si+1835h], 14h
        mov byte ptr [si+17BDh], 0FFh
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E614:              ; CODE XREF: update?+418j
        push    si
        mov di, [si+6155h]
        mov si, word_51842
        call    sub_4F200
        pop si
        call    sound?5
        mov dx, 0F7Eh
        mov byte ptr [si+1835h], 15h
        mov byte ptr [si+1833h], 0FFh
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E634:              ; CODE XREF: update?+442j
        push    si
        mov di, [si+6155h]
        mov si, word_51844
        call    sub_4F200
        pop si
        call    sound?5
        mov dx, 0F8Eh
        mov byte ptr [si+1835h], 16h
        mov byte ptr [si+18ADh], 0FFh
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E654:              ; CODE XREF: update?+472j
        push    si
        mov di, [si+6155h]
        mov si, word_51846
        call    sub_4F200
        pop si
        call    sound?5
        mov dx, 0F9Eh
        mov byte ptr [si+1835h], 17h
        mov byte ptr [si+1837h], 0FFh
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E674:              ; CODE XREF: update?+242j update?+2AAj ...
        cmp byte_5195A, 0
        jnz short locret_4E6B9
        call    sound?10
        push    si
        mov byte_5A19B, 1
        mov byte_510BB, 1
        mov byte_510BA, 0
        cmp byte_5A2F9, 0
        jnz short loc_4E6A4
        cmp byte_510B3, 0
        jz  short loc_4E6A4
        mov byte_5A323, 1
        call    sub_4A95F

loc_4E6A4:              ; CODE XREF: update?+803j update?+80Aj
        call    sub_4D24D
        mov word_51978, 40h ; '@'
        pop si
        mov dx, 0E5Eh
        mov byte ptr [si+1835h], 0Dh
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

locret_4E6B9:               ; CODE XREF: update?+7E9j
        retn
// ; ---------------------------------------------------------------------------

loc_4E6BA:              ; CODE XREF: update?+2B2j
        mov ax, [si+1830h]
        cmp ax, 0
        jz  short loc_4E6C4
        retn
// ; ---------------------------------------------------------------------------

loc_4E6C4:              ; CODE XREF: update?+831j
        mov byte ptr [si+1831h], 1
        push    si
        mov di, [si+6155h]
        mov si, word_5157A
        call    sub_4F200
        pop si
        mov dx, 0FAEh
        mov byte ptr [si+1835h], 0Eh
        jmp loc_4E9E7
// ; ---------------------------------------------------------------------------

loc_4E6E1:              ; CODE XREF: update?+38Ej
        mov ax, [si+1838h]
        cmp ax, 0
        jz  short loc_4E6EB
        retn
// ; ---------------------------------------------------------------------------

loc_4E6EB:              ; CODE XREF: update?+858j
        mov ax, [si+18AEh]
        cmp ax, 0
        jnz short loc_4E6F5
        retn
// ; ---------------------------------------------------------------------------

loc_4E6F5:              ; CODE XREF: update?+862j
        mov byte ptr [si+1839h], 1
        push    si
        mov di, [si+6155h]
        mov si, word_5157C
        call    sub_4F200
        pop si
        mov dx, 0FBEh
        mov byte ptr [si+1835h], 0Fh
        jmp loc_4E9E7
// ; ---------------------------------------------------------------------------

loc_4E712:              ; CODE XREF: update?+249j update?+3EFj
        push    si
        mov di, [si+6155h]
        mov si, word_51840
        call    sub_4F200
        pop si
        cmp byte_5196B, 0
        jz  short loc_4E72D
        mov word_510CD, 0Ah
        retn
// ; ---------------------------------------------------------------------------

loc_4E72D:              ; CODE XREF: update?+894j
        push    si
        mov di, [si+60DDh]
        mov si, word_51848
        call    sub_4F200
        pop si
        jmp short loc_4E7B8
// ; ---------------------------------------------------------------------------

loc_4E73C:              ; CODE XREF: update?+2B9j update?+41Fj
        push    si
        mov di, [si+6155h]
        mov si, word_51842
        call    sub_4F200
        pop si
        cmp byte_5196B, 0
        jz  short loc_4E757
        mov word_510CD, 0Ah
        retn
// ; ---------------------------------------------------------------------------

loc_4E757:              ; CODE XREF: update?+8BEj
        push    si
        mov di, [si+6153h]
        mov si, word_51848
        call    sub_4F200
        pop si
        jmp short loc_4E7B8
// ; ---------------------------------------------------------------------------

loc_4E766:              ; CODE XREF: update?+325j update?+449j
        push    si
        mov di, [si+6155h]
        mov si, word_51844
        call    sub_4F200
        pop si
        cmp byte_5196B, 0
        jz  short loc_4E781
        mov word_510CD, 0Ah
        retn
// ; ---------------------------------------------------------------------------

loc_4E781:              ; CODE XREF: update?+8E8j
        push    si
        mov di, [si+61CDh]
        mov si, word_51848
        call    sub_4F200
        pop si
        jmp short loc_4E7B8
// ; ---------------------------------------------------------------------------

loc_4E790:              ; CODE XREF: update?+395j update?+479j
        push    si
        mov di, [si+6155h]
        mov si, word_51846
        call    sub_4F200
        pop si
        cmp byte_5196B, 0
        jz  short loc_4E7AB
        mov word_510CD, 0Ah
        retn
// ; ---------------------------------------------------------------------------

loc_4E7AB:              ; CODE XREF: update?+912j
        push    si
        mov di, [si+6157h]
        mov si, word_51848
        call    sub_4F200
        pop si

loc_4E7B8:              ; CODE XREF: update?+8AAj update?+8D4j ...
        mov byte_5196A, 7
        mov byte_5196B, 1
        push    si
        mov si, 0
        mov cx, 5A0h

loc_4E7C9:              ; CODE XREF: update?+94Aj
        cmp word ptr leveldata[si], 12h
        jnz short loc_4E7D7
        push(cx);
        push    si
        call    sub_4A61F
        pop si
        pop(cx);

loc_4E7D7:              ; CODE XREF: update?+93Ej
        add si, 2
        loop    loc_4E7C9
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4E7DE:              ; CODE XREF: update?+250j update?+257j ...
        cmp word ptr [si+1744h], 0
        jz  short loc_4E7E6
        retn
// ; ---------------------------------------------------------------------------

loc_4E7E6:              ; CODE XREF: update?+953j
        mov dx, 0FCEh
        mov byte ptr [si+1835h], 18h
        mov byte ptr [si+1745h], 3
        jmp short loc_4E838
// ; ---------------------------------------------------------------------------

loc_4E7F5:              ; CODE XREF: update?+2C0j update?+2C7j ...
        cmp word ptr [si+1830h], 0
        jz  short loc_4E7FD
        retn
// ; ---------------------------------------------------------------------------

loc_4E7FD:              ; CODE XREF: update?+96Aj
        mov dx, 0FDEh
        mov byte ptr [si+1835h], 19h
        mov byte ptr [si+1831h], 3
        jmp short loc_4E838
// ; ---------------------------------------------------------------------------

loc_4E80C:              ; CODE XREF: update?+32Cj update?+333j ...
        cmp word ptr [si+1924h], 0
        jz  short loc_4E814
        retn
// ; ---------------------------------------------------------------------------

loc_4E814:              ; CODE XREF: update?+981j
        mov dx, 0FEEh
        mov byte ptr [si+1835h], 1Ah
        mov byte ptr [si+1925h], 3
        jmp short loc_4E838
// ; ---------------------------------------------------------------------------

loc_4E823:              ; CODE XREF: update?+39Cj update?+3A3j ...
        cmp word ptr [si+1838h], 0
        jz  short loc_4E82B
        retn
// ; ---------------------------------------------------------------------------

loc_4E82B:              ; CODE XREF: update?+998j
        mov dx, 0FFEh
        mov byte ptr [si+1835h], 1Bh
        mov byte ptr [si+1839h], 3

loc_4E838:              ; CODE XREF: update?+963j update?+97Aj ...
        mov word_510EE, 0
        mov word_510D9, 1
        jmp loc_4E9F3
// ; ---------------------------------------------------------------------------

loc_4E847:              ; CODE XREF: update?+265j
        cmp word_510CB, 0
        jz  short loc_4E853
        mov dx, 100Eh
        jmp short loc_4E856
// ; ---------------------------------------------------------------------------

loc_4E853:              ; CODE XREF: update?+9BCj
        mov dx, 101Eh

loc_4E856:              ; CODE XREF: update?+9C1j
        mov byte ptr [si+1835h], 1Ch
        mov byte ptr [si+17BDh], 3
        jmp loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E863:              ; CODE XREF: update?+2D6j
        mov dx, 102Eh
        mov byte ptr [si+1833h], 1Dh
        mov byte ptr [si+1832h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        sub si, 2
        jmp short loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E87F:              ; CODE XREF: update?+341j
        cmp word_510CB, 0
        jz  short loc_4E88B
        mov dx, 103Eh
        jmp short loc_4E88E
// ; ---------------------------------------------------------------------------

loc_4E88B:              ; CODE XREF: update?+9F4j
        mov dx, 104Eh

loc_4E88E:              ; CODE XREF: update?+9F9j
        mov byte ptr [si+1835h], 1Eh
        mov byte ptr [si+18ADh], 3
        jmp short loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E89A:              ; CODE XREF: update?+3B1j
        mov dx, 105Eh
        mov byte ptr [si+1837h], 1Fh
        mov byte ptr [si+1836h], 3
        mov byte ptr [si+1835h], 3
        mov byte ptr leveldata[si], 0
        add si, 2
        jmp short loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E8B6:              ; CODE XREF: update?+3F6j
        mov dx, 106Eh
        mov byte ptr [si+1835h], 20h ; ' '
        mov byte ptr [si+17BDh], 3
        jmp short loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E8C5:              ; CODE XREF: update?+426j
        mov dx, 107Eh
        mov byte ptr [si+1835h], 21h ; '!'
        mov byte ptr [si+1833h], 3
        jmp short loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E8D4:              ; CODE XREF: update?+450j
        mov dx, 108Eh
        mov byte ptr [si+1835h], 22h ; '"'
        mov byte ptr [si+18ADh], 3
        jmp short loc_4E8F0
// ; ---------------------------------------------------------------------------

loc_4E8E3:              ; CODE XREF: update?+480j
        mov dx, 109Eh
        mov byte ptr [si+1835h], 23h ; '#'
        mov byte ptr [si+1837h], 3

loc_4E8F0:              ; CODE XREF: update?+4DAj update?+4F7j ...
        mov word_510EE, 0
        jmp loc_4E9ED
// ; ---------------------------------------------------------------------------

loc_4E8F9:              ; CODE XREF: update?+26Cj
        mov ax, [si+1744h]
        cmp ax, 0
        jz  short loc_4E903
        retn
// ; ---------------------------------------------------------------------------

loc_4E903:              ; CODE XREF: update?+A70j
        mov byte ptr [si+1745h], 12h
        push    si
        mov di, [si+6155h]
        mov si, word_5157C
        call    sub_4F200
        pop si
        mov dx, 10AEh
        mov byte ptr [si+1835h], 24h ; '$'
        jmp loc_4E9E7
// ; ---------------------------------------------------------------------------

loc_4E920:              ; CODE XREF: update?+2DEj
        mov ax, [si+1830h]
        cmp ax, 0
        jz  short loc_4E92A
        retn
// ; ---------------------------------------------------------------------------

loc_4E92A:              ; CODE XREF: update?+A97j
        mov byte ptr [si+1831h], 12h
        push    si
        mov di, [si+6155h]
        mov si, word_5157A
        call    sub_4F200
        pop si
        mov dx, 10BEh
        mov byte ptr [si+1835h], 25h ; '%'
        jmp loc_4E9E7
// ; ---------------------------------------------------------------------------

loc_4E947:              ; CODE XREF: update?+348j
        mov ax, [si+1924h]
        cmp ax, 0
        jz  short loc_4E951
        retn
// ; ---------------------------------------------------------------------------

loc_4E951:              ; CODE XREF: update?+ABEj
        mov byte ptr [si+1925h], 12h
        push    si
        mov di, [si+6155h]
        mov si, word_5157C
        call    sub_4F200
        pop si
        mov dx, 10CEh
        mov byte ptr [si+1835h], 27h ; '''
        jmp short loc_4E9E7
// ; ---------------------------------------------------------------------------

loc_4E96D:              ; CODE XREF: update?+3B8j
        mov ax, [si+1838h]
        cmp ax, 0
        jz  short loc_4E977
        retn
// ; ---------------------------------------------------------------------------

loc_4E977:              ; CODE XREF: update?+AE4j
        mov byte ptr [si+1839h], 12h
        push    si
        mov di, [si+6155h]
        mov si, word_5157C
        call    sub_4F200
        pop si
        mov dx, 10DEh
        mov byte ptr [si+1835h], 26h ; '&'
        jmp short loc_4E9E7
// ; ---------------------------------------------------------------------------

loc_4E993:              ; CODE XREF: update?+2E6j
        mov ax, [si+1830h]
        cmp ax, 0
        jz  short loc_4E99D
        retn
// ; ---------------------------------------------------------------------------

loc_4E99D:              ; CODE XREF: update?+B0Aj
        mov byte ptr [si+1831h], 8
        push    si
        mov di, [si+6155h]
        mov si, word_5157A
        call    sub_4F200
        pop si
        mov dx, 10EEh
        mov byte ptr [si+1835h], 28h ; '('
        jmp short loc_4E9E7
// ; ---------------------------------------------------------------------------

loc_4E9B9:              ; CODE XREF: update?+3C0j
        mov ax, [si+1838h]
        cmp ax, 0
        jz  short loc_4E9C3
        retn
// ; ---------------------------------------------------------------------------

loc_4E9C3:              ; CODE XREF: update?+B30j
        mov ax, [si+18AEh]
        cmp ax, 0
        jnz short loc_4E9CD
        retn
// ; ---------------------------------------------------------------------------

loc_4E9CD:              ; CODE XREF: update?+B3Aj
        mov byte ptr [si+1839h], 1
        push    si
        mov di, [si+6155h]
        mov si, word_5157C
        call    sub_4F200
        pop si
        mov dx, 10FEh
        mov byte ptr [si+1835h], 29h ; ')'

loc_4E9E7:              ; CODE XREF: update?+84Ej update?+87Fj ...
        mov word_510EE, 8

loc_4E9ED:              ; CODE XREF: update?+A66j
        mov word_510D9, 0

loc_4E9F3:              ; CODE XREF: update?+4B0j update?+9B4j
        push    si
        push(di);
        push    es
        mov si, dx
        mov di, 0DE0h
        mov ax, ds
        mov es, ax
        assume es:data
        mov cx, 7
        rep movsw
        pop es
        assume es:nothing
        pop(di);
        pop si

loc_4EA07:              ; CODE XREF: update?+21j
        mov word_510CD, 0
        mov ax, word_510EE
        cmp ax, 0
        jz  short loc_4EA6B
        dec ax
        mov word_510EE, ax
        jnz short loc_4EA1E
        call    sound?6

loc_4EA1E:              ; CODE XREF: update?+B89j
        mov bl, [si+1835h]
        cmp bl, 0Eh
        jnz short loc_4EA2A
        jmp loc_4ED49
// ; ---------------------------------------------------------------------------

loc_4EA2A:              ; CODE XREF: update?+B95j
        cmp bl, 0Fh
        jnz short loc_4EA32
        jmp loc_4ED81
// ; ---------------------------------------------------------------------------

loc_4EA32:              ; CODE XREF: update?+B9Dj
        cmp bl, 28h ; '('
        jnz short loc_4EA3A
        jmp loc_4EDB9
// ; ---------------------------------------------------------------------------

loc_4EA3A:              ; CODE XREF: update?+BA5j
        cmp bl, 29h ; ')'
        jnz short loc_4EA42
        jmp loc_4EDF1
// ; ---------------------------------------------------------------------------

loc_4EA42:              ; CODE XREF: update?+BADj
        cmp bl, 24h ; '$'
        jnz short loc_4EA4A
        jmp loc_4EE29
// ; ---------------------------------------------------------------------------

loc_4EA4A:              ; CODE XREF: update?+BB5j
        cmp bl, 25h ; '%'
        jnz short loc_4EA52
        jmp loc_4EE61
// ; ---------------------------------------------------------------------------

loc_4EA52:              ; CODE XREF: update?+BBDj
        cmp bl, 27h ; '''
        jnz short loc_4EA5A
        jmp loc_4EE99
// ; ---------------------------------------------------------------------------

loc_4EA5A:              ; CODE XREF: update?+BC5j
        cmp bl, 26h ; '&'
        jnz short loc_4EA62
        jmp loc_4EED1
// ; ---------------------------------------------------------------------------

loc_4EA62:              ; CODE XREF: update?+BCDj
        cmp bl, 2Ah ; '*'
        jnz short locret_4EA6A
        jmp loc_4EF09
// ; ---------------------------------------------------------------------------

locret_4EA6A:               ; CODE XREF: update?+BD5j
        retn
// ; ---------------------------------------------------------------------------

loc_4EA6B:              ; CODE XREF: update?+B83j
        push    si
        push(di);
        mov ax, word_510FA
        add word_510E8, ax
        mov ax, word_510FC
        add word_510EA, ax
        mov di, [si+6155h]
        add di, word_510F0
        mov si, word_510F8
        mov ax, [si]
        add si, 2
        mov word_510F8, si
        mov si, ax
        mov bx, word_510F4
        mov dx, word_510F6
        push    ds
        mov ax, es
        mov ds, ax

loc_4EA9F:              ; CODE XREF: update?+C28j
        mov cx, bx
        rep movsb
        add si, 7Ah ; 'z'
        sub si, bx
        add di, 7Ah ; 'z'
        sub di, bx
        cmp si, 4D34h
        jb  short loc_4EAB7
        sub si, 4D0Ch

loc_4EAB7:              ; CODE XREF: update?+C21j
        dec dx
        jnz short loc_4EA9F
        mov ax, ds
        pop ds
        cmp word_510D9, 0
        jz  short loc_4EAFA
        sub di, 7A0h
        add di, word_510F2
        mov si, word_510F8
        add si, 10h
        mov si, [si]
        mov dx, word_510F6
        push    ds
        mov ds, ax

loc_4EADC:              ; CODE XREF: update?+C65j
        mov cx, bx
        rep movsb
        add si, 7Ah ; 'z'
        sub si, bx
        add di, 7Ah ; 'z'
        sub di, bx
        cmp si, 4D34h
        jb  short loc_4EAF4
        sub si, 4D0Ch

loc_4EAF4:              ; CODE XREF: update?+C5Ej
        dec dx
        jnz short loc_4EADC
        pop ds
        jmp short loc_4EB04
// ; ---------------------------------------------------------------------------

loc_4EAFA:              ; CODE XREF: update?+C32j
        mov ax, word_510F0
        add ax, word_510F2
        mov word_510F0, ax

loc_4EB04:              ; CODE XREF: update?+C68j
        mov si, word_510F8
        cmp word ptr [si], 0FFFFh
        jz  short loc_4EB10
        pop(di);
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4EB10:              ; CODE XREF: update?+C7Bj
        pop(di);
        pop si
        mov ax, word_510FA
        shr ax, 1
        mov bx, word_510FC
        shr bx, 1
        add word_510C3, ax
        add word_510C5, bx
        mov bl, [si+1835h]
        mov byte ptr [si+1835h], 0
        cmp bl, 1
        jnz short loc_4EB36
        jmp loc_4EC93
// ; ---------------------------------------------------------------------------

loc_4EB36:              ; CODE XREF: update?+CA1j
        cmp bl, 2
        jnz short loc_4EB3E
        jmp loc_4ECB1
// ; ---------------------------------------------------------------------------

loc_4EB3E:              ; CODE XREF: update?+CA9j
        cmp bl, 3
        jnz short loc_4EB46
        jmp loc_4ECCF
// ; ---------------------------------------------------------------------------

loc_4EB46:              ; CODE XREF: update?+CB1j
        cmp bl, 4
        jnz short loc_4EB4E
        jmp loc_4EF53
// ; ---------------------------------------------------------------------------

loc_4EB4E:              ; CODE XREF: update?+CB9j
        cmp bl, 5
        jnz short loc_4EB56
        jmp loc_4EC93
// ; ---------------------------------------------------------------------------

loc_4EB56:              ; CODE XREF: update?+CC1j
        cmp bl, 6
        jnz short loc_4EB5E
        jmp loc_4ECB1
// ; ---------------------------------------------------------------------------

loc_4EB5E:              ; CODE XREF: update?+CC9j
        cmp bl, 7
        jnz short loc_4EB66
        jmp loc_4ECCF
// ; ---------------------------------------------------------------------------

loc_4EB66:              ; CODE XREF: update?+CD1j
        cmp bl, 8
        jnz short loc_4EB6E
        jmp loc_4EF53
// ; ---------------------------------------------------------------------------

loc_4EB6E:              ; CODE XREF: update?+CD9j
        cmp bl, 9
        jnz short loc_4EB76
        jmp loc_4EC85
// ; ---------------------------------------------------------------------------

loc_4EB76:              ; CODE XREF: update?+CE1j
        cmp bl, 0Ah
        jnz short loc_4EB7E
        jmp loc_4ECA3
// ; ---------------------------------------------------------------------------

loc_4EB7E:              ; CODE XREF: update?+CE9j
        cmp bl, 0Bh
        jnz short loc_4EB86
        jmp loc_4ECC1
// ; ---------------------------------------------------------------------------

loc_4EB86:              ; CODE XREF: update?+CF1j
        cmp bl, 0Ch
        jnz short loc_4EB8E
        jmp loc_4EF45
// ; ---------------------------------------------------------------------------

loc_4EB8E:              ; CODE XREF: update?+CF9j
        cmp bl, 0Eh
        jnz short loc_4EB96
        jmp loc_4ECE3
// ; ---------------------------------------------------------------------------

loc_4EB96:              ; CODE XREF: update?+D01j
        cmp bl, 0Fh
        jnz short loc_4EB9E
        jmp loc_4ED06
// ; ---------------------------------------------------------------------------

loc_4EB9E:              ; CODE XREF: update?+D09j
        cmp bl, 10h
        jnz short loc_4EBA6
        jmp loc_4EF71
// ; ---------------------------------------------------------------------------

loc_4EBA6:              ; CODE XREF: update?+D11j
        cmp bl, 11h
        jnz short loc_4EBAE
        jmp loc_4EF8D
// ; ---------------------------------------------------------------------------

loc_4EBAE:              ; CODE XREF: update?+D19j
        cmp bl, 13h
        jnz short loc_4EBB6
        jmp loc_4EFC5
// ; ---------------------------------------------------------------------------

loc_4EBB6:              ; CODE XREF: update?+D21j
        cmp bl, 12h
        jnz short loc_4EBBE
        jmp loc_4EFA9
// ; ---------------------------------------------------------------------------

loc_4EBBE:              ; CODE XREF: update?+D29j
        cmp bl, 14h
        jnz short loc_4EBC6
        jmp loc_4EF63
// ; ---------------------------------------------------------------------------

loc_4EBC6:              ; CODE XREF: update?+D31j
        cmp bl, 15h
        jnz short loc_4EBCE
        jmp loc_4EF7F
// ; ---------------------------------------------------------------------------

loc_4EBCE:              ; CODE XREF: update?+D39j
        cmp bl, 17h
        jnz short loc_4EBD6
        jmp loc_4EFB7
// ; ---------------------------------------------------------------------------

loc_4EBD6:              ; CODE XREF: update?+D41j
        cmp bl, 16h
        jnz short loc_4EBDE
        jmp loc_4EF9B
// ; ---------------------------------------------------------------------------

loc_4EBDE:              ; CODE XREF: update?+D49j
        cmp bl, 0Dh
        jnz short loc_4EBE6
        jmp loc_4ED42
// ; ---------------------------------------------------------------------------

loc_4EBE6:              ; CODE XREF: update?+D51j
        cmp bl, 18h
        jnz short loc_4EBEE
        jmp loc_4EFD3
// ; ---------------------------------------------------------------------------

loc_4EBEE:              ; CODE XREF: update?+D59j
        cmp bl, 19h
        jnz short loc_4EBF6
        jmp loc_4F001
// ; ---------------------------------------------------------------------------

loc_4EBF6:              ; CODE XREF: update?+D61j
        cmp bl, 1Ah
        jnz short loc_4EBFE
        jmp loc_4F02E
// ; ---------------------------------------------------------------------------

loc_4EBFE:              ; CODE XREF: update?+D69j
        cmp bl, 1Bh
        jnz short loc_4EC06
        jmp loc_4F05C
// ; ---------------------------------------------------------------------------

loc_4EC06:              ; CODE XREF: update?+D71j
        cmp bl, 1Ch
        jnz short loc_4EC0E
        jmp loc_4F089
// ; ---------------------------------------------------------------------------

loc_4EC0E:              ; CODE XREF: update?+D79j
        cmp bl, 1Dh
        jnz short loc_4EC16
        jmp loc_4F09C
// ; ---------------------------------------------------------------------------

loc_4EC16:              ; CODE XREF: update?+D81j
        cmp bl, 1Eh
        jnz short loc_4EC1E
        jmp loc_4F0AC
// ; ---------------------------------------------------------------------------

loc_4EC1E:              ; CODE XREF: update?+D89j
        cmp bl, 1Fh
        jnz short loc_4EC26
        jmp loc_4F0BF
// ; ---------------------------------------------------------------------------

loc_4EC26:              ; CODE XREF: update?+D91j
        cmp bl, 20h ; ' '
        jnz short loc_4EC2E
        jmp loc_4F0CF
// ; ---------------------------------------------------------------------------

loc_4EC2E:              ; CODE XREF: update?+D99j
        cmp bl, 21h ; '!'
        jnz short loc_4EC36
        jmp loc_4F0E6
// ; ---------------------------------------------------------------------------

loc_4EC36:              ; CODE XREF: update?+DA1j
        cmp bl, 22h ; '"'
        jnz short loc_4EC3E
        jmp loc_4F0FD
// ; ---------------------------------------------------------------------------

loc_4EC3E:              ; CODE XREF: update?+DA9j
        cmp bl, 23h ; '#'
        jnz short loc_4EC46
        jmp loc_4F114
// ; ---------------------------------------------------------------------------

loc_4EC46:              ; CODE XREF: update?+DB1j
        cmp bl, 24h ; '$'
        jnz short loc_4EC4E
        jmp loc_4F12B
// ; ---------------------------------------------------------------------------

loc_4EC4E:              ; CODE XREF: update?+DB9j
        cmp bl, 25h ; '%'
        jnz short loc_4EC56
        jmp loc_4F148
// ; ---------------------------------------------------------------------------

loc_4EC56:              ; CODE XREF: update?+DC1j
        cmp bl, 27h ; '''
        jnz short loc_4EC5E
        jmp loc_4F165
// ; ---------------------------------------------------------------------------

loc_4EC5E:              ; CODE XREF: update?+DC9j
        cmp bl, 26h ; '&'
        jnz short loc_4EC66
        jmp loc_4F182
// ; ---------------------------------------------------------------------------

loc_4EC66:              ; CODE XREF: update?+DD1j
        cmp bl, 28h ; '('
        jnz short loc_4EC6E
        jmp loc_4F19F
// ; ---------------------------------------------------------------------------

loc_4EC6E:              ; CODE XREF: update?+DD9j
        cmp bl, 29h ; ')'
        jnz short loc_4EC76
        jmp loc_4F1BC
// ; ---------------------------------------------------------------------------

loc_4EC76:              ; CODE XREF: update?+DE1j
        cmp bl, 2Ah ; '*'
        jnz short loc_4EC7E
        jmp loc_4F1EA
// ; ---------------------------------------------------------------------------

loc_4EC7E:              ; CODE XREF: update?+DE9j
        mov word_51974, 1
        retn
// ; ---------------------------------------------------------------------------

loc_4EC85:              ; CODE XREF: update?+CE3j
        cmp byte_5195A, 0
        jbe short loc_4EC90
        dec byte_5195A

loc_4EC90:              ; CODE XREF: update?+DFAj
        call    sub_4FD21

loc_4EC93:              ; CODE XREF: update?+CA3j update?+CC3j
        mov word ptr leveldata[si], 3
        add si, 78h ; 'x'
        call    sub_487FE
        sub si, 78h ; 'x'
        retn
// ; ---------------------------------------------------------------------------

loc_4ECA3:              ; CODE XREF: update?+CEBj
        cmp byte_5195A, 0
        jbe short loc_4ECAE
        dec byte_5195A

loc_4ECAE:              ; CODE XREF: update?+E18j
        call    sub_4FD21

loc_4ECB1:              ; CODE XREF: update?+CABj update?+CCBj
        mov word ptr leveldata[si], 3
        add si, 2
        call    sub_487FE
        sub si, 2
        retn
// ; ---------------------------------------------------------------------------

loc_4ECC1:              ; CODE XREF: update?+CF3j
        cmp byte_5195A, 0
        jbe short loc_4ECCC
        dec byte_5195A

loc_4ECCC:              ; CODE XREF: update?+E36j
        call    sub_4FD21

loc_4ECCF:              ; CODE XREF: update?+CB3j update?+CD3j
        cmp byte ptr [si+17BCh], 1Fh
        jz  short loc_4ECDC
        mov word ptr [si+17BCh], 0

loc_4ECDC:              ; CODE XREF: update?+E44j
        mov word ptr leveldata[si], 3
        retn
// ; ---------------------------------------------------------------------------

loc_4ECE3:              ; CODE XREF: update?+D03j
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4ECF0
        mov word ptr leveldata[si], 0

loc_4ECF0:              ; CODE XREF: update?+E58j
        mov word ptr [si+1832h], 3
        mov word ptr [si+1830h], 1
        sub si, 4
        call    sub_4ED29
        add si, 2
        retn
// ; ---------------------------------------------------------------------------

loc_4ED06:              ; CODE XREF: update?+D0Bj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4ED13
        mov word ptr leveldata[si], 0

loc_4ED13:              ; CODE XREF: update?+E7Bj
        mov word ptr [si+1836h], 3
        mov word ptr [si+1838h], 1
        add si, 4
        call    sub_4ED29
        sub si, 2
        retn
update?     endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================


sub_4ED29   proc near       ; CODE XREF: update?+E6Fp update?+E92p
        cmp byte ptr [si+18ACh], 11h
        jz  short loc_4ED38
        cmp byte ptr [si+18ACh], 0BBh ; '?'
        jz  short loc_4ED38
        retn
// ; ---------------------------------------------------------------------------

loc_4ED38:              ; CODE XREF: sub_4ED29+5j sub_4ED29+Cj
        add si, 78h ; 'x'
        call    sub_4A61F
        sub si, 78h ; 'x'
        retn
sub_4ED29   endp

// ; ---------------------------------------------------------------------------
; START OF FUNCTION CHUNK FOR update?

loc_4ED42:              ; CODE XREF: update?+D53j
        mov word_51974, 1
        retn
// ; ---------------------------------------------------------------------------

loc_4ED49:              ; CODE XREF: update?+B97j
        mov bl, byte_50941
        cmp bl, 2
        jnz short loc_4ED5A
        cmp word ptr [si+1832h], 1
        jnz short loc_4ED5A
        retn
// ; ---------------------------------------------------------------------------

loc_4ED5A:              ; CODE XREF: update?+EC0j update?+EC7j
        mov word ptr leveldata[si], 3
        mov word ptr [si+1832h], 1
        cmp byte ptr [si+1830h], 1Fh
        jz  short loc_4ED73
        mov word ptr [si+1830h], 0

loc_4ED73:              ; CODE XREF: update?+EDBj
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4ED81:              ; CODE XREF: update?+B9Fj
        mov bl, byte_50941
        cmp bl, 4
        jnz short loc_4ED92
        cmp word ptr [si+1836h], 1
        jnz short loc_4ED92
        retn
// ; ---------------------------------------------------------------------------

loc_4ED92:              ; CODE XREF: update?+EF8j update?+EFFj
        mov word ptr leveldata[si], 3
        mov word ptr [si+1836h], 1
        cmp byte ptr [si+1838h], 1Fh
        jz  short loc_4EDAB
        mov word ptr [si+1838h], 0

loc_4EDAB:              ; CODE XREF: update?+F13j
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4EDB9:              ; CODE XREF: update?+BA7j
        mov bl, byte_50941
        cmp bl, 2
        jnz short loc_4EDCA
        cmp word ptr [si+1832h], 8
        jnz short loc_4EDCA
        retn
// ; ---------------------------------------------------------------------------

loc_4EDCA:              ; CODE XREF: update?+F30j update?+F37j
        mov word ptr leveldata[si], 3
        mov word ptr [si+1832h], 8
        cmp byte ptr [si+1830h], 1Fh
        jz  short loc_4EDE3
        mov word ptr [si+1830h], 0

loc_4EDE3:              ; CODE XREF: update?+F4Bj
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4EDF1:              ; CODE XREF: update?+BAFj
        mov bl, byte_50941
        cmp bl, 4
        jnz short loc_4EE02
        cmp word ptr [si+1836h], 8
        jnz short loc_4EE02
        retn
// ; ---------------------------------------------------------------------------

loc_4EE02:              ; CODE XREF: update?+F68j update?+F6Fj
        mov word ptr leveldata[si], 3
        mov word ptr [si+1836h], 8
        cmp byte ptr [si+1838h], 1Fh
        jz  short loc_4EE1B
        mov word ptr [si+1838h], 0

loc_4EE1B:              ; CODE XREF: update?+F83j
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4EE29:              ; CODE XREF: update?+BB7j
        mov bl, byte_50941
        cmp bl, 1
        jnz short loc_4EE3A
        cmp word ptr [si+17BCh], 12h
        jnz short loc_4EE3A
        retn
// ; ---------------------------------------------------------------------------

loc_4EE3A:              ; CODE XREF: update?+FA0j update?+FA7j
        mov word ptr leveldata[si], 3
        mov word ptr [si+17BCh], 12h
        cmp byte ptr [si+1744h], 1Fh
        jz  short loc_4EE53
        mov word ptr [si+1744h], 0

loc_4EE53:              ; CODE XREF: update?+FBBj
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4EE61:              ; CODE XREF: update?+BBFj
        mov bl, byte_50941
        cmp bl, 2
        jnz short loc_4EE72
        cmp word ptr [si+1832h], 12h
        jnz short loc_4EE72
        retn
// ; ---------------------------------------------------------------------------

loc_4EE72:              ; CODE XREF: update?+FD8j update?+FDFj
        mov word ptr leveldata[si], 3
        mov word ptr [si+1832h], 12h
        cmp byte ptr [si+1830h], 1Fh
        jz  short loc_4EE8B
        mov word ptr [si+1830h], 0

loc_4EE8B:              ; CODE XREF: update?+FF3j
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4EE99:              ; CODE XREF: update?+BC7j
        mov bl, byte_50941
        cmp bl, 3
        jnz short loc_4EEAA
        cmp word ptr [si+18ACh], 12h
        jnz short loc_4EEAA
        retn
// ; ---------------------------------------------------------------------------

loc_4EEAA:              ; CODE XREF: update?+1010j
                    ; update?+1017j
        mov word ptr leveldata[si], 3
        mov word ptr [si+18ACh], 12h
        cmp byte ptr [si+1924h], 1Fh
        jz  short loc_4EEC3
        mov word ptr [si+1924h], 0

loc_4EEC3:              ; CODE XREF: update?+102Bj
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4EED1:              ; CODE XREF: update?+BCFj
        mov bl, byte_50941
        cmp bl, 4
        jnz short loc_4EEE2
        cmp word ptr [si+1836h], 12h
        jnz short loc_4EEE2
        retn
// ; ---------------------------------------------------------------------------

loc_4EEE2:              ; CODE XREF: update?+1048j
                    ; update?+104Fj
        mov word ptr leveldata[si], 3
        mov word ptr [si+1836h], 12h
        cmp byte ptr [si+1838h], 1Fh
        jz  short loc_4EEFB
        mov word ptr [si+1838h], 0

loc_4EEFB:              ; CODE XREF: update?+1063j
        push    si
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4EF09:              ; CODE XREF: update?+BD7j
        mov bl, byte_50941
        cmp bl, 9
        jnz short loc_4EF2C
        cmp word_510EE, 20h ; ' '
        jg  short locret_4EF2B
        push    si
        mov di, [si+6155h]
        mov si, word_51790
        call    sub_4F200
        mov byte_510DB, 1
        pop si

locret_4EF2B:               ; CODE XREF: update?+1087j
        retn
// ; ---------------------------------------------------------------------------

loc_4EF2C:              ; CODE XREF: update?+1080j
        push    si
        mov word ptr leveldata[si], 3
        mov di, [si+6155h]
        mov si, word_5157E
        call    sub_4F200
        mov byte_510DB, 0
        pop si
        retn
// ; ---------------------------------------------------------------------------

loc_4EF45:              ; CODE XREF: update?+CFBj
        cmp byte_5195A, 0
        jbe short loc_4EF50
        dec byte_5195A

loc_4EF50:              ; CODE XREF: update?+10BAj
        call    sub_4FD21

loc_4EF53:              ; CODE XREF: update?+CBBj update?+CDBj
        sub si, 2
        call    sub_487FE
        add si, 2
        mov word ptr leveldata[si], 3
        retn
// ; ---------------------------------------------------------------------------

loc_4EF63:              ; CODE XREF: update?+D33j
        cmp byte_5195A, 0
        jbe short loc_4EF6E
        dec byte_5195A

loc_4EF6E:              ; CODE XREF: update?+10D8j
        call    sub_4FD21

loc_4EF71:              ; CODE XREF: update?+D13j
        cmp byte ptr [si+17BCh], 1Fh
        jz  short locret_4EF7E
        mov word ptr [si+17BCh], 0

locret_4EF7E:               ; CODE XREF: update?+10E6j
        retn
// ; ---------------------------------------------------------------------------

loc_4EF7F:              ; CODE XREF: update?+D3Bj
        cmp byte_5195A, 0
        jbe short loc_4EF8A
        dec byte_5195A

loc_4EF8A:              ; CODE XREF: update?+10F4j
        call    sub_4FD21

loc_4EF8D:              ; CODE XREF: update?+D1Bj
        cmp byte ptr [si+1832h], 1Fh
        jz  short locret_4EF9A
        mov word ptr [si+1832h], 0

locret_4EF9A:               ; CODE XREF: update?+1102j
        retn
// ; ---------------------------------------------------------------------------

loc_4EF9B:              ; CODE XREF: update?+D4Bj
        cmp byte_5195A, 0
        jbe short loc_4EFA6
        dec byte_5195A

loc_4EFA6:              ; CODE XREF: update?+1110j
        call    sub_4FD21

loc_4EFA9:              ; CODE XREF: update?+D2Bj
        cmp byte ptr [si+18ACh], 1Fh
        jz  short locret_4EFB6
        mov word ptr [si+18ACh], 0

locret_4EFB6:               ; CODE XREF: update?+111Ej
        retn
// ; ---------------------------------------------------------------------------

loc_4EFB7:              ; CODE XREF: update?+D43j
        cmp byte_5195A, 0
        jbe short loc_4EFC2
        dec byte_5195A

loc_4EFC2:              ; CODE XREF: update?+112Cj
        call    sub_4FD21

loc_4EFC5:              ; CODE XREF: update?+D23j
        cmp byte ptr [si+1836h], 1Fh
        jz  short locret_4EFD2
        mov word ptr [si+1836h], 0

locret_4EFD2:               ; CODE XREF: update?+113Aj
        retn
// ; ---------------------------------------------------------------------------

loc_4EFD3:              ; CODE XREF: update?+D5Bj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4EFE0
        mov word ptr leveldata[si], 0

loc_4EFE0:              ; CODE XREF: update?+1148j
        mov word ptr [si+1744h], 3
        mov word_510D9, 0
        sub si, 0F0h ; '?'
        cmp byte ptr [si+18ADh], 1
        jnz short locret_4F000
        add si, 78h ; 'x'
        call    sub_4F2AF
        sub si, 78h ; 'x'

locret_4F000:               ; CODE XREF: update?+1165j
        retn
// ; ---------------------------------------------------------------------------

loc_4F001:              ; CODE XREF: update?+D63j
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F00E
        mov word ptr leveldata[si], 0

loc_4F00E:              ; CODE XREF: update?+1176j
        mov word ptr [si+1830h], 3
        mov word_510D9, 0
        sub si, 4
        cmp byte ptr [si+1837h], 1
        jnz short locret_4F02D
        add si, 2
        call    sub_4F2AF
        sub si, 2

locret_4F02D:               ; CODE XREF: update?+1192j
        retn
// ; ---------------------------------------------------------------------------

loc_4F02E:              ; CODE XREF: update?+D6Bj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F03B
        mov word ptr leveldata[si], 0

loc_4F03B:              ; CODE XREF: update?+11A3j
        mov word ptr [si+1924h], 3
        mov word_510D9, 0
        add si, 0F0h ; '?'
        cmp byte ptr [si+17BDh], 1
        jnz short locret_4F05B
        sub si, 78h ; 'x'
        call    sub_4F2AF
        add si, 78h ; 'x'

locret_4F05B:               ; CODE XREF: update?+11C0j
        retn
// ; ---------------------------------------------------------------------------

loc_4F05C:              ; CODE XREF: update?+D73j
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F069
        mov word ptr leveldata[si], 0

loc_4F069:              ; CODE XREF: update?+11D1j
        mov word ptr [si+1838h], 3
        mov word_510D9, 0
        add si, 4
        cmp byte ptr [si+1833h], 1
        jnz short locret_4F088
        sub si, 2
        call    sub_4F2AF
        add si, 2

locret_4F088:               ; CODE XREF: update?+11EDj
        retn
// ; ---------------------------------------------------------------------------

loc_4F089:              ; CODE XREF: update?+D7Bj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F096
        mov word ptr leveldata[si], 0

loc_4F096:              ; CODE XREF: update?+11FEj
        sub si, 78h ; 'x'
        jmp loc_4FDAF
// ; ---------------------------------------------------------------------------

loc_4F09C:              ; CODE XREF: update?+D83j
        cmp byte ptr [si+1836h], 1Fh
        jz  short loc_4F0A9
        mov word ptr [si+1836h], 0

loc_4F0A9:              ; CODE XREF: update?+1211j
        jmp loc_4FDAF
// ; ---------------------------------------------------------------------------

loc_4F0AC:              ; CODE XREF: update?+D8Bj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F0B9
        mov word ptr leveldata[si], 0

loc_4F0B9:              ; CODE XREF: update?+1221j
        add si, 78h ; 'x'
        jmp loc_4FDAF
// ; ---------------------------------------------------------------------------

loc_4F0BF:              ; CODE XREF: update?+D93j
        cmp byte ptr [si+1832h], 1Fh
        jz  short loc_4F0CC
        mov word ptr [si+1832h], 0

loc_4F0CC:              ; CODE XREF: update?+1234j
        jmp loc_4FDAF
// ; ---------------------------------------------------------------------------

loc_4F0CF:              ; CODE XREF: update?+D9Bj
        cmp byte ptr [si+17BCh], 1Fh
        jz  short loc_4F0DC
        mov word ptr [si+17BCh], 0

loc_4F0DC:              ; CODE XREF: update?+1244j
        sub si, 78h ; 'x'
        call    sub_4FDB5
        add si, 78h ; 'x'
        retn
// ; ---------------------------------------------------------------------------

loc_4F0E6:              ; CODE XREF: update?+DA3j
        cmp byte ptr [si+1832h], 1Fh
        jz  short loc_4F0F3
        mov word ptr [si+1832h], 0

loc_4F0F3:              ; CODE XREF: update?+125Bj
        sub si, 2
        call    sub_4FDB5
        add si, 2
        retn
// ; ---------------------------------------------------------------------------

loc_4F0FD:              ; CODE XREF: update?+DABj
        cmp byte ptr [si+18ACh], 1Fh
        jz  short loc_4F10A
        mov word ptr [si+18ACh], 0

loc_4F10A:              ; CODE XREF: update?+1272j
        add si, 78h ; 'x'
        call    sub_4FDB5
        sub si, 78h ; 'x'
        retn
// ; ---------------------------------------------------------------------------

loc_4F114:              ; CODE XREF: update?+DB3j
        cmp byte ptr [si+1836h], 1Fh
        jz  short loc_4F121
        mov word ptr [si+1836h], 0

loc_4F121:              ; CODE XREF: update?+1289j
        add si, 2
        call    sub_4FDB5
        sub si, 2
        retn
// ; ---------------------------------------------------------------------------

loc_4F12B:              ; CODE XREF: update?+DBBj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F138
        mov word ptr leveldata[si], 0

loc_4F138:              ; CODE XREF: update?+12A0j
        mov word ptr [si+17BCh], 3
        mov word ptr [si+1744h], 12h
        sub si, 78h ; 'x'
        retn
// ; ---------------------------------------------------------------------------

loc_4F148:              ; CODE XREF: update?+DC3j
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F155
        mov word ptr leveldata[si], 0

loc_4F155:              ; CODE XREF: update?+12BDj
        mov word ptr [si+1832h], 3
        mov word ptr [si+1830h], 12h
        sub si, 2
        retn
// ; ---------------------------------------------------------------------------

loc_4F165:              ; CODE XREF: update?+DCBj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F172
        mov word ptr leveldata[si], 0

loc_4F172:              ; CODE XREF: update?+12DAj
        mov word ptr [si+18ACh], 3
        mov word ptr [si+1924h], 12h
        add si, 78h ; 'x'
        retn
// ; ---------------------------------------------------------------------------

loc_4F182:              ; CODE XREF: update?+DD3j
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F18F
        mov word ptr leveldata[si], 0

loc_4F18F:              ; CODE XREF: update?+12F7j
        mov word ptr [si+1836h], 3
        mov word ptr [si+1838h], 12h
        add si, 2
        retn
// ; ---------------------------------------------------------------------------

loc_4F19F:              ; CODE XREF: update?+DDBj
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F1AC
        mov word ptr leveldata[si], 0

loc_4F1AC:              ; CODE XREF: update?+1314j
        mov word ptr [si+1832h], 3
        mov word ptr [si+1830h], 8
        sub si, 2
        retn
// ; ---------------------------------------------------------------------------

loc_4F1BC:              ; CODE XREF: update?+DE3j
        cmp byte ptr leveldata[si], 1Fh
        jz  short loc_4F1C9
        mov word ptr leveldata[si], 0

loc_4F1C9:              ; CODE XREF: update?+1331j
        mov word ptr [si+1836h], 3
        mov word ptr [si+1838h], 8
        cmp word ptr [si+18B0h], 0
        jnz short loc_4F1E6
        mov byte ptr [si+1839h], 20h ; ' '
        mov byte ptr [si+18B1h], 8

loc_4F1E6:              ; CODE XREF: update?+134Aj
        add si, 2
        retn
// ; ---------------------------------------------------------------------------

loc_4F1EA:              ; CODE XREF: update?+DEBj
        mov word ptr leveldata[si], 3
        mov byte_510DB, 2
        dec byte_5195C
        call    sub_4FDCE
        call    sound?6
        retn
; END OF FUNCTION CHUNK FOR update?

; =============== S U B R O U T I N E =======================================


sub_4F200   proc near       ; CODE XREF: sub_4A291+26p
                    ; sub_4A910+2Ap ...
        push    ds
        mov ax, es
        mov ds, ax
        mov dx, 10h

loc_4F208:              ; CODE XREF: sub_4F200+1Bj
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        cmp si, 4D34h
        jb  short loc_4F21A
        sub si, 4D0Ch

loc_4F21A:              ; CODE XREF: sub_4F200+14j
        dec dx
        jnz short loc_4F208
        pop ds
        retn
sub_4F200   endp


; =============== S U B R O U T I N E =======================================


sub_4F21F   proc near       ; CODE XREF: update?+273p update?+2EDp ...
        cmp ax, 0FFFFh
        jz  short loc_4F296
        cmp ax, 0AAAAh
        jz  short loc_4F296
        cmp ah, 0
        jz  short loc_4F296
        cmp al, 1
        jz  short loc_4F24F
        cmp al, 1Fh
        jz  short loc_4F298
        cmp al, 8
        jz  short loc_4F296
        cmp al, 9
        jz  short loc_4F296
        cmp al, 0Ah
        jz  short loc_4F296
        cmp al, 0Bh
        jz  short loc_4F296
        cmp al, 0Ch
        jz  short loc_4F296
        call    sub_4A61F
        stc
        retn
// ; ---------------------------------------------------------------------------

loc_4F24F:              ; CODE XREF: sub_4F21F+11j
        cmp bl, 2
        jz  short loc_4F25E
        cmp bl, 4
        jz  short loc_4F27A
        call    sub_4A61F
        stc
        retn
// ; ---------------------------------------------------------------------------

loc_4F25E:              ; CODE XREF: sub_4F21F+33j
        and ah, 0F0h
        cmp ah, 20h ; ' '
        jz  short loc_4F278
        cmp ah, 40h ; '@'
        jz  short loc_4F278
        cmp ah, 50h ; 'P'
        jz  short loc_4F278
        cmp ah, 70h ; 'p'
        jz  short loc_4F278
        call    sub_4A61F

loc_4F278:              ; CODE XREF: sub_4F21F+45j
                    ; sub_4F21F+4Aj ...
        stc
        retn
// ; ---------------------------------------------------------------------------

loc_4F27A:              ; CODE XREF: sub_4F21F+38j
        and ah, 0F0h
        cmp ah, 30h ; '0'
        jz  short loc_4F294
        cmp ah, 40h ; '@'
        jz  short loc_4F294
        cmp ah, 60h ; '`'
        jz  short loc_4F294
        cmp ah, 70h ; 'p'
        jz  short loc_4F294
        call    sub_4A61F

loc_4F294:              ; CODE XREF: sub_4F21F+61j
                    ; sub_4F21F+66j ...
        stc
        retn
// ; ---------------------------------------------------------------------------

loc_4F296:              ; CODE XREF: sub_4F21F+3j sub_4F21F+8j ...
        stc
        retn
// ; ---------------------------------------------------------------------------

loc_4F298:              ; CODE XREF: sub_4F21F+15j
        test    ah, 80h
        jnz short loc_4F2A2
        cmp ah, 4
        jge short loc_4F2A7

loc_4F2A2:              ; CODE XREF: sub_4F21F+7Cj
        call    sub_4A61F
        stc
        retn
// ; ---------------------------------------------------------------------------

loc_4F2A7:              ; CODE XREF: sub_4F21F+81j
        mov leveldata[si], 0
        clc
        retn
sub_4F21F   endp


; =============== S U B R O U T I N E =======================================


sub_4F2AF   proc near       ; CODE XREF: update?+116Ap
                    ; update?+1197p ...
        mov cl, byte_51037
        xor ch, ch
        cmp cx, 0
        jz  short locret_4F2CA
        mov di, 0D28h

loc_4F2BD:              ; CODE XREF: sub_4F2AF+19j
        mov bx, [di]
        xchg    bl, bh
        cmp bx, si
        jz  short loc_4F2CB
        add di, 6
        loop    loc_4F2BD

locret_4F2CA:               ; CODE XREF: sub_4F2AF+9j
        retn
// ; ---------------------------------------------------------------------------

loc_4F2CB:              ; CODE XREF: sub_4F2AF+14j
        al = [di+2]
        mov byte_5101C, al
        al = [di+3]
        mov byte_51035, al
        al = [di+4]
        mov byte_510D7, al
        mov ax, word_510AC
        xor ax, timeOfDay
        mov word_510AC, ax
        retn
sub_4F2AF   endp

// ; ---------------------------------------------------------------------------
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  2Eh ; .
        db  8Bh ; ?
        db 0C0h ; +
        db  8Bh ; ?
        db 0C0h ; +

; =============== S U B R O U T I N E =======================================


movefun4  proc near       ; DATA XREF: data:162Co
        cmp byte_510D7, 1
        jz  short notSnikSnak
        cmp byte ptr leveldata[si], sniksnak ; 17
        jnz short notSnikSnak
        mov bl, byte ptr leveldata[si+1]
        xor bh, bh
        shl bx, 1
        mov ax, movingFunctions3[bx]
        cmp ax, 0FFFFh
        jz  short notSnikSnak
        call    ax

notSnikSnak:                ; CODE XREF: movefun4+5j
                    ; movefun4+Cj ...
        retn
movefun4  endp


; =============== S U B R O U T I N E =======================================


sub_4F312   proc near       ; DATA XREF: data:movingFunctions3o
        mov ax, word_5195D
        and ax, 3
        jz  short loc_4F320
        cmp ax, 3
        jz  short loc_4F362
        retn
// ; ---------------------------------------------------------------------------

loc_4F320:              ; CODE XREF: sub_4F312+6j
        push    ds
        push    si
        push    bx
        mov di, [si+6155h]
        mov si, [bx+1388h]
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4F332:              ; CODE XREF: sub_4F312+28j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4F332
        pop bx
        pop si
        pop ds
        shr bx, 1
        inc bx
        db  83h, 0E3h, 07h ;and bx, 7
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4F34A:              ; CODE XREF: sub_4F312+79j
                    ; sub_4F312+9Cj ...
        cmp ah, 1Bh
        jz  short locret_4F361
        cmp ah, 19h
        jz  short locret_4F361
        cmp ah, 18h
        jz  short locret_4F361
        cmp ah, 1Ah
        jz  short locret_4F361
        call    sub_4A61F

locret_4F361:               ; CODE XREF: sub_4F312+3Bj
                    ; sub_4F312+40j ...
        retn
// ; ---------------------------------------------------------------------------

loc_4F362:              ; CODE XREF: sub_4F312+Bj
        mov bl, [si+1835h]
        cmp bl, 0
        jz  short loc_4F37B
        cmp bl, 2
        jz  short loc_4F39E
        cmp bl, 4
        jz  short loc_4F3C1
        cmp bl, 6
        jz  short loc_4F3E7
        retn
// ; ---------------------------------------------------------------------------

loc_4F37B:              ; CODE XREF: sub_4F312+57j
        cmp word ptr [si+17BCh], 0
        jz  short loc_4F38E
        mov ax, [si+17BCh]
        cmp byte ptr [si+17BCh], 3
        jz  short loc_4F34A
        retn
// ; ---------------------------------------------------------------------------

loc_4F38E:              ; CODE XREF: sub_4F312+6Ej
        mov word ptr leveldata[si], 1BBh
        sub si, 78h ; 'x'
        mov word ptr leveldata[si], 1011h
        retn
// ; ---------------------------------------------------------------------------

loc_4F39E:              ; CODE XREF: sub_4F312+5Cj
        cmp word ptr [si+1832h], 0
        jz  short loc_4F3B1
        mov ax, [si+1832h]
        cmp byte ptr [si+1832h], 3
        jz  short loc_4F34A
        retn
// ; ---------------------------------------------------------------------------

loc_4F3B1:              ; CODE XREF: sub_4F312+91j
        mov word ptr leveldata[si], 2BBh
        sub si, 2
        mov word ptr leveldata[si], 1811h
        retn
// ; ---------------------------------------------------------------------------

loc_4F3C1:              ; CODE XREF: sub_4F312+61j
        cmp word ptr [si+18ACh], 0
        jz  short loc_4F3D7
        mov ax, [si+18ACh]
        cmp byte ptr [si+18ACh], 3
        jnz short locret_4F3D6
        jmp loc_4F34A
// ; ---------------------------------------------------------------------------

locret_4F3D6:               ; CODE XREF: sub_4F312+BFj
        retn
// ; ---------------------------------------------------------------------------

loc_4F3D7:              ; CODE XREF: sub_4F312+B4j
        mov word ptr leveldata[si], 3BBh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 2011h
        retn
// ; ---------------------------------------------------------------------------

loc_4F3E7:              ; CODE XREF: sub_4F312+66j
        cmp word ptr [si+1836h], 0
        jz  short loc_4F3FD
        mov ax, [si+1836h]
        cmp byte ptr [si+1836h], 3
        jnz short locret_4F3FC
        jmp loc_4F34A
// ; ---------------------------------------------------------------------------

locret_4F3FC:               ; CODE XREF: sub_4F312+E5j
        retn
// ; ---------------------------------------------------------------------------

loc_4F3FD:              ; CODE XREF: sub_4F312+DAj
        mov word ptr leveldata[si], 4BBh
        add si, 2
        mov word ptr leveldata[si], 2811h
        retn
sub_4F312   endp


; =============== S U B R O U T I N E =======================================


sub_4F40D   proc near       ; DATA XREF: data:155Ao
        mov ax, word_5195D
        and ax, 3
        jz  short loc_4F41B
        cmp ax, 3
        jz  short loc_4F45F
        retn
// ; ---------------------------------------------------------------------------

loc_4F41B:              ; CODE XREF: sub_4F40D+6j
        push    ds
        push    si
        mov di, [si+6155h]
        mov si, [bx+1388h]
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4F42C:              ; CODE XREF: sub_4F40D+27j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4F42C
        pop si
        pop ds
        shr bx, 1
        inc bl
        and bl, 7
        or  bl, 8
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4F447:              ; CODE XREF: sub_4F40D+7Bj
                    ; sub_4F40D+9Ej ...
        cmp ah, 1Bh
        jz  short locret_4F45E
        cmp ah, 19h
        jz  short locret_4F45E
        cmp ah, 18h
        jz  short locret_4F45E
        cmp ah, 1Ah
        jz  short locret_4F45E
        call    sub_4A61F

locret_4F45E:               ; CODE XREF: sub_4F40D+3Dj
                    ; sub_4F40D+42j ...
        retn
// ; ---------------------------------------------------------------------------

loc_4F45F:              ; CODE XREF: sub_4F40D+Bj
        mov bl, [si+1835h]
        cmp bl, 8
        jz  short loc_4F478
        cmp bl, 0Ah
        jz  short loc_4F4E4
        cmp bl, 0Ch
        jz  short loc_4F4BE
        cmp bl, 0Eh
        jz  short loc_4F49B
        retn
// ; ---------------------------------------------------------------------------

loc_4F478:              ; CODE XREF: sub_4F40D+59j
        cmp word ptr [si+17BCh], 0
        jz  short loc_4F48B
        mov ax, [si+17BCh]
        cmp byte ptr [si+17BCh], 3
        jz  short loc_4F447
        retn
// ; ---------------------------------------------------------------------------

loc_4F48B:              ; CODE XREF: sub_4F40D+70j
        mov word ptr leveldata[si], 1BBh
        sub si, 78h ; 'x'
        mov word ptr leveldata[si], 1011h
        retn
// ; ---------------------------------------------------------------------------

loc_4F49B:              ; CODE XREF: sub_4F40D+68j
        cmp word ptr [si+1832h], 0
        jz  short loc_4F4AE
        mov ax, [si+1832h]
        cmp byte ptr [si+1832h], 3
        jz  short loc_4F447
        retn
// ; ---------------------------------------------------------------------------

loc_4F4AE:              ; CODE XREF: sub_4F40D+93j
        mov word ptr leveldata[si], 2BBh
        sub si, 2
        mov word ptr leveldata[si], 1811h
        retn
// ; ---------------------------------------------------------------------------

loc_4F4BE:              ; CODE XREF: sub_4F40D+63j
        cmp word ptr [si+18ACh], 0
        jz  short loc_4F4D4
        mov ax, [si+18ACh]
        cmp byte ptr [si+18ACh], 3
        jnz short locret_4F4D3
        jmp loc_4F447
// ; ---------------------------------------------------------------------------

locret_4F4D3:               ; CODE XREF: sub_4F40D+C1j
        retn
// ; ---------------------------------------------------------------------------

loc_4F4D4:              ; CODE XREF: sub_4F40D+B6j
        mov word ptr leveldata[si], 3BBh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 2011h
        retn
// ; ---------------------------------------------------------------------------

loc_4F4E4:              ; CODE XREF: sub_4F40D+5Ej
        cmp word ptr [si+1836h], 0
        jz  short loc_4F4FA
        mov ax, [si+1836h]
        cmp byte ptr [si+1836h], 3
        jnz short locret_4F4F9
        jmp loc_4F447
// ; ---------------------------------------------------------------------------

locret_4F4F9:               ; CODE XREF: sub_4F40D+E7j
        retn
// ; ---------------------------------------------------------------------------

loc_4F4FA:              ; CODE XREF: sub_4F40D+DCj
        mov word ptr leveldata[si], 4BBh
        add si, 2
        mov word ptr leveldata[si], 2811h
        retn
sub_4F40D   endp


; =============== S U B R O U T I N E =======================================


sub_4F50A   proc near       ; DATA XREF: data:156Ao
        push    si
        mov di, [si+61CDh]
        mov si, [bx+1388h]
        sub bx, 1Eh
        sub di, [bx+6C95h]
        sub di, [bx+6C95h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 12h

loc_4F526:              ; CODE XREF: sub_4F50A+24j
        movsb
        movsb
        add di, 78h ; 'x'
        add si, 78h ; 'x'
        loop    loc_4F526
        pop ds
        pop si
        shr bx, 1
        cmp bl, 7
        jnz short loc_4F546
        cmp byte ptr [si+18ACh], 1Fh
        jz  short loc_4F546
        mov word ptr [si+18ACh], 0

loc_4F546:              ; CODE XREF: sub_4F50A+2Dj
                    ; sub_4F50A+34j
        cmp bl, 8
        jge short loc_4F553
        add bl, 10h
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4F553:              ; CODE XREF: sub_4F50A+3Fj
        mov word ptr leveldata[si], 11h
        cmp word ptr [si+1832h], 0
        jnz short loc_4F566
        mov byte ptr [si+1835h], 1
        retn
// ; ---------------------------------------------------------------------------

loc_4F566:              ; CODE XREF: sub_4F50A+54j
        cmp byte ptr [si+1832h], 3
        jnz short loc_4F573
        mov byte ptr [si+1835h], 1
        retn
// ; ---------------------------------------------------------------------------

loc_4F573:              ; CODE XREF: sub_4F50A+61j
        cmp word ptr [si+17BCh], 0
        jnz short loc_4F58A
        mov word ptr leveldata[si], 1BBh
        sub si, 78h ; 'x'
        mov word ptr leveldata[si], 1011h
        retn
// ; ---------------------------------------------------------------------------

loc_4F58A:              ; CODE XREF: sub_4F50A+6Ej
        cmp byte ptr [si+17BCh], 3
        jnz short loc_4F595
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4F595:              ; CODE XREF: sub_4F50A+85j
        cmp word ptr [si+1836h], 0
        jnz short loc_4F5A2
        mov byte ptr [si+1835h], 9
        retn
// ; ---------------------------------------------------------------------------

loc_4F5A2:              ; CODE XREF: sub_4F50A+90j
        cmp byte ptr [si+1836h], 3
        jnz short loc_4F5AF
        mov byte ptr [si+1835h], 9
        retn
// ; ---------------------------------------------------------------------------

loc_4F5AF:              ; CODE XREF: sub_4F50A+9Dj
        mov byte ptr [si+1835h], 1
        retn
sub_4F50A   endp


; =============== S U B R O U T I N E =======================================


sub_4F5B5   proc near       ; DATA XREF: data:157Ao
        push    si
        mov di, [si+6155h]
        mov si, [bx+1388h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4F5C6:              ; CODE XREF: sub_4F5B5+1Bj
        movsb
        movsb
        movsb
        movsb
        add di, 76h ; 'v'
        add si, 76h ; 'v'
        loop    loc_4F5C6
        pop ds
        pop si
        shr bx, 1
        db  83h, 0E3h, 07h ;and bx, 7
        inc bx
        cmp bl, 7
        jnz short loc_4F5EC
        cmp byte ptr [si+1836h], 1Fh
        jz  short loc_4F5EC
        mov word ptr [si+1836h], 0

loc_4F5EC:              ; CODE XREF: sub_4F5B5+28j
                    ; sub_4F5B5+2Fj
        cmp bl, 8
        jge short loc_4F5F9
        add bl, 18h
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4F5F9:              ; CODE XREF: sub_4F5B5+3Aj
        mov word ptr leveldata[si], 11h
        cmp word ptr [si+18ACh], 0
        jnz short loc_4F60C
        mov byte ptr [si+1835h], 3
        retn
// ; ---------------------------------------------------------------------------

loc_4F60C:              ; CODE XREF: sub_4F5B5+4Fj
        cmp byte ptr [si+18ACh], 3
        jnz short loc_4F619
        mov byte ptr [si+1835h], 3
        retn
// ; ---------------------------------------------------------------------------

loc_4F619:              ; CODE XREF: sub_4F5B5+5Cj
        cmp word ptr [si+1832h], 0
        jnz short loc_4F630
        mov word ptr leveldata[si], 2BBh
        sub si, 2
        mov word ptr leveldata[si], 1811h
        retn
// ; ---------------------------------------------------------------------------

loc_4F630:              ; CODE XREF: sub_4F5B5+69j
        cmp byte ptr [si+1832h], 3
        jnz short loc_4F63B
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4F63B:              ; CODE XREF: sub_4F5B5+80j
        cmp word ptr [si+17BCh], 0
        jnz short loc_4F648
        mov byte ptr [si+1835h], 0Fh
        retn
// ; ---------------------------------------------------------------------------

loc_4F648:              ; CODE XREF: sub_4F5B5+8Bj
        cmp byte ptr [si+17BCh], 3
        jnz short loc_4F655
        mov byte ptr [si+1835h], 0Fh
        retn
// ; ---------------------------------------------------------------------------

loc_4F655:              ; CODE XREF: sub_4F5B5+98j
        mov byte ptr [si+1835h], 3
        retn
sub_4F5B5   endp


; =============== S U B R O U T I N E =======================================


sub_4F65B   proc near       ; DATA XREF: data:158Ao
        push    si
        mov di, [si+60DDh]
        mov si, [bx+1388h]
        sub bx, 40h ; '@'
        add di, [bx+6C95h]
sub_4F65B   endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================


sub_4F66B   proc near
        add di, [bx+6C95h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 12h

loc_4F677:              ; CODE XREF: sub_4F66B+14j
        movsb
        movsb
        add di, 78h ; 'x'
        add si, 78h ; 'x'
        loop    loc_4F677
        pop ds
        pop si
        shr bx, 1
        inc bl
        cmp bl, 7
        jnz short loc_4F699
        cmp byte ptr [si+17BCh], 1Fh
        jz  short loc_4F699
        mov word ptr [si+17BCh], 0

loc_4F699:              ; CODE XREF: sub_4F66B+1Fj
                    ; sub_4F66B+26j
        cmp bl, 8
        jge short loc_4F6A6
        add bl, 20h ; ' '
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4F6A6:              ; CODE XREF: sub_4F66B+31j
        mov word ptr leveldata[si], 11h
        cmp word ptr [si+1836h], 0
        jnz short loc_4F6B9
        mov byte ptr [si+1835h], 5
        retn
// ; ---------------------------------------------------------------------------

loc_4F6B9:              ; CODE XREF: sub_4F66B+46j
        cmp byte ptr [si+1836h], 3
        jnz short loc_4F6C6
        mov byte ptr [si+1835h], 5
        retn
// ; ---------------------------------------------------------------------------

loc_4F6C6:              ; CODE XREF: sub_4F66B+53j
        cmp word ptr [si+18ACh], 0
        jnz short loc_4F6DD
        mov word ptr leveldata[si], 3BBh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 2011h
        retn
// ; ---------------------------------------------------------------------------

loc_4F6DD:              ; CODE XREF: sub_4F66B+60j
        cmp byte ptr [si+18ACh], 3
        jnz short loc_4F6E8
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4F6E8:              ; CODE XREF: sub_4F66B+77j
        cmp word ptr [si+1832h], 0
        jnz short loc_4F6F5
        mov byte ptr [si+1835h], 0Dh
        retn
// ; ---------------------------------------------------------------------------

loc_4F6F5:              ; CODE XREF: sub_4F66B+82j
        cmp byte ptr [si+1832h], 3
        jnz short loc_4F702
        mov byte ptr [si+1835h], 0Dh
        retn
// ; ---------------------------------------------------------------------------

loc_4F702:              ; CODE XREF: sub_4F66B+8Fj
        mov byte ptr [si+1835h], 5
        retn
sub_4F66B   endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================


sub_4F708   proc near       ; DATA XREF: data:159Ao
        push    si
        mov di, [si+6153h]
        mov si, [bx+1388h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4F719:              ; CODE XREF: sub_4F708+1Bj
        movsb
        movsb
        movsb
        movsb
        add di, 76h ; 'v'
        add si, 76h ; 'v'
        loop    loc_4F719
        pop ds
        pop si
        shr bx, 1
        and bl, 7
        inc bl
        cmp bl, 7
        jnz short loc_4F740
        cmp byte ptr [si+1832h], 1Fh
        jz  short loc_4F740
        mov word ptr [si+1832h], 0

loc_4F740:              ; CODE XREF: sub_4F708+29j
                    ; sub_4F708+30j
        cmp bl, 8
        jge short loc_4F74D
        add bl, 28h ; '('
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4F74D:              ; CODE XREF: sub_4F708+3Bj
        mov word ptr leveldata[si], 11h
        cmp word ptr [si+17BCh], 0
        jnz short loc_4F760
        mov byte ptr [si+1835h], 7
        retn
// ; ---------------------------------------------------------------------------

loc_4F760:              ; CODE XREF: sub_4F708+50j
        cmp byte ptr [si+17BCh], 3
        jnz short loc_4F76D
        mov byte ptr [si+1835h], 7
        retn
// ; ---------------------------------------------------------------------------

loc_4F76D:              ; CODE XREF: sub_4F708+5Dj
        cmp word ptr [si+1836h], 0
        jnz short loc_4F784
        mov word ptr leveldata[si], 4BBh
        add si, 2
        mov word ptr leveldata[si], 2811h
        retn
// ; ---------------------------------------------------------------------------

loc_4F784:              ; CODE XREF: sub_4F708+6Aj
        cmp byte ptr [si+1836h], 3
        jnz short loc_4F78F
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4F78F:              ; CODE XREF: sub_4F708+81j
        cmp word ptr [si+18ACh], 0
        jnz short loc_4F79C
        mov byte ptr [si+1835h], 0Bh
        retn
// ; ---------------------------------------------------------------------------

loc_4F79C:              ; CODE XREF: sub_4F708+8Cj
        cmp byte ptr [si+18ACh], 3
        jnz short loc_4F7A9
        mov byte ptr [si+1835h], 0Bh
        retn
// ; ---------------------------------------------------------------------------

loc_4F7A9:              ; CODE XREF: sub_4F708+99j
        mov byte ptr [si+1835h], 7
        retn
sub_4F708   endp


; =============== S U B R O U T I N E =======================================


movefun6  proc near       ; DATA XREF: data:163Ao
        cmp byte_510D7, 1
        jz  short locret_4F7D0
        cmp byte ptr leveldata[si], 18h
        jnz short locret_4F7D0
        mov bl, byte ptr leveldata[si+1]
        xor bh, bh
        shl bx, 1
        mov ax, movingFunctions2[bx]
        cmp ax, 0FFFFh
        jz  short locret_4F7D0
        call    ax

locret_4F7D0:               ; CODE XREF: movefun6+5j
                    ; movefun6+Cj ...
        retn
movefun6  endp


; =============== S U B R O U T I N E =======================================


sub_4F7D1   proc near       ; DATA XREF: data:movingFunctions2o
        mov ax, word_5195D
        and ax, 3
        jz  short loc_4F7DF
        cmp ax, 3
        jz  short loc_4F80D
        retn
// ; ---------------------------------------------------------------------------

loc_4F7DF:              ; CODE XREF: sub_4F7D1+6j
        push    ds
        push    si
        push    bx
        mov di, [si+6155h]
        mov si, [bx+13E8h]
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4F7F1:              ; CODE XREF: sub_4F7D1+28j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4F7F1
        pop bx
        pop si
        pop ds
        shr bx, 1
        inc bx
        db  83h, 0E3h, 07h ;and bx, 7
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4F809:              ; CODE XREF: sub_4F7D1+61j
                    ; sub_4F7D1+80j ...
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4F80D:              ; CODE XREF: sub_4F7D1+Bj
        mov bl, [si+1835h]
        cmp bl, 0
        jz  short loc_4F826
        cmp bl, 2
        jz  short loc_4F845
        cmp bl, 4
        jz  short loc_4F864
        cmp bl, 6
        jz  short loc_4F883
        retn
// ; ---------------------------------------------------------------------------

loc_4F826:              ; CODE XREF: sub_4F7D1+43j
        cmp word ptr [si+17BCh], 0
        jz  short loc_4F835
        cmp byte ptr [si+17BCh], 3
        jz  short loc_4F809
        retn
// ; ---------------------------------------------------------------------------

loc_4F835:              ; CODE XREF: sub_4F7D1+5Aj
        mov word ptr leveldata[si], 1BBh
        sub si, 78h ; 'x'
        mov word ptr leveldata[si], 1018h
        retn
// ; ---------------------------------------------------------------------------

loc_4F845:              ; CODE XREF: sub_4F7D1+48j
        cmp word ptr [si+1832h], 0
        jz  short loc_4F854
        cmp byte ptr [si+1832h], 3
        jz  short loc_4F809
        retn
// ; ---------------------------------------------------------------------------

loc_4F854:              ; CODE XREF: sub_4F7D1+79j
        mov word ptr leveldata[si], 2BBh
        sub si, 2
        mov word ptr leveldata[si], 1818h
        retn
// ; ---------------------------------------------------------------------------

loc_4F864:              ; CODE XREF: sub_4F7D1+4Dj
        cmp word ptr [si+18ACh], 0
        jz  short loc_4F873
        cmp byte ptr [si+18ACh], 3
        jz  short loc_4F809
        retn
// ; ---------------------------------------------------------------------------

loc_4F873:              ; CODE XREF: sub_4F7D1+98j
        mov word ptr leveldata[si], 3BBh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 2018h
        retn
// ; ---------------------------------------------------------------------------

loc_4F883:              ; CODE XREF: sub_4F7D1+52j
        cmp word ptr [si+1836h], 0
        jz  short loc_4F895
        cmp byte ptr [si+1836h], 3
        jnz short locret_4F894
        jmp loc_4F809
// ; ---------------------------------------------------------------------------

locret_4F894:               ; CODE XREF: sub_4F7D1+BEj
        retn
// ; ---------------------------------------------------------------------------

loc_4F895:              ; CODE XREF: sub_4F7D1+B7j
        mov word ptr leveldata[si], 4BBh
        add si, 2
        mov word ptr leveldata[si], 2818h
        retn
sub_4F7D1   endp


; =============== S U B R O U T I N E =======================================


sub_4F8A5   proc near       ; DATA XREF: data:15BAo
        mov ax, word_5195D
        and ax, 3
        jz  short loc_4F8B3
        cmp ax, 3
        jz  short loc_4F8E3
        retn
// ; ---------------------------------------------------------------------------

loc_4F8B3:              ; CODE XREF: sub_4F8A5+6j
        push    ds
        push    si
        mov di, [si+6155h]
        mov si, [bx+13E8h]
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4F8C4:              ; CODE XREF: sub_4F8A5+27j
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4F8C4
        pop si
        pop ds
        shr bx, 1
        inc bl
        and bl, 7
        or  bl, 8
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4F8DF:              ; CODE XREF: sub_4F8A5+63j
                    ; sub_4F8A5+82j ...
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4F8E3:              ; CODE XREF: sub_4F8A5+Bj
        mov bl, [si+1835h]
        cmp bl, 8
        jz  short loc_4F8FC
        cmp bl, 0Ah
        jz  short loc_4F959
        cmp bl, 0Ch
        jz  short loc_4F93A
        cmp bl, 0Eh
        jz  short loc_4F91B
        retn
// ; ---------------------------------------------------------------------------

loc_4F8FC:              ; CODE XREF: sub_4F8A5+45j
        cmp word ptr [si+17BCh], 0
        jz  short loc_4F90B
        cmp byte ptr [si+17BCh], 3
        jz  short loc_4F8DF
        retn
// ; ---------------------------------------------------------------------------

loc_4F90B:              ; CODE XREF: sub_4F8A5+5Cj
        mov word ptr leveldata[si], 1BBh
        sub si, 78h ; 'x'
        mov word ptr leveldata[si], 1018h
        retn
// ; ---------------------------------------------------------------------------

loc_4F91B:              ; CODE XREF: sub_4F8A5+54j
        cmp word ptr [si+1832h], 0
        jz  short loc_4F92A
        cmp byte ptr [si+1832h], 3
        jz  short loc_4F8DF
        retn
// ; ---------------------------------------------------------------------------

loc_4F92A:              ; CODE XREF: sub_4F8A5+7Bj
        mov word ptr leveldata[si], 2BBh
        sub si, 2
        mov word ptr leveldata[si], 1818h
        retn
// ; ---------------------------------------------------------------------------

loc_4F93A:              ; CODE XREF: sub_4F8A5+4Fj
        cmp word ptr [si+18ACh], 0
        jz  short loc_4F949
        cmp byte ptr [si+18ACh], 3
        jz  short loc_4F8DF
        retn
// ; ---------------------------------------------------------------------------

loc_4F949:              ; CODE XREF: sub_4F8A5+9Aj
        mov word ptr leveldata[si], 3BBh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 2018h
        retn
// ; ---------------------------------------------------------------------------

loc_4F959:              ; CODE XREF: sub_4F8A5+4Aj
        cmp word ptr [si+1836h], 0
        jz  short loc_4F96B
        cmp byte ptr [si+1836h], 3
        jnz short locret_4F96A
        jmp loc_4F8DF
// ; ---------------------------------------------------------------------------

locret_4F96A:               ; CODE XREF: sub_4F8A5+C0j
        retn
// ; ---------------------------------------------------------------------------

loc_4F96B:              ; CODE XREF: sub_4F8A5+B9j
        mov word ptr leveldata[si], 4BBh
        add si, 2
        mov word ptr leveldata[si], 2818h
        retn
sub_4F8A5   endp


; =============== S U B R O U T I N E =======================================


sub_4F97B   proc near       ; DATA XREF: data:15CAo
        push    si
        mov di, [si+61CDh]
        mov si, [bx+13E8h]
        sub bx, 1Eh
        sub di, [bx+6C95h]
        sub di, [bx+6C95h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 12h

loc_4F997:              ; CODE XREF: sub_4F97B+24j
        movsb
        movsb
        add di, 78h ; 'x'
        add si, 78h ; 'x'
        loop    loc_4F997
        pop ds
        pop si
        shr bx, 1
        cmp bl, 7
        jnz short loc_4F9B7
        cmp byte ptr [si+18ACh], 1Fh
        jz  short loc_4F9B7
        mov word ptr [si+18ACh], 0

loc_4F9B7:              ; CODE XREF: sub_4F97B+2Dj
                    ; sub_4F97B+34j
        cmp bl, 8
        jge short loc_4F9C4
        add bl, 10h
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4F9C4:              ; CODE XREF: sub_4F97B+3Fj
        mov word ptr leveldata[si], 18h
        cmp word ptr [si+1832h], 0
        jnz short loc_4F9D7
        mov byte ptr [si+1835h], 1
        retn
// ; ---------------------------------------------------------------------------

loc_4F9D7:              ; CODE XREF: sub_4F97B+54j
        cmp byte ptr [si+1832h], 3
        jnz short loc_4F9E4
        mov byte ptr [si+1835h], 1
        retn
// ; ---------------------------------------------------------------------------

loc_4F9E4:              ; CODE XREF: sub_4F97B+61j
        cmp word ptr [si+17BCh], 0
        jnz short loc_4F9FB
        mov word ptr leveldata[si], 1BBh
        sub si, 78h ; 'x'
        mov word ptr leveldata[si], 1018h
        retn
// ; ---------------------------------------------------------------------------

loc_4F9FB:              ; CODE XREF: sub_4F97B+6Ej
        cmp byte ptr [si+17BCh], 3
        jnz short loc_4FA06
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4FA06:              ; CODE XREF: sub_4F97B+85j
        cmp word ptr [si+1836h], 0
        jnz short loc_4FA13
        mov byte ptr [si+1835h], 9
        retn
// ; ---------------------------------------------------------------------------

loc_4FA13:              ; CODE XREF: sub_4F97B+90j
        cmp byte ptr [si+1836h], 3
        jnz short loc_4FA20
        mov byte ptr [si+1835h], 9
        retn
// ; ---------------------------------------------------------------------------

loc_4FA20:              ; CODE XREF: sub_4F97B+9Dj
        mov byte ptr [si+1835h], 1
        retn
sub_4F97B   endp


; =============== S U B R O U T I N E =======================================


sub_4FA26   proc near       ; DATA XREF: data:15DAo
        push    si
        mov di, [si+6155h]
        mov si, [bx+13E8h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4FA37:              ; CODE XREF: sub_4FA26+1Bj
        movsb
        movsb
        movsb
        movsb
        add di, 76h ; 'v'
        add si, 76h ; 'v'
        loop    loc_4FA37
        pop ds
        pop si
        shr bx, 1
        db  83h, 0E3h, 07h ;and bx, 7
        inc bx
        cmp bl, 7
        jnz short loc_4FA5D
        cmp byte ptr [si+1836h], 1Fh
        jz  short loc_4FA5D
        mov word ptr [si+1836h], 0

loc_4FA5D:              ; CODE XREF: sub_4FA26+28j
                    ; sub_4FA26+2Fj
        cmp bl, 8
        jge short loc_4FA6A
        add bl, 18h
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4FA6A:              ; CODE XREF: sub_4FA26+3Aj
        mov word ptr leveldata[si], 18h
        cmp word ptr [si+18ACh], 0
        jnz short loc_4FA7D
        mov byte ptr [si+1835h], 3
        retn
// ; ---------------------------------------------------------------------------

loc_4FA7D:              ; CODE XREF: sub_4FA26+4Fj
        cmp byte ptr [si+18ACh], 3
        jnz short loc_4FA8A
        mov byte ptr [si+1835h], 3
        retn
// ; ---------------------------------------------------------------------------

loc_4FA8A:              ; CODE XREF: sub_4FA26+5Cj
        cmp word ptr [si+1832h], 0
        jnz short loc_4FAA1
        mov word ptr leveldata[si], 2BBh
        sub si, 2
        mov word ptr leveldata[si], 1818h
        retn
// ; ---------------------------------------------------------------------------

loc_4FAA1:              ; CODE XREF: sub_4FA26+69j
        cmp byte ptr [si+1832h], 3
        jnz short loc_4FAAC
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4FAAC:              ; CODE XREF: sub_4FA26+80j
        cmp word ptr [si+17BCh], 0
        jnz short loc_4FAB9
        mov byte ptr [si+1835h], 0Fh
        retn
// ; ---------------------------------------------------------------------------

loc_4FAB9:              ; CODE XREF: sub_4FA26+8Bj
        cmp byte ptr [si+17BCh], 3
        jnz short loc_4FAC6
        mov byte ptr [si+1835h], 0Fh
        retn
// ; ---------------------------------------------------------------------------

loc_4FAC6:              ; CODE XREF: sub_4FA26+98j
        mov byte ptr [si+1835h], 3
        retn
sub_4FA26   endp


; =============== S U B R O U T I N E =======================================


sub_4FACC   proc near       ; DATA XREF: data:15EAo
        push    si
        mov di, [si+60DDh]
        mov si, [bx+13E8h]
        sub bx, 40h ; '@'
        add di, [bx+6C95h]
        add di, [bx+6C95h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 12h

loc_4FAE8:              ; CODE XREF: sub_4FACC+24j
        movsb
        movsb
        add di, 78h ; 'x'
        add si, 78h ; 'x'
        loop    loc_4FAE8
        pop ds
        pop si
        shr bx, 1
        inc bl
        cmp bl, 7
        jnz short loc_4FB0A
        cmp byte ptr [si+17BCh], 1Fh
        jz  short loc_4FB0A
        mov word ptr [si+17BCh], 0

loc_4FB0A:              ; CODE XREF: sub_4FACC+2Fj
                    ; sub_4FACC+36j
        cmp bl, 8
        jge short loc_4FB17
        add bl, 20h ; ' '
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4FB17:              ; CODE XREF: sub_4FACC+41j
        mov word ptr leveldata[si], 18h
        cmp word ptr [si+1836h], 0
        jnz short loc_4FB2A
        mov byte ptr [si+1835h], 5
        retn
// ; ---------------------------------------------------------------------------

loc_4FB2A:              ; CODE XREF: sub_4FACC+56j
        cmp byte ptr [si+1836h], 3
        jnz short loc_4FB37
        mov byte ptr [si+1835h], 5
        retn
// ; ---------------------------------------------------------------------------

loc_4FB37:              ; CODE XREF: sub_4FACC+63j
        cmp word ptr [si+18ACh], 0
        jnz short loc_4FB4E
        mov word ptr leveldata[si], 3BBh
        add si, 78h ; 'x'
        mov word ptr leveldata[si], 2018h
        retn
// ; ---------------------------------------------------------------------------

loc_4FB4E:              ; CODE XREF: sub_4FACC+70j
        cmp byte ptr [si+18ACh], 3
        jnz short loc_4FB59
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4FB59:              ; CODE XREF: sub_4FACC+87j
        cmp word ptr [si+1832h], 0
        jnz short loc_4FB66
        mov byte ptr [si+1835h], 0Dh
        retn
// ; ---------------------------------------------------------------------------

loc_4FB66:              ; CODE XREF: sub_4FACC+92j
        cmp byte ptr [si+1832h], 3
        jnz short loc_4FB73
        mov byte ptr [si+1835h], 0Dh
        retn
// ; ---------------------------------------------------------------------------

loc_4FB73:              ; CODE XREF: sub_4FACC+9Fj
        mov byte ptr [si+1835h], 5
        retn
sub_4FACC   endp


; =============== S U B R O U T I N E =======================================


sub_4FB79   proc near       ; DATA XREF: data:15FAo
        push    si
        mov di, [si+6153h]
        mov si, [bx+13E8h]
        push    ds
        mov ax, es
        mov ds, ax
        mov cx, 10h

loc_4FB8A:              ; CODE XREF: sub_4FB79+1Bj
        movsb
        movsb
        movsb
        movsb
        add di, 76h ; 'v'
        add si, 76h ; 'v'
        loop    loc_4FB8A
        pop ds
        pop si
        shr bx, 1
        and bl, 7
        inc bl
        cmp bl, 7
        jnz short loc_4FBB1
        cmp byte ptr [si+1832h], 1Fh
        jz  short loc_4FBB1
        mov word ptr [si+1832h], 0

loc_4FBB1:              ; CODE XREF: sub_4FB79+29j
                    ; sub_4FB79+30j
        cmp bl, 8
        jge short loc_4FBBE
        add bl, 28h ; '('
        mov [si+1835h], bl
        retn
// ; ---------------------------------------------------------------------------

loc_4FBBE:              ; CODE XREF: sub_4FB79+3Bj
        mov word ptr leveldata[si], 18h
        cmp word ptr [si+17BCh], 0
        jnz short loc_4FBD1
        mov byte ptr [si+1835h], 7
        retn
// ; ---------------------------------------------------------------------------

loc_4FBD1:              ; CODE XREF: sub_4FB79+50j
        cmp byte ptr [si+17BCh], 3
        jnz short loc_4FBDE
        mov byte ptr [si+1835h], 7
        retn
// ; ---------------------------------------------------------------------------

loc_4FBDE:              ; CODE XREF: sub_4FB79+5Dj
        cmp word ptr [si+1836h], 0
        jnz short loc_4FBF5
        mov word ptr leveldata[si], 4BBh
        add si, 2
        mov word ptr leveldata[si], 2818h
        retn
// ; ---------------------------------------------------------------------------

loc_4FBF5:              ; CODE XREF: sub_4FB79+6Aj
        cmp byte ptr [si+1836h], 3
        jnz short loc_4FC00
        call    sub_4A61F
        retn
// ; ---------------------------------------------------------------------------

loc_4FC00:              ; CODE XREF: sub_4FB79+81j
        cmp word ptr [si+18ACh], 0
        jnz short loc_4FC0D
        mov byte ptr [si+1835h], 0Bh
        retn
// ; ---------------------------------------------------------------------------

loc_4FC0D:              ; CODE XREF: sub_4FB79+8Cj
        cmp byte ptr [si+18ACh], 3
        jnz short loc_4FC1A
        mov byte ptr [si+1835h], 0Bh
        retn
// ; ---------------------------------------------------------------------------

loc_4FC1A:              ; CODE XREF: sub_4FB79+99j
        mov byte ptr [si+1835h], 7
        retn
sub_4FB79   endp


; =============== S U B R O U T I N E =======================================


sub_4FC20   proc near       ; CODE XREF: somethingspsig:loc_4944Fp
                    ; sub_501C0+22p ...
        cmp byte_510E3, 0
        jz  short loc_4FC6F
        cmp videoStatusUnk, 1
        jnz short loc_4FC33
        mov di, 177h
        jmp short loc_4FC36
// ; ---------------------------------------------------------------------------

loc_4FC33:              ; CODE XREF: sub_4FC20+Cj
        mov di, 201h

loc_4FC36:              ; CODE XREF: sub_4FC20+11j
        mov si, 87D1h
        mov ah, 8
        call    sub_500F3
        cmp videoStatusUnk, 1
        jnz short loc_4FC4A
        mov di, 6AEh
        jmp short loc_4FC4D
// ; ---------------------------------------------------------------------------

loc_4FC4A:              ; CODE XREF: sub_4FC20+23j
        mov di, 932h

loc_4FC4D:              ; CODE XREF: sub_4FC20+28j
        mov si, 87DAh
        mov ah, 8
        call    sub_500F3
        cmp videoStatusUnk, 1
        jnz short loc_4FC61
        mov di, 6B4h
        jmp short loc_4FC64
// ; ---------------------------------------------------------------------------

loc_4FC61:              ; CODE XREF: sub_4FC20+3Aj
        mov di, 938h

loc_4FC64:              ; CODE XREF: sub_4FC20+3Fj
        mov si, 87F6h
        mov ah, 8
        call    sub_500F3
        jmp loc_4FD1A
// ; ---------------------------------------------------------------------------

loc_4FC6F:              ; CODE XREF: sub_4FC20+5j
        cmp byte_510DE, 0
        jz  short loc_4FCC7
        cmp videoStatusUnk, 1
        jnz short loc_4FC82
        mov di, 177h
        jmp short loc_4FC85
// ; ---------------------------------------------------------------------------

loc_4FC82:              ; CODE XREF: sub_4FC20+5Bj
        mov di, 201h

loc_4FC85:              ; CODE XREF: sub_4FC20+60j
        mov si, 87D1h
        mov ah, 8
        call    sub_500F3
        cmp videoStatusUnk, 1
        jnz short loc_4FC99
        mov di, 6AEh
        jmp short loc_4FC9C
// ; ---------------------------------------------------------------------------

loc_4FC99:              ; CODE XREF: sub_4FC20+72j
        mov di, 932h

loc_4FC9C:              ; CODE XREF: sub_4FC20+77j
        mov si, 87DAh
        mov ah, 8
        call    sub_500F3
        cmp videoStatusUnk, 1
        jnz short loc_4FCB0
        mov di, 6B4h
        jmp short loc_4FCB3
// ; ---------------------------------------------------------------------------

loc_4FCB0:              ; CODE XREF: sub_4FC20+89j
        mov di, 938h

loc_4FCB3:              ; CODE XREF: sub_4FC20+8Ej
        mov si, 87DEh
        mov ah, 8
        call    sub_500F3
        cmp videoStatusUnk, 2
        jnz short loc_4FD1A
        call    sub_50199
        jmp short loc_4FD1A
// ; ---------------------------------------------------------------------------

loc_4FCC7:              ; CODE XREF: sub_4FC20+54j
        cmp videoStatusUnk, 1
        jnz short loc_4FCD3
        mov di, 177h
        jmp short loc_4FCD6
// ; ---------------------------------------------------------------------------

loc_4FCD3:              ; CODE XREF: sub_4FC20+ACj
        mov di, 201h

loc_4FCD6:              ; CODE XREF: sub_4FC20+B1j
        mov si, 879Fh
        mov ah, 6
        call    sub_500F3
        cmp videoStatusUnk, 1
        jnz short loc_4FCEA
        mov di, 6AEh
        jmp short loc_4FCED
// ; ---------------------------------------------------------------------------

loc_4FCEA:              ; CODE XREF: sub_4FC20+C3j
        mov di, 932h

loc_4FCED:              ; CODE XREF: sub_4FC20+C8j
        mov si, 87A8h
        mov byte ptr [si+3], 0
        mov ah, 8
        call    sub_500F3
        cmp videoStatusUnk, 1
        jnz short loc_4FD05
        mov di, 6B4h
        jmp short loc_4FD08
// ; ---------------------------------------------------------------------------

loc_4FD05:              ; CODE XREF: sub_4FC20+DEj
        mov di, 938h

loc_4FD08:              ; CODE XREF: sub_4FC20+E3j
        mov si, 87ACh
        mov ah, 8
        call    sub_500F3
        cmp videoStatusUnk, 2
        jnz short loc_4FD1A
        call    sub_50199

loc_4FD1A:              ; CODE XREF: sub_4FC20+4Cj
                    ; sub_4FC20+A0j ...
        call    sub_4FD21
        call    sub_4FDFD
        retn
sub_4FC20   endp


; =============== S U B R O U T I N E =======================================


sub_4FD21   proc near       ; CODE XREF: sub_4A3BB+13p
                    ; update?:loc_4EC90p ...
        push    si
        cmp byte_5195A, 1
        jnb short loc_4FD2E
        mov byte_5195A, 0

loc_4FD2E:              ; CODE XREF: sub_4FD21+6j
        al = byte_5195A
        mov si, 87C5h
        call    sub_4BF4D
        cmp videoStatusUnk, 1
        jnz short loc_4FD43
        mov di, 6CEh
        jmp short loc_4FD46
// ; ---------------------------------------------------------------------------

loc_4FD43:              ; CODE XREF: sub_4FD21+1Bj
        mov di, 952h

loc_4FD46:              ; CODE XREF: sub_4FD21+20j
        mov si, 87C5h
        cmp byte_5195A, 0
        jnz short loc_4FD54
        mov ah, 6
        jmp short loc_4FD56
// ; ---------------------------------------------------------------------------

loc_4FD54:              ; CODE XREF: sub_4FD21+2Dj
        mov ah, 8

loc_4FD56:              ; CODE XREF: sub_4FD21+31j
        call    sub_500F3
        cmp videoStatusUnk, 2
        jnz short loc_4FD63
        call    sub_50103

loc_4FD63:              ; CODE XREF: sub_4FD21+3Dj
        pop si
        retn
sub_4FD21   endp


; =============== S U B R O U T I N E =======================================


sub_4FD65   proc near       ; CODE XREF: runLevel+E9p
        cmp videoStatusUnk, 1
        jz  short loc_4FD6D
        retn
// ; ---------------------------------------------------------------------------

loc_4FD6D:              ; CODE XREF: sub_4FD65+5j
        al = byte_5197C
        cmp al, 0
        jnz short loc_4FD75
        retn
// ; ---------------------------------------------------------------------------

loc_4FD75:              ; CODE XREF: sub_4FD65+Dj
        dec al
        jz  short loc_4FD7D
        mov byte_5197C, al
        retn
// ; ---------------------------------------------------------------------------

loc_4FD7D:              ; CODE XREF: sub_4FD65+12j
        mov byte_5197C, al
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        push    ds
        mov si, word_5182E
        mov di, 6D2h
        mov ax, es
        mov ds, ax
        mov cx, 7

loc_4FD99:              ; CODE XREF: sub_4FD65+3Cj
        movsb
        movsb
        add si, 78h ; 'x'
        add di, 78h ; 'x'
        loop    loc_4FD99
        pop ds
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        retn
sub_4FD65   endp

// ; ---------------------------------------------------------------------------
; START OF FUNCTION CHUNK FOR update?

loc_4FDAF:              ; CODE XREF: update?+1209j
                    ; update?:loc_4F0A9j ...
        mov word ptr leveldata[si], 3
; END OF FUNCTION CHUNK FOR update?

; =============== S U B R O U T I N E =======================================


sub_4FDB5   proc near       ; CODE XREF: update?+124Fp
                    ; update?+1266p ...
        cmp byte ptr word_59B73, 0
        jnz short loc_4FDCA
        cmp byte_510DB, 0
        jz  short loc_4FDCA
        cmp word_510DC, si
        jnz short loc_4FDCA
        retn
// ; ---------------------------------------------------------------------------

loc_4FDCA:              ; CODE XREF: sub_4FDB5+5j sub_4FDB5+Cj ...
        inc byte_5195C
sub_4FDB5   endp ; sp-analysis failed


; =============== S U B R O U T I N E =======================================


sub_4FDCE   proc near       ; CODE XREF: sub_4955B+7F4p
                    ; update?+1369p
        cmp videoStatusUnk, 1
        jz  short loc_4FDD6
        retn
// ; ---------------------------------------------------------------------------

loc_4FDD6:              ; CODE XREF: sub_4FDCE+5j
        push    si
        al = byte_5195C
        mov si, 87C9h
        call    sub_4BF4D
        mov di, 6D2h
        mov si, 87CAh
        cmp byte_5195C, 0
        jnz short loc_4FDF1
        mov ah, 8
        jmp short loc_4FDF3
// ; ---------------------------------------------------------------------------

loc_4FDF1:              ; CODE XREF: sub_4FDCE+1Dj
        mov ah, 6

loc_4FDF3:              ; CODE XREF: sub_4FDCE+21j
        call    sub_500F3
        pop si
        mov byte_5197C, 46h ; 'F'
        retn
sub_4FDCE   endp


; =============== S U B R O U T I N E =======================================

; Attributes: bp-based frame

sub_4FDFD   proc near       ; CODE XREF: runLevel+29p
                    ; runLevel:noFlashing2p ...

var_2       = word ptr -2

        push    bp
        mov bp, sp
        add sp, 0FFFEh
        mov [bp+var_2], 0
        al = byte_510B0
        cmp byte ptr word_510B7, al
        jz  short loc_4FE36
        mov byte ptr word_510B7, al
        mov si, 87CDh
        call    sub_4BF4D
        mov si, 87CEh
        cmp videoStatusUnk, 1
        jnz short loc_4FE29
        mov di, 188h
        jmp short loc_4FE2C
// ; ---------------------------------------------------------------------------

loc_4FE29:              ; CODE XREF: sub_4FDFD+25j
        mov di, 212h

loc_4FE2C:              ; CODE XREF: sub_4FDFD+2Aj
        mov ah, 6
        call    sub_500F3
        mov [bp+var_2], 1

loc_4FE36:              ; CODE XREF: sub_4FDFD+12j
        al = byte_510B1
        cmp byte ptr word_510B7+1, al
        jz  short loc_4FE5F
        mov byte ptr word_510B7+1, al
        mov si, 87CDh
        call    sub_4BF4D
        mov si, 87CEh
        cmp videoStatusUnk, 1
        jnz short loc_4FE57
        mov di, 185h
        jmp short loc_4FE5A
// ; ---------------------------------------------------------------------------

loc_4FE57:              ; CODE XREF: sub_4FDFD+53j
        mov di, 20Fh

loc_4FE5A:              ; CODE XREF: sub_4FDFD+58j
        mov ah, 6
        call    sub_500F3

loc_4FE5F:              ; CODE XREF: sub_4FDFD+40j
        al = byte_510B2
        cmp byte_510B9, al
        jz  short loc_4FE88
        mov byte_510B9, al
        mov si, 87CDh
        call    sub_4BF4D
        mov si, 87CEh
        cmp videoStatusUnk, 1
        jnz short loc_4FE80
        mov di, 182h
        jmp short loc_4FE83
// ; ---------------------------------------------------------------------------

loc_4FE80:              ; CODE XREF: sub_4FDFD+7Cj
        mov di, 20Ch

loc_4FE83:              ; CODE XREF: sub_4FDFD+81j
        mov ah, 6
        call    sub_500F3

loc_4FE88:              ; CODE XREF: sub_4FDFD+69j
        cmp videoStatusUnk, 2
        jnz short loc_4FE98
        cmp [bp+var_2], 0
        jz  short loc_4FE98
        call    sub_50144

loc_4FE98:              ; CODE XREF: sub_4FDFD+90j
                    ; sub_4FDFD+96j
        mov sp, bp
        pop bp
        retn
sub_4FDFD   endp


; =============== S U B R O U T I N E =======================================


sub_4FE9C   proc near       ; CODE XREF: sub_500F3+7p
        mov byte_51969, ah
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register

loc_4FEBE:              ; CODE XREF: sub_4FE9C+1C3j
        mov bl, [si]
        cmp bl, 0
        jnz short loc_4FEC8
        jmp loc_50062
// ; ---------------------------------------------------------------------------

loc_4FEC8:              ; CODE XREF: sub_4FE9C+27j
        cmp bl, 0Ah
        jnz short loc_4FED0
        jmp loc_50062
// ; ---------------------------------------------------------------------------

loc_4FED0:              ; CODE XREF: sub_4FE9C+2Fj
        inc si
        sub bl, 20h ; ' '
        xor bh, bh
        add bx, 5D15h
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = byte_51969
        out dx, al      ; EGA port: graphics controller data register
        mov ah, [bx]
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = byte_51969
        out dx, al      ; EGA port: graphics controller data register
        mov ah, [bx]
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = byte_51969
        out dx, al      ; EGA port: graphics controller data register
        mov ah, [bx]
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = byte_51969
        out dx, al      ; EGA port: graphics controller data register
        mov ah, [bx]
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = byte_51969
        out dx, al      ; EGA port: graphics controller data register
        mov ah, [bx]
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = byte_51969
        out dx, al      ; EGA port: graphics controller data register
        mov ah, [bx]
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        mov dx, 3CEh
        al = 0
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; set/reset.
                    ; Data bits 0-3 select planes for write mode 00
        inc dx
        al = byte_51969
        out dx, al      ; EGA port: graphics controller data register
        mov ah, [bx]
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = ah
        out dx, al      ; EGA port: graphics controller data register
        or  es:[di], al
        add di, 7Ah ; 'z'
        add bx, 40h ; '@'
        sub di, 355h
        jmp loc_4FEBE
// ; ---------------------------------------------------------------------------

loc_50062:              ; CODE XREF: sub_4FE9C+29j
                    ; sub_4FE9C+31j
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        retn
sub_4FE9C   endp


; =============== S U B R O U T I N E =======================================


sub_50077   proc near       ; CODE XREF: sub_500F3:loc_500FFp
        push    es
        mov ax, seg zeg000
        mov es, ax
        assume es:zeg000

loc_5007D:              ; CODE XREF: sub_50077+78j
        mov bl, [si]
        cmp bl, 0
        jnz short loc_50086
        jmp short loc_500F1
// ; ---------------------------------------------------------------------------

loc_50086:              ; CODE XREF: sub_50077+Bj
        cmp bl, 0Ah
        jnz short loc_5008D
        jmp short loc_500F1
// ; ---------------------------------------------------------------------------

loc_5008D:              ; CODE XREF: sub_50077+12j
        inc si
        sub bl, 20h ; ' '
        xor bh, bh
        add bx, 5D15h
        mov ah, [bx]
        mov es:[di], ah
        add di, 0A8h ; '?'
        add bx, 40h ; '@'
        mov ah, [bx]
        mov es:[di], ah
        add di, 0A8h ; '?'
        add bx, 40h ; '@'
        mov ah, [bx]
        mov es:[di], ah
        add di, 0A8h ; '?'
        add bx, 40h ; '@'
        mov ah, [bx]
        mov es:[di], ah
        add di, 0A8h ; '?'
        add bx, 40h ; '@'
        mov ah, [bx]
        mov es:[di], ah
        add di, 0A8h ; '?'
        add bx, 40h ; '@'
        mov ah, [bx]
        mov es:[di], ah
        add di, 0A8h ; '?'
        add bx, 40h ; '@'
        mov ah, [bx]
        mov es:[di], ah
        add di, 0A8h ; '?'
        add bx, 40h ; '@'
        sub di, 497h
        jmp short loc_5007D
// ; ---------------------------------------------------------------------------

loc_500F1:              ; CODE XREF: sub_50077+Dj
                    ; sub_50077+14j
        pop es
        assume es:nothing
        retn
sub_50077   endp


; =============== S U B R O U T I N E =======================================


sub_500F3   proc near       ; CODE XREF: sub_4955B+446p
                    ; sub_4955B+6AFp ...
        cmp videoStatusUnk, 1
        jnz short loc_500FF
        call    sub_4FE9C
        jmp short locret_50102
// ; ---------------------------------------------------------------------------

loc_500FF:              ; CODE XREF: sub_500F3+5j
        call    sub_50077

locret_50102:               ; CODE XREF: sub_500F3+Aj
        retn
sub_500F3   endp


; =============== S U B R O U T I N E =======================================


sub_50103   proc near       ; CODE XREF: sub_4FD21+3Fp
        push    ds
        push    es
        mov ax, seg zeg000
        mov ds, ax
        assume ds:zeg000
        mov es, ax
        assume es:zeg000
        mov si, 951h
        mov di, 1911h
        pushf
        mov bx, 6

loc_50116:              ; CODE XREF: sub_50103+3Bj
        mov cx, 1Ch

loc_50119:              ; CODE XREF: sub_50103+30j
        lodsb
        rcr al, 1
        inc di
        lodsb
        rcr al, 1
        stosb
        lodsb
        rcr al, 1
        stosb
        lodsb
        rcr al, 1
        stosb
        lodsb
        rcr al, 1
        stosb
        add si, 25h ; '%'
        add di, 25h ; '%'
        loop    loc_50119
        add si, 0B28h
        add di, 0B28h
        dec bx
        jnz short loc_50116
        popf
        pop es
        assume es:nothing
        pop ds
        assume ds:data
        retn
sub_50103   endp


; =============== S U B R O U T I N E =======================================


sub_50144   proc near       ; CODE XREF: sub_4FDFD+98p
        push    ds
        push    es
        mov ax, seg zeg000
        mov ds, ax
        assume ds:zeg000
        mov es, ax
        assume es:zeg000
        mov si, 20Bh
        mov di, 11CBh
        pushf
        mov bx, 6

loc_50157:              ; CODE XREF: sub_50144+4Fj
        mov cx, 1Ch

loc_5015A:              ; CODE XREF: sub_50144+44j
        lodsb
        rcr al, 1
        inc di
        lodsb
        rcr al, 1
        stosb
        lodsb
        rcr al, 1
        stosb
        lodsb
        rcr al, 1
        stosb
        lodsb
        rcr al, 1
        stosb
        lodsb
        rcr al, 1
        stosb
        lodsb
        rcr al, 1
        stosb
        lodsb
        rcr al, 1
        stosb
        lodsb
        rcr al, 1
        stosb
        lodsb
        rcr al, 1
        stosb
        add si, 20h ; ' '
        add di, 20h ; ' '
        loop    loc_5015A
        add si, 0B28h
        add di, 0B28h
        dec bx
        jnz short loc_50157
        popf
        pop es
        assume es:nothing
        pop ds
        assume ds:data
        retn
sub_50144   endp


; =============== S U B R O U T I N E =======================================


sub_50199   proc near       ; CODE XREF: sub_4FC20+A2p
                    ; sub_4FC20+F7p
        push    es
        mov ax, seg zeg000
        mov es, ax
        assume es:zeg000
        mov di, 0FC0h
        push    ds
        mov ax, es
        mov ds, ax
        assume ds:zeg000
        mov si, 0
        mov bx, 7

loc_501AD:              ; CODE XREF: sub_50199+22j
        pushf
        mov cx, 0FC0h

loc_501B1:              ; CODE XREF: sub_50199+1Ej
        lodsb
        popf
        rcr al, 1
        pushf
        stosb
        loop    loc_501B1
        popf
        dec bx
        jnz short loc_501AD
        pop ds
        assume ds:data
        pop es
        assume es:nothing
        retn
sub_50199   endp


; =============== S U B R O U T I N E =======================================


sub_501C0   proc near       ; CODE XREF: start+338p sub_4955B+678p ...
        cmp videoStatusUnk, 2
        jnz short loc_501EA
        push    es
        mov ax, seg zeg000
        mov es, ax
        assume es:zeg000
        mov di, 0
        mov si, 4C15h
        mov dx, 60h ; '`'

loc_501D6:              ; CODE XREF: sub_501C0+1Fj
        mov cx, 28h ; '('
        rep movsb
        add di, 2
        dec dx
        jnz short loc_501D6
        pop es
        assume es:nothing
        call    sub_4FC20
        call    sub_5024B
        jmp short locret_5024A
// ; ---------------------------------------------------------------------------

loc_501EA:              ; CODE XREF: sub_501C0+5j
        mov si, 4C15h
        mov di, 0
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        mov cx, 18h

loc_50211:              ; CODE XREF: sub_501C0+71j
        push(cx);
        mov ah, 1

loc_50214:              ; CODE XREF: sub_501C0+6Bj
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = ah
        out dx, al      ; EGA port: sequencer data register
        mov cx, 28h ; '('
        rep movsb
        sub di, 28h ; '('
        shl ah, 1
        test    ah, 0Fh
        jnz short loc_50214
        add di, 7Ah ; 'z'
        pop(cx);
        loop    loc_50211
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: sequencer data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        call    sub_4FC20

locret_5024A:               ; CODE XREF: sub_501C0+28j
        retn
sub_501C0   endp


; =============== S U B R O U T I N E =======================================


sub_5024B   proc near       ; CODE XREF: gameloop?+31p
                    ; sub_501C0+25p
        cmp videoStatusUnk, 1
        jnz short loc_50253
        retn
// ; ---------------------------------------------------------------------------

loc_50253:              ; CODE XREF: sub_5024B+5j
        mov si, 0
        mov bl, byte_510A6
        xor bh, bh
        mov ax, 0FC0h
        mul bx
        add si, ax
        push    ds
        mov ax, seg zeg000
        mov ds, ax
        assume ds:zeg000
        mov di, 0
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0
        out dx, al      ; EGA port: graphics controller data register
        mov dx, 3CEh
        al = 8
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; bit mask
                    ; Bits 0-7 select bits to be masked in all planes
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: graphics controller data register
        mov cx, 18h

loc_5028D:              ; CODE XREF: sub_5024B+62j
        push(cx);
        mov ah, 1

loc_50290:              ; CODE XREF: sub_5024B+5Cj
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = ah
        out dx, al      ; EGA port: sequencer data register
        mov cx, 2Ah ; '*'
        rep movsb
        sub di, 2Ah ; '*'
        shl ah, 1
        test    ah, 0Fh
        jnz short loc_50290
        add di, 7Ah ; 'z'
        pop(cx);
        loop    loc_5028D
        mov dx, 3C4h
        al = 2
        out dx, al      ; EGA: sequencer address reg
                    ; map mask: data bits 0-3 enable writes to bit planes 0-3
        inc dx
        al = 0FFh
        out dx, al      ; EGA port: sequencer data register
        mov dx, 3CEh
        al = 1
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; enable set/reset
        inc dx
        al = 0Fh
        out dx, al      ; EGA port: graphics controller data register
        pop ds
        assume ds:data
        mov dx, 3CEh
        al = 5
        out dx, al      ; EGA: graph 1 and 2 addr reg:
                    ; mode register.Data bits:
                    ; 0-1: Write mode 0-2
                    ; 2: test condition
                    ; 3: read mode: 1=color compare, 0=direct
                    ; 4: 1=use odd/even RAM addressing
                    ; 5: 1=use CGA mid-res map (2-bits/pixel)
        inc dx
        al = 1
        out dx, al      ; EGA port: graphics controller data register
        retn
sub_5024B   endp


; =============== S U B R O U T I N E =======================================


sub_502CF   proc near       ; CODE XREF: start+2E6p
        mov si, 95A5h
        mov di, 531Bh
        mov ah, 1
        call    sub_4BA5F
        retn
sub_502CF   endp


; =============== S U B R O U T I N E =======================================


showNewCredits  proc near       ; CODE XREF: start+2ECp
        mov si, 9583h
        mov di, 9EE0h
        mov ah, 0Eh
        call    sub_4BA5F
        mov si, 9814h
        mov di, 0A3A4h
        mov ah, 0Eh
        call    sub_4BA5F

loc_502F1:              ; CODE XREF: showNewCredits+28j
        al = keyPressed
        cmp byte_519C3, 1
        jnz short loc_50301
        mov word_51970, 1

loc_50301:              ; CODE XREF: showNewCredits+1Ej
        cmp al, 0
        jz  short loc_502F1
        mov byte_510AB, 1
        retn
showNewCredits  endp

// ; ---------------------------------------------------------------------------
code        ends



include VARS.INC
include SEGMENTS.INC
include DATA.INC


end start
