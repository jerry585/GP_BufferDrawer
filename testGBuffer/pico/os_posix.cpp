// /* Copyright (C) 2003, 2004 Mihai Preda.
//    See the file LICENSE for license information.
//    http://procod.com/picostorage/
// */
// //#include <gise_config_internal.h>
// //#include <igis_performance.h>
// //#include <gise_pthread.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <errno.h>
// #include <string.h>
// #include <stdio.h>
// 
// //#include <unistd.h>
// //#include <dirent.h>
// 
// //===<- added by hyc.20080220
// #if !defined(__linux) && !defined(_REWORKS) && !defined(_ACOREOS)
// #	define _VXWORKS
// #	include <ioLib.h>
// #endif
// int ioctl(int,int,int);
// //=====
// 
// //GISE_BEGIN_NAMESPACE(pico)
// 
// //===<- added by hyc.20080220
// //VxWorks不支持pread,pwrite，采用以下方法模拟（正确性？）。
// #if defined(_VXWORKS) || defined(_REWORKS) || defined(_ACOREOS)
// int pread(int fd, void *buf, int len, int64 pos)
// {
// 	int64 newPos;
// 	if (!os_seek(fd,pos,SEEK_SET,&newPos))
// 		return -1;
// 	//gisePerformanceLog("vxworks: read");
// 	return read(fd,(char*)buf,len);
// }
// int pwrite(int fd, const void *buf, int len, int64 pos)
// {
// 	int64 newPos;
// 	if (!os_seek(fd,pos,SEEK_SET,&newPos))
// 		return -1;
// 	return write(fd,(char*)buf,len);
// }
// #endif
// //=====
// // ReWorks不支持
// #ifdef _REWORKS
// int ftruncate(int fd, off_t length)
// {
// 	assert(false);return 0;
// }
// int fdatasync(int fd)
// {
// 	assert(false);return 0;
// }
// #endif
// 
// void os_mutex_init(os_pMutex &pmutex) {
//     pmutex = new pthread_mutex_t;
//     pthread_mutex_init((pthread_mutex_t*)pmutex, 0);
// }
// 
// void os_mutex_lock(os_pMutex pmutex) {
//     int ret = pthread_mutex_lock((pthread_mutex_t*)pmutex);
//     assert(ret == 0);
//     (void)ret;
// }
// 
// void os_mutex_unlock(os_pMutex pmutex) {
//     int ret = pthread_mutex_unlock((pthread_mutex_t*)pmutex);
//     assert(ret == 0);
//     (void)ret;
// }
// 
// void os_mutex_destroy(os_pMutex pmutex) {
//     int ret = pthread_mutex_destroy((pthread_mutex_t*)pmutex);
//     assert(ret == 0);
//     (void)ret;
//     delete (pthread_mutex_t*)pmutex;
// }
// 
// 
// bool os_open(const char *path, os_OpenMode openMode, int *pfd, bool* readonly) {
//     int flags = 0;
//     switch(openMode) {
//     case OSMODE_EXISTING:
//         break;
//     case OSMODE_CREATE:
//         flags = O_CREAT | O_EXCL;
//         break;
//     case OSMODE_EXISTING_OR_CREATE:
//         flags = O_CREAT;
//         break;
//     default:
//         assert(0);
//         //flags = O_CREAT | O_TRUNC;
//     }
// 
//     //| O_LARGEFILE
//     *pfd = open(path, flags | O_RDWR, S_IRUSR | S_IWUSR);
//     
//     //added by hyc 20111116. 只读打开
//     *readonly = false;
//     if (*pfd == -1)
//     {
//     	*readonly = true;
//     	*pfd = open(path, flags | O_RDONLY, S_IRUSR | S_IWUSR);
//     }
//     return (*pfd != -1);
// }
// 
// bool os_close(int fd) {
//     return (close(fd) == 0);
// }
// 
// bool os_delete(const char *name) {
//     return (remove(name) == 0) ||
//         (errno == ENOENT);
// }
// 
// bool os_pread(int fd, void *buf, int len, int64 pos, int *lenRead) {
//     ssize_t nRead;
//     do {
//          nRead = pread(fd, buf, len, pos);
//     } while (nRead == -1 && errno == EINTR);
//     if (nRead != -1) {
//         *lenRead = nRead;
//         return true;
//     }
//     return false;
// }
// 
// bool os_pwrite(int fd, const void *buf, int len, int64 pos) {
//     ssize_t nWrite;
//     do {
//         nWrite = pwrite(fd, buf, len, pos);
//     } while (nWrite == -1 && errno == EINTR);
//     if (nWrite != -1) {
//         assert(nWrite == len);
//         return true;
//     }
//     return false;
// }
// 
//     /*
// int os_read(int fd, void *buf, int len, int *lenRead) {
//     ssize_t nRead;
//     do {
//         nRead = read(fd, buf, len);
//     } while (nRead == -1 && errno == EINTR);
//     int ret = 0;
//     if (nRead == -1) {
//         ret = errno; 
//         assert(ret != EAGAIN);
//     } else {
//         *lenRead = nRead;
//     }
//     return ret;
// }
// 
// int os_write(int fd, const void *buf, int len) {
//     ssize_t nWrite;
//     do {
//         nWrite = write(fd, buf, len);
//     } while (nWrite == -1 && errno == EINTR);
//     int ret = 0;
//     if (nWrite == -1) {
//         ret = errno; 
//         assert(ret != EAGAIN);
//     } else {
//         assert(nWrite == len);
//     }
//     return ret;
// }
//     */
// 
// bool os_flush(int fd) {
// //return true;
// 	
// #ifdef _VXWORKS
//   int rc = ioctl(fd, FIOSYNC,0);
//   if (rc != 0)
// 	rc = ioctl(fd, FIOFLUSH,0);
//   return rc == 0;
// #else
//     return (fdatasync(fd) == 0);
// #endif
// 
// }
// 
// bool os_seek(int fd, int64 pos, int whence, int64 *pNewPos) {
// 	gisePerformanceLog("vxworks: lseek");
//     int64 newPos = lseek(fd, pos, whence);
//     if (newPos != -1) {
//         if (pNewPos) {
//             *pNewPos = newPos;
//         }
//         return true;
//     }
//     return false;
// }
// 
// bool os_getSize(int fd, int64 *pSize) {
//     //int64 initialPos;    
//     //return os_seek(fd, 0, SEEK_CUR, &initialPos) &&
//     return os_seek(fd, 0, SEEK_END, pSize);
//     //os_seek(fd, initialPos, SEEK_SET, 0);
// }
// 
// bool os_setSize(int fd, int64 newSize){
//     return (ftruncate(fd, newSize) == 0);
// }
// 
//     int os_getError() {
//         return errno;
//     }
// 
// //GISE_END_NAMESPACE
