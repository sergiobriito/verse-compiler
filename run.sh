rm -rf out out.o out.asm 
make run
nasm -f elf32 out.asm -o out.o
gcc -m32 -o out out.o -lm -nostartfiles
./out