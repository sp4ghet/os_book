
[FORMAT "WCOFF"]				; ƒIƒuƒWƒFƒNƒgƒtƒ@ƒCƒ‹‚ðì‚éƒ‚[ƒh	
[INSTRSET "i486p"]				; 486‚Ì–½—ß‚Ü‚ÅŽg‚¢‚½‚¢‚Æ‚¢‚¤‹Lq
[BITS 32]						; 32ƒrƒbƒgƒ‚[ƒh—p‚Ì‹@ŠBŒê‚ðì‚ç‚¹‚é
[FILE "naskfunc.nas"]			; ƒ\[ƒXƒtƒ@ƒCƒ‹–¼î•ñ

		GLOBAL	_io_hlt,_write_mem8

[SECTION .text]

_io_hlt:	; void io_hlt(void);
		HLT
		RET

_write_mem8:	; void write_mem8(int addr, int data);
		MOV		ECX,[ESP+4]		; [ESP+4]‚Éaddr‚ª“ü‚Á‚Ä‚¢‚é‚Ì‚Å‚»‚ê‚ðECX‚É“Ç‚Ýž‚Þ
		MOV		AL,[ESP+8]		; [ESP+8]‚Édata‚ª“ü‚Á‚Ä‚¢‚é‚Ì‚Å‚»‚ê‚ðAL‚É“Ç‚Ýž‚Þ
		MOV		[ECX],AL
		RET
