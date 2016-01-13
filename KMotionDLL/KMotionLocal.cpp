// KMotionLocal.cpp: implementation of the CKMotionLocal class.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "StdAfx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/mman.h>

#ifdef _KMOTIONX
static const char *memname = "kmxShared";
static const size_t region_size = sysconf(_SC_PAGE_SIZE);
static void *ptr;
static void error_and_die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

CKMotionLocal KMotionLocal;  // declare one global instance 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CKMotionLocal::CKMotionLocal()
{
  int fd = shm_open(memname, O_CREAT | O_EXCL | O_RDWR, 0666);
  if (fd == -1){
    if(errno == EEXIST){
      fd = shm_open(memname, O_RDWR, 0666);
    }
    if (fd == -1){
      error_and_die("shm_open");
    }
  } else {
    if (ftruncate(fd, region_size) != 0){
      error_and_die("ftruncate");
    } else {
      //log_info("Success");
    }
  }

  ptr = mmap(0, region_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED) {
    error_and_die("mmap");
  }

  close(fd);

  sharePtr = (u_int32_t *) ptr;
  (*sharePtr)++;
}

CKMotionLocal::~CKMotionLocal()
{
  (*sharePtr)--;
  log_info("share= %d", *sharePtr);
  //TODO if shared memory is not unmapped we get problems on next run
  if((*sharePtr) < 1){
    log_info("DEMAPPING");
    if (munmap(ptr, region_size) != 0)
      perror("munmap");

    if (shm_unlink(memname) != 0)
      perror("shm_unlink");

  }
}
#else
#pragma data_seg (".SHARED") // any name you like

int volatile share=0;
#pragma data_seg ()

#pragma comment(linker, "/section:.SHARED,RWS")

CKMotionLocal KMotionLocal;  // declare one global instance

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CKMotionLocal::CKMotionLocal()
{

}

CKMotionLocal::~CKMotionLocal()
{

}
#endif
