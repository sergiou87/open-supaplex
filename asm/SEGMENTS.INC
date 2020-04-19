
; ===========================================================================
zeg000      segment para public 'UNK' use16
        db 7E00h dup(0)
zeg000      ends

; ===========================================================================
menuseg      segment para public 'UNK' use16
        db 7D00h dup(0)
menuseg      ends

; ===========================================================================
gfxseg      segment para public 'UNK' use16
        db 7D00h dup(0)
gfxseg      ends

; ===========================================================================
controlsseg      segment para public 'UNK' use16
        db 7D00h dup(0)
controlsseg      ends

; ===========================================================================
backseg      segment para public 'UNK' use16
        db 7D00h dup(0)
backseg      ends

; ===========================================================================
demoseg      segment para public 'UNK' use16
        db 16h dup(0FFh), 0FA0Ah dup(0)
demoseg      ends


; ===========================================================================
doserrors      segment para public 'UNK' use16
        db 0
        db 'DOS ERROR: Function Number Invalid',0
        db 'DOS ERROR: File Not Found',0
        db 'DOS ERROR: Path Not Found',0
        db 'DOS ERROR: Too Many Open Files',0
        db 'DOS ERROR: Access Denied',0
        db 'DOS ERROR: Handle Invalid',0
        db 66h dup(0)
doserrors      ends

; ===========================================================================
soundseg      segment para public 'UNK' use16
        db 0CFh ; ?
        db 1Fh dup(0)
        db 0DFh dup(0)
        db 14F1h dup(0)
soundseg      ends

; ===========================================================================
sound2seg      segment para public 'UNK' use16
        db 0CFh ; ?
        db 9C4Fh dup(0)
sound2seg      ends

; Segment type: Uninitialized
stackseg        segment para stack 'STACK' use16
        db 400h dup(0)
stackseg        ends
