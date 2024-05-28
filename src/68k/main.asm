VDPData equ $C00000
VDPCtrl equ $C00004
VRAMWCtrl equ $40000000

	org		$00000000
	dc.l	$00FFFE00   ;0 Reset SP                                     $000000
	dc.l	START       ;1 Reset PC                                     $000004
	dc.l	INTR        ;2 Bus error                                    $000008
	dc.l	INTR        ;3 Address error                                $00000C
	dc.l	INTR        ;4 Illegal instruction                          $000010
	dc.l	INTR        ;5 Division by zero                             $000014
	dc.l	INTR        ;6 CHK instruction                              $000018
	dc.l	INTR        ;7 TRAPV instruction                            $00001C
	dc.l	INTR        ;8 Privilege violation                          $000020
	dc.l	INTR        ;9 Trace                                        $000024
	dc.l	INTR        ;10                                             $000028
	dc.l	INTR        ;11                                             $00002C
	dc.l	INTR        ;12                                             $000030
	dc.l	INTR        ;13                                             $000034
	dc.l	INTR        ;14 Reserved                                    $000038
	dc.l	INTR        ;15 Uninitialised interrupt                     $00003C
	dc.l	INTR        ;16 Reserved                                    $000040
	dc.l	INTR        ;17 Reserved                                    $000044
	dc.l	INTR        ;18 Reserved                                    $000048
	dc.l	INTR        ;19 Reserved                                    $00004C
	dc.l	INTR        ;20 Reserved                                    $000050
	dc.l	INTR        ;21 Reserved                                    $000054
	dc.l	INTR        ;22 Reserved                                    $000058
	dc.l	INTR        ;23 Reserved                                    $00005C
	dc.l	INTR        ;24 Spurious interrupt                          $000060
	dc.l	INTR        ;25 IRQ Level 1                                 $000064
	dc.l	INTR        ;26 IRQ Level 2 EXT interrupt                   $000068
	dc.l	INTR        ;27 IRQ Level 3                                 $00006C
	dc.l	HSYNC       ;28 IRQ Level 4 Horizontal VDP interrupt        $000070
	dc.l	INTR        ;29 IRQ Level 5                                 $000074
	dc.l	VSYNC       ;30 IRQ Level 6 Vertical VDP interrupt          $000078
	dc.l	INTR        ;31 IRQ Level 7                                 $00007C
	dc.l	INTR        ;32 TRAP #00                                    $000080
	dc.l	INTR        ;33 TRAP #01                                    $000084
	dc.l	INTR        ;34 TRAP #02                                    $000088
	dc.l	INTR        ;35 TRAP #03                                    $00008C
	dc.l	INTR        ;36 TRAP #04                                    $000090
	dc.l	INTR        ;37 TRAP #05                                    $000094
	dc.l	INTR        ;38 TRAP #06                                    $000098
	dc.l	INTR        ;39 TRAP #07                                    $00009C
	dc.l	INTR        ;40 TRAP #08                                    $0000A0
	dc.l	INTR        ;41 TRAP #09                                    $0000A4
	dc.l	INTR        ;42 TRAP #10                                    $0000A8
	dc.l	INTR        ;43 TRAP #11                                    $0000AC
	dc.l	INTR        ;44 TRAP #12                                    $0000B0
	dc.l	INTR        ;45 TRAP #13                                    $0000B4
	dc.l	INTR        ;46 TRAP #14                                    $0000B8
	dc.l	INTR        ;47 TRAP #15                                    $0000BC
	dc.l	INTR        ;48 (FP) Branch or set on unordered condition   $0000C0
	dc.l	INTR        ;49 (FP) Inexact result                         $0000C4
	dc.l	INTR        ;50 (FP) Divide by zero                         $0000C8
	dc.l	INTR        ;51 (FP) Underflow                              $0000CC
	dc.l	INTR        ;52 (FP) Operand error                          $0000D0
	dc.l	INTR        ;53 (FP) Overflow                               $0000D4
	dc.l	INTR        ;54 (FP) Signaling NAN                          $0000D8
	dc.l	INTR        ;55 (FP) Unimplimented data type                $0000DC
	dc.l	INTR        ;56 MMU Configuration error                     $0000E0
	dc.l	INTR        ;57 MMU Illegal operation error                 $0000E4
	dc.l	INTR        ;58 MMU Access violation error                  $0000E8
	dc.l 	INTR        ;59 Reserved                                    $0000EC
	dc.l 	INTR        ;60 Reserved                                    $0000F0
	dc.l 	INTR        ;61 Reserved                                    $0000F4
	dc.l 	INTR        ;62 Reserved                                    $0000F8
	dc.l 	INTR        ;63 Reserved                                    $0000FC

	dc.b	"SEGA MEGA DRIVE "                                  ;Console name (16)          $000100
	dc.b	"(C)SIFU 2021.JUL"                                  ;Copyright information (16) $000110
	dc.b	"TEST ROM                                        "  ;Domestic name (48)         $000120
	dc.b	"TEST ROM                                        "  ;Overseas name (48)         $000150
	dc.b	"GM 00000000-00"                                    ;Serial number (14)         $000180
	dc.w	$0000                                               ;Checksum (2)               $00018E
	dc.b	"JD              "                                  ;I/O Support (16)           $000190
	dc.l	$00000000                                           ;ROM start address (4)      $0001A0
	dc.l	$003FFFFF                                           ;ROM end address (4)        $0001A4
	dc.l	$00FF0000                                           ;Backup RAM start (4)       $0001A8
	dc.l	$00FFFFFF                                           ;Backup RAM end (4)         $0001AC
	dc.b	"            "                                      ;Backup RAM support (12)    $0001B0
	dc.b	"            "                                      ;Modem support (12)         $0001BC
	dc.b	"                                        "          ;Memo (40)                  $0001C8
	dc.b	"JUE             "                                  ;Country support (16)       $0001F0

INTR
	rte

HSYNC
	rte

VSYNC
	move.w	%0000011100000000,sr         ;Enable interrupts on 68k
	rte

VDPREG
	dc.b	%00010100   ;0 Mode set reg.1                                   0       0       0       IE1     0       1       M3      0
	dc.b	%01100100   ;1 Mode ser reg.2                                   0       DISP    IE0     M1      M2      1       0       0
	dc.b	%00110000   ;2 Pattern name table base address for Scroll A     0       0       SA15    SA14    SA13    0       0       0              $C000
	dc.b	%00111100   ;3 Pattern name table base address for Window       0       0       WD15    WD14    WD13    WD12    WD11    0              $F000
	dc.b	%00000111   ;4 Pattern name table base address for Scroll B     0       0       0       0       0       SB15    SB14    SB13           $E000
	dc.b	%01101100   ;5 Sprite attribute table base address              0       AT15    AT14    AT13    AT12    AT11    AT10    AT9            $D800 
	dc.b	%00000000   ;6 Unused                                           0       0       0       0       0       0       0       0
	dc.b	%00000000   ;7 Background colour                                0       0       CP1     CP0     COL3    COL2    COL1    COL0
	dc.b	%00000000   ;8 Unused                                           0       0       0       0       0       0       0       0
	dc.b	%00000000   ;9 Unused                                           0       0       0       0       0       0       0       0
	dc.b	%00000000   ;10 H Interrupt register                            BIT7    BIT6    BIT5    BIT4    BIT3    BIT2    BIT1    BIT0
	dc.b	%00001000   ;11 Mode set reg.3                                  0       0       0       0       IE2     VSCR    HSCR    LSCR
	dc.b	%10000001   ;12 Mode set reg.4                                  RS0     0       0       0       S/TE    LSM1    LSM0    RS1
	dc.b	%00110111   ;13 H Scroll data table base address                0       0       HS15    HS14    HS13    HS12    HS11    HS10           $FC00
	dc.b	%00000000   ;14 Unused                                          0       0       0       0       0       0       0       0
	dc.b	%00000010   ;15 Auto increment data                             INC7    INC6    INC5    INC4    INC3    INC2    INC1    INC0
	dc.b	%00000001   ;16 Scroll size                                     0       0       VSZ1    VSZ0    0       0       HSZ1    HSZ0
	dc.b	%00000000   ;17 Window H position                               RIGT    0       0       WHP5    WHP4    WHP3    WHP2    WHP1
	dc.b	%00000000   ;18 Window V position                               DOWN    0       0       WVP4    WVP3    WVP2    WVP1    WVP0
	dc.b	%00000000   ;19 DMA length counter low                          LG7     LG6     LG5     LG4     LG3     LG2     LG1     LG0
	dc.b	%00000000   ;20 DMA length counter high                         LG15    LG14    LG13    LG12    LG11    LG10    LG9     LG8
	dc.b	%00000000   ;21 DMA source address low                          SA8     SA7     SA6     SA5     SA4     SA3     SA2     SA1
	dc.b	%00000000   ;22 DMA source address mid                          SA16    SA15    SA14    SA13    SA12    SA11    SA10    SA9
	dc.b	%00000000   ;23 DMA source address high                         DMD1    DMD0    SA22    SA21    SA20    SA19    SA18    SA17

TEST_TILES_START
	dc.l	$00000000
	dc.l	$00000000
	dc.l	$00000000
	dc.l	$00000000
	dc.l	$00000000
	dc.l	$00000000
	dc.l	$00000000
	dc.l	$00000000

TEST_TILE_X
	dc.l	$10000010
	dc.l	$01000100
	dc.l	$00101000
	dc.l	$00010000
	dc.l	$00101000
	dc.l	$01000100
	dc.l	$10000010
	dc.l	$00000000

TEST_TILE_F
	dc.l	$22223330
	dc.l	$20000000
	dc.l	$20000000
	dc.l	$22233300
	dc.l	$20000000
	dc.l	$20000000
	dc.l	$20000000
	dc.l	$00000000

TEST_TILES_END

START
	move.b	($A10001),d0         ;TMSS
	andi.b	#$0F,d0
	beq		NOTMSS
	move.l	#"SEGA",($A14000)

NOTMSS
	lea VDPREG,a0
	move.w (VDPCtrl),d0          ;Read $C00004 = Status register
	move.l #$00008000,d1
	moveq.l #24-1,d2             ;Loop counter for 24 registers
NextVDPSetting	
	move.b (a0)+,d1              ;000080aa   000081bb   000082cc
	move.w d1,(VDPCtrl)
	add.l #$00000100,d1          ;000081aa   000082bb   000083cc
	dbra d2,NextVDPSetting


;	move.w	#($FFFF/2)-1,d0      ;Iterating in words
	move.w	#3-1,d0           ;Iterating in words
	move.l	#$40000000,d1
	move.l	d1,VDPCtrl
VRAMClrLoop
	move.w	#$0000,VDPData       ;Fill with zeros (VDP reg15 auto-increment is on)
	dbra	d0,VRAMClrLoop


    ;Define Palette 0
	move.l #$C0000000,d0                    ;Color 0
	move.l d0,VDPCtrl
	move.w #%0000000000000000,VDPData       ;----BBB-GGG-RRR-

	move.l #$C0020000,d0                    ;Color 1
	move.l d0,VDPCtrl
	move.w #%0000000000001110,VDPData

	move.l #$C0040000,d0                    ;Color 2
	move.l d0,VDPCtrl
	move.w #%0000000011100000,VDPData

	move.l #$C0060000,d0                    ;Color 3
	move.l d0,VDPCtrl
	move.w #%0000111000000000,VDPdata


	move.l #$40000000,d1
	move.l d1,VDPCtrl

	move.l	#((TEST_TILES_END-TEST_TILES_START)/4)-1,d0
	lea TEST_TILES_START,a0
TileLoop
	move.l (a0)+,VDPData
	dbra d0,TileLoop


	move.l #$40000003,d0
	move.l d0,VDPCtrl               ;b15 b14 b13 b12 b11 b10 b9  b8      b7  b6  b5  b4  b3  b2  b1  b0
	move.w #$0001,VDPdata           ;0   0   0   0   0   0   0   0       0   0   0   0   0   0   0   1
	move.w #$0001,VDPdata           ;0   0   0   0   0   0   0   0       0   0   0   0   0   0   1   0
	move.w #$0001,VDPdata           ;0   0   0   0   0   0   0   0       0   0   0   0   0   0   1   1	


;	move.w	#($1000/2)-1,d0
	move.w	#3-1,d0
	move.l	#$60000003,d1
	move.l	d1,VDPCtrl
ScrollBLoop
	move.w	#$0002,VDPData
	dbra	d0,ScrollBLoop


	move.w	%0000011100000000,sr      ;Enable interrupts on 68k

STUCK
	jmp STUCK
