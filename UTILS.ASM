.386
.model small
nosmart



code segment para public 'CODE' use16
        assume cs:code

; =============== S U B R O U T I N E =======================================

; atoi, stored in al
public atoi
atoi        proc near       ; CODE XREF: start+202p start+223p ...
        xor ax, ax      ; reset ax
        mov bl, 0Ah     ; bl = 10
        or  cx, cx      ; test cx

again:                  ; CODE XREF: atoi+22j
        jz  short atoireturn    ; return if cx == 0
        mov bh, es:[di] ; load dest char
        cmp bh, 30h ; '0'
        jb  short atoireturn    ; bail if less than '0'
        cmp bh, 39h ; '9'
        ja  short atoireturn    ; bail if bigger than '9'
        sub bh, 30h ; '0'   ; bh now binary of nr
        mul bl      ; ax = 10 * al
        add al, bh      ; al += bh
        jnb short leaveAL
        mov al, 0

leaveAL:                ; CODE XREF: atoi+1Cj
        inc di
        dec cx
        jmp short again
; ---------------------------------------------------------------------------

atoireturn:                 ; CODE XREF: atoi:againj atoi+Ej ...
        retn
atoi        endp






code      ends

end
