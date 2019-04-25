#define GPO_REG 0x00
#define IT_TIME_REG 0x01
#define EH_MODE_REG 0x02
#define RF_MNGT_REG 0x03
#define RFA1SS_REG 0x04
#define ENDA1_REG 0x05
#define RFA2SS_REG 0x06
#define ENDA2_REG 0x07
#define RFA3SS_REG 0x08
#define ENDA3_REG 0x09
#define RFA4SS_REG 0x0A
#define I2CSS_REG 0x0B
#define LOCK_CCFILE_REG 0x0C
#define MB_MODE_REG 0x0D
#define MB_WDG_REG 0x0E
#define LOCK_CFG_REG 0x0F
#define LOCK_DFSID_REG 0x10
#define LOCK_AFI_REG 0x11
#define DSFID_REG 0x12
#define AFI_REG 0x13
#define MEM_SIZE_REG 0x14
#define BLK_SIZE_REG 0x16
#define IC_REF_REG 0x17
#define UID_REG 0x18
#define IC_REV_REG 0x20
#define ST_RESERVED1_REG 0x21
#define ST_RESERVED2_REG 0x22
#define ST_RESERVED3_REG 0x23

#define GPO_CTRL_DYN_REG 0x00
#define ST_RESERVED_DYN_REG 0x01
#define EH_CTRL_DYN_REG 0x02
#define RF_MNGT_DYN_REG 0x03
#define I2C_SSO_DYN_REG 0x04
#define IT_STS_DYN_REG 0x05
#define MB_CTRL_DYN_REG 0x06
#define MB_LEN_DYN_REG 0x07

#define UID_SIZE 0x08
#define PWD_SIZE 0x08
#define MEM_LEN 0x02
#define SYSTEM_AREA_SIZE 0x24
#define DYN_REG_AREA_SIZE 0x08

#define MAILBOX_MEM_SIZE 30

/*Dyn ctrl mask*/
#define MB_EN_DISABLE 0x00
#define MB_EN_ENABLE 0x01
#define HOST_PUT_MSG 0x02
#define RF_PUT_MSG 0x04
#define HOST_MISS_MSG 0x10
#define RF_MISS_MSG 0x20
#define HOST_CURRENT_MSG 0x40
#define RF_CURRENT_MSG 0x80

struct FILE;

enum area_type{
	USER_AREA = 0,
	SYS_AREA = 1,
	DYN_REG_AREA = 2,
	MAILBOX_AREA = 3,
};

struct st25dv_info{
	char bus_id;
	FILE *area[4];
	char *i2c_path;
};

char default_pwd[PWD_SIZE];
char bad_pwd[PWD_SIZE];

int init_st25dv(const char *bus_id ,struct st25dv_info *st25dv_data);
void clear_st25dv(struct st25dv_info *st25dv_data);
int read_bytes_area(struct st25dv_info *st25dv_data, size_t count, int offset, char *ptr, enum area_type area);
int write_bytes_area(struct st25dv_info *st25dv_data, size_t count, int offset, char *ptr, enum area_type area);
int read_uid(struct st25dv_info *st25dv_data, char *ptr);
int present_pwd(struct st25dv_info *st25dv_data, char *ptr);
int write_pwd(struct st25dv_info *st25dv_data, char *ptr);
int set_mb_mode(struct st25dv_info *st25dv_data, char *mode);
int get_mb_mode(struct st25dv_info *st25dv_data, char *mb_state);
int get_i2c_sso_state(struct st25dv_info *st25dv_data, char *i2c_sso_state);
int get_mb_ctrl_state(struct st25dv_info *st25dv_data, char *mb_ctrl_state);
int get_mb_len_state(struct st25dv_info *st25dv_data, char *mb_len_state);
int init_mailbox(struct st25dv_info *st25dv_data);
int read_mailbox_msg(struct st25dv_info *st25dv_data, char *buffer);
int write_mailbox_msg(struct st25dv_info *st25dv_data, char *buffer, size_t count);
size_t generate_text_ndef_record(char *input_ptr, size_t size_input, char *output_ptr, size_t max_output_size);
