section .text
global _start
extern printf
extern exit
_start:

push dword [n]
push dword fmt
call printf

call exit

section .data
n dd 30
fmt db "%d", 10, 0
