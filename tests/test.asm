

section  .text

global main
main:
	push rbp
	mov rbp,rsp
	sub rsp,8
	mov DWORD [rbp - 4],21
	mov eax,DWORD [rbp - 4]
	mov rsp,rbp
	pop rbp
	ret
