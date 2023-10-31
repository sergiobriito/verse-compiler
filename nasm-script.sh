nasm -f elf out.asm -o out.o
gcc -m32 -o out out.o -lm -nostartfiles
./out