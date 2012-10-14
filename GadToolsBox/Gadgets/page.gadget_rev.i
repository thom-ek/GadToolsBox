VERSION		EQU	37
REVISION	EQU	2
DATE	MACRO
		dc.b	'18.4.98'
	ENDM
VERS	MACRO
		dc.b	'listview.gadget 37.2'
	ENDM
VSTRING	MACRO
		dc.b	'listview.gadget 37.2 (18.4.98)',13,10,0
	ENDM
VERSTAG	MACRO
		dc.b	0,'$VER: listview.gadget 37.2 (18.4.98)',0
	ENDM
