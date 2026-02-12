nasm -felf64 test.asm -o test.o

echo " C file  running"
echo ""
echo ""

gcc test.c -o test
./test
echo $?
exit

echo "----------------"
echo ""
echo ""
echo "C4 file running "
gcc test.o -o test
./test



echo $?
