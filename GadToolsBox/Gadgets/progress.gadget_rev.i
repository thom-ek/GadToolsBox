VERSION		EQU	37
REVISION	EQU	1
DATE	MACRO
		dc.b	'15.1.98'
	ENDM
VERS	MACRO
		dc.b	'progress.gadget 37.1'
	ENDM
VSTRING	MACRO
		dc.b	'progress.gadget 37.1 (15.1.98)',13,10,0
	ENDM
VERSTAG	MACRO
		dc.b	0,'$VER: progress.gadget 37.1 (15.1.98)',0
	ENDM
