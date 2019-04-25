#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include<signal.h>
#include "exec_rw_lib.h"
#include "srv_nfc_term.h"

static int var_sigint = SRV_SUCCESS; 

pid_t start_env(char *args[], struct srv_info *srv_data){

	pid_t pid_child;

	if((pid_child = popen_rw(args, &srv_data->fd_stdin, &srv_data->fd_stdout)) < 0){
		printf("popen_rw fail\n");
		return SRV_ENV_FAIL;
	}
	return pid_child;
}

int srv_setup(struct st25dv_info *st25dv_data, struct srv_info *srv_data){

	char mb_mode, sso_state;

	//init srv struct
	srv_data->mb_ctrl_state = 0;
	srv_data->answer_size = 0;
	//open sso to configure sys file
	present_pwd(st25dv_data, default_pwd);
	get_i2c_sso_state(st25dv_data, &sso_state);	
	if(!sso_state){
	        return SRV_ENV_FAIL;
	}
	//configure sys file
	get_mb_mode(st25dv_data, &mb_mode);
	if(!mb_mode){
		mb_mode = MB_EN_ENABLE;
		set_mb_mode(st25dv_data, &mb_mode);
	}
	else{
		mb_mode = MB_EN_DISABLE;
		set_mb_mode(st25dv_data, &mb_mode);
		usleep(2500);
		mb_mode = MB_EN_ENABLE;
		set_mb_mode(st25dv_data, &mb_mode);
	}
	//disable sso
	present_pwd(st25dv_data, bad_pwd);
	return SRV_SUCCESS;
}

int srv_clean(struct st25dv_info *st25dv_data, struct srv_info *srv_data){

	char mb_mode;
	
	mb_mode = MB_EN_DISABLE;
	//open sso to configure sys file
	present_pwd(st25dv_data, default_pwd);
	set_mb_mode(st25dv_data, &mb_mode);
	//disable sso
	present_pwd(st25dv_data, bad_pwd);
	close(srv_data->fd_stdin);
	close(srv_data->fd_stdout);
	return SRV_SUCCESS;
}

int srv_send(struct st25dv_info *st25dv_data, struct srv_info *srv_data){

	int status = 0, retry = 0;
//	char mb_ctrl_state;

	if(!srv_data->answer_size)
		srv_data->answer_size = get_all(srv_data->fd_stdout, srv_data->answer_buf, MAILBOX_MEM_SIZE);
	if(!srv_data->answer_size)
		return SRV_NO_ANSWER;
	//printf("DEBUG: answer_size = %d, mb_ctrl_state = 0x%02x\n", srv_data->answer_size, srv_data->mb_ctrl_state);
	if(srv_data->mb_ctrl_state == (MB_EN_ENABLE | RF_CURRENT_MSG) ||
		srv_data->mb_ctrl_state == MB_EN_ENABLE || srv_data->mb_ctrl_state == (MB_EN_ENABLE | HOST_CURRENT_MSG)){	
		while(status != srv_data->answer_size){
			//printf("DEBUG: answer_size = %d, status = %d, mb_ctrl_state = 0x%02x\n", srv_data->answer_size, status, srv_data->mb_ctrl_state);
			if(retry++ > 3)
				return SRV_ANSWER_FAIL;
			status = write_mailbox_msg(st25dv_data, srv_data->answer_buf, srv_data->answer_size);
		}
		//printf("\nDEBUG: answer write\n");
		srv_data->answer_size = 0;
	}
	return SRV_SUCCESS;
}

//refresh st25dv data
char is_session_open(struct st25dv_info *st25dv_data, struct srv_info *srv_data){

	char mb_ctrl_state = 0;
	
	get_mb_ctrl_state(st25dv_data, &srv_data->mb_ctrl_state);
	return srv_data->mb_ctrl_state;
	//return mb_ctrl_state & MB_EN_ENABLE;
}

int get_client_cmd(struct st25dv_info *st25dv_data, struct srv_info *srv_data){

	int status;
//	char mb_ctrl_state;

//	get_mb_ctrl_state(st25dv_data, &mb_ctrl_state);
	if(srv_data->mb_ctrl_state & RF_PUT_MSG){
		status = -1;
		while(status == -1)
			status = read_mailbox_msg(st25dv_data, srv_data->msg_buffer);
		srv_data->msg_buffer[status] = '\n';
		status++;
		srv_data->msg_buffer[status] = '\0';
		printf("=====>%s", srv_data->msg_buffer);
		write(srv_data->fd_stdin, srv_data->msg_buffer, status);
		return status;
	}
	return SRV_NO_CMD;
}

void sig_handler(int signo)
{
	if (signo == SIGINT)
	        var_sigint = SRV_EXIT_SIGINT;
}

int srv_main_loop(struct st25dv_info *st25dv_data){
	
	char *args[]={"bash",  NULL};
	struct srv_info srv_data;
	
	if (signal(SIGINT, sig_handler) == SIG_ERR)
		return SRV_SIGCATCH_ERR;
	if(start_env(args, &srv_data) == SRV_ENV_FAIL)
		return SRV_ENV_FAIL;
	if(srv_setup(st25dv_data, &srv_data) != SRV_SUCCESS)
		return SRV_SETUP_FAIL;
wait_session:
	while(!is_session_open(st25dv_data, &srv_data) && var_sigint != SRV_EXIT_SIGINT)
		usleep(50000);
	printf("DEBUG: session open\n");
	while(var_sigint != SRV_EXIT_SIGINT){
		srv_send(st25dv_data, &srv_data);
		usleep(25000);
		if(!is_session_open(st25dv_data, &srv_data)){
			printf("DEBUG: session close\n");
			goto wait_session;
		}
		get_client_cmd(st25dv_data, &srv_data);
		usleep(50000);
	}
	srv_clean(st25dv_data, &srv_data);
	return var_sigint;
}
