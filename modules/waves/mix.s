.file "mix.s";
.text;
	.align 4
.global _mix_mod;
.type _mix_mod, STT_FUNC;
_mix_mod:
	LINK 0;
	[FP+8] = R0;
	[FP+12] = R1;
	[FP+16] = R2;
	R0 = [FP+8];
	P2 = R0;
	P2 += 4;
	R0 = [P2];
	P2 = [FP+12];
	[P2] = R0;
	R0 = [FP+12];
	P1 = R0;
	P1 += 4;
	P2 = [FP+8];
	R0 = [P2];
	[P1] = R0;
	UNLINK;
	rts;
	.size	_mix_mod, .-_mix_mod
	.align 4
	

////////////////////////////////
/*
extern void mix_voice(
	const fract32* in,
	fract32* out,
	const fract16* mix
*/
	
.global _mix_voice;
.type _mix_voice, STT_FUNC;
	
_mix_voice:
	// push data registers, no stack variables
	LINK 0
	[--sp] = (r7:4, p5:1) 	;
	// i/o arrays
	p5 = r0			; 
	p4 = r1 		;
	// mix array
	p3 = r2			;	
	// loop over voices
	//// FIXME: HW loop too much overhead?
	p2 = 2			;
	loop mix_voice_lp_in lc1 = p2	;
	loop_begin mix_voice_lp_in	;
	// load input value
	r7 = [p5++]		;
	// reset output pointer
	p4 = r1			       ;
	// loop over outputs
	p1 = 4			;
	loop mix_voice_lp_out lc0 = p1	;
	loop_begin mix_voice_lp_out	;
	// load mix value (16b)
	r6 = W [p3++] (X)		;
	// truncate input and multiply
	r5 = r7.h * r6.l ;
	// load output value (32b)
	r4 = [p4]		;
	// add scaled input (saturating)
	r0 = r4 + r5 (S)	;
	// store output to bus
	[p4++] = r0		;
	// end loops
	loop_end mix_voice_lp_out	;
	loop_end mix_voice_lp_in	;
	// pop stack and return
	(r7:4, p5:1) = [sp++] 	;
	UNLINK;
	rts;
	
	.size	_mix_voice, .-_mix_voice
	.align 4


	
.global _mix_adc;
.type _mix_adc, STT_FUNC;
_mix_adc:
	LINK 32;
	[FP+8] = R0;
	[FP+12] = R1;
	[FP+16] = R2;
	P2 = [FP+12];
	R1 = [P2];
	P2 = [FP+8];
	R0 = [P2];
	[FP+-28] = R1;
	[FP+-32] = R0;
	R1 = [FP+-28];
	R0 = [FP+-32];
	R0 = R1 + R0 (S);
	P2 = [FP+12];
	[P2] = R0;
	R0 = [FP+12];
	R0 += 4;
	[FP+12] = R0;
	P2 = [FP+12];
	R1 = [P2];
	P2 = [FP+8];
	R0 = [P2];
	[FP+-20] = R1;
	[FP+-24] = R0;
	R1 = [FP+-20];
	R0 = [FP+-24];
	R0 = R1 + R0 (S);
	P2 = [FP+12];
	[P2] = R0;
	R0 = [FP+12];
	R0 += 4;
	[FP+12] = R0;
	P2 = [FP+12];
	R1 = [P2];
	P2 = [FP+8];
	R0 = [P2];
	[FP+-12] = R1;
	[FP+-16] = R0;
	R1 = [FP+-12];
	R0 = [FP+-16];
	R0 = R1 + R0 (S);
	P2 = [FP+12];
	[P2] = R0;
	R0 = [FP+12];
	R0 += 4;
	[FP+12] = R0;
	P2 = [FP+12];
	R1 = [P2];
	P2 = [FP+8];
	R0 = [P2];
	[FP+-4] = R1;
	[FP+-8] = R0;
	R1 = [FP+-4];
	R0 = [FP+-8];
	R0 = R1 + R0 (S);
	P2 = [FP+12];
	[P2] = R0;
	UNLINK;
	rts;
	.size	_mix_adc, .-_mix_adc
	.local	_i
	.comm	_i,4,4
	.local	_j
	.comm	_j,4,4
	.ident	"GCC: (ADI/svn-5865) 4.3.5"