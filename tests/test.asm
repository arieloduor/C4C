

section  .text

global main
main:
	push rbp
	mov rbp,rsp
	sub rsp,80
	mov DWORD [rbp - 4],90
	mov DWORD [rbp - 8],89
	lea r11,DWORD [rbp - 4]
	mov QWORD [rbp - 16],r11
	mov r11,QWORD [rbp - 16]
	mov QWORD [rbp - 24],r11
	lea r11,DWORD [rbp - 8]
	mov QWORD [rbp - 32],r11
	mov r11,QWORD [rbp - 32]
	mov QWORD [rbp - 40],r11
	lea r11,QWORD [rbp - 24]
	mov QWORD [rbp - 48],r11
	mov r11,QWORD [rbp - 48]
	mov QWORD [rbp - 56],r11
	lea r11,QWORD [rbp - 40]
	mov QWORD [rbp - 64],r11
	mov r11,QWORD [rbp - 64]
	mov QWORD [rbp - 72],r11
	mov rax,QWORD [rbp - 56]
	mov r11,QWORD [rax - 0]
	mov QWORD [rbp - 80],r11
	mov rax,QWORD [rbp - 80]
	mov QWORD [rax - 0],165
	mov eax,DWORD [rbp - 4]
	mov rsp,rbp
	pop rbp
	ret
