// Adapted from AMDDCPU.ASM in the 3DNow! SDK

int __declspec(naked) _AMD3D_DetectHardware() {
    __asm {
	push	ebx
	push	edx
	push	ecx
	pushfd
	pushfd			;save EFLAGS to stack
	pop	eax		;store EFLAGS in EAX
	mov	edx, eax	;save in EBX for testing later
	xor	eax, 0200000h	;switch bit 21
	push	eax	;copy "changed" value to stack
	popfd		;save "changed" EAX to EFLAGS
	pushfd
	pop	eax
	cmp	eax, edx
	jz	AMD3D_DH_Done	; CPU doesn't support
				; CPUID instruction

#ifndef	AMD3D_EMULATION
	xor	eax, eax	   ; prepare for CPUID function 0
	cpuid		   ; we expect to get AuthenticAMD
			   ; in ebx,edx,ecx
	cmp	ebx, 'htuA'
	jnz	AMD3D_DH_Done
	cmp	edx, 'itne'
	jnz	AMD3D_DH_Done
	cmp	ecx, 'DMAc'
	jnz	AMD3D_DH_Done

	; Note: add code here to recognize other processor
	;       vendors who support AMD-3D(tm) Technology
	;       and extended function/feature id.

	mov	eax, 080000000h 	; Check for support of
				; extended functions
	cpuid
	test	eax, eax
	jz	AMD3D_DH_Done

				; now we know extended
				; functions are supported
	mov	eax, 080000001h	;prepare for CPUID
				;"Get extended features"
	cpuid
	test	edx, 080000000h	;edx contains extended
				;feature flags bit 31 = 1
				;=> AMD-3D(tm) Technology
	jz	AMD3D_DH_Done
#endif

;  Add code here to enable your AMD-3D Technology specific software
;  Add code here to enable your AMD-3D Technology specific software
;  Add code here to enable your AMD-3D Technology specific software

        mov     eax, 0      ; flag on return AMD-3D H/W is present
        jmp     AMD3D_IsPresent

AMD3D_DH_Done:
        mov     eax, 1          ; AMD-3D H/W not present
AMD3D_IsPresent:
	popfd
	pop	ecx
	pop	edx
	pop	ebx
	ret
    }
}
