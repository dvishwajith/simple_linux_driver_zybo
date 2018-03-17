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
#include <stdbool.h>

#include <sys/types.h>


#define PQ_BUTTON_SIG_ID 45

int fd_button ;
bool is_button_open;
struct sigaction sig ;

static void interrupt_recieved(int n, siginfo_t *info, void *not_used){
	// puts("Button value revieved ");/
	// for (int i = 0; i < 100; ++i)
	// {
		printf("Button value revieved\n");
	// }
}

void signal_handler_func(int sig){
	printf("Button value revieved ");
}

static int init_buttton_module(){
	fd_button = open("/dev/dvbutton",O_RDWR);
	if(fd_button < 0){
		// printf("Unable to open button devoice\n");
		printf("Unable to open button devoice\n");
		return -1 ;
	}

	printf("Signal initialisation \n");

	
	sig.sa_sigaction = &interrupt_recieved ;
	sig.sa_flags = SA_SIGINFO ;
//	signal(PQ_BUTTON_SIG_ID,&signal_handler_func);
	if(sigaction(PQ_BUTTON_SIG_ID,&sig ,NULL)){
		printf("Error:init_buttton_module:: sigaction function return error\n");
	}else{
		printf("sigction done\n");
	}

	printf("taking pid of function \n");
	int pid = getpid();
	
	printf("sending pid of function %d\n",pid);


	if(write(fd_button,&pid , 4) < 0){
		printf("Cant write pid to the driver\n");
		return -1 ;
	}
	is_button_open = true ;
	printf("button pid write and initialisation done\n");
	return 0 ;
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
