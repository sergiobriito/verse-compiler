section .text
global _start
extern printf
extern exit
_start:

mov byte [i],0

for_loop_label_1:
push dword [i]
push dword fmt
call printf
add byte [i], 1
mov eax, [i]
mov ebx, 10
cmp eax, ebx
jl for_loop_label_1

jmp end_for_loop_1

end_for_loop_1:
push dword name
call printf
push dword [n]
push dword fmt
call printf
mov eax, [n]
mov ebx, 100
cmp eax, ebx
jg if_label_2

jmp else_label_2

if_label_2:
push dword good
call printf
jmp end_if_label_2

else_label_2:
push dword bad
call printf
jmp end_if_label_2


end_if_label_2:

call exit

section .data
bad db "BAD",10,0
bad_len equ $ - bad
good db "GOOOD",10,0
good_len equ $ - good
i dd 0
n dd 203
name db "sergio",10,0
name_len equ $ - name
fmt db "%d", 10, 0
