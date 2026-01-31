

section  .text

global main
main:
	push rbp
	mov rbp,rsp
	sub rsp,8
	mov DWORD [rbp - 4],0
	mov DWORD [rbp - 4],90
	mov r11d,DWORD [rbp - 4]
	mov DWORD [rbp - 8],r11d
	mov eax,DWORD [rbp - 8]
	mov rsp,rbp
	pop rbp
	ret
