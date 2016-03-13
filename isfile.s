.data

string:
	#.asciz "/etc/passwd" #Path
	#.asciz "/etc/pm" #Path
	.asciz "/media/sf_Sharedfolder/gas/isfile.ss" #Path

sb: 
	.skip 64 			#Allocate 64 Byte for the stat information
	

.text	

	.global main		#Name of startpoint for the linker.

main:


	mov $106, %eax 		#Move stat syscall in to eax
	mov $string, %ebx	#First argument, path
	mov $sb, %ecx		#Second argument, the buffer
	int $0x80			#Intr the core.


	movl %eax, %ebx		#Put result from eax as the fist argument
	mov $1, %eax		#Exit 
	int $0x80			#Intr the core.



#stat(const char *pathname, struct stat *buf)
#https://en.wikibooks.org/wiki/X86_Assembly/Data_Transfer
#/usr/include/x86_64-linux-gnu/asm/unistd_32.h