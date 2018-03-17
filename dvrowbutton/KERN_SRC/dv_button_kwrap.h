/*
 * dv_button_kwrap.h
 *
 *  Created on: Sep 9, 2016
 *      Author: vishwajith
 */

#ifndef DV_BUTTON_KWRAP_H_
#define DV_BUTTON_KWRAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <sys/types.h>


#define PQ_BUTTON_SIG_ID 44

int fd_button ;
bool is_button_open;


void interrupt_handler(int n, siginfo_t *info, void *not_used){
	printf("Button value revieved %d",info->si_int);
}

int init_buttton_module(){
	fd_button = open("/dev/dvbutton",O_RDWR);
	if(fd_button < 0){
		printf("Unable to open button devoice\n");
		return -1 ;
	}

	struct sigaction sig ;
	sig.sa_sigaction = interrupt_handler ;
	sig.sa_flags = SA_SIGINFO ;
	sigaction(PQ_BUTTON_SIG_ID,&sig ,NULL);

	int pid = getpid();

	if(write(fd_button,&pid , 4) < 0){
		printf("Cant write pid to the driver\n");
		return -1 ;
	}
	is_button_open = true ;

}

int close_button(){
	if(is_button_open){
		if( close(fd_button) < 0 ){
			printf("Cant close file\n");
			return -1;
		}
		return 0 ;
	}
	return 0 ;
}

#endif /* DV_BUTTON_KWRAP_H_ */
