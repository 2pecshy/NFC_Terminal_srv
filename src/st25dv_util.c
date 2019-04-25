#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include "st25dv_util.h"

char default_pwd[PWD_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char bad_pwd[PWD_SIZE] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
int init_st25dv(const char *bus_id ,struct st25dv_info *st25dv_data){

	int bus_id_len = strlen(bus_id);
	char user_area_path[] = "/sys/bus/i2c/devices/*-0053/st25dv_user";
	char mailbox_area_path[] = "/sys/bus/i2c/devices/*-0053/st25dv_mailbox";
	char sys_area_path[] = "/sys/bus/i2c/devices/*-0057/st25dv_sys";
	char dyn_reg_path[] = "/sys/bus/i2c/devices/*-0053/st25dv_dyn_reg";
	char *tmp;
	
	if(bus_id_len > 1)
		return 1;
        st25dv_data->bus_id = *bus_id;
	tmp = strchr(user_area_path, '*');
	*tmp = *bus_id;
	tmp = strchr(mailbox_area_path, '*');
	*tmp = *bus_id;
        tmp = strchr(sys_area_path, '*');
	*tmp = *bus_id;
	tmp = strchr(dyn_reg_path, '*');
	*tmp = *bus_id;
	st25dv_data->area[USER_AREA] = fopen(user_area_path, "rb+");
	if(st25dv_data->area[USER_AREA] == NULL)
		return 1;
	st25dv_data->area[MAILBOX_AREA] = fopen(mailbox_area_path, "rb+");
	if(st25dv_data->area[MAILBOX_AREA] == NULL){
		fclose(st25dv_data->area[USER_AREA]);
		return 1;
	}
	st25dv_data->area[SYS_AREA] = fopen(sys_area_path, "rb+");
	if(st25dv_data->area[SYS_AREA] == NULL){
		fclose(st25dv_data->area[USER_AREA]);
		fclose(st25dv_data->area[MAILBOX_AREA]);
		return 1;
	}
	st25dv_data->area[DYN_REG_AREA] = fopen(dyn_reg_path, "rb+");
	if(st25dv_data->area[DYN_REG_AREA] == NULL){
		fclose(st25dv_data->area[USER_AREA]);
		fclose(st25dv_data->area[MAILBOX_AREA]);
		fclose(st25dv_data->area[SYS_AREA]);
		return 1;
	}
	return 0;
}

void clear_st25dv(struct st25dv_info *st25dv_data){
	fclose(st25dv_data->area[USER_AREA]);
	fclose(st25dv_data->area[MAILBOX_AREA]);
	fclose(st25dv_data->area[SYS_AREA]);
	fclose(st25dv_data->area[DYN_REG_AREA]);
}

int read_bytes_area(struct st25dv_info *st25dv_data, size_t count, int offset, char *ptr, enum area_type area){
	int fd = fileno(st25dv_data->area[area]);
	if(lseek(fd, offset, SEEK_SET) != offset)
		return -1;
	return read(fd, ptr, count);
}

int write_bytes_area(struct st25dv_info *st25dv_data, size_t count, int offset, char *ptr, enum area_type area){
	int fd = fileno(st25dv_data->area[area]);
	int tmp = lseek(fd, offset, SEEK_SET);
	if(tmp != offset){
		//printf("debug:write_bytes_area: %d\n", tmp);
		return -1;
	}
	return write(fd, ptr, count);
}

int present_pwd(struct st25dv_info *st25dv_data, char *ptr){

	char present_pwd_path[] = "/sys/bus/i2c/devices/*-0057/st25dv_present_pwd";
	char *tmp;
	FILE *present_pwd_fd;

	tmp = strchr(present_pwd_path, '*');
	*tmp = st25dv_data->bus_id;
	present_pwd_fd = fopen(present_pwd_path, "w");
	fwrite (ptr, sizeof(char), PWD_SIZE, present_pwd_fd);
	fclose(present_pwd_fd);
	return 0;
}

int write_pwd(struct st25dv_info *st25dv_data, char *ptr){

	char write_pwd_path[] = "/sys/bus/i2c/devices/*-0057/st25dv_write_pwd";
	char *tmp;
	FILE *write_pwd_fd;

	tmp = strchr(write_pwd_path, '*');
	*tmp = st25dv_data->bus_id;
	write_pwd_fd = fopen(write_pwd_path, "w");
	fwrite (ptr, sizeof(char), PWD_SIZE, write_pwd_fd);
	fclose(write_pwd_fd);
	return 0;
}

int read_uid(struct st25dv_info *st25dv_data, char *ptr){ 
	return read_bytes_area(st25dv_data, UID_SIZE, UID_REG, ptr, SYS_AREA);
}

int set_mb_mode(struct st25dv_info *st25dv_data, char *mode){
	return write_bytes_area(st25dv_data, 1, MB_MODE_REG, mode, SYS_AREA);
}

int get_mb_mode(struct st25dv_info *st25dv_data, char *mb_state){
       return read_bytes_area(st25dv_data, 1, MB_MODE_REG, mb_state, SYS_AREA);
}

int get_i2c_sso_state(struct st25dv_info *st25dv_data, char *i2c_sso_state){
	return read_bytes_area(st25dv_data, 1, I2C_SSO_DYN_REG, i2c_sso_state, DYN_REG_AREA);
}

int get_mb_ctrl_state(struct st25dv_info *st25dv_data, char *mb_ctrl_state){
	return read_bytes_area(st25dv_data, 1, MB_CTRL_DYN_REG, mb_ctrl_state, DYN_REG_AREA);
}

int get_mb_len_state(struct st25dv_info *st25dv_data, char *mb_len_state){
	return read_bytes_area(st25dv_data, 1, MB_LEN_DYN_REG, mb_len_state, DYN_REG_AREA);
}

int set_mb_ctrl_state(struct st25dv_info *st25dv_data, char *mb_ctrl_state){
	return write_bytes_area(st25dv_data, 1, MB_CTRL_DYN_REG, mb_ctrl_state, DYN_REG_AREA);
}

int init_mailbox(struct st25dv_info *st25dv_data){
	char mb_mode_enable = 0x01;
	char mb_state, mb_len;
	char ctrl_state_enable = 0x01;
	set_mb_mode(st25dv_data, &mb_mode_enable);
	get_mb_mode(st25dv_data, &mb_state);
	if((mb_state & 1) == 0)
		return -1;
	set_mb_ctrl_state(st25dv_data, &ctrl_state_enable);
	get_mb_len_state(st25dv_data, &mb_len);
	if(mb_len != 0x00)
		return -1;
	return 0;
}

int read_mailbox_msg(struct st25dv_info *st25dv_data, char *buffer){
	char mb_len;
	int status;
	get_mb_len_state(st25dv_data, &mb_len);
	mb_len++;
	//printf("\nmb_len = %d\n", mb_len);
	status = read_bytes_area(st25dv_data, mb_len, 0, buffer, MAILBOX_AREA);
	if(status != mb_len)
		return -1;
	return mb_len;
}

int write_mailbox_msg(struct st25dv_info *st25dv_data, char *buffer, size_t count){
	char mb_len;
	int status;
	status = write_bytes_area(st25dv_data, count, 0, buffer, MAILBOX_AREA);
	//printf("debug:status = %d\n", status);
	if(status != count)
		return -1;
	get_mb_len_state(st25dv_data, &mb_len);
	//printf("status = %d, mb_len = 0x%02x\n", status, mb_len);
	if(mb_len+1 != count)
		return -1;
	return status;
}

size_t generate_text_ndef_record(char *input_ptr, size_t size_input, char *output_ptr, size_t max_output_size){

	char cc_header[5] = {0xE1, 0x40, 0x40, 0x05, 0x03};
	char ndef_len[1] = {0x00};
	char ndef_header[2] = {0xD1, 0x01};
	char payload_len[1] = {0x00};
	char lang[2] = {0x65, 0x6E};
	char ndef_type[2] = {0x54, 0x02};
	char *current_output_ptr;
	uint16_t ndef_length;
        size_t output_size;

	if(size_input <= 0)
		return 0;
	output_size = 5 + 1 + 2 + 1 + 2 + 2 + size_input + 1;
	if(output_size > max_output_size)
		return 0;
	payload_len[0] = size_input + 2 + 1;
	ndef_length = 2 + 1 + 2 + 2 + size_input;
	ndef_len[0] = ndef_length;
	current_output_ptr = output_ptr;
	memcpy(current_output_ptr, cc_header, 5);
        current_output_ptr += 5;
	memcpy(current_output_ptr, ndef_len, 1);
	current_output_ptr += 1;
	memcpy(current_output_ptr, ndef_header, 2);
	current_output_ptr += 2;
	memcpy(current_output_ptr, payload_len, 1);
	current_output_ptr += 1;
	memcpy(current_output_ptr, ndef_type, 2);
	current_output_ptr += 2;
	memcpy(current_output_ptr, lang, 2);
	current_output_ptr += 2;
	memcpy(current_output_ptr, input_ptr, size_input);
	current_output_ptr += size_input;
	*current_output_ptr = 0xFE;
	return output_size;
}
