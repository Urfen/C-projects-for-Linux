	.data
string :
	.ascii "Hello, World!\n"
	.text
	.global main
main :
	mov $4, %eax
	mov $1, %ebx
	mov $string, %ecx
	mov $14, %edx
	int $0x80
