

section  .text

global main
main:
	push rbp
	mov rbp,rsp
	sub rsp,16
	mov DWORD [rbp - 4],10
	mov r11d,DWORD [rbp - 4]
	mov DWORD [rbp - 8],r11d
	add DWORD [rbp - 8],20
	mov r11d,DWORD [rbp - 8]
	mov DWORD [rbp - 12],r11d
	mov eax,DWORD [rbp - 12]
	mov rsp,rbp
	pop rbp
	ret
