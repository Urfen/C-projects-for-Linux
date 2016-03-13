#include <stdio.h> 
#include <string.h>

int PasswordOkay(){
	char GoodPassword = 'F';
	char Password [8];
	gets (Password) ;
	if ( ! strcmp (Password,"SPOCKSUX"))
		GoodPassword = 'T'; 
	return (GoodPassword == 'T');
}

int main() {
	puts (" Enter Password : ") ;
	if (PasswordOkay())
		puts("Hello , Dr.Bones.");
	else
		puts("Access denied.");
}