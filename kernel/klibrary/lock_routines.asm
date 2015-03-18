; *******************************
; void spin_lock(uint32 * p);
;
global __kspin_lock
__kspin_lock:
    push ebp
    mov ebp, esp        ; we might as well the the stack frame right?

    mov eax, [ebp + 8]  ; eax contains address of uint32 used for the lock
    mov ecx, 1

.tryacquire             ; try and get the lock.
    xchg ecx, [eax]
    test ecx, ecx
    jz .acquired        ; if the lock wasn't 0 (unlocked) repeat.

.pauseloop
    pause               ; see 'About PAUSE' below
    test dword [eax], 1
    jne .pauseloop
    jmp .tryacquire

.acquired
    pop ebp
    ret

; About PAUSE
;
; After a few iterations the branch predictor will predict that the
; conditional branch (3) will never be taken and the pipeline will
; fill with CMP instructions (2). This goes on until finally another
; processor writes a zero to lockvar. At this point we have the
; pipeline full of speculative (i.e. not yet committed) CMP instructions
; some of which already read lockvar and reported an (incorrect)
; nonzero result to the following conditional branch (3) (also
; speculative). This is when the memory order violation happens.
; Whenever the processor "sees" an external write (a write from another
; processor), it searches in its pipeline for instructions which
; speculatively accessed the same memory location and did not yet
; commit. If any such instructions are found then the speculative
; state of the processor is invalid and is erased with a pipeline
; flush. - from Stack Overflow.

