#include "st25dv_util.h"

#define SRV_SUCCESS 0
#define SRV_NO_ANSWER 1
#define SRV_NO_CMD 2
#define SRV_ENV_FAIL -1
#define SRV_SETUP_FAIL -2
#define SRV_ANSWER_FAIL -3
#define SRV_EXIT_SIGINT -4
#define SRV_SIGCATCH_ERR -5

struct srv_info{
	
	char answer_buf[MAILBOX_MEM_SIZE];
	char msg_buffer[MAILBOX_MEM_SIZE];
	char mb_ctrl_state;
	int fd_stdin, fd_stdout;
	int  answer_size;
};

int init_srv(struct st25dv_info *st25dv_data, struct srv_info *srv_data);
int srv_send(struct st25dv_info *st25dv_data, struct srv_info *srv_data);
char is_session_open(struct st25dv_info *st25dv_data, struct srv_info *srv_data);
int get_client_cmd(struct st25dv_info *st25dv_data, struct srv_info *srv_data);
int srv_main_loop(struct st25dv_info *st25dv_data);
