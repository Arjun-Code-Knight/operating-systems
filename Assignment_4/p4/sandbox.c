/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/ucontext.h>
#include <stdio.h>
#include <stdlib.h>

extern void readString(char *s, int r);

unsigned long returnAddress = 0;
unsigned long startAddress = 0;

void signal_handler(int signal, siginfo_t *siginfo, void *u_context)
{
        if (returnAddress == 0) {
		/* Function call - save start and return address */
		startAddress = (unsigned long) siginfo->si_addr;
                returnAddress  = (unsigned long) *(unsigned long**) ((ucontext_t *)u_context)->uc_mcontext.gregs[15];
		
		/* Unprotect the page where the function resides */          	
		mprotect((void*)(startAddress & 0xfffff000), 1,PROT_WRITE | PROT_EXEC | PROT_READ);
		
		/* Protect the page from where the function call is made */
                mprotect((void*)(returnAddress & 0xfffff000), 1, PROT_NONE);

        } else if ((unsigned long) siginfo->si_addr == returnAddress) { /* Upon returning, check the return address is correct or not */

                /* Unprotect the page containing return address */
                mprotect((void*)(returnAddress & 0xfffff000), 1, PROT_WRITE | PROT_EXEC | PROT_READ);
		
		/* Protect the page from where the function is returning */
                mprotect((void*)(startAddress & 0xfffff000), 1, PROT_NONE);
		
 		/* Make return and start address 0 for the next call */
                returnAddress = 0;
		startAddress = 0;
        } else {
		/* Return address didn't match - malicious code */
                printf("malicious buffer overflow detected, will exit!\n");
                exit(0);
        }
}

void init_sandbox()
{
	/* Initializing a signal handler to catch SIGSEGV */
        struct sigaction sa_segfault;
	sa_segfault.sa_sigaction = signal_handler;
        sigemptyset(&sa_segfault.sa_mask);
	sigaddset(&sa_segfault.sa_mask,SIGSEGV);
        sa_segfault.sa_flags = SA_RESTART | SA_SIGINFO;
        sigprocmask(SIG_UNBLOCK,&sa_segfault.sa_mask,NULL);
        sigaction(SIGSEGV,&sa_segfault,NULL);
        mprotect((void*)((unsigned long)readString & 0xfffff000), 1,PROT_READ | PROT_WRITE/* | PROT_EXEC */); // cannot execute !!!
}
