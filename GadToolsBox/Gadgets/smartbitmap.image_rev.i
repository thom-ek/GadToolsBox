VERSION		EQU	39
REVISION	EQU	1
DATE	MACRO
		dc.b	'10.1.98'
	ENDM
VERS	MACRO
		dc.b	'smartbitmap.image 39.1'
	ENDM
VSTRING	MACRO
		dc.b	'smartbitmap.image 39.1 (10.1.98)',13,10,0
	ENDM
VERSTAG	MACRO
		dc.b	0,'$VER: smartbitmap.image 39.1 (10.1.98)',0
	ENDM
