

section  .text

global main
main:
	push rbp
	mov rbp,rsp
	sub rsp,32
	mov QWORD [rbp - 8],0
	cmp QWORD [rbp - 8],0
	mov QWORD [rbp - 16],0
	setae BYTE [rbp - 16]
	cmp QWORD [rbp - 16],0
	je __c4_label.2
	mov QWORD [rbp - 8],85
	jmp __c4_label.1
__c4_label.2:
	mov QWORD [rbp - 8],90
__c4_label.1:
	mov r11,QWORD [rbp - 8]
	mov QWORD [rbp - 24],r11
	add QWORD [rbp - 24],10
	mov r11,QWORD [rbp - 24]
	mov QWORD [rbp - 8],r11
	mov r11,QWORD [rbp - 8]
	mov DWORD [rbp - 28],r11d
	mov eax,DWORD [rbp - 28]
	mov rsp,rbp
	pop rbp
	ret
