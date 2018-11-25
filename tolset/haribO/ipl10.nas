; haribote-ipl


CYLS	EQU		10				; どこまで読み込むか

		ORG		0x7c00			; このプログラムがどこに読み込まれるのか

; configuration for floppy disk image

		JMP		entry
		DB		0x90
		DB		"HARIBOTE"		; ブートセクタの名前を自由に書いてよい（8バイト）
		DW		512				; 1セクタの大きさ（512にしなければいけない）
		DB		1				; クラスタの大きさ（1セクタにしなければいけない）
		DW		1				; FATがどこから始まるか（普通は1セクタ目からにする）
		DB		2				; FATの個数（2にしなければいけない）
		DW		224				; ルートディレクトリ領域の大きさ（普通は224エントリにする）
		DW		2880			; このドライブの大きさ（2880セクタにしなければいけない）
		DB		0xf0			; メディアのタイプ（0xf0にしなければいけない）
		DW		9				; FAT領域の長さ（9セクタにしなければいけない）
		DW		18				; 1トラックにいくつのセクタがあるか（18にしなければいけない）
		DW		2				; ヘッドの数（2にしなければいけない）
		DD		0				; パーティションを使ってないのでここは必ず0
		DD		2880			; このドライブ大きさをもう一度書く
		DB		0,0,0x29		; よくわからないけどこの値にしておくといいらしい
		DD		0xffffffff		; たぶんボリュームシリアル番号
		DB		"HARIBOTEOS "	; ディスクの名前（11バイト）
		DB		"FAT12   "		; フォーマットの名前（8バイト）
		RESB	18				; とりあえず18バイトあけておく


; main program

entry:
    MOV AX,0
    MOV SS,AX
    MOV SP,0x7c00
    MOV DS,AX

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

; run OS from disk

MOV [0x0ff0], CH
JMP 0xc200

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

fin:
    HLT
    JMP fin

msg:
    DB 0x0a, 0x0a           ;newlines
    DB "disk load error"
    DB 0x0a                 ;newline
    DB 0                    ;string terminator

RESB	0x7dfe-$

DB		0x55, 0xaa
