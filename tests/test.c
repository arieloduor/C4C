enum OpenFlag
{
	OpenFlag_O_RDONLY = 0,
	OpenFlag_O_WRONLY = 1,
	OpenFlag_O_RDWR = 2,
	OpenFlag_O_CREAT = 64,
	OpenFlag_O_EXCL = 128,
	OpenFlag_O_NOCTTY = 256,
	OpenFlag_O_TRUNC = 512,
	OpenFlag_O_APPEND = 1024,
	OpenFlag_O_NONBLOCK = 2048,
	OpenFlag_O_DSYNC = 4096,
	OpenFlag_O_SYNC = 1052672,
	OpenFlag_O_RSYNC = 1052672,
	OpenFlag_O_DIRECTORY = 65536,
	OpenFlag_O_NOFOLLOW = 131072,
	OpenFlag_O_CLOEXEC = 524288,
};

int open(char  *__c4internal_name_c4_tmp_0,int __c4internal_flags_c4_tmp_1,int __c4internal_mode_c4_tmp_2);
int creat(char  *__c4internal_name_c4_tmp_3,int __c4internal_mode_c4_tmp_4);
long int read(int __c4internal_fd_c4_tmp_5,char  *__c4internal_buf_c4_tmp_6,unsigned long int __c4internal_buflen_c4_tmp_7);
long int write(int __c4internal_fd_c4_tmp_8,char  *__c4internal_buf_c4_tmp_9,unsigned long int __c4internal_buflen_c4_tmp_10);
int close(int __c4internal_fd_c4_tmp_11);
int puts(char  *__c4internal_str_c4_tmp_12);
void *malloc(unsigned long int __c4internal_size_c4_tmp_13);
void free(void  *__c4internal_ptr_c4_tmp_14);
int main()
{
	int fd = open("test.rs",OpenFlag_O_RDONLY,0);
	char *buf = malloc(1000);
	read(fd,buf,1000);
	puts(buf);
	free(buf);
	return 0;
}
