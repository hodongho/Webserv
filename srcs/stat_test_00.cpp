#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char *argv[])
{
	struct stat	statbuf;
	
    if (argc != 2)
        return (1);
	if (stat(argv[1], &statbuf) < 0) {
		std::cerr << "stat error" << std::endl;
        exit(1);
    }
    if (S_ISDIR(statbuf.st_mode))
    {
        std::cout << "argv[1] is directory : " << argv[1] << std::endl;
    }
    else if (S_ISREG(statbuf.st_mode))
    {
        std::cout << "argv[1] is Regular file : " << argv[1] << std::endl;
    }
     // : 일반 파일 여부
    // S_ISDIR(buf.st_mode) // : 디렉토리 여부
	return (0);
}



/*
buf
    - 파일의 상태 및 정보를 저장할 buf 구조체

struct stat {
    dev_t     st_dev;     //  ID of device containing file
    ino_t     st_ino;     //  inode number
    mode_t    st_mode;    //  파일의 종류 및 접근권한
    nlink_t   st_nlink;   //  hardlink 된 횟수
    uid_t     st_uid;     //  파일의 owner
    gid_t     st_gid;     //  group ID of owner
    dev_t     st_rdev;    //  device ID (if special file)
    off_t     st_size;    //  파일의 크기(bytes)
    blksize_t st_blksize; //  blocksize for file system I/O
    blkcnt_t  st_blocks;  //  number of 512B blocks allocated
    time_t    st_atime;   //  time of last access
    time_t    st_mtime;   //  time of last modification
    time_t    st_ctime;   //  time of last status change
};

 이 중에서 주요 내용을 보면,
 st_mode : 파일의 종류와 file에 대한 access 권한 정보
   파일의 종류 체크하는 POSIX macro입니다.
   
  S_ISREG(buf.st_mode) : 일반 파일 여부
  S_ISDIR(buf.st_mode) : 디렉토리 여부
  S_ISCHR(buf.st_mode) : character device 여부
  S_ISBLK(buf.st_mode) : block device 여부
  S_ISFIFO(buf.st_mode) : FIFO 여부
  S_ISLNK(buf.st_mode) : symbolic link 여부
  S_ISSOCK(buf.st_mode) : socket 여부 (주로 AF_UNIX로 socket 생성된 경우)

*/