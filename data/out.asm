global _start
_start:
	mov rax, 1
	push rax
	mov rax, 2
	push rax
	mov rax, 3
	push rax
	mov rax, 4
	push rax
	mov rax, 5
	push rax
	; Add
	pop rax
	pop rbx
	add rax, rbx
	push rax
	; Add
	pop rax
	pop rbx
	add rax, rbx
	push rax
	; Add
	pop rax
	pop rbx
	add rax, rbx
	push rax
	; Add
	pop rax
	pop rbx
	add rax, rbx
	push rax
	mov rax, 8
	push rax
	; Call var: x
	push QWORD [rsp + 8]
	; Add
	pop rax
	pop rbx
	add rax, rbx
	push rax
	; Call var: y
	push QWORD [rsp + 0]
	; Call var: x
	push QWORD [rsp + 16]
	; Add
	pop rax
	pop rbx
	add rax, rbx
	push rax
	; Call var: x
	push QWORD [rsp + 16]
	; Call var: y
	push QWORD [rsp + 16]
	mov rax, 2
	push rax
	; Add
	pop rax
	pop rbx
	add rax, rbx
	push rax
	; Add
	pop rax
	pop rbx
	add rax, rbx
	push rax
	; Call var: z
	push QWORD [rsp + 0]
	; Call var: u
	push QWORD [rsp + 16]
	; Call var: x
	push QWORD [rsp + 40]
	; Call var: y
	push QWORD [rsp + 40]
	; Add
	pop rax
	pop rbx
	add rax, rbx
	push rax
	; Add
	pop rax
	pop rbx
	add rax, rbx
	push rax
	; Add
	pop rax
	pop rbx
	add rax, rbx
	push rax
	; Call var: t
	push QWORD [rsp + 0]
	; Exit by var.
	mov rax, 60
	pop rdi
	syscall
	mov rax, 60
	mov rdi, 0
	syscall
