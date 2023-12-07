	.text

	.extern correct		;Everything Im using not already in here
	.extern play4Csharp 		
	.extern play4D
	.extern play4Fsharp
	.extern play4G
	.extern play4A
	.extern play5C

JT:                             ;Jump Table setup
	.word default
	.word case1
	.word case2
	.word case3
	.word case4
	.word case5

	.global playSound
	
playSound:                      ;Function that will use jump table
	mov &correct, r12
	cmp #6, r12
	jge default
	add r12, r12		;Multiplying by two to get address correct
	mov JT(r12), r0
	
case1:                          ;Cases to play certain sounds based on correct
	call #play4Csharp
	jmp esac
	
case2:
	call #play4D
	jmp esac
	
case3:
	call #play4Fsharp
	jmp esac
	
case4:
	call #play4G
	jmp esac
	
case5:
	call #play4A
	jmp esac
	
default:
	call #play5C
	jmp esac

esac:                          	;Hopefully will return
	pop r0
	
