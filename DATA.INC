
; ===========================================================================

; Segment type: Pure data
; Segment permissions: Read/Write

data        segment para public 'DATA' use16
        assume cs:data
dataStart   db 5, 0, 6, 0, 9Dh, 0, 0Eh, 0
off_50318   dw offset sub_4AB1B ; DATA XREF: runMainMenu+28Fr
            dw offset programStart
word_5031C  dw 0Fh, 9Dh, 17h, 40EEh, 5, 18h, 9Dh, 20h, 41DFh, 5, 21h
        dw 9Dh, 29h, 42ECh, 5, 2Ah, 9Dh, 32h, 4529h, 5, 33h, 9Dh
        dw 3Bh, 4539h, 5, 3Ch, 9Dh, 45h, 5E21h, 8Ch, 5Ah, 9Bh
        dw 6Ch, 4642h, 8Ch, 79h, 9Bh, 8Ah, 468Fh, 60h, 8Ch, 73h
        dw 0A3h, 4755h, 53h, 0A8h, 7Eh, 0C0h, 47F9h, 0Bh, 8Eh
        dw 43h, 99h, 4AA9h, 0Bh, 0B5h, 43h, 0C0h, 4A51h, 0Bh, 9Ah
        dw 43h, 0B4h, 4B01h, 2 dup(8Eh), 132h, 99h, 4B51h, 8Eh
        dw 0B5h, 132h, 0C0h, 4B0Bh, 129h, 25h, 138h, 34h, 4B97h
        dw 0FFFFh, 0Ch, 0Dh, 6Bh, 24h, 5B03h, 0Ch, 31h, 6Bh, 48h
        dw 5AF9h, 0Ch, 55h, 6Bh, 6Ch, 5B0Dh, 0Ch, 79h, 6Bh, 90h
        dw 5B17h, 84h, 0Dh, 0D3h, 1Fh, 5ADBh, 7Eh, 2Bh, 0A9h, 36h
        dw 5AE5h, 0AEh, 2Bh, 0D9h, 36h, 5AEFh, 84h, 56h, 0AFh
        dw 78h, 5B21h, 86h, 84h, 0A8h, 98h, 5B3Eh, 0C9h, 50h, 0DDh
        dw 9Ah, 5B58h, 0E9h, 50h, 0FCh, 9Ah, 5B61h, 0, 0B5h, 13Fh
        dw 0C7h, 5B6Dh, 11Ch, 0, 13Fh, 0B4h, 5B6Dh, 0FFFFh, 9001h
        dw 5100h, 500h, 0, 7Fh, 4Ch, 12h, 7A03h, 4700h, 500h, 100h
        dw 7Ah, 46h, 45h, 4C00h, 200h, 2E00h, 200h, 46h, 8, 7
        dw 0FFh, 3 dup(0), 67h, 27h, 10h, 7601h, 4D00h, 2600h
        dw 300h, 77h, 4Eh, 6, 7C01h, 5500h, 200h, 100h, 7Ch, 70h
        dw 8, 7902h, 7300h, 300h, 100h, 78h, 75h, 2, 7900h, 7E00h
        dw 1800h, 100h, 90h, 7Fh, 2, 0FFh, 2 dup(0), 100h, 5Eh
        dw 2Ch, 3, 5E00h, 2A00h, 1300h, 100h, 71h, 51h, 28h, 7203h
        dw 5200h, 0E00h, 0FF00h, 3 dup(0), 5E01h, 4E00h, 200h
        dw 0, 5Fh, 4Eh, 0Bh, 6A03h, 4F00h, 900h, 100h, 73h, 81h
        dw 2Ah, 7400h, 8100h, 1400h, 0FF00h, 3 dup(0), 5801h, 7300h
        dw 300h, 0, 59h, 73h, 17h, 7001h, 7300h, 300h, 100h, 77h
        dw 5Eh, 2, 7803h, 5D00h, 800h, 300h, 78h, 5Eh, 8, 0FFh
        dw 2 dup(0), 100h, 16h, 72h, 2, 0B00h, 7200h, 0B00h, 100h
        dw 0Ah, 97h, 26h, 600h, 9700h, 400h, 100h, 5, 9Ch, 6, 0E01h
        dw 9B00h, 300h, 0, 0Fh, 99h, 6, 1501h, 0A000h, 800h, 0
        dw 16h, 0A0h, 14h, 2A02h, 9F00h, 900h, 0, 33h, 97h, 4Ah
        dw 7D03h, 9800h, 0A00h, 0, 87h, 0A1h, 9, 9001h, 0A100h
        dw 500h, 0FF00h, 3 dup(0), 2203h, 7100h, 500h, 200h, 46h
        dw 74h, 3, 4901h, 7100h, 500h, 100h, 49h, 54h, 7, 4803h
        dw 4C00h, 200h, 0, 9, 6Fh, 5, 900h, 7000h, 600h, 100h
        dw 7, 70h, 44h, 801h, 7000h, 4400h, 0, 9, 2Dh, 6, 900h
        dw 2E00h, 500h, 0FF00h, 3 dup(0), 8A01h, 2500h, 200h, 0
        dw 7Dh, 25h, 0Dh, 7C01h, 4300h, 1F00h, 300h, 7Dh, 44h
        dw 6, 8300h, 4900h, 1300h, 100h, 96h, 51h, 9, 9703h, 2400h
        dw 400h, 200h, 0B6h, 27h, 4, 0C001h, 2600h, 300h, 0FF00h
        dw 3 dup(0), 9C01h, 4100h, 700h, 100h, 9Ch, 45h, 2, 9D01h
        dw 4500h, 200h, 0, 0B6h, 47h, 2, 0B701h, 5500h, 0E00h
        dw 100h, 0B7h, 66h, 7, 0B701h, 7C00h, 0D00h, 200h, 0A8h
        dw 8Bh, 0Fh, 0A500h, 8B00h, 300h, 0FF00h, 3 dup(0), 0B401h
        dw 4100h, 700h, 100h, 0B4h, 45h, 2, 0B501h, 4500h, 200h
        dw 0, 0B6h, 47h, 2, 0B701h, 5500h, 0E00h, 100h, 0B7h, 66h
        dw 7, 0B701h, 7C00h, 0D00h, 200h, 0A8h, 8Bh, 0Fh, 0A500h
        dw 8B00h, 300h, 0FF00h, 3 dup(0), 0B400h, 5A00h, 600h
        dw 100h, 0BAh, 5Ah, 18h, 0BB00h, 4300h, 2600h, 100h, 0E1h
        dw 43h, 21h, 0E200h, 2300h, 2100h, 0FF00h, 3 dup(0), 9601h
        dw 0A000h, 400h, 0, 96h, 0A1h, 1Fh, 0B501h, 0A100h, 1D00h
        dw 200h, 0B5h, 85h, 8, 0BD01h, 7D00h, 1600h, 200h, 0BDh
        dw 67h, 4, 0C001h, 6300h, 1900h, 0, 0C0h, 4Ah, 33h, 0F302h
        dw 4A00h, 1100h, 0FF00h, 3 dup(0), 103h, 7E01h, 0A00h
        dw 0FF00h, 3 dup(0), 103h, 8301h, 0A00h, 0FF00h, 3 dup(0)
        dw 103h, 8801h, 0A00h, 0FF00h, 3 dup(0), 103h, 8D01h, 0A00h
        dw 0FF00h, 3 dup(0), 103h, 9201h, 0A00h, 0FF00h, 3 dup(0)
        dw 103h, 9701h, 0A00h, 0FF00h, 3 dup(0), 103h, 7401h, 400h
        dw 0, 105h, 77h, 4, 103h, 6F01h, 200h, 0, 103h, 70h, 7
        dw 0C00h, 7101h, 200h, 100h, 110h, 72h, 2, 1101h, 7201h
        dw 200h, 0, 118h, 71h, 7, 1800h, 7201h, 700h, 0, 11Bh
        dw 77h, 3, 2400h, 7701h, 200h, 100h, 125h, 83h, 0Ch, 2100h
        dw 8301h, 400h, 300h, 11Bh, 7Eh, 4, 0FFh, 2 dup(0), 100h
        dw 9Fh, 0B4h, 0Eh, 0A000h, 0A700h, 1C00h, 100h, 0BBh, 0A6h
        dw 8, 0BB00h, 9500h, 0A00h, 0FF00h, 3 dup(0), 0B01h, 9800h
        dw 0B00h, 0, 0Ch, 8Eh, 38h, 0FFh, 3 dup(0), 0Ch, 99h, 38h
        dw 4301h, 9900h, 0B00h, 0FF00h, 3 dup(0), 0B01h, 0BF00h
        dw 0B00h, 0, 0Ch, 0B5h, 38h, 0FFh, 3 dup(0), 0Ch, 0C0h
        dw 38h, 4301h, 0C000h, 0B00h, 0FF00h, 3 dup(0), 8D01h
        dw 6900h, 1000h, 0, 8Eh, 5Ah, 0Dh, 0FFh, 3 dup(0), 8Dh
        dw 6Ah, 0Eh, 9A01h, 6900h, 0F00h, 0FF00h, 3 dup(0), 8D01h
        dw 8700h, 1000h, 0, 8Eh, 78h, 0Dh, 0FFh, 3 dup(0), 8Dh
        dw 88h, 0Eh, 9A01h, 8700h, 0F00h, 0FF00h, 3 dup(0), 8E01h
        dw 9800h, 0B00h, 0, 8Fh, 8Eh, 0A3h, 0FFh, 3 dup(0), 8Fh
        dw 99h, 0A4h, 3201h, 9801h, 0B00h, 0FF00h, 3 dup(0), 8E01h
        dw 0BF00h, 0B00h, 0, 8Fh, 0B5h, 0A4h, 0FFh, 3 dup(0), 8Eh
        dw 0C0h, 0A4h, 3201h, 0C001h, 0C00h, 0FF00h, 3 dup(0)
byte_50910  db 0            ; DATA XREF: sub_4B671+5w sub_4B6C9+5w ...
byte_50911  db 0            ; DATA XREF: sub_4B671+Aw sub_4B6C9+Aw ...
byte_50912  db 0            ; DATA XREF: sub_4B671w sub_4B6C9w ...
byte_50913  db 0            ; DATA XREF: code:4647w code:4694w ...
byte_50914  db 0            ; DATA XREF: code:464Cw code:4699w ...
byte_50915  db 0            ; DATA XREF: code:4642w code:468Fw ...
byte_50916  db 0            ; DATA XREF: sub_4B72B+5w sub_4B771+5w ...
byte_50917  db 0            ; DATA XREF: sub_4B72B+Aw sub_4B771+Aw ...
byte_50918  db 0            ; DATA XREF: sub_4B72Bw sub_4B771w ...
byte_50919  db 0            ; DATA XREF: sub_4CD3C+Ar
                    ; sub_4CD3C:loc_4CD4Dw
byte_5091A  db 0            ; DATA XREF: sub_4AF0C+E0w
                    ; sub_4AF0C+135w ...
        align 8
currVideoMode   db 20h dup(0)       ; DATA XREF: checkVideo+4w
                    ; sub_4D2E1+2r
byte_50940  db 0            ; DATA XREF: start+2F9w
                    ; start:loc_46F20w ...
byte_50941  db 0            ; DATA XREF: sub_47E98+23r
                    ; sub_47E98+50r ...
word_50942  dw 0            ; DATA XREF: sub_48E59:loc_48E67w
                    ; sub_48E59:loc_48E8Dw
word_50944  dw 0            ; DATA XREF: sub_48E59+11w
                    ; sub_48E59:loc_48EC8w
byte_50946  db 0            ; DATA XREF: start+2F2r sub_48E59+20r ...
word_50947  dw 0            ; DATA XREF: sub_4921B+6w
                    ; sub_4921B+3Aw
word_50949  dw 0            ; DATA XREF: sub_4921B+9w
                    ; sub_4921B+74w
word_5094B  dw 0            ; DATA XREF: sub_48E59+37r
                    ; sub_4921B+Cw ...
word_5094D  dw 0            ; DATA XREF: sub_48E59+72r
                    ; sub_4921B+Fw ...
word_5094F  dw 0            ; DATA XREF: sub_48E59+14w
                    ; sub_48E59:loc_48EBBw ...
word_50951  dw 0            ; DATA XREF: sub_48E59+17w
                    ; sub_48E59:loc_48EF6w ...
byte_50953  db 0            ; DATA XREF: sub_48E59+1Aw
byte_50954  db 0, 10h dup(1), 4, 2, 0Eh dup(1), 2 dup(4), 2 dup(2)
                    ; DATA XREF: sub_48E59+1Dw
        db 0Ch dup(1), 3 dup(4), 3 dup(2), 0Ah dup(1), 4 dup(4)
        db 4 dup(2), 8 dup(1), 5 dup(4), 5 dup(2), 6 dup(1), 6 dup(4)
        db 6 dup(2), 5 dup(0), 6 dup(4), 6 dup(2), 5 dup(0), 6 dup(4)
        db 6 dup(2), 5 dup(0), 6 dup(4), 6 dup(2), 5 dup(0), 6 dup(4)
        db 6 dup(2), 5 dup(0), 6 dup(4), 6 dup(2), 6 dup(3), 5 dup(4)
        db 5 dup(2), 8 dup(3), 4 dup(4), 4 dup(2), 0Ah dup(3)
        db 3 dup(4), 3 dup(2), 0Ch dup(3), 2 dup(4), 2 dup(2)
        db 0Eh dup(3), 4, 2, 10h dup(3)
word_50A76  dw 0            ; DATA XREF: readConfig:loc_474BEw
                    ; readConfig+20r ...
word_50A78  dw 0            ; DATA XREF: fileReadUnk1+28o
                    ; fileReadUnk1+4Er ...
word_50A7A  dw 0            ; DATA XREF: fileReadUnk1+44r
byte_50A7C  db 5A0h dup(0)
byte_5101C  db 0, 18h dup(14h)  ; DATA XREF: sub_4955B+21Fw
                    ; sub_4955B+224w ...
byte_51035  db 0            ; DATA XREF: uncalled?:loc_47FA4r
                    ; uncalled?:loc_48053r ...
byte_51036  db 0            ; DATA XREF: sub_4A3BBr sub_4A3BB+7r
byte_51037  db 3Fh dup(0)       ; DATA XREF: sub_4F2AFr
word_51076  dw 13h dup(0)       ; DATA XREF: readLevels+10Ar
int8loc     dd 0            ; DATA XREF: int8handler+AEr
                    ; setint8+Cw ...
        dw 0
word_510A2  dw 0            ; DATA XREF: sub_47E98+5Aw
                    ; runLevel+58w ...
        dw 0FFFFh
byte_510A6  db 0            ; DATA XREF: loadScreen2+ADw
                    ; readTitleDatAndGraphics?+1Ew ...
word_510A7  dw 0            ; DATA XREF: setint24+Cw resetint24+2r
word_510A9  dw 0            ; DATA XREF: setint24+8w resetint24+6r
byte_510AB  db 0            ; DATA XREF: showNewCredits+2Aw
word_510AC  dw 0            ; DATA XREF: sub_4F2AF+2Er
                    ; sub_4F2AF+35w
byte_510AE  db 0            ; DATA XREF: int8handler+Cr
                    ; sub_48A20+39w ...
byte_510AF  db 0            ; DATA XREF: int8handler+13r
                    ; int8handler+18w ...
byte_510B0  db 0            ; DATA XREF: int8handler+24r
                    ; int8handler+29w ...
byte_510B1  db 0            ; DATA XREF: int8handler+35r
                    ; int8handler+3Aw ...
byte_510B2  db 0            ; DATA XREF: int8handler+46w
                    ; sub_48A20+4Dw ...
byte_510B3  db 0            ; DATA XREF: start+3C7w runLevel+364r ...
byte_510B4  db 0            ; DATA XREF: sub_48A20+52w
                    ; sub_4A95F+3w ...
byte_510B5  db 0            ; DATA XREF: sub_48A20+57w
                    ; sub_4A95F+9w ...
byte_510B6  db 0            ; DATA XREF: sub_48A20+5Cw
                    ; sub_4A95F+Fw ...
word_510B7  dw 0            ; DATA XREF: sub_48A20+2Aw
                    ; sub_4955B+68Cw ...
byte_510B9  db 0            ; DATA XREF: sub_48A20+34w
                    ; sub_4955B+68Fw ...
byte_510BA  db 0            ; DATA XREF: runLevel+Cw runLevel+18w ...
byte_510BB  db 0            ; DATA XREF: sub_4ADFF+D4w
                    ; sub_4D24D+Er ...
word_510BC  dw 0            ; DATA XREF: sub_48A20+3w
word_510BE  dw 0            ; DATA XREF: sub_48A20+9w
byte_510C0  db 0            ; DATA XREF: sub_48A20+61w
                    ; sub_49EBE+2r ...
word_510C1  dw 0            ; DATA XREF: crt?2r crt?1r ...
word_510C3  dw 0            ; DATA XREF: sub_48A20r sub_4A291+8w ...
word_510C5  dw 0            ; DATA XREF: sub_48A20+6r sub_4A291+Ew ...
word_510C7  dw 0            ; DATA XREF: gameloop?+C6r update?+14w
word_510C9  dw 0            ; DATA XREF: sub_4955B+67Er
                    ; gameloop?+Ar ...
word_510CB  dw 0            ; DATA XREF: sub_48A20+Ew
                    ; update?:loc_4E10Cw ...
word_510CD  dw 0            ; DATA XREF: sub_48A20+24w
                    ; update?:loc_4DF05w ...
word_510CF  dw 0            ; DATA XREF: gameloop?+B1r update?+7w ...
word_510D1  dw 0            ; DATA XREF: sub_48A20+11w
                    ; runLevel+77w ...
byte_510D3  db 0            ; DATA XREF: update?:loc_4DEEDw
                    ; update?+1AAw ...
        dw 0
        db 0
byte_510D7  db 0            ; DATA XREF: sub_48A20+80w
                    ; sub_4955B+269w ...
byte_510D8  db 0            ; DATA XREF: update?:loc_4DEB4w
                    ; update?+4Cw ...
word_510D9  dw 0            ; DATA XREF: sub_48A20+8Aw
                    ; update?+9AEw ...
byte_510DB  db 0            ; DATA XREF: sub_48A20+8Fw
                    ; runLevel:loc_48B09w ...
word_510DC  dw 0            ; DATA XREF: sub_48A20+94w
                    ; sub_4A910+11r ...
byte_510DE  db 0            ; DATA XREF: start:loc_46FDFw
                    ; start+3E7w ...
word_510DF  dw 0            ; DATA XREF: readDemo+9w readDemo+D5r ...
byte_510E1  db 0            ; DATA XREF: sub_492F1+Fw
                    ; sub_492F1:loc_49311r ...
byte_510E2  db 0            ; DATA XREF: sub_492A8r sub_492A8+7w ...
byte_510E3  db 0            ; DATA XREF: runLevel+34Er
                    ; somethingspsig+E4w ...
word_510E4  dw 0            ; DATA XREF: sub_492F1+4Fr
                    ; somethingspsig+13r ...
word_510E6  dw 0            ; DATA XREF: sub_4B159+81w
                    ; demoSomething?+58w ...
word_510E8  dw 0            ; DATA XREF: sub_49EBE:loc_49ECCr
                    ; sub_4A291+17w ...
word_510EA  dw 0            ; DATA XREF: sub_49EBE+12r
                    ; sub_4A291+1Bw ...
        dw 0
word_510EE  dw 0            ; DATA XREF: update?+49Ew
                    ; update?:loc_4E838w ...
word_510F0  dw 0            ; DATA XREF: update?+BEFr
                    ; update?:loc_4EAFAr ...
word_510F2  dw 0            ; DATA XREF: update?+C38r update?+C6Dr
word_510F4  dw 0            ; DATA XREF: update?+C02r
word_510F6  dw 0            ; DATA XREF: update?+C06r update?+C45r
word_510F8  dw 0            ; DATA XREF: update?+BF3r update?+BFCw ...
word_510FA  dw 0            ; DATA XREF: update?+BDDr update?+C82r
word_510FC  dw 0            ; DATA XREF: update?+BE4r update?+C87r
        db 10h dup(0), 0ACh, 6, 0Ch, 0FFh, 2, 0, 12h, 0, 1Eh, 11h
        db 2 dup(0), 0FEh, 0FFh, 2 dup(0), 0ACh, 6, 0Ch, 0FFh
        db 2, 0, 12h, 0, 30h, 11h, 2 dup(0), 0FEh, 0FFh, 6 dup(0)
        db 4, 0, 10h, 0, 66h, 11h, 0FEh, 0FFh, 4 dup(0), 60h, 0F8h
        db 0F4h, 0, 2, 0, 12h, 0, 42h, 11h, 2 dup(0), 2, 3 dup(0)
        db 60h, 0F8h, 0F4h, 0, 2, 0, 12h, 0, 54h, 11h, 2 dup(0)
        db 2, 3 dup(0), 0FEh, 0FFh, 2 dup(0), 4, 0, 10h, 0, 78h
        db 11h, 2, 9 dup(0), 2, 0, 10h, 0, 8Ah, 11h, 6 dup(0)
        db 0ACh, 6, 0Ch, 0FFh, 2, 0, 12h, 0, 1Eh, 11h, 2 dup(0)
        db 0FEh, 0FFh, 2 dup(0), 0ACh, 6, 0Ch, 0FFh, 2, 0, 12h
        db 0, 30h, 11h, 2 dup(0), 0FEh, 0FFh, 6 dup(0), 4, 0, 10h
        db 0, 0DCh, 11h, 0FEh, 0FFh, 4 dup(0), 60h, 0F8h, 0F4h
        db 0, 2, 0, 12h, 0, 42h, 11h, 2 dup(0), 2, 3 dup(0), 60h
        db 0F8h, 0F4h, 0, 2, 0, 12h, 0, 54h, 11h, 2 dup(0), 2
        db 3 dup(0), 0FEh, 0FFh, 2 dup(0), 4, 0, 10h, 0, 0EEh
        db 11h, 2, 5 dup(0), 60h, 0F8h, 2 dup(0), 2, 0, 10h, 2 dup(0)
        db 12h, 6 dup(0), 0FEh, 0FFh, 2 dup(0), 2, 0, 10h, 2 dup(0)
        db 12h, 6 dup(0), 0A0h, 7, 2 dup(0), 2, 0, 10h, 2 dup(0)
        db 12h, 6 dup(0), 2, 3 dup(0), 2, 0, 10h, 2 dup(0), 12h
        db 6 dup(0), 0ACh, 6, 0Ch, 0FFh, 2, 0, 12h, 0, 1Eh, 11h
        db 2 dup(0), 0FEh, 0FFh, 2 dup(0), 0ACh, 6, 0Ch, 0FFh
        db 2, 0, 12h, 0, 30h, 11h, 2 dup(0), 0FEh, 0FFh, 6 dup(0)
        db 4, 0, 10h, 0, 2 dup(12h), 0FEh, 0FFh, 4 dup(0), 60h
        db 0F8h, 0F4h, 0, 2, 0, 12h, 0, 42h, 11h, 2 dup(0), 2
        db 3 dup(0), 60h, 0F8h, 0F4h, 0, 2, 0, 12h, 0, 54h, 11h
        db 2 dup(0), 2, 3 dup(0), 0FEh, 0FFh, 2 dup(0), 4, 0, 10h
        db 0, 24h, 12h, 2, 5 dup(0), 60h, 0F8h, 2 dup(0), 2, 0
        db 10h, 0, 36h, 12h, 6 dup(0), 0FEh, 0FFh, 2 dup(0), 2
        db 0, 10h, 0, 36h, 12h, 6 dup(0), 0A0h, 7, 2 dup(0), 2
        db 0, 10h, 0, 36h, 12h, 6 dup(0), 2, 3 dup(0), 2, 0, 10h
        db 0, 36h, 12h, 6 dup(0), 0FCh, 0FFh, 2 dup(0), 6, 0, 10h
        db 0, 46h, 12h, 0FEh, 0FFh, 8 dup(0), 6, 0, 10h, 0, 58h
        db 12h, 2, 7 dup(0), 0C0h, 0F0h, 2, 0, 10h, 0, 40h, 13h
        db 2 dup(0), 0FCh, 0FFh, 4 dup(0), 0FCh, 0FFh, 2, 0, 10h
        db 0, 0F8h, 12h, 0FCh, 0FFh, 6 dup(0), 40h, 0Fh, 2, 0
        db 10h, 0, 64h, 13h, 2 dup(0), 4, 5 dup(0), 4, 0, 2, 0
        db 10h, 0, 1Ch, 13h, 4, 5 dup(0), 0Ch, 0FFh, 0Ch, 0FFh
        db 2, 0, 12h, 0, 1Eh, 11h, 2 dup(0), 0FEh, 0FFh, 2 dup(0)
        db 0Ch, 0FFh, 0Ch, 0FFh, 2, 0, 12h, 0, 30h, 11h, 2 dup(0)
        db 0FEh, 0FFh, 6 dup(0), 4, 0, 10h, 0, 48h, 14h, 0FEh
        db 0FFh, 6 dup(0), 0F4h, 0, 2, 0, 12h, 0, 42h, 11h, 2 dup(0)
        db 2, 5 dup(0), 0F4h, 0, 2, 0, 12h, 0, 54h, 11h, 2 dup(0)
        db 2, 3 dup(0), 0FEh, 0FFh, 2 dup(0), 4, 0, 10h, 0, 5Ah
        db 14h, 2, 5 dup(0), 60h, 0F8h, 2 dup(0), 2, 0, 10h, 0
        db 6Eh, 14h, 6 dup(0), 0FEh, 0FFh, 2 dup(0), 2, 0, 10h
        db 0, 6Eh, 14h, 6 dup(0), 0A0h, 7, 2 dup(0), 2, 0, 10h
        db 0, 6Eh, 14h, 6 dup(0), 2, 3 dup(0), 2, 0, 10h, 0, 6Eh
        db 14h, 6 dup(0), 6Ch, 0F7h, 0Ch, 0FFh, 2, 0, 22h, 0, 88h
        db 14h, 2 dup(0), 0FEh, 0FFh, 2 dup(0), 0FCh, 0FFh, 2 dup(0)
        db 6, 0, 10h, 0, 9Ah, 14h, 0FEh, 0FFh, 6 dup(0), 0F4h
        db 0, 2, 0, 22h, 0, 0ACh, 14h, 2 dup(0), 2, 7 dup(0), 6
        db 0, 10h, 0, 0BEh, 14h, 2, 5 dup(0), 0FCh, 0FFh, 2 dup(0)
        db 6, 0, 10h, 0, 0D0h, 14h, 0FEh, 0FFh, 8 dup(0), 6, 0
        db 10h, 0, 0E2h, 14h, 2, 9 dup(0), 2, 0, 10h, 0, 84h, 14h
        db 6 dup(0), 0E6h, 2Ah, 0E6h, 2Ah, 0E8h, 2Ah, 0E8h, 2Ah
        db 0EAh, 2Ah, 0EAh, 2Ah, 0E8h, 2Ah, 0E8h, 2Ah, 2 dup(0FFh)
        db 0ECh, 2Ah, 0ECh, 2Ah, 0EEh, 2Ah, 0EEh, 2Ah, 0F0h, 2Ah
        db 0F0h, 2Ah, 0EEh, 2Ah, 0EEh, 2Ah, 2 dup(0FFh), 0F2h
        db 29h, 0F2h, 29h, 0F4h, 29h, 0F4h, 29h, 0F6h, 29h, 0F6h
        db 29h, 0F4h, 29h, 0F4h, 29h, 2 dup(0FFh), 0F8h, 29h, 0F8h
        db 29h, 0FAh, 29h, 0FAh, 29h, 0FCh, 29h, 0FCh, 29h, 0FAh
        db 29h, 0FAh, 29h, 2 dup(0FFh), 0B6h, 1Ah, 0BAh, 1Ah, 0BEh
        db 1Ah, 0C2h, 1Ah, 0C6h, 1Ah, 0CAh, 1Ah, 0CEh, 1Ah, 0D2h
        db 1Ah, 2 dup(0FFh), 0D6h, 1Ah, 52h, 22h, 56h, 22h, 5Ah
        db 22h, 5Eh, 22h, 62h, 22h, 66h, 22h, 6Ah, 22h, 2 dup(0FFh)
        db 6, 2Ah, 6, 2Ah, 6, 2Ah, 6, 2Ah, 8, 2Ah, 8, 2Ah, 8, 2Ah
        db 8, 2Ah, 0Ah, 2Ah, 0Ah, 2Ah, 0Ah, 2Ah, 0Ah, 2Ah, 0Ch
        db 2Ah, 0Ch, 2Ah, 0Ch, 2Ah, 0Ch, 2Ah, 0Eh, 2Ah, 0Eh, 2Ah
        db 0Eh, 2Ah, 0Eh, 2Ah, 10h, 2Ah, 10h, 2Ah, 10h, 2Ah, 10h
        db 2Ah, 12h, 2Ah, 12h, 2Ah, 12h, 2Ah, 12h, 2Ah, 14h, 2Ah
        db 14h, 2Ah, 14h, 2Ah, 14h, 2Ah, 16h, 2Ah, 16h, 2Ah, 16h
        db 2Ah, 16h, 2Ah, 90h, 0Bh, 90h, 0Bh, 90h, 0Bh, 90h, 0Bh
        db 2 dup(0FFh), 72h, 0Bh, 76h, 0Bh, 7Ah, 0Bh, 7Eh, 0Bh
        db 82h, 0Bh, 86h, 0Bh, 8Ah, 0Bh, 8Eh, 0Bh, 2 dup(0FFh)
        db 92h, 0Bh, 96h, 0Bh, 12h, 13h, 16h, 13h, 1Ah, 13h, 1Eh
        db 13h, 22h, 13h, 26h, 13h, 2 dup(0FFh), 9Ah, 33h, 9Ch
        db 33h, 9Eh, 33h, 0A0h, 33h, 3Ah, 3Bh, 3Ch, 3Bh, 3Eh, 3Bh
        db 14h, 5, 2 dup(0FFh), 6Eh, 23h, 72h, 23h, 76h, 23h, 7Ah
        db 23h, 7Eh, 23h, 82h, 23h, 86h, 23h, 8Ah, 23h, 2 dup(0FFh)
        db 8Eh, 23h, 92h, 23h, 0Eh, 2Bh, 12h, 2Bh, 16h, 2Bh, 1Ah
        db 2Bh, 1Eh, 2Bh, 22h, 2Bh, 2 dup(0FFh), 6, 5, 8, 5, 0Ah
        db 5, 0Eh, 5, 10h, 5, 12h, 5, 14h, 5, 2 dup(0FFh), 0BAh
        db 42h, 0C0h, 42h, 0C6h, 42h, 0CCh, 42h, 0D2h, 42h, 0D8h
        db 42h, 5Ah, 4Ah, 60h, 4Ah, 2 dup(0FFh), 66h, 4Ah, 6Ch
        db 4Ah, 72h, 4Ah, 78h, 4Ah, 0EEh, 4, 0F4h, 4, 0FAh, 4
        db 0, 5, 2 dup(0FFh)
word_5157A  dw 4A62h        ; DATA XREF: update?+83Er update?+AA4r ...
word_5157C  dw 502h         ; DATA XREF: update?+86Fr update?+A7Dr ...
word_5157E  dw 4A80h        ; DATA XREF: sub_4A291+22r update?+85r ...
word_51580  dw 1AB2h        ; DATA XREF: sub_4AAB4+14r
        db 40h
        db 3Bh
        dw 1BEEh
        dw 1BF0h
        dw 1BF2h
        dw 1BF4h
        dw 1BF2h
        dw 1BF0h
        dw 1BF2h
        dw 1BF4h
        dw 1BF2h
        dw 1BF0h
        dw 1BEEh
        db 40h
        db 3Bh
        db 18h
        db 2Ah
        db 18h
        db 2Ah
        db 96h
        db 33h
word_515A2  dw 32A2h        ; DATA XREF: uncalled?+FAr
        db 7Ah, 33h, 7Eh, 33h, 82h, 33h, 86h, 33h, 8Ah, 33h, 8Eh
        db 33h, 92h, 33h, 96h, 33h, 1Ah, 3Bh, 1Eh, 3Bh, 22h, 3Bh
        db 26h, 3Bh, 2Ah, 3Bh, 2Eh, 3Bh, 32h, 3Bh, 36h, 3Bh
word_515C4  dw 1358h        ; DATA XREF: uncalled?2+DAr
        db 8Eh, 0Ch, 92h, 0Ch, 96h, 0Ch, 9Ah, 0Ch, 8Fh, 0Ch, 0A2h
        db 0Ch, 0A6h, 0Ch, 0AAh, 0Ch, 2Eh, 14h, 32h, 14h, 36h
        db 14h, 3Ah, 14h, 3Eh, 14h, 42h, 14h, 46h, 14h, 4Ah, 14h
        db 0CEh, 1Bh, 0D0h, 1Bh, 0D2h, 1Bh, 0D4h, 1Bh, 0D6h, 1Bh
        db 0D8h, 1Bh, 0DAh, 1Bh, 0DCh, 1Bh, 0DEh, 1Bh, 0E0h, 1Bh
        db 0E2h, 1Bh, 0E4h, 1Bh, 0E6h, 1Bh, 0E8h, 1Bh, 0EAh, 1Bh
        db 0ECh, 1Bh, 2, 2Ah, 0B8h, 1Ah, 0BCh, 1Ah, 0C0h, 1Ah
        db 0C4h, 1Ah, 0C8h, 1Ah, 0CCh, 1Ah, 0D0h, 1Ah, 0D4h, 1Ah
        db 2 dup(0FFh), 0B6h, 1Ah, 0BAh, 1Ah, 0BEh, 1Ah, 0C2h
        db 1Ah, 0C6h, 1Ah, 0CAh, 1Ah, 0CEh, 1Ah, 0D2h, 1Ah, 2 dup(0FFh)
        db 0D6h, 1Ah, 52h, 22h, 56h, 22h, 5Ah, 22h, 5Eh, 22h, 62h
        db 22h, 66h, 22h, 6Ah, 22h, 2 dup(0FFh), 0D8h, 1Ah, 54h
        db 22h, 58h, 22h, 5Ch, 22h, 60h, 22h, 64h, 22h, 68h, 22h
        db 6Ch, 22h, 2 dup(0FFh), 74h, 4Bh, 68h, 4Ch, 50h, 0, 44h
        db 1, 38h, 2, 2Ch, 3, 20h, 4, 14h, 5, 2 dup(0FFh), 0D4h
        db 43h, 0C8h, 44h, 0BCh, 45h, 0B0h, 46h, 0A4h, 47h, 98h
        db 48h, 8Ch, 49h, 80h, 4Ah, 2 dup(0FFh), 8Ch, 49h, 98h
        db 48h, 0A4h, 47h, 0B0h, 46h, 0BCh, 45h, 0C8h, 44h
byte_51680  db 0D4h, 43h, 0E0h, 42h, 2 dup(0FFh), 20h, 4, 2Ch, 3, 38h
        db 2, 44h, 1, 50h, 0, 68h, 4Ch, 74h, 4Bh, 80h, 4Ah, 2 dup(0FFh)
        db 5Ah, 2Ah, 56h, 3Ah, 0BAh, 32h, 58h, 3Ah, 5Ch, 2Ah, 5Ah
        db 3Ah, 54h, 3Ah, 5Ch, 3Ah, 5Ah, 2Ah, 5Ch, 3Ah, 54h, 3Ah
        db 5Ah, 3Ah, 5Ch, 2Ah, 58h, 3Ah, 0BAh, 32h, 56h, 3Ah, 6Ah
        db 3Bh, 6Ch, 3Bh, 6Eh, 3Bh, 70h, 3Bh, 72h, 3Bh, 74h, 3Bh
        db 76h, 3Bh, 78h, 3Bh, 26h, 2Bh, 2Ah, 2Bh, 2Eh, 2Bh, 32h
        db 2Bh, 0AEh, 32h, 0B2h, 32h, 0B6h, 32h, 0BAh, 32h, 88h
        db 3Ah, 8Ah, 3Ah, 8Ch, 3Ah, 8Eh, 3Ah, 90h, 3Ah, 92h, 3Ah
        db 94h, 3Ah, 96h, 3Ah, 0BEh, 32h, 0C2h, 32h, 0C6h, 32h
        db 0CAh, 32h, 0CEh, 32h, 0D2h, 32h, 4Eh, 3Ah, 52h, 3Ah
        db 0E2h, 31h, 0E4h, 31h, 0E6h, 31h, 0E8h, 31h, 0EAh, 31h
        db 0ECh, 31h, 0EEh, 31h, 0F0h, 31h, 0E2h, 31h, 0F0h, 31h
        db 0EEh, 31h, 0ECh, 31h, 0EAh, 31h, 0E8h, 31h, 0E6h, 31h
        db 0E4h, 31h, 0F4h, 31h, 0F6h, 31h, 0F8h, 31h, 0FAh, 31h
        db 0FCh, 31h, 0FEh, 31h, 0, 32h, 2, 32h, 0A2h, 22h, 0A6h
        db 22h, 0AAh, 22h, 0AEh, 22h, 0B2h, 22h, 0B6h, 22h, 0BAh
        db 22h, 0BEh, 22h, 0EEh, 30h, 0F0h, 30h, 0F2h, 30h, 0F4h
        db 30h, 0F6h, 30h, 72h, 31h, 74h, 31h, 0FCh, 30h, 0C2h
        db 22h, 0C6h, 22h, 42h, 2Ah, 46h, 2Ah, 4Ah, 2Ah, 4Eh, 2Ah
        db 52h, 2Ah, 56h, 2Ah, 5Eh, 3Ah, 62h, 3Ah, 66h, 3Ah, 6Ah
        db 3Ah, 6Eh, 3Ah, 72h, 3Ah, 12h, 42h, 0B2h, 49h, 2 dup(0FFh)
        db 3Ah, 4, 3Eh, 4, 42h, 4, 46h, 4, 46h, 4, 0E6h, 0Bh, 86h
        db 13h, 1Ah, 1Bh, 1Eh, 1Bh, 2 dup(0FFh)
word_5177E  dw 0CAEh, 0CB0h, 0CB2h, 0CB4h, 144Eh, 1450h, 1452h, 1454h
                    ; DATA XREF: sub_4A910+26r
        dw 0FFFFh
word_51790  dw 4A7Eh, 0FFFFh, 0CAEh, 0FFFFh, 8 dup(32FCh), 0FFFFh
                    ; DATA XREF: update?+108Er
        dw 0BC2h, 0BC8h, 0BCEh, 0BD4h, 0BDAh, 0BE0h, 1362h, 1368h
        dw 0FFFFh, 8 dup(32FAh), 0FFFFh, 136Eh, 1374h, 137Ah, 1380h
        dw 1B02h, 1B08h, 1B0Eh, 1B14h, 0FFFFh, 41EEh, 41F4h, 41FAh
        dw 4200h, 4206h, 420Ch, 498Eh, 4994h, 0FFFFh, 499Ah, 49A0h
        dw 49A6h, 49ACh, 422h, 428h, 42Eh, 434h, 0FFFFh, 45EAh
        dw 45ECh, 45EEh, 45F0h, 45F2h, 45F4h, 45F6h, 45F8h, 45FAh
        dw 45FCh, 45FEh, 4600h, 0FFFFh, 460Ch, 460Ah, 4608h, 0FFFFh
        dw 4602h, 4604h, 4606h, 0FFFFh
word_5182E  dw 2A64h, 456Ch, 456Dh, 456Eh, 456Fh, 49B6h, 49B7h, 49B8h
                    ; DATA XREF: sub_4FD65+26r
        dw 49B9h
word_51840  dw 2A06h        ; DATA XREF: update?+632r update?+769r ...
word_51842  dw 132Ch        ; DATA XREF: update?+65Er update?+789r ...
word_51844  dw 2A08h        ; DATA XREF: update?+68Ar update?+7A9r ...
word_51846  dw 132Ah        ; DATA XREF: update?+6B6r update?+7C9r ...
word_51848  dw 2A62h        ; DATA XREF: update?+8A2r update?+8CCr ...
word_5184A  dw 2A66h        ; DATA XREF: sub_48F6D+Br
word_5184C  dw 2A67h        ; DATA XREF: sub_48F6D+4Fr
word_5184E  dw 2E36h        ; DATA XREF: sub_48F6D+EEr
word_51850  dw 2E37h        ; DATA XREF: sub_48F6D+85r
word_51852  dw 2A68h        ; DATA XREF: sub_48F6D+CBr
word_51854  dw 2A69h        ; DATA XREF: sub_48F6D+65r
word_51856  dw 2E38h        ; DATA XREF: sub_48F6D+27r
word_51858  dw 2E39h        ; DATA XREF: sub_48F6D+9Fr
movingFunctions3 dw offset sub_4F312    ; DATA XREF: uncalled?4+16r
        dw 86F2h
        dw 86F2h
        dw 86F2h
        dw 86F2h
        dw 86F2h
        dw 86F2h
        dw 86F2h
        dw offset sub_4F40D
        dw 87EDh
        dw 87EDh
        dw 87EDh
        dw 87EDh
        dw 87EDh
        dw 87EDh
        dw 87EDh
        dw offset sub_4F50A
        dw 88EAh
        dw 88EAh
        dw 88EAh
        dw 88EAh
        dw 88EAh
        dw 88EAh
        dw 88EAh
        dw offset sub_4F5B5
        dw 8995h
        dw 8995h
        dw 8995h
        dw 8995h
        dw 8995h
        dw 8995h
        dw 8995h
        dw offset sub_4F65B
        dw 8A3Bh
        dw 8A3Bh
        dw 8A3Bh
        dw 8A3Bh
        dw 8A3Bh
        dw 8A3Bh
        dw 8A3Bh
        dw offset sub_4F708
        dw 8AE8h
        dw 8AE8h
        dw 8AE8h
        dw 8AE8h
        dw 8AE8h
        dw 8AE8h
        dw 8AE8h
movingFunctions2 dw offset sub_4F7D1    ; DATA XREF: uncalled?6+16r
        dw 8BB1h
        dw 8BB1h
        dw 8BB1h
        dw 8BB1h
        dw 8BB1h
        dw 8BB1h
        dw 8BB1h
        dw offset sub_4F8A5
        dw 8C85h
        dw 8C85h
        dw 8C85h
        dw 8C85h
        dw 8C85h
        dw 8C85h
        dw 8C85h
        dw offset sub_4F97B
        dw 8D5Bh
        dw 8D5Bh
        dw 8D5Bh
        dw 8D5Bh
        dw 8D5Bh
        dw 8D5Bh
        dw 8D5Bh
        dw offset sub_4FA26
        dw 8E06h
        dw 8E06h
        dw 8E06h
        dw 8E06h
        dw 8E06h
        dw 8E06h
        dw 8E06h
        dw offset sub_4FACC
        dw 8EACh
        dw 8EACh
        dw 8EACh
        dw 8EACh
        dw 8EACh
        dw 8EACh
        dw 8EACh
        dw offset sub_4FB79
        dw 8F59h
        dw 8F59h
        dw 8F59h
        dw 8F59h
        dw 8F59h
        dw 8F59h
        dw 8F59h
movingFunctions dw 0            ; DATA XREF: gameloop?+70r
        dw offset uncalled?
        dw 0
        dw 0
        dw offset uncalled?2
        dw 0
        dw 0
        dw 0
        dw offset uncalled?3
        dw 0
        dw 0
        dw 0
        dw 0
        dw 0
        dw 0
        dw 0
        dw 0
        dw offset uncalled?4
        dw 0
        dw offset uncalled?5
        dw 0
        dw 0
        dw 0
        dw 0
        dw offset uncalled?6
        dw offset uncalled?7
        dw 0
        dw 0
        dw 0
        dw 0
        dw 0
        dw offset loc_4A543
byte_5195A  db 0            ; DATA XREF: sub_4A3BB:loc_4A3C6w
                    ; sub_4C4F9+14r ...
byte_5195B  db 0            ; DATA XREF: sub_4A3BB+Fw
                    ; sub_4C4F9+36r ...
byte_5195C  db 0            ; DATA XREF: sub_48A20+1Aw
                    ; update?:loc_4E314r ...
word_5195D  dw 0F000h       ; DATA XREF: sub_48A20+70w
                    ; runLevel+32Aw ...
word_5195F  dw 0            ; DATA XREF: runLevel+10Cr
                    ; runLevel:loc_48CBEr ...
word_51961  dw 0            ; DATA XREF: runLevel+215r
                    ; runLevel:loc_48D59r ...
word_51963  dw 0            ; DATA XREF: runLevel+37Bw
                    ; sub_4914A+7w ...
word_51965  dw 0            ; DATA XREF: runLevel+381w
                    ; sub_4914A+25w ...
word_51967  dw 0            ; DATA XREF: crt?2+2Dr crt?2+3Aw ...
byte_51969  db 0            ; DATA XREF: sub_4BA5F:loc_4BA69w
                    ; sub_4BA5F+88r ...
byte_5196A  db 0            ; DATA XREF: sub_48A20+66w
                    ; uncalled?5+1Br ...
byte_5196B  db 0            ; DATA XREF: sub_48A20+6Bw
                    ; update?+88Fr ...
word_5196C  dw 0            ; DATA XREF: start+41Dw
                    ; sub_4B159:loc_4B163w ...
        dw 1
word_51970  dw 0            ; DATA XREF: start+15Cw
                    ; runLevel:loc_48B38r ...
        align 4
word_51974  dw 0            ; DATA XREF: int9handler+AFw
                    ; sub_48A20+14w ...
        align 4
word_51978  dw 0            ; DATA XREF: sub_48A20+17w
                    ; runLevel+70r ...
word_5197A  dw 0            ; DATA XREF: start+364r start+38Ar ...
byte_5197C  db 0            ; DATA XREF: sub_48A20+1Fw
                    ; sub_4955B+44Aw ...
algn_5197D:
        align 2
byte_5197E  db 0            ; DATA XREF: sub_47E98:keyIsPressedr
                    ; sub_47E98:loc_47F0Cr ...
byte_5197F  db 0            ; DATA XREF: sub_4955B:loc_496C0r
byte_51980  db 0            ; DATA XREF: sub_4955B:loc_496CDr
byte_51981  db 0            ; DATA XREF: sub_4955B:loc_496DAr
byte_51982  db 0            ; DATA XREF: sub_4955B:loc_496E7r
byte_51983  db 0            ; DATA XREF: sub_4955B:loc_496F4r
byte_51984  db 0            ; DATA XREF: sub_4955B:loc_49701r
byte_51985  db 0            ; DATA XREF: sub_4955B:loc_4970Er
byte_51986  db 0            ; DATA XREF: sub_4955B:loc_4971Br
byte_51987  db 0            ; DATA XREF: sub_4955B:loc_49728r
byte_51988  db 0            ; DATA XREF: sub_4955B:loc_49735r
byte_51989  db 0            ; DATA XREF: sub_4955B:loc_498A2r
byte_5198A  db 0            ; DATA XREF: sub_4955B:loc_498FCr
        dw 0
byte_5198D  db 0            ; DATA XREF: sub_4955B:loc_49D29r
byte_5198E  db 0            ; DATA XREF: sub_4955B:loc_499AAr
        align 2
byte_51990  db 0            ; DATA XREF: sub_4955B:loc_4969Fr
        dd 0
        db 0
byte_51996  db 0            ; DATA XREF: sub_4955B:loc_49C96r
                    ; sub_4955B:loc_49CA8r ...
        dw 0
byte_51999  db 0            ; DATA XREF: runLevel:loc_48B4Ar
                    ; sub_4955B:loc_4959Ar ...
byte_5199A  db 0            ; DATA XREF: resetint9r
                    ; sub_4955B:loc_49742r ...
        align 2
byte_5199C  db 0            ; DATA XREF: sub_4955B:loc_49693r
byte_5199D  db 0            ; DATA XREF: sub_4955B:loc_49656r
        align 4
byte_519A0  db 0            ; DATA XREF: sub_4955B:loc_4967Br
byte_519A1  db 0            ; DATA XREF: sub_4955B:loc_49585r
        db    0
dword_519A3 dd 0            ; DATA XREF: sub_4955B:loc_49A7Fr
word_519A7  dw 0            ; DATA XREF: resetint9+7r
                    ; sub_4B419+42r
word_519A9  dw 0            ; DATA XREF: sub_4955B:loc_49663r
word_519AB  dw 0            ; DATA XREF: sub_4955B:loc_49687r
word_519AD  dw 0            ; DATA XREF: sub_4955B:loc_4966Fr
dword_519AF dd 0            ; DATA XREF: sub_4955B:loc_49649r
word_519B3  dw 0            ; DATA XREF: resetint9+Er
                    ; sub_4955B:loc_49D48r ...
byte_519B5  db 0            ; DATA XREF: resetint9+15r
                    ; int9handler+A8r ...
byte_519B6  db 0            ; DATA XREF: sub_4A1BF:loc_4A215r
byte_519B7  db 0            ; DATA XREF: resetint9+1Cr
byte_519B8  db 0            ; DATA XREF: sub_4955B:loc_49761r
                    ; sub_4955B:loc_497E5r ...
byte_519B9  db 0            ; DATA XREF: sub_4955B:loc_49786r
                    ; sub_4955B:loc_497F5r ...
byte_519BA  db 0            ; DATA XREF: sub_4955B:loc_497ABr
                    ; sub_4955B:loc_49805r ...
byte_519BB  db 0            ; DATA XREF: sub_4955B:loc_49814r
                    ; runMainMenu:loc_4C8E8r
byte_519BC  db 0            ; DATA XREF: sub_4955B:loc_49823r
                    ; runMainMenu:loc_4C8F8r
byte_519BD  db 0            ; DATA XREF: sub_4955B:loc_49832r
                    ; runMainMenu:loc_4C908r
byte_519BE  db 0            ; DATA XREF: sub_4955B:loc_49841r
                    ; runMainMenu:loc_4C918r
byte_519BF  db 0            ; DATA XREF: sub_4955B:loc_49850r
                    ; runMainMenu:loc_4C928r
byte_519C0  db 0            ; DATA XREF: sub_4955B:loc_4985Fr
                    ; runMainMenu:loc_4C937r
byte_519C1  db 0            ; DATA XREF: sub_4955B:loc_4986Er
                    ; runMainMenu:loc_4C946r
byte_519C2  db 0            ; DATA XREF: resetint9+23r
                    ; sub_4955B:loc_49CC8r ...
byte_519C3  db 0            ; DATA XREF: resetint9+2Ar
                    ; sub_4955B:loc_49984r ...
byte_519C4  db 0            ; DATA XREF: sub_4914A+6Dr
byte_519C5  db 0            ; DATA XREF: sub_4914A:loc_49168r
                    ; sub_4A1BF+2r ...
byte_519C6  db 0            ; DATA XREF: sub_4914A+7Fr
        align 2
byte_519C8  db 0            ; DATA XREF: sub_4914Ar
                    ; sub_4A1BF:loc_4A1D6r ...
byte_519C9  db 0            ; DATA XREF: sub_4914A:loc_49186r
                    ; sub_49EBE+60r
byte_519CA  db 0            ; DATA XREF: sub_4914A:loc_49159r
                    ; sub_4A1BF:loc_4A200r ...
        align 2
byte_519CC  db 0            ; DATA XREF: sub_4914A+B0r
byte_519CD  db 0            ; DATA XREF: sub_4914A:loc_49177r
                    ; sub_4A1BF:loc_4A1EBr ...
byte_519CE  db 0            ; DATA XREF: sub_4914A+C2r
byte_519CF  db 0            ; DATA XREF: resetint9+31r
                    ; sub_4914A+5Br
byte_519D0  db 0            ; DATA XREF: sub_4914A:loc_491E8r
byte_519D1  db 0            ; DATA XREF: resetint9+38r
        align 4
byte_519D4  db 0            ; DATA XREF: runMainMenu:loc_4C955r
byte_519D5  db 1Fh dup(0)       ; DATA XREF: sub_4955B:loc_4987Dr
                    ; sub_4955B:loc_49962r ...
byte_519F4  db 0            ; DATA XREF: sub_48E59:loc_48F34r
                    ; sub_48E59:loc_48F5Fr ...
byte_519F5  db 0            ; DATA XREF: sub_48E59:loc_48F28r
                    ; sub_48E59:loc_48F54r ...
byte_519F6  db 0            ; DATA XREF: sub_48E59:loc_48EB1r
                    ; sub_4A1BF+48r
byte_519F7  db 0            ; DATA XREF: sub_48E59:loc_48EA5r
                    ; sub_4A1BF+1Er
byte_519F8  db 0            ; DATA XREF: sub_48E59:loc_48EECr
                    ; sub_4A1BF+33r
byte_519F9  db 0            ; DATA XREF: sub_48E59:loc_48EE0r
                    ; sub_4A1BF+9r
        db    0
        db    0
        db    0
int9off     dw 0            ; DATA XREF: setint9+1Bw resetint9+41r
int9seg     dw 0            ; DATA XREF: setint9+17w resetint9+45r
word_51A01  dw 0            ; DATA XREF: runLevel:loc_48E30w
                    ; sub_4945Dw ...
dword_51A03 dd 0            ; DATA XREF: runLevel:loc_48B78r
                    ; runLevel+D3r ...
word_51A07  dw 1            ; DATA XREF: runLevel:loc_48DA8r
                    ; runLevel:loc_48DB2r ...
keyPressed  db 0            ; DATA XREF: int9handler:storeKeyw
                    ; int9handler:storeNoKeyPressedw ...
        align 4
a1234567890 db '1234567890-',0
        db 8,0
aQwertyuiop db 'QWERTYUIOP',0
        align 2
        db 0Ah,0
aAsdfghjkl  db 'ASDFGHJKL',0
        dd 0
aZxcvbnm    db 'ZXCVBNM',0
        dd 0
        db 0
aSpace      db 20h
        db 66h dup(0), 2Eh, 0, 1Eh, 0, 31h, 0, 14h, 0, 39h, 0
        db 1Fh, 0, 14h, 0, 18h, 0, 2 dup(0FFh)
word_51ABC  dw 1            ; DATA XREF: start+414w sub_4945D+84r ...
byte_51ABE  db 0            ; DATA XREF: start+3FFw sub_4AB1B+1DBw ...
aLevels_dat_0   db 'LEVELS.DAT',0       ; DATA XREF: readLevels:loc_4D59Fo
                    ; start:loc_46DA6w ...
byte_51ACA  db 7Ah dup(0)
leveldata   dw 900h dup(0), 0Dh dup(2020h), 20h, 0Dh dup(2020h), 20h
                    ; DATA XREF: uncalled?r
                    ; uncalled?:loc_47F98r ...
        dw 612h dup(0), 202Dh, 4552h, 4C50h, 5941h, 5320h, 494Bh
        dw 5050h, 4445h, 4C20h, 5645h, 4C45h, 2153h, 2021h, 0A2Dh
        dw 2 dup(2D2Dh), 5520h, 424Eh, 4C45h, 4549h, 4556h, 4241h
        dw 454Ch, 2 dup(2121h), 2D20h, 2D2Dh, 0A2Dh, 0Dh dup(2020h)
        dw 20h, 0Dh dup(2020h), 20h
byte_53A10  db 1            ; DATA XREF: sub_47F39+Cr
                    ; sub_47F39+16w ...
aInsertDisk1    db 'INSERT DISK 1',0
aInsertDisk2    db 'INSERT DISK 2',0
aSupaplex_cfg   db 'SUPAPLEX.CFG',0     ; DATA XREF: readConfig+3o
aMoving_dat db 'MOVING.DAT',0       ; DATA XREF: readMoving+9o
aFixed_dat  db 'FIXED.DAT',0        ; DATA XREF: readMoving+CDo
aPanel_dat  db 'PANEL.DAT',0        ; DATA XREF: readPanelDat+3o
aMenu_dat   db 'MENU.DAT',0         ; DATA XREF: readMenuDat+3o
aTitle_dat  db 'TITLE.DAT',0        ; DATA XREF: readTitleDatAndGraphics?+26o
aTitle1_dat db 'TITLE1.DAT',0
aTitle2_dat db 'TITLE2.DAT',0
aBack_dat   db 'BACK.DAT',0         ; DATA XREF: readBackDat+3o
aGfx_dat    db 'GFX.DAT',0          ; DATA XREF: readGfxDat+3o
aChars6_dat db 'CHARS6.DAT',0       ; DATA XREF: readChars6Dat+3o
aChars8_dat db 'CHARS8.DAT',0
aLevel_lst  db 'LEVEL.LST',0        ; DATA XREF: readLevelsLst+3o
                    ; readLevelsLst+A0o ...
aControls_dat   db 'CONTROLS.DAT',0     ; DATA XREF: readControlsDat+3o
aBeep_snd   db 'BEEP.SND',0         ; DATA XREF: loadBeep+3o loadBeep2+3o
aSample_snd db 'SAMPLE.SND',0       ; DATA XREF: loadBeep2+Co
aAdlib_snd  db 'ADLIB.SND',0        ; DATA XREF: loadAdlib+3o
                    ; loadAdlib2+3o
aRoland_snd db 'ROLAND.SND',0       ; DATA XREF: loadRoland+3o
                    ; loadRoland2+3o
aBlaster_snd    db 'BLASTER.SND',0      ; DATA XREF: loadAdlib2+Co
                    ; loadRoland2+Co
aPlayer_lst db 'PLAYER.LST',0       ; DATA XREF: readPlayersLst+Ao
                    ; start+18Fw ...
aPalettes_dat   db 'PALETTES.DAT',0     ; DATA XREF: readPallettes+3o
aHallfame_lst   db 'HALLFAME.LST',0     ; DATA XREF: readHallfameLst+Ao
                    ; start+192w ...
aDemo0_bin  db 'DEMO0.BIN',0        ; DATA XREF: readDemo:loc_4763Co
                    ; start+18Cw ...
videoStatusUnk  db 2701h dup(0), 2, 3, 5, 0, 0Dh, 0Ah, 4, 0Ch, 2, 2 dup(6)
                    ; DATA XREF: start+28Cw
                    ; slideDownGameDashr ...
        db 2, 3, 2 dup(9), 3, 0Bh, 8, 3, 6, 2, 2 dup(7), 0Ah, 8
        db 6, 0Dh, 9, 6, 4, 0Bh, 1, 9, 1, 0, 4, 0Bh, 1, 0, 4, 0Dh
        db 1, 0, 0Ch, 0Fh, 1, 0, 0Ch, 0Fh, 6, 4, 0Ch, 2, 5, 6
        db 8, 0Fh, 0Ch, 6, 0Eh, 2 dup(0Ch), 0Dh, 0Eh, 4 dup(0)
        db 4 dup(0Fh), 3 dup(6), 8, 3 dup(0Ah), 7, 3 dup(0Ah)
        db 7, 3 dup(0Bh), 7, 0Eh, 2 dup(1), 4, 3 dup(9), 7, 1
        db 3, 7, 0, 4 dup(8), 9, 2 dup(0), 4, 0Bh, 2 dup(0), 0Ch
        db 0, 2, 0Ah, 1, 3 dup(5), 8, 3 dup(6), 4 dup(8), 7, 4 dup(0)
        db 4 dup(0Fh), 4 dup(8), 3 dup(0Ah), 7, 3 dup(0Ah), 7
        db 3 dup(0Bh), 7, 0Eh, 2 dup(1), 4, 3 dup(9), 7, 8 dup(8)
        db 9, 2 dup(0), 4, 0Bh, 2 dup(0), 0Ch, 4 dup(8), 3 dup(5)
        db 8, 3 dup(6), 5 dup(8), 6 dup(0), 2, 1, 2 dup(0), 3
        db 2, 2 dup(0), 4, 3, 2 dup(0), 5, 4, 2 dup(0), 6, 5, 2 dup(0)
        db 7, 6, 2 dup(0), 8, 7, 2 dup(0), 9, 8, 2 dup(0), 0Ah
        db 9, 2 dup(0), 0Bh, 0Ah, 2 dup(0), 0Ch, 0Bh, 2 dup(0)
        db 0Dh, 0Ch, 2 dup(0), 0Eh, 0Dh, 2 dup(0), 0Fh, 0Eh, 4 dup(0Fh)
        db 4 dup(0), 3 dup(0Fh), 1, 2 dup(0Eh), 0, 2, 0, 8, 5
        db 3, 0, 0Bh, 6, 4, 3 dup(0Bh), 5, 0Eh, 2 dup(1), 6, 3 dup(9)
        db 2 dup(7), 9, 0Eh, 8, 2 dup(5), 0Dh, 9, 8, 4, 0, 0Ah
        db 0Bh, 2 dup(0), 0Bh, 0Fh, 9, 4, 0Ch, 3 dup(6), 0Dh, 3 dup(7)
        db 0Eh, 3 dup(8), 0Fh, 4 dup(0), 3 dup(0Fh), 1, 2 dup(0Eh)
        db 0, 2, 0, 8, 5, 3, 0, 0Bh, 6, 4, 3 dup(0Bh), 5, 0Eh
        db 2 dup(1), 6, 3 dup(9), 2 dup(7), 9, 0Eh, 8, 2 dup(5)
        db 0Dh, 9, 3 dup(3), 0Ah, 0Bh, 2 dup(0), 0Bh, 0Fh, 9, 4
        db 0Ch, 3 dup(6), 0Dh, 3 dup(7), 0Eh, 3 dup(8), 0Fh, 4 dup(0)
        db 3 dup(7), 1, 2 dup(7), 0, 2, 0, 4, 2, 3, 0, 5, 3, 4
        db 4 dup(5), 7, 2 dup(0), 6, 3 dup(4), 7, 3, 4, 7, 8, 2 dup(2)
        db 6, 9, 4, 2, 0, 0Ah, 5, 2 dup(0), 0Bh, 7, 4, 2, 0Ch
        db 3 dup(3), 0Dh, 3 dup(3), 0Eh, 3 dup(4), 0Fh, 80h dup(0)
        db 64h, 49h, 66h, 49h, 68h, 49h, 6Ah, 49h, 6Ch, 49h, 6Eh
        db 49h, 70h, 49h, 72h, 49h, 74h, 49h, 76h, 49h, 78h, 49h
        db 7Ah, 49h, 7Ch, 49h, 7Eh, 49h, 80h, 49h, 82h, 49h, 84h
        db 49h, 86h, 49h, 88h, 49h, 8Ah, 49h, 8Ch, 49h, 8Eh, 49h
        db 90h, 49h, 92h, 49h, 94h, 49h, 96h, 49h, 98h, 49h, 9Ah
        db 49h, 9Ch, 49h, 9Eh, 49h, 0A0h, 49h, 0A2h, 49h, 0A4h
        db 49h, 0A6h, 49h, 0A8h, 49h, 0AAh, 49h, 0ACh, 49h, 0AEh
        db 49h, 0B0h, 49h, 0B2h, 49h, 0B4h, 49h, 0B6h, 49h, 0B8h
        db 49h, 0BAh, 49h, 0BCh, 49h, 0BEh, 49h, 0C0h, 49h, 0C2h
        db 49h, 0C4h, 49h, 0C6h, 49h, 0C8h, 49h, 0CAh, 49h, 0CCh
        db 49h, 0CEh, 49h, 0D0h, 49h, 0D2h, 49h, 0D4h, 49h, 0D6h
        db 49h, 0D8h, 49h, 0DAh, 49h, 4, 51h, 6, 51h, 8, 51h, 0Ah
        db 51h, 0Ch, 51h, 0Eh, 51h, 10h, 51h, 12h, 51h, 14h, 51h
        db 16h, 51h, 18h, 51h, 1Ah, 51h, 1Ch, 51h, 1Eh, 51h, 20h
        db 51h, 22h, 51h, 24h, 51h, 26h, 51h, 28h, 51h, 2Ah, 51h
        db 2Ch, 51h, 2Eh, 51h, 30h, 51h, 32h, 51h, 34h, 51h, 36h
        db 51h, 38h, 51h, 3Ah, 51h, 3Ch, 51h, 3Eh, 51h, 40h, 51h
        db 42h, 51h, 44h, 51h, 46h, 51h, 48h, 51h, 4Ah, 51h, 4Ch
        db 51h, 4Eh, 51h, 50h, 51h, 52h, 51h, 54h, 51h, 56h, 51h
        db 58h, 51h, 5Ah, 51h, 5Ch, 51h, 5Eh, 51h, 60h, 51h, 62h
        db 51h, 64h, 51h, 66h, 51h, 68h, 51h, 6Ah, 51h, 6Ch, 51h
        db 6Eh, 51h, 70h, 51h, 72h, 51h, 74h, 51h, 76h, 51h, 78h
        db 51h, 7Ah, 51h, 0A4h, 58h, 0A6h, 58h, 0A8h, 58h, 0AAh
        db 58h, 0ACh, 58h, 0AEh, 58h, 0B0h, 58h, 0B2h, 58h, 0B4h
        db 58h, 0B6h, 58h, 0B8h, 58h, 0BAh, 58h, 0BCh, 58h, 0BEh
        db 58h, 0C0h, 58h, 0C2h, 58h, 0C4h, 58h, 0C6h, 58h, 0C8h
        db 58h, 0CAh, 58h, 0CCh, 58h, 0CEh, 58h, 0D0h, 58h, 0D2h
        db 58h, 0D4h, 58h, 0D6h, 58h, 0D8h, 58h, 0DAh, 58h, 0DCh
        db 58h, 0DEh, 58h, 0E0h, 58h, 0E2h, 58h, 0E4h, 58h, 0E6h
        db 58h, 0E8h, 58h, 0EAh, 58h, 0ECh, 58h, 0EEh, 58h, 0F0h
        db 58h, 0F2h, 58h, 0F4h, 58h, 0F6h, 58h, 0F8h, 58h, 0FAh
        db 58h, 0FCh, 58h, 0FEh, 58h, 0, 59h, 2, 59h, 4, 59h, 6
        db 59h, 8, 59h, 0Ah, 59h, 0Ch, 59h, 0Eh, 59h, 10h, 59h
        db 12h, 59h, 14h, 59h, 16h, 59h, 18h, 59h, 1Ah, 59h, 44h
        db 60h, 46h, 60h, 48h, 60h, 4Ah, 60h, 4Ch, 60h, 4Eh, 60h
        db 50h, 60h, 52h, 60h, 54h, 60h, 56h, 60h, 58h, 60h, 5Ah
        db 60h, 5Ch, 60h, 5Eh, 3 dup(60h), 62h, 60h, 64h, 60h
        db 66h, 60h, 68h, 60h, 6Ah, 60h, 6Ch, 60h, 6Eh, 60h, 70h
        db 60h, 72h, 60h, 74h, 60h, 76h, 60h, 78h, 60h, 7Ah, 60h
        db 7Ch, 60h, 7Eh, 60h, 80h, 60h, 82h, 60h, 84h, 60h, 86h
        db 60h, 88h, 60h, 8Ah, 60h, 8Ch, 60h, 8Eh, 60h, 90h, 60h
        db 92h, 60h, 94h, 60h, 96h, 60h, 98h, 60h, 9Ah, 60h, 9Ch
        db 60h, 9Eh, 60h, 0A0h, 60h, 0A2h, 60h, 0A4h, 60h, 0A6h
        db 60h, 0A8h, 60h, 0AAh, 60h, 0ACh, 60h, 0AEh, 60h, 0B0h
        db 60h, 0B2h, 60h, 0B4h, 60h, 0B6h, 60h, 0B8h, 60h, 0BAh
        db 60h, 0E4h, 67h, 0E6h, 67h, 0E8h, 67h, 0EAh, 67h, 0ECh
        db 67h, 0EEh, 67h, 0F0h, 67h, 0F2h, 67h, 0F4h, 67h, 0F6h
        db 67h, 0F8h, 67h, 0FAh, 67h, 0FCh, 67h, 0FEh, 67h, 0
        db 68h, 2, 68h, 4, 68h, 6, 68h, 8, 68h, 0Ah, 68h, 0Ch
        db 68h, 0Eh, 68h, 10h, 68h, 12h, 68h, 14h, 68h, 16h, 68h
        db 18h, 68h, 1Ah, 68h, 1Ch, 68h, 1Eh, 68h, 20h, 68h, 22h
        db 68h, 24h, 68h, 26h, 68h, 28h, 68h, 2Ah, 68h, 2Ch, 68h
        db 2Eh, 68h, 30h, 68h, 32h, 68h, 34h, 68h, 36h, 68h, 38h
        db 68h, 3Ah, 68h, 3Ch, 68h, 3Eh, 68h, 40h, 68h, 42h, 68h
        db 44h, 68h, 46h, 68h, 48h, 68h, 4Ah, 68h, 4Ch, 68h, 4Eh
        db 68h, 50h, 68h, 52h, 68h, 54h, 68h, 56h, 68h, 58h, 68h
        db 5Ah, 68h, 84h, 6Fh, 86h, 6Fh, 88h, 6Fh, 8Ah, 6Fh, 8Ch
        db 6Fh, 8Eh, 6Fh, 90h, 6Fh, 92h, 6Fh, 94h, 6Fh, 96h, 6Fh
        db 98h, 6Fh, 9Ah, 6Fh, 9Ch, 6Fh, 9Eh, 6Fh, 0A0h, 6Fh, 0A2h
        db 6Fh, 0A4h, 6Fh, 0A6h, 6Fh, 0A8h, 6Fh, 0AAh, 6Fh, 0ACh
        db 6Fh, 0AEh, 6Fh, 0B0h, 6Fh, 0B2h, 6Fh, 0B4h, 6Fh, 0B6h
        db 6Fh, 0B8h, 6Fh, 0BAh, 6Fh, 0BCh, 6Fh, 0BEh, 6Fh, 0C0h
        db 6Fh, 0C2h, 6Fh, 0C4h, 6Fh, 0C6h, 6Fh, 0C8h, 6Fh, 0CAh
        db 6Fh, 0CCh, 6Fh, 0CEh, 6Fh, 0D0h, 6Fh, 0D2h, 6Fh, 0D4h
        db 6Fh, 0D6h, 6Fh, 0D8h, 6Fh, 0DAh, 6Fh, 0DCh, 6Fh, 0DEh
        db 6Fh, 0E0h, 6Fh, 0E2h, 6Fh, 0E4h, 6Fh, 0E6h, 6Fh, 0E8h
        db 6Fh, 0EAh, 6Fh, 0ECh, 6Fh, 0EEh, 6Fh, 0F0h, 6Fh, 0F2h
        db 6Fh, 0F4h, 6Fh, 0F6h, 6Fh, 0F8h, 6Fh, 0FAh, 6Fh, 24h
        db 77h, 26h, 77h, 28h, 77h, 2Ah, 77h, 2Ch, 77h, 2Eh, 77h
        db 30h, 77h, 32h, 77h, 34h, 77h, 36h, 77h, 38h, 77h, 3Ah
        db 77h, 3Ch, 77h, 3Eh, 77h, 40h, 77h, 42h, 77h, 44h, 77h
        db 46h, 77h, 48h, 77h, 4Ah, 77h, 4Ch, 77h, 4Eh, 77h, 50h
        db 77h, 52h, 77h, 54h, 77h, 56h, 77h, 58h, 77h, 5Ah, 77h
        db 5Ch, 77h, 5Eh, 77h, 60h, 77h, 62h, 77h, 64h, 77h, 66h
        db 77h, 68h, 77h, 6Ah, 77h, 6Ch, 77h, 6Eh, 77h, 70h, 77h
        db 72h, 77h, 74h, 77h, 76h, 77h, 78h, 77h, 7Ah, 77h, 7Ch
        db 77h, 7Eh, 77h, 80h, 77h, 82h, 77h, 84h, 77h, 86h, 77h
        db 88h, 77h, 8Ah, 77h, 8Ch, 77h, 8Eh, 77h, 90h, 77h, 92h
        db 77h, 94h, 77h, 96h, 77h, 98h, 77h, 9Ah, 77h, 0C4h, 7Eh
        db 0C6h, 7Eh, 0C8h, 7Eh, 0CAh, 7Eh, 0CCh, 7Eh, 0CEh, 7Eh
        db 0D0h, 7Eh, 0D2h, 7Eh, 0D4h, 7Eh, 0D6h, 7Eh, 0D8h, 7Eh
        db 0DAh, 7Eh, 0DCh, 7Eh, 0DEh, 7Eh, 0E0h, 7Eh, 0E2h, 7Eh
        db 0E4h, 7Eh, 0E6h, 7Eh, 0E8h, 7Eh, 0EAh, 7Eh, 0ECh, 7Eh
        db 0EEh, 7Eh, 0F0h, 7Eh, 0F2h, 7Eh, 0F4h, 7Eh, 0F6h, 7Eh
        db 0F8h, 7Eh, 0FAh, 7Eh, 0FCh, 7Eh, 0FEh, 7Eh, 0, 7Fh
        db 2, 7Fh, 4, 7Fh, 6, 7Fh, 8, 7Fh, 0Ah, 7Fh, 0Ch, 7Fh
        db 0Eh, 7Fh, 10h, 7Fh, 12h, 7Fh, 14h, 7Fh, 16h, 7Fh, 18h
        db 7Fh, 1Ah, 7Fh, 1Ch, 7Fh, 1Eh, 7Fh, 20h, 7Fh, 22h, 7Fh
        db 24h, 7Fh, 26h, 7Fh, 28h, 7Fh, 2Ah, 7Fh, 2Ch, 7Fh, 2Eh
        db 7Fh, 30h, 7Fh, 32h, 7Fh, 34h, 7Fh, 36h, 7Fh, 38h, 7Fh
        db 3Ah, 7Fh, 64h, 86h, 66h, 86h, 68h, 86h, 6Ah, 86h, 6Ch
        db 86h, 6Eh, 86h, 70h, 86h, 72h, 86h, 74h, 86h, 76h, 86h
        db 78h, 86h, 7Ah, 86h, 7Ch, 86h, 7Eh, 86h, 80h, 86h, 82h
        db 86h, 84h, 3 dup(86h), 88h, 86h, 8Ah, 86h, 8Ch, 86h
        db 8Eh, 86h, 90h, 86h, 92h, 86h, 94h, 86h, 96h, 86h, 98h
        db 86h, 9Ah, 86h, 9Ch, 86h, 9Eh, 86h, 0A0h, 86h, 0A2h
        db 86h, 0A4h, 86h, 0A6h, 86h, 0A8h, 86h, 0AAh, 86h, 0ACh
        db 86h, 0AEh, 86h, 0B0h, 86h, 0B2h, 86h, 0B4h, 86h, 0B6h
        db 86h, 0B8h, 86h, 0BAh, 86h, 0BCh, 86h, 0BEh, 86h, 0C0h
        db 86h, 0C2h, 86h, 0C4h, 86h, 0C6h, 86h, 0C8h, 86h, 0CAh
        db 86h, 0CCh, 86h, 0CEh, 86h, 0D0h, 86h, 0D2h, 86h, 0D4h
        db 86h, 0D6h, 86h, 0D8h, 86h, 0DAh, 86h, 4, 8Eh, 6, 8Eh
        db 8, 8Eh, 0Ah, 8Eh, 0Ch, 8Eh, 0Eh, 8Eh, 10h, 8Eh, 12h
        db 8Eh, 14h, 8Eh, 16h, 8Eh, 18h, 8Eh, 1Ah, 8Eh, 1Ch, 8Eh
        db 1Eh, 8Eh, 20h, 8Eh, 22h, 8Eh, 24h, 8Eh, 26h, 8Eh, 28h
        db 8Eh, 2Ah, 8Eh, 2Ch, 8Eh, 2Eh, 8Eh, 30h, 8Eh, 32h, 8Eh
        db 34h, 8Eh, 36h, 8Eh, 38h, 8Eh, 3Ah, 8Eh, 3Ch, 8Eh, 3Eh
        db 8Eh, 40h, 8Eh, 42h, 8Eh, 44h, 8Eh, 46h, 8Eh, 48h, 8Eh
        db 4Ah, 8Eh, 4Ch, 8Eh, 4Eh, 8Eh, 50h, 8Eh, 52h, 8Eh, 54h
        db 8Eh, 56h, 8Eh, 58h, 8Eh, 5Ah, 8Eh, 5Ch, 8Eh, 5Eh, 8Eh
        db 60h, 8Eh, 62h, 8Eh, 64h, 8Eh, 66h, 8Eh, 68h, 8Eh, 6Ah
        db 8Eh, 6Ch, 8Eh, 6Eh, 8Eh, 70h, 8Eh, 72h, 8Eh, 74h, 8Eh
        db 76h, 8Eh, 78h, 8Eh, 7Ah, 8Eh, 0A4h, 95h, 0A6h, 95h
        db 0A8h, 95h, 0AAh, 95h, 0ACh, 95h, 0AEh, 95h, 0B0h, 95h
        db 0B2h, 95h, 0B4h, 95h, 0B6h, 95h, 0B8h, 95h, 0BAh, 95h
        db 0BCh, 95h, 0BEh, 95h, 0C0h, 95h, 0C2h, 95h, 0C4h, 95h
        db 0C6h, 95h, 0C8h, 95h, 0CAh, 95h, 0CCh, 95h, 0CEh, 95h
        db 0D0h, 95h, 0D2h, 95h, 0D4h, 95h, 0D6h, 95h, 0D8h, 95h
        db 0DAh, 95h, 0DCh, 95h, 0DEh, 95h, 0E0h, 95h, 0E2h, 95h
        db 0E4h, 95h, 0E6h, 95h, 0E8h, 95h, 0EAh, 95h, 0ECh, 95h
        db 0EEh, 95h, 0F0h, 95h, 0F2h, 95h, 0F4h, 95h, 0F6h, 95h
        db 0F8h, 95h, 0FAh, 95h, 0FCh, 95h, 0FEh, 95h, 0, 96h
        db 2, 96h, 4, 96h, 6, 96h, 8, 96h, 0Ah, 96h, 0Ch, 96h
        db 0Eh, 96h, 10h, 96h, 12h, 96h, 14h, 96h, 16h, 96h, 18h
        db 96h, 1Ah, 96h, 44h, 9Dh, 46h, 9Dh, 48h, 9Dh, 4Ah, 9Dh
        db 4Ch, 9Dh, 4Eh, 9Dh, 50h, 9Dh, 52h, 9Dh, 54h, 9Dh, 56h
        db 9Dh, 58h, 9Dh, 5Ah, 9Dh, 5Ch, 9Dh, 5Eh, 9Dh, 60h, 9Dh
        db 62h, 9Dh, 64h, 9Dh, 66h, 9Dh, 68h, 9Dh, 6Ah, 9Dh, 6Ch
        db 9Dh, 6Eh, 9Dh, 70h, 9Dh, 72h, 9Dh, 74h, 9Dh, 76h, 9Dh
        db 78h, 9Dh, 7Ah, 9Dh, 7Ch, 9Dh, 7Eh, 9Dh, 80h, 9Dh, 82h
        db 9Dh, 84h, 9Dh, 86h, 9Dh, 88h, 9Dh, 8Ah, 9Dh, 8Ch, 9Dh
        db 8Eh, 9Dh, 90h, 9Dh, 92h, 9Dh, 94h, 9Dh, 96h, 9Dh, 98h
        db 9Dh, 9Ah, 9Dh, 9Ch, 9Dh, 9Eh, 9Dh, 0A0h, 9Dh, 0A2h
        db 9Dh, 0A4h, 9Dh, 0A6h, 9Dh, 0A8h, 9Dh, 0AAh, 9Dh, 0ACh
        db 9Dh, 0AEh, 9Dh, 0B0h, 9Dh, 0B2h, 9Dh, 0B4h, 9Dh, 0B6h
        db 9Dh, 0B8h, 9Dh, 0BAh, 9Dh, 0E4h, 0A4h, 0E6h, 0A4h, 0E8h
        db 0A4h, 0EAh, 0A4h, 0ECh, 0A4h, 0EEh, 0A4h, 0F0h, 0A4h
        db 0F2h, 0A4h, 0F4h, 0A4h, 0F6h, 0A4h, 0F8h, 0A4h, 0FAh
        db 0A4h, 0FCh, 0A4h, 0FEh, 0A4h, 0, 0A5h, 2, 0A5h, 4, 0A5h
        db 6, 0A5h, 8, 0A5h, 0Ah, 0A5h, 0Ch, 0A5h, 0Eh, 0A5h, 10h
        db 0A5h, 12h, 0A5h, 14h, 0A5h, 16h, 0A5h, 18h, 0A5h, 1Ah
        db 0A5h, 1Ch, 0A5h, 1Eh, 0A5h, 20h, 0A5h, 22h, 0A5h, 24h
        db 0A5h, 26h, 0A5h, 28h, 0A5h, 2Ah, 0A5h, 2Ch, 0A5h, 2Eh
        db 0A5h, 30h, 0A5h, 32h, 0A5h, 34h, 0A5h, 36h, 0A5h, 38h
        db 0A5h, 3Ah, 0A5h, 3Ch, 0A5h, 3Eh, 0A5h, 40h, 0A5h, 42h
        db 0A5h, 44h, 0A5h, 46h, 0A5h, 48h, 0A5h, 4Ah, 0A5h, 4Ch
        db 0A5h, 4Eh, 0A5h, 50h, 0A5h, 52h, 0A5h, 54h, 0A5h, 56h
        db 0A5h, 58h, 0A5h, 5Ah, 0A5h, 84h, 0ACh, 86h, 0ACh, 88h
        db 0ACh, 8Ah, 0ACh, 8Ch, 0ACh, 8Eh, 0ACh, 90h, 0ACh, 92h
        db 0ACh, 94h, 0ACh, 96h, 0ACh, 98h, 0ACh, 9Ah, 0ACh, 9Ch
        db 0ACh, 9Eh, 0ACh, 0A0h, 0ACh, 0A2h, 0ACh, 0A4h, 0ACh
        db 0A6h, 0ACh, 0A8h, 0ACh, 0AAh, 3 dup(0ACh), 0AEh, 0ACh
        db 0B0h, 0ACh, 0B2h, 0ACh, 0B4h, 0ACh, 0B6h, 0ACh, 0B8h
        db 0ACh, 0BAh, 0ACh, 0BCh, 0ACh, 0BEh, 0ACh, 0C0h, 0ACh
        db 0C2h, 0ACh, 0C4h, 0ACh, 0C6h, 0ACh, 0C8h, 0ACh, 0CAh
        db 0ACh, 0CCh, 0ACh, 0CEh, 0ACh, 0D0h, 0ACh, 0D2h, 0ACh
        db 0D4h, 0ACh, 0D6h, 0ACh, 0D8h, 0ACh, 0DAh, 0ACh, 0DCh
        db 0ACh, 0DEh, 0ACh, 0E0h, 0ACh, 0E2h, 0ACh, 0E4h, 0ACh
        db 0E6h, 0ACh, 0E8h, 0ACh, 0EAh, 0ACh, 0ECh, 0ACh, 0EEh
        db 0ACh, 0F0h, 0ACh, 0F2h, 0ACh, 0F4h, 0ACh, 0F6h, 0ACh
        db 0F8h, 0ACh, 0FAh, 0ACh, 24h, 0B4h, 26h, 0B4h, 28h, 0B4h
        db 2Ah, 0B4h, 2Ch, 0B4h, 2Eh, 0B4h, 30h, 0B4h, 32h, 0B4h
        db 34h, 0B4h, 36h, 0B4h, 38h, 0B4h, 3Ah, 0B4h, 3Ch, 0B4h
        db 3Eh, 0B4h, 40h, 0B4h, 42h, 0B4h, 44h, 0B4h, 46h, 0B4h
        db 48h, 0B4h, 4Ah, 0B4h, 4Ch, 0B4h, 4Eh, 0B4h, 50h, 0B4h
        db 52h, 0B4h, 54h, 0B4h, 56h, 0B4h, 58h, 0B4h, 5Ah, 0B4h
        db 5Ch, 0B4h, 5Eh, 0B4h, 60h, 0B4h, 62h, 0B4h, 64h, 0B4h
        db 66h, 0B4h, 68h, 0B4h, 6Ah, 0B4h, 6Ch, 0B4h, 6Eh, 0B4h
        db 70h, 0B4h, 72h, 0B4h, 74h, 0B4h, 76h, 0B4h, 78h, 0B4h
        db 7Ah, 0B4h, 7Ch, 0B4h, 7Eh, 0B4h, 80h, 0B4h, 82h, 0B4h
        db 84h, 0B4h, 86h, 0B4h, 88h, 0B4h, 8Ah, 0B4h, 8Ch, 0B4h
        db 8Eh, 0B4h, 90h, 0B4h, 92h, 0B4h, 94h, 0B4h, 96h, 0B4h
        db 98h, 0B4h, 9Ah, 0B4h, 0C4h, 0BBh, 0C6h, 0BBh, 0C8h
        db 0BBh, 0CAh, 0BBh, 0CCh, 0BBh, 0CEh, 0BBh, 0D0h, 0BBh
        db 0D2h, 0BBh, 0D4h, 0BBh, 0D6h, 0BBh, 0D8h, 0BBh, 0DAh
        db 0BBh, 0DCh, 0BBh, 0DEh, 0BBh, 0E0h, 0BBh, 0E2h, 0BBh
        db 0E4h, 0BBh, 0E6h, 0BBh, 0E8h, 0BBh, 0EAh, 0BBh, 0ECh
        db 0BBh, 0EEh, 0BBh, 0F0h, 0BBh, 0F2h, 0BBh, 0F4h, 0BBh
        db 0F6h, 0BBh, 0F8h, 0BBh, 0FAh, 0BBh, 0FCh, 0BBh, 0FEh
        db 0BBh, 0, 0BCh, 2, 0BCh, 4, 0BCh, 6, 0BCh, 8, 0BCh, 0Ah
        db 0BCh, 0Ch, 0BCh, 0Eh, 0BCh, 10h, 0BCh, 12h, 0BCh, 14h
        db 0BCh, 16h, 0BCh, 18h, 0BCh, 1Ah, 0BCh, 1Ch, 0BCh, 1Eh
        db 0BCh, 20h, 0BCh, 22h, 0BCh, 24h, 0BCh, 26h, 0BCh, 28h
        db 0BCh, 2Ah, 0BCh, 2Ch, 0BCh, 2Eh, 0BCh, 30h, 0BCh, 32h
        db 0BCh, 34h, 0BCh, 36h, 0BCh, 38h, 0BCh, 3Ah, 0BCh, 64h
        db 0C3h, 66h, 0C3h, 68h, 0C3h, 6Ah, 0C3h, 6Ch, 0C3h, 6Eh
        db 0C3h, 70h, 0C3h, 72h, 0C3h, 74h, 0C3h, 76h, 0C3h, 78h
        db 0C3h, 7Ah, 0C3h, 7Ch, 0C3h, 7Eh, 0C3h, 80h, 0C3h, 82h
        db 0C3h, 84h, 0C3h, 86h, 0C3h, 88h, 0C3h, 8Ah, 0C3h, 8Ch
        db 0C3h, 8Eh, 0C3h, 90h, 0C3h, 92h, 0C3h, 94h, 0C3h, 96h
        db 0C3h, 98h, 0C3h, 9Ah, 0C3h, 9Ch, 0C3h, 9Eh, 0C3h, 0A0h
        db 0C3h, 0A2h, 0C3h, 0A4h, 0C3h, 0A6h, 0C3h, 0A8h, 0C3h
        db 0AAh, 0C3h, 0ACh, 0C3h, 0AEh, 0C3h, 0B0h, 0C3h, 0B2h
        db 0C3h, 0B4h, 0C3h, 0B6h, 0C3h, 0B8h, 0C3h, 0BAh, 0C3h
        db 0BCh, 0C3h, 0BEh, 0C3h, 0C0h, 0C3h, 0C2h, 0C3h, 0C4h
        db 0C3h, 0C6h, 0C3h, 0C8h, 0C3h, 0CAh, 0C3h, 0CCh, 0C3h
        db 0CEh, 0C3h, 0D0h, 0C3h, 0D2h, 0C3h, 0D4h, 0C3h, 0D6h
        db 0C3h, 0D8h, 0C3h, 0DAh, 0C3h, 4, 0CBh, 6, 0CBh, 8, 0CBh
        db 0Ah, 0CBh, 0Ch, 0CBh, 0Eh, 0CBh, 10h, 0CBh, 12h, 0CBh
        db 14h, 0CBh, 16h, 0CBh, 18h, 0CBh, 1Ah, 0CBh, 1Ch, 0CBh
        db 1Eh, 0CBh, 20h, 0CBh, 22h, 0CBh, 24h, 0CBh, 26h, 0CBh
        db 28h, 0CBh, 2Ah, 0CBh, 2Ch, 0CBh, 2Eh, 0CBh, 30h, 0CBh
        db 32h, 0CBh, 34h, 0CBh, 36h, 0CBh, 38h, 0CBh, 3Ah, 0CBh
        db 3Ch, 0CBh, 3Eh, 0CBh, 40h, 0CBh, 42h, 0CBh, 44h, 0CBh
        db 46h, 0CBh, 48h, 0CBh, 4Ah, 0CBh, 4Ch, 0CBh, 4Eh, 0CBh
        db 50h, 0CBh, 52h, 0CBh, 54h, 0CBh, 56h, 0CBh, 58h, 0CBh
        db 5Ah, 0CBh, 5Ch, 0CBh, 5Eh, 0CBh, 60h, 0CBh, 62h, 0CBh
        db 64h, 0CBh, 66h, 0CBh, 68h, 0CBh, 6Ah, 0CBh, 6Ch, 0CBh
        db 6Eh, 0CBh, 70h, 0CBh, 72h, 0CBh, 74h, 0CBh, 76h, 0CBh
        db 78h, 0CBh, 7Ah, 0CBh, 0A4h, 0D2h, 0A6h, 0D2h, 0A8h
        db 0D2h, 0AAh, 0D2h, 0ACh, 0D2h, 0AEh, 0D2h, 0B0h, 0D2h
        db 0B2h, 0D2h, 0B4h, 0D2h, 0B6h, 0D2h, 0B8h, 0D2h, 0BAh
        db 0D2h, 0BCh, 0D2h, 0BEh, 0D2h, 0C0h, 0D2h, 0C2h, 0D2h
        db 0C4h, 0D2h, 0C6h, 0D2h, 0C8h, 0D2h, 0CAh, 0D2h, 0CCh
        db 0D2h, 0CEh, 0D2h, 0D0h, 3 dup(0D2h), 0D4h, 0D2h, 0D6h
        db 0D2h, 0D8h, 0D2h, 0DAh, 0D2h, 0DCh, 0D2h, 0DEh, 0D2h
        db 0E0h, 0D2h, 0E2h, 0D2h, 0E4h, 0D2h, 0E6h, 0D2h, 0E8h
        db 0D2h, 0EAh, 0D2h, 0ECh, 0D2h, 0EEh, 0D2h, 0F0h, 0D2h
        db 0F2h, 0D2h, 0F4h, 0D2h, 0F6h, 0D2h, 0F8h, 0D2h, 0FAh
        db 0D2h, 0FCh, 0D2h, 0FEh, 0D2h, 0, 0D3h, 2, 0D3h, 4, 0D3h
        db 6, 0D3h, 8, 0D3h, 0Ah, 0D3h, 0Ch, 0D3h, 0Eh, 0D3h, 10h
        db 0D3h, 12h, 0D3h, 14h, 0D3h, 16h, 0D3h, 18h, 0D3h, 1Ah
        db 0D3h, 44h, 0DAh, 46h, 0DAh, 48h, 0DAh, 4Ah, 0DAh, 4Ch
        db 0DAh, 4Eh, 0DAh, 50h, 0DAh, 52h, 0DAh, 54h, 0DAh, 56h
        db 0DAh, 58h, 0DAh, 5Ah, 0DAh, 5Ch, 0DAh, 5Eh, 0DAh, 60h
        db 0DAh, 62h, 0DAh, 64h, 0DAh, 66h, 0DAh, 68h, 0DAh, 6Ah
        db 0DAh, 6Ch, 0DAh, 6Eh, 0DAh, 70h, 0DAh, 72h, 0DAh, 74h
        db 0DAh, 76h, 0DAh, 78h, 0DAh, 7Ah, 0DAh, 7Ch, 0DAh, 7Eh
        db 0DAh, 80h, 0DAh, 82h, 0DAh, 84h, 0DAh, 86h, 0DAh, 88h
        db 0DAh, 8Ah, 0DAh, 8Ch, 0DAh, 8Eh, 0DAh, 90h, 0DAh, 92h
        db 0DAh, 94h, 0DAh, 96h, 0DAh, 98h, 0DAh, 9Ah, 0DAh, 9Ch
        db 0DAh, 9Eh, 0DAh, 0A0h, 0DAh, 0A2h, 0DAh, 0A4h, 0DAh
        db 0A6h, 0DAh, 0A8h, 0DAh, 0AAh, 0DAh, 0ACh, 0DAh, 0AEh
        db 0DAh, 0B0h, 0DAh, 0B2h, 0DAh, 0B4h, 0DAh, 0B6h, 0DAh
        db 0B8h, 0DAh, 0BAh, 0DAh, 0E4h, 0E1h, 0E6h, 0E1h, 0E8h
        db 0E1h, 0EAh, 0E1h, 0ECh, 0E1h, 0EEh, 0E1h, 0F0h, 0E1h
        db 0F2h, 0E1h, 0F4h, 0E1h, 0F6h, 0E1h, 0F8h, 0E1h, 0FAh
        db 0E1h, 0FCh, 0E1h, 0FEh, 0E1h, 0, 0E2h, 2, 0E2h, 4, 0E2h
        db 6, 0E2h, 8, 0E2h, 0Ah, 0E2h, 0Ch, 0E2h, 0Eh, 0E2h, 10h
        db 0E2h, 12h, 0E2h, 14h, 0E2h, 16h, 0E2h, 18h, 0E2h, 1Ah
        db 0E2h, 1Ch, 0E2h, 1Eh, 0E2h, 20h, 0E2h, 22h, 0E2h, 24h
        db 0E2h, 26h, 0E2h, 28h, 0E2h, 2Ah, 0E2h, 2Ch, 0E2h, 2Eh
        db 0E2h, 30h, 0E2h, 32h, 0E2h, 34h, 0E2h, 36h, 0E2h, 38h
        db 0E2h, 3Ah, 0E2h, 3Ch, 0E2h, 3Eh, 0E2h, 40h, 0E2h, 42h
        db 0E2h, 44h, 0E2h, 46h, 0E2h, 48h, 0E2h, 4Ah, 0E2h, 4Ch
        db 0E2h, 4Eh, 0E2h, 50h, 0E2h, 52h, 0E2h, 54h, 0E2h, 56h
        db 0E2h, 58h, 0E2h, 5Ah, 0E2h, 84h, 0E9h, 86h, 0E9h, 88h
        db 0E9h, 8Ah, 0E9h, 8Ch, 0E9h, 8Eh, 0E9h, 90h, 0E9h, 92h
        db 0E9h, 94h, 0E9h, 96h, 0E9h, 98h, 0E9h, 9Ah, 0E9h, 9Ch
        db 0E9h, 9Eh, 0E9h, 0A0h, 0E9h, 0A2h, 0E9h, 0A4h, 0E9h
        db 0A6h, 0E9h, 0A8h, 0E9h, 0AAh, 0E9h, 0ACh, 0E9h, 0AEh
        db 0E9h, 0B0h, 0E9h, 0B2h, 0E9h, 0B4h, 0E9h, 0B6h, 0E9h
        db 0B8h, 0E9h, 0BAh, 0E9h, 0BCh, 0E9h, 0BEh, 0E9h, 0C0h
        db 0E9h, 0C2h, 0E9h, 0C4h, 0E9h, 0C6h, 0E9h, 0C8h, 0E9h
        db 0CAh, 0E9h, 0CCh, 0E9h, 0CEh, 0E9h, 0D0h, 0E9h, 0D2h
        db 0E9h, 0D4h, 0E9h, 0D6h, 0E9h, 0D8h, 0E9h, 0DAh, 0E9h
        db 0DCh, 0E9h, 0DEh, 0E9h, 0E0h, 0E9h, 0E2h, 0E9h, 0E4h
        db 0E9h, 0E6h, 0E9h, 0E8h, 0E9h, 0EAh, 0E9h, 0ECh, 0E9h
        db 0EEh, 0E9h, 0F0h, 0E9h, 0F2h, 0E9h, 0F4h, 0E9h, 0F6h
        db 0E9h, 0F8h, 0E9h, 0FAh, 0E9h, 24h, 0F1h, 26h, 0F1h
        db 28h, 0F1h, 2Ah, 0F1h, 2Ch, 0F1h, 2Eh, 0F1h, 30h, 0F1h
        db 32h, 0F1h, 34h, 0F1h, 36h, 0F1h, 38h, 0F1h, 3Ah, 0F1h
        db 3Ch, 0F1h, 3Eh, 0F1h, 40h, 0F1h, 42h, 0F1h, 44h, 0F1h
        db 46h, 0F1h, 48h, 0F1h, 4Ah, 0F1h, 4Ch, 0F1h, 4Eh, 0F1h
        db 50h, 0F1h, 52h, 0F1h, 54h, 0F1h, 56h, 0F1h, 58h, 0F1h
        db 5Ah, 0F1h, 5Ch, 0F1h, 5Eh, 0F1h, 60h, 0F1h, 62h, 0F1h
        db 64h, 0F1h, 66h, 0F1h, 68h, 0F1h, 6Ah, 0F1h, 6Ch, 0F1h
        db 6Eh, 0F1h, 70h, 0F1h, 72h, 0F1h, 74h, 0F1h, 76h, 0F1h
        db 78h, 0F1h, 7Ah, 0F1h, 7Ch, 0F1h, 7Eh, 0F1h, 80h, 0F1h
        db 82h, 0F1h, 84h, 0F1h, 86h, 0F1h, 88h, 0F1h, 8Ah, 0F1h
        db 8Ch, 0F1h, 8Eh, 0F1h, 90h, 0F1h, 92h, 0F1h, 94h, 0F1h
        db 96h, 0F1h, 98h, 0F1h, 9Ah, 0F1h, 0C4h, 0F8h, 0C6h, 0F8h
        db 0C8h, 0F8h, 0CAh, 0F8h, 0CCh, 0F8h, 0CEh, 0F8h, 0D0h
        db 0F8h, 0D2h, 0F8h, 0D4h, 0F8h, 0D6h, 0F8h, 0D8h, 0F8h
        db 0DAh, 0F8h, 0DCh, 0F8h, 0DEh, 0F8h, 0E0h, 0F8h, 0E2h
        db 0F8h, 0E4h, 0F8h, 0E6h, 0F8h, 0E8h, 0F8h, 0EAh, 0F8h
        db 0ECh, 0F8h, 0EEh, 0F8h, 0F0h, 0F8h, 0F2h, 0F8h, 0F4h
        db 0F8h, 0F6h, 3 dup(0F8h), 0FAh, 0F8h, 0FCh, 0F8h, 0FEh
        db 0F8h, 0, 0F9h, 2, 0F9h, 4, 0F9h, 6, 0F9h, 8, 0F9h, 0Ah
        db 0F9h, 0Ch, 0F9h, 0Eh, 0F9h, 10h, 0F9h, 12h, 0F9h, 14h
        db 0F9h, 16h, 0F9h, 18h, 0F9h, 1Ah, 0F9h, 1Ch, 0F9h, 1Eh
        db 0F9h, 20h, 0F9h, 22h, 0F9h, 24h, 0F9h, 26h, 0F9h, 28h
        db 0F9h, 2Ah, 0F9h, 2Ch, 0F9h, 2Eh, 0F9h, 30h, 0F9h, 32h
        db 0F9h, 34h, 0F9h, 36h, 0F9h, 38h, 0F9h, 3Ah, 0F9h, 2 dup(0)
        db 7Ah, 0, 0F4h, 0, 6Eh, 1, 0E8h, 1, 62h, 2, 0DCh, 2, 56h
        db 3, 0D0h, 3, 4Ah, 4, 0C4h, 4, 3Eh, 5, 0B8h, 5, 32h, 6
        db 0ACh, 6, 26h, 7, 0A0h, 7, 1Ah, 8
timeOfDay   dw 0            ; DATA XREF: sub_492F1+13r getTime+7w ...
movingObjects?  db 1498h dup(0)     ; DATA XREF: gameloop?:loc_49E33o
                    ; gameloop?+8Do
word_58463  dw 0            ; DATA XREF: code:5B6Dw
word_58465  dw 0            ; DATA XREF: runMainMenu+10w
                    ; runMainMenu:loc_4C7FDw ...
word_58467  dw 0            ; DATA XREF: start+315w start+40Aw ...
word_58469  dw 0            ; DATA XREF: sub_4B72B+12r
                    ; sub_4B72B+23r ...
word_5846B  dw 0            ; DATA XREF: sub_4B72B+16r
                    ; sub_4B72B+20w ...
word_5846D  dw 0            ; DATA XREF: sub_4B671+12r
                    ; sub_4B671+23r ...
word_5846F  dw 0            ; DATA XREF: sub_4B671+16r
                    ; sub_4B671+20w ...
word_58471  dw 0            ; DATA XREF: code:4654r code:4668r ...
word_58473  dw 0            ; DATA XREF: code:4658r code:4665w ...
word_58475  dw 0            ; DATA XREF: sub_4AB1B+5Ew
                    ; sub_4AB1B+97r ...
dword_58477 dd 0            ; DATA XREF: sub_4AD0E+14w
                    ; sub_4AD0E+A8r
word_5847B  dw 0            ; DATA XREF: sub_4B85C+22w
                    ; sub_4B899+Dr ...
word_5847D  dw 0            ; DATA XREF: sub_4AD0E+6Cw
                    ; sub_4AD0E+81r ...
word_5847F  dw 0            ; DATA XREF: sub_4AD0E+70w
                    ; sub_4AD0E+8Ar ...
word_58481  dw 0            ; DATA XREF: sub_4D2E9:loc_4D33Bw
                    ; sub_4D34F+55w ...
word_58483  dw 0            ; DATA XREF: sub_4AD0E+74w
                    ; sub_4AD0E+8Er ...
word_58485  dw 0            ; DATA XREF: sub_4D2E9+58w
                    ; sub_4D34F+3Bw ...
byte_58487  db 0            ; DATA XREF: sub_4D2E9w
                    ; sub_4D2E9:loc_4D310w ...
dword_58488 dd 20000001h        ; DATA XREF: sub_4D2E9+5Ew
                    ; sub_4D34F+2Ar ...
        db  20h
aWelcomeToSupap db 'WELCOME TO SUPAPLEX  ',0
aYourName   db 'YOUR NAME:             ',0
aInvalidName    db 'INVALID NAME           ',0
aPlayerExists   db 'PLAYER EXISTS          ',0
aPlayerListFull db 'PLAYER LIST FULL       ',0
aMax3SkipsPossi db 'MAX 3 SKIPS POSSIBLE   ',0
a????????   db '????????',0
aGoliath    db ' GOLIATH',0
        db '        ',0
        db '                       ',0
aOhOnYourNot    db 'OH ON YOUR NOT         ',0
aWelcomeOhSmall db 'WELCOME OH SMALL ONE   ',0
aSkipNotPossibl db 'SKIP NOT POSSIBLE      ',0
aDelete???  db 'DELETE ',27h,'        ',27h,' ???  ',0
aSkipLevel???   db 'SKIP LEVEL     ???     ',0
aNoPlayerSelect db 'NO PLAYER SELECTED     ',0
aColorblindIGue db 'COLORBLIND I GUESS     ',0
a000Goliath0000 db '000 GOLIATH  000:00:00',0
a0000000000 db '000 -------- 000:00:00',0
a0000000000_0   db '000 -------- 000:00:00',0
a0000000000_1   db '000 -------- 000:00:00',0
a0000000000_2   db '000 -------- 000:00:00',0
        db '   ',0
a0000000    db '000:00:00',0
a000        db '000',0
aNotYouSorry    db ' NOT YOU SORRY    ',0
        db '                  ',0
        db '                  ',0
aSupaplexByDr_0 db 'SUPAPLEX  BY DREAM FACTORY',0
aCDigitalInte_0 db '(C) DIGITAL INTEGRATION LTD 1991',0
        db  5Fh ; _
        db  5Fh ; _
        db  5Fh ; _
        db  5Fh ; _
        db  5Fh ; _
        db  5Fh ; _
        db  5Fh ; _
        db  5Fh ; _
        db  5Fh ; _
        db  5Fh ; _
        db  5Fh ; _
word_586FB  dw 5F5Fh        ; DATA XREF: sub_4AF0C+35w
                    ; sub_4AF0C+67w
word_586FD  dw 5F5Fh        ; DATA XREF: sub_4AF0C+38w
                    ; sub_4AF0C+6Dw
word_586FF  dw 5F5Fh        ; DATA XREF: sub_4AF0C+3Bw
                    ; sub_4AF0C+73w
word_58701  dw 5F5Fh        ; DATA XREF: sub_4AF0C+3Ew
                    ; sub_4AF0C+79w
word_58703  dw 5F5Fh        ; DATA XREF: sub_4AF0C+41w
                    ; sub_4AF0C+7Cw
        dw 5F5Fh
        dw 5F5Fh
        dw 5F5Fh
        dw 5F5Fh
word_5870D  dw 5F5Fh        ; DATA XREF: sub_4AF0C+44w
                    ; sub_4AF0C+82w
byte_5870F  db 5Fh          ; DATA XREF: sub_4AF0C+47w
                    ; sub_4AF0C+C8w
word_58710  dw 5F5Fh        ; DATA XREF: sub_4AF0C+4Aw
                    ; sub_4AF0C+D4w
word_58712  dw 5F5Fh        ; DATA XREF: sub_4AF0C+4Dw
                    ; sub_4AF0C+88w
word_58714  dw 5F5Fh        ; DATA XREF: sub_4AF0C+50w
                    ; sub_4AF0C+8Ew
a__________ db '__________',0
aSupaplexPlayer db 'SUPAPLEX PLAYER STATISTICS',0
aCurrentPlayer_ db 'CURRENT PLAYER :  ........',0
aCurrentLevel__ db 'CURRENT LEVEL  :       ...',0
aUsedTime______ db 'USED TIME      : ...:..:..',0
aAverageTimeUse db 'AVERAGE TIME USED PER LEVEL : 00.0 MINUTES',0
aStillUnderOneM db 'STILL UNDER ONE MINUTE (KEEP IT UP...)',0
aYouVeCompleted db 'YOU',27h,'VE COMPLETED ALL LEVELS! CONGRATULATIONS!!!',0
aInsertDataDisk db 'INSERT DATA DISK INTO DRIVE',0
aPressTheDriveL db 'PRESS THE DRIVE LETTER (A/B) ?',0
aPressEnter db 'PRESS ENTER',0
aInsertGameDisk db 'INSERT GAME DISK INTO DRIVE',0
aPressEscapeToQ db 'PRESS ESCAPE TO QUIT',0
aHardLuck   db 'HARD LUCK!',0
aYouCompletedAl db 'YOU COMPLETED ALL THE NECESSARY INFOTRONS',0
aButFailedToRea db 'BUT FAILED TO REACH THE EXIT',0
aYouHaveCollect db 'YOU HAVE COLLECTED ??? OUT OF THE ???',0
aInfotronsNeede db 'INFOTRONS NEEDED',0
aWhyNotGiveItAn db 'WHY NOT GIVE IT ANOTHER TRY?',0
aSupaplexByDrea db 'SUPAPLEX  BY DREAM FACTORY',0
aOriginalDesign db 'ORIGINAL DESIGN BY PHILIP JESPERSEN',0
aAndMichaelStop db 'AND MICHAEL STOPP',0
aNearlyAllLevel db 'NEARLY ALL LEVELS BY MICHEAL STOPP',0
aAFewLevelsByPh db 'A FEW LEVELS BY PHILIP JESPERSEN',0
aHardlyAnyLevel db 'HARDLY ANY LEVELS BY BARBARA STOPP',0
aNotePressEnter db 'NOTE: PRESS ENTER TO REMOVE PANEL',0
aCDigitalIntegr db '(C) DIGITAL INTEGRATION LTD 1991',0
aCongratulation db 'CONGRATULATIONS',0
aYouHaveComplet db 'YOU HAVE COMPLETED ALL 111 LEVELS OF SUPAPLEX',0
aYourBrainIsInF db 'YOUR BRAIN IS IN FANTASTIC SHAPE',0
aNotManyPeopleA db 'NOT MANY PEOPLE ARE ABLE TO MANAGE THIS',0
aWibble??   db 'WIBBLE??',0
word_58AB8  dw 3231h        ; DATA XREF: sub_4945D+BEr
word_58ABA  dw 3433h        ; DATA XREF: sub_4945D+C4r
a56789012345678 db '567890123456789012345678',0
a045        db '045',0
a00     db ' 00',0
a00_0       db ' 00',0
aDemo       db '  DEMO  ',0
word_58AEA  dw 3030h        ; DATA XREF: sub_4945D+C1w
word_58AEC  dw 30h          ; DATA XREF: sub_4945D+CAw
aDemoLevel  db '----- DEMO LEVEL! -----',0
aRecordingDemo0 db '--- RECORDING DEMO0 ---',0
        db '                      ',0
        db '                      ',0
a001Supaplex000 db '001 SUPAPLEX 000:00:00',0
a002Supaplex000 db '002 SUPAPLEX 000:00:00',0
a003Supaplex000 db '003 SUPAPLEX 000:00:00',0
a004Supaplex000 db '004 SUPAPLEX 000:00:00',0
a005Supaplex000 db '005 SUPAPLEX 000:00:00',0
a006Supaplex000 db '006 SUPAPLEX 000:00:00',0
a007Supaplex000 db '007 SUPAPLEX 000:00:00',0
a008Supaplex000 db '008 SUPAPLEX 000:00:00',0
a009Supaplex000 db '009 SUPAPLEX 000:00:00',0
a010Supaplex000 db '010 SUPAPLEX 000:00:00',0
a011Supaplex000 db '011 SUPAPLEX 000:00:00',0
a012Supaplex000 db '012 SUPAPLEX 000:00:00',0
a013Supaplex000 db '013 SUPAPLEX 000:00:00',0
a014Supaplex000 db '014 SUPAPLEX 000:00:00',0
a015Supaplex000 db '015 SUPAPLEX 000:00:00',0
a016Supaplex000 db '016 SUPAPLEX 000:00:00',0
a017Supaplex000 db '017 SUPAPLEX 000:00:00',0
a018Supaplex000 db '018 SUPAPLEX 000:00:00',0
a019Supaplex000 db '019 SUPAPLEX 000:00:00',0
a020Supaplex000 db '020 SUPAPLEX 000:00:00',0
        db '                      ',0
        db '                      ',0
byte_58D46  db 0            ; DATA XREF: start+216w code:467Ar ...
byte_58D47  db 65h dup(0)       ; DATA XREF: sub_4B721r sub_4BF8D+C6w
word_58DAC  dw 2D2Dh        ; DATA XREF: start+1AEw
                    ; sub_4B419:loc_4B52Ao
word_58DAE  dw 2D2Dh        ; DATA XREF: start+1B4w
word_58DB0  dw 2D2Dh        ; DATA XREF: start+1BAw
word_58DB2  dw 2D2Dh        ; DATA XREF: start+1C0w
byte_58DB4  db 78h dup(0), 8 dup(2Dh), 78h dup(0), 8 dup(2Dh), 78h dup(0)
                    ; DATA XREF: sub_4B419+103o
        db 8 dup(2Dh), 78h dup(0), 8 dup(2Dh), 78h dup(0), 8 dup(2Dh)
        db 78h dup(0), 8 dup(2Dh), 78h dup(0), 8 dup(2Dh), 78h dup(0)
        db 8 dup(2Dh), 78h dup(0), 8 dup(2Dh), 78h dup(0), 8 dup(2Dh)
        db 78h dup(0), 8 dup(2Dh), 78h dup(0), 8 dup(2Dh), 78h dup(0)
        db 8 dup(2Dh), 78h dup(0), 8 dup(2Dh), 78h dup(0), 8 dup(2Dh)
        db 78h dup(0), 8 dup(2Dh), 78h dup(0), 8 dup(2Dh), 78h dup(0)
        db 8 dup(2Dh), 78h dup(0), 8 dup(2Dh), 78h dup(0), 2 dup(8)
        db 6Fh dup(4), 2 dup(8)
byte_5981F  db 0            ; DATA XREF: start+213w
                    ; sub_4A95F:loc_4A980r ...
byte_59820  db 0            ; DATA XREF: sub_4AB1B:loc_4AB56w
                    ; sub_4AB1B+18Er
byte_59821  db 0            ; DATA XREF: sub_4C141+Ew
                    ; sub_4C141+33r
byte_59822  db 0            ; DATA XREF: sub_4C141+14w
                    ; sub_4C141+42r
byte_59823  db 0            ; DATA XREF: sub_4C141+1Aw
                    ; sub_4C141+60r
asc_59824   db '        ',0         ; DATA XREF: sub_4B419+12Bo
        align 4
        db '        ',0
        align 4
        db '        ',0
        align 4
        db '        ',0
        align 4
        db '        ',0
        align 4
        db '        ',0
        align 4
        db '        ',0
        align 4
        db '        ',0
        align 4
soundEnabled?   db 0            ; DATA XREF: int8handler:loc_473AAr
                    ; sub_4D8B0+28w ...
byte_59885  db 0            ; DATA XREF: readConfig+A0w
                    ; readConfig+AAw ...
byte_59886  db 0            ; DATA XREF: start+34Dr
                    ; start:loc_46FB4r ...
byte_59887  db 0            ; DATA XREF: sub_4CAFC+D9r code:6CCAw ...
byte_59888  db 0            ; DATA XREF: sub_4CAFC+A4r
                    ; sub_4CAFC:loc_4CBC6r ...
byte_59889  db 0            ; DATA XREF: int8handler+61w
                    ; loadBeep+1Ew ...
byte_5988A  db 0            ; DATA XREF: int8handler+73w
                    ; loadBeep+23w ...
byte_5988B  db 0            ; DATA XREF: int8handler:loc_473B4r
                    ; int8handler+5Bw ...
byte_5988C  db 0            ; DATA XREF: int8handler:loc_473C6r
                    ; int8handler+6Dw ...
byte_5988D  db 53h          ; DATA XREF: sub_4955B+4BBw
                    ; sub_4955B:loc_49B6Fr
word_5988E  dw 4650h        ; DATA XREF: sub_4955B+4C3w
byte_59890  db 58h          ; DATA XREF: sub_4955B:loc_49A2Cw
word_59891  dw 3336h        ; DATA XREF: sub_4955B+4D7w
aVersions146_xB db 'VERSIONS 1-4 + 6.X BY HERMAN PERK',0
aSupaplexSpeedF db 'SUPAPLEX SPEED FIX VERSION 6.3',0
byte_598D4  db 100h dup(0)      ; DATA XREF: start:loc_46CFBr
                    ; start:loc_46D09w ...
byte_599D4  db 0            ; DATA XREF: start+E2w start+EEw ...
byte_599D5  db 0            ; DATA XREF: start:loc_46CF6w
                    ; start:loc_46D37r ...
word_599D6  dw 0            ; DATA XREF: sub_4B159+5Fw
                    ; readLevels+EAr
word_599D8  dw 0            ; DATA XREF: readDemo+21w readDemo+84w ...
word_599DA  dw 0            ; DATA XREF: start+9Bw start+B9w ...
word_599DC  dw 0CEh, 16Ah, 146h, 0CDh, 24Dh, 12Ch, 1A7h, 1FBh, 1D2h
                    ; DATA XREF: fileReadUnk1+1o
        dw 2FDh, 0F001h, 0F1F0h, 0F003h, 0F350h, 0F007h, 0F460h
        dw 0F00Bh, 0F0F0h, 0F01Dh, 0F0F0h, 0F026h, 50F0h, 0F037h
        dw 41D0h, 105Fh, 0F3F3h, 0F068h, 10F0h, 106Ch, 94F4h
aLevels_dat db 'LEVELS.DAT',0       ; DATA XREF: readLevels:loc_4D599o
                    ; readLevels+6Fo
commandLine db 100h dup(0)      ; DATA XREF: start:readAndProcessCommandLineo
                    ; start+30o
cbCommandLine   db 0            ; DATA XREF: start+Aw
aVersions5_xByE db 'VERSIONS 5.X BY ELMER PRODUCTIONS',0
        db 16h dup(0)
byte_59B5C  db 0            ; DATA XREF: sub_492F1+1Dw
                    ; sub_492F1+44w ...
byte_59B5D  db 0            ; DATA XREF: somethingspsig+7w
byte_59B5E  db 0            ; DATA XREF: somethingspsig+Ew
byte_59B5F  db 0            ; DATA XREF: sub_492F1+19w
                    ; somethingspsig+3r
word_59B60  dw 0            ; DATA XREF: start:loc_46D5Ao
byte_59B62  db 0            ; DATA XREF: readLevelsLst+85r
byte_59B63  db 0            ; DATA XREF: start+155r
byte_59B64  db 0            ; DATA XREF: start+285r
word_59B65  dw 0            ; DATA XREF: sub_4B419:loc_4B423r
dword_59B67 dd 0            ; DATA XREF: videoloop+2r
                    ; videoloop+33r
byte_59B6B  db 0            ; DATA XREF: start+2CAr start+42Er ...
byte_59B6C  db 0            ; DATA XREF: sub_49EBE+82r
                    ; sub_49EBE:loc_49F56r ...
byte_59B6D  db 0            ; DATA XREF: sub_49EBE+F2r
word_59B6E  dw 0            ; DATA XREF: sub_4945D+3Cr
        db 0
byte_59B71  db 0            ; DATA XREF: start+37Br
byte_59B72  db 0            ; DATA XREF: sub_49EBE:loc_49FA9r
word_59B73  dw 0            ; DATA XREF: sub_4FDB5r
        db 0
dword_59B76 dd 0            ; DATA XREF: start+195r
                    ; sub_4B419:loc_4B4EFr
byte_59B7A  db 0            ; DATA XREF: runLevel+84r runLevel+8Bw ...
byte_59B7B  db 1            ; DATA XREF: sub_49D53w sub_49D58+1Ar ...
byte_59B7C  db 0            ; DATA XREF: sub_4955B+20Dw
                    ; sub_4955B:loc_4976Fr ...
byte_59B7D  db 0            ; DATA XREF: sub_4955B+232w
                    ; sub_4955B:loc_49794r ...
byte_59B7E  db 0            ; DATA XREF: sub_4955B+257w
                    ; sub_4955B:loc_497B9r ...
byte_59B7F  db 0            ; DATA XREF: sub_4955B+34Ew
                    ; sub_4955B:loc_498B5r ...
byte_59B80  db 0            ; DATA XREF: sub_4955B+353w
                    ; sub_4955B:loc_498C2r ...
byte_59B81  db 0            ; DATA XREF: sub_4955B+3A8w
                    ; sub_4955B:loc_4990Fr ...
byte_59B82  db 0            ; DATA XREF: sub_4955B+3ADw
                    ; sub_4955B:loc_4991Cr ...
byte_59B83  db 0            ; DATA XREF: sub_4C2F2+17r
                    ; sub_4C2F2+1Ew ...
byte_59B84  db 0            ; DATA XREF: start+241w
                    ; start:loc_46E75w ...
byte_59B85  db 0            ; DATA XREF: start+1A9w
                    ; readPlayersLstr ...
byte_59B86  db 0            ; DATA XREF: readMoving+19r sub_47E98w ...
        align 2
word_59B88  dw 0            ; DATA XREF: sub_4914A+52r
                    ; sub_49EBE:loc_49F25w ...
word_59B8A  dw 0            ; DATA XREF: sub_4914A+56r
                    ; sub_49EBE+6Bw ...
word_59B8C  dw 0            ; DATA XREF: sub_4B419+1Dr
                    ; sub_4B419+30r ...
word_59B8E  dw 0            ; DATA XREF: sub_4B419+21r
                    ; sub_4B419+2Dw ...
word_59B90  dw 0            ; DATA XREF: runLevel+207r
                    ; runLevel+25Cw ...
word_59B92  dw 0            ; DATA XREF: runLevel+218r
                    ; runLevel+23Fw ...
byte_59B94  db 0            ; DATA XREF: runLevel:loc_48BF7w
                    ; runLevel+14Fr ...
byte_59B95  db 0            ; DATA XREF: runLevel+1EBw
                    ; runLevel+1F1r ...
byte_59B96  db 0            ; DATA XREF: int8handler+8w
                    ; runLevel+140r ...
gameSpeed   db 5            ; DATA XREF: int9handler+4Bw
                    ; int9handler+5Bw ...
speed?      db 0FFh         ; DATA XREF: start+12Dw
                    ; int9handler+56w ...
speed?3     db 0FFh         ; DATA XREF: start+235w
                    ; int9handler+34r ...
byte_59B9A  db 2            ; DATA XREF: sub_4B419+182w
                    ; sub_4B419+203r ...
        db 5A5h dup(0)
byte_5A140  db 59h dup(0)       ; DATA XREF: sub_4945D+5Ew
word_5A199  dw 0            ; DATA XREF: sub_492F1+16w
byte_5A19B  db 0            ; DATA XREF: start+423w start+461r ...
byte_5A19C  db 0            ; DATA XREF: start+45Ar runLevel+7w ...
aDemoSuccessf_0 db '    DEMO SUCCESSFUL    ',0
aDemoFailed_0   db '      DEMO FAILED      ',0
aLevelSuccessfu db '   LEVEL SUCCESSFUL    ',0
aLevelFailed    db '     LEVEL FAILED      ',0
aLevelSet?? db '     LEVEL SET ??      ',0 ; DATA XREF: sub_4B419+B0w
aSupaplexLevelS db '  SUPAPLEX LEVEL SET   ',0
aDemoSuccessful db 'Demo successful: ',0 ; DATA XREF: start+468o
aDemoFailed db 'Demo failed:     ',0 ; DATA XREF: start:loc_4708Eo
aSpWithoutDemo  db 'SP without demo: ',0 ; DATA XREF: start+11Eo
aFileLevel  db '!! File < Level: ',0 ; DATA XREF: start+ADo
aFileDemo   db '!! File >> Demo: ',0 ; DATA XREF: start:loc_46CB7o
a@ErrorLevel?Su db '"@"-ERROR: Level(?) successful: ',0 ; DATA XREF: start+47Bo
a@ErrorLevel?Fa db '"@"-ERROR: Level(?) failed:     ',0 ; DATA XREF: start:loc_470A1o
a@ErrorBadOrMis db '"@"-ERROR: Bad or missing ":filename.ext"!',0Dh,0Ah,0
                    ; DATA XREF: start+10Do
byte_5A2F6  db 0            ; DATA XREF: start:loc_46D48w
                    ; start+2A8r ...
speed?2     db 0            ; DATA XREF: int9handler+8Dr
                    ; int9handler+98r ...
byte_5A2F8  db 0            ; DATA XREF: runLevel:loc_48AD8r
                    ; runLevel:loc_48ADFw ...
byte_5A2F9  db 0            ; DATA XREF: start+3CCw start+435w ...
fh1     dw 0            ; DATA XREF: readFromFh1+3r
                    ; writeToFh1+3r ...
aSavegame_sav   db 'SAVEGAME.SAV',0     ; DATA XREF: sub_4955B+470o
                    ; sub_4955B+531o ...
word_5A309  dw 5053h        ; DATA XREF: sub_4955B+55Ar
word_5A30B  dw 1A0Dh        ; DATA XREF: sub_4955B+564r
word_5A30D  dw 0            ; DATA XREF: sub_4955B:loc_49AB1r
word_5A30F  dw 0            ; DATA XREF: sub_4955B+560r
aWr     db 'WR',0
aLd     db 'LD',0
aXx     db 'XX',0
aDb     db 'DB',0
        db '--',0
byte_5A320  db 0            ; DATA XREF: int8handler:loc_473D8w
                    ; int8handler+80r ...
byte_5A321  db 0            ; DATA XREF: int8handler+7Cw
                    ; int8handler:loc_473F0r ...
byte_5A322  db 0            ; DATA XREF: int8handler+87w
                    ; int8handler+97w ...
byte_5A323  db 0            ; DATA XREF: runLevel+53w
                    ; runLevel+36Br ...
a00s0010_sp db '00S001$0.SP',0      ; DATA XREF: sub_49544:loc_49557w
                    ; start+3D1w ...
aMyspsig_txt    db 'MYSPSIG.TXT',0      ; DATA XREF: somethingspsig+6Ao
word_5A33C  dw 0            ; DATA XREF: readDemo+Dw sub_4A3E9+69r ...
byte_5A33E  db 0            ; DATA XREF: somethingspsig+E9r
                    ; sub_4945D+D2r ...
byte_5A33F  db 0            ; DATA XREF: start+327w
                    ; start:loc_46F77w ...
data        ends
