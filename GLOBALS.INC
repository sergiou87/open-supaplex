.386
.model small
nosmart


code segment para public 'CODE' use16
        assume cs:code

    global atoi:proc
    global conprintln:proc
    global conprint:proc

ends code


data        segment para public 'DATA' use16
        assume cs:data

    global crlf:word
    global demoFileName:byte:100h

ends data