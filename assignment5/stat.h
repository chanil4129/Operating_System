#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Device
// 20182601
// Continuos Sector based File
// CS 기반 파일을 생성하기 위해 fcntl.h에 플래그 추가
#define T_CS   4   

struct stat {
  short type;  // Type of file
  int dev;     // File system's disk device
  uint ino;    // Inode number
  short nlink; // Number of links to file
  uint size;   // Size of file in bytes
};
