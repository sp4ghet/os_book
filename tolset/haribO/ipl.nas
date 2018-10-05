; hello-os

ORG 0x7c00
JMP entry


; configuration for floppy disk image

DB 0x90
DB "HELLOIPL"
DW 512
DB 1
DW 1
DB 2
DW 224
DW 2880
DB 0xf0
DW 9
DW 18
DW 2
DD 0
DD 2880
DB 0,0,0x29
DD 0xffffffff
DB "HELLO-OS   "
DB "FAT12   "
RESB 18


; main program

entry:
    MOV AX,0
    MOV SS,AX
    MOV SP,0x7c00
    MOV DS,AX
    MOV ES,AX

    MOV SI,msg

CYLS EQU 10

;read disk
    MOV AX,0x0820
    MOV ES, AX
    MOV CH, 0
    MOV DH, 0
    MOV CL, 2

readloop:
    MOV SI, 0       ;init counter
    
retry:
    MOV AH, 0x02
    MOV AL, 1
    MOV BX, 0
    MOV DL, 0x00    ;A drive
    INT 0x13
    JNC next
    ADD SI, 1       ;++
    CMP SI, 5       ;SI == 5 ? jmp
    JAE error
    MOV AH, 0x00    ;reset drive
    MOV DL, 0x00
    INT 0x13        ;reset drive invocation
    JMP retry

next:
    ; read sector 2~18
    MOV AX, ES
    ADD AX, 0x0020
    MOV ES, AX
    ADD CL, 1
    CMP CL, 18
    JBE readloop

    ; read other head and other cylinder
    MOV CL, 1
    ADD DH, 1 ;move to other head
    CMP DH, 2 
    JB readloop ;read if headId < 2 (0,1)
    MOV DH, 0   ;if not, move to next cylinder and switch back to head0
    ADD CH, 1
    CMP CH, CYLS
    JB readloop
    JMP 0xc200

fin:
    HLT
    JMP fin

error:
    MOV SI, msg

putloop:
    MOV AL,[SI]
    ADD SI,1
    CMP AL,0
    JE fin
    MOV AH,0x0e
    MOV BX,15
    INT 0x10
    JMP putloop

msg:
    DB 0x0a, 0x0a           ;newlines
    DB "disk load error"
    DB 0x0a                 ;newline
    DB 0                    ;string terminator

RESB	0x7dfe-$

DB		0x55, 0xaa
