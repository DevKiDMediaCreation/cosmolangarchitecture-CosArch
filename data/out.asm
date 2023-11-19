global _start
_start:
	mov rax, 7
	push rax
	mov rax, 8
	push rax
	mov rax, 9
	push rax
	mov rax, 256
	push rax
	; Call var: y
	push QWORD [rsp + 16]
	; Call var: x
	push QWORD [rsp + 32]
	; Exit by var.
	mov rax, 60
	pop rdi
	syscall
	mov rax, 60
	mov rdi, 0
	syscall
