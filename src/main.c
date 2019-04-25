#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "srv_nfc_term.h"

int main(int argc, char *argv[]){

	struct st25dv_info st25dv_data;
	int status;
	
	if(init_st25dv("1" , &st25dv_data)){
		printf("fail to init st25dv\n");
		return 1;
	}
	status = srv_main_loop(&st25dv_data);
	switch(status){
	case SRV_ENV_FAIL:
		printf("Fail to start shell\n");
		break;
	case SRV_SETUP_FAIL:
		printf("Fail to setup the st25dv\n");
		break;
	case SRV_EXIT_SIGINT:
		printf("Exit by signal sigint\n");
		break;
	case SRV_SIGCATCH_ERR:
		printf("Can't catch SIGINT\n");
		break;
	case SRV_SUCCESS:
		printf("exit without error\n");
		break;
	default:
		printf("undefined error.\n");	
	}
	return 0;
}
