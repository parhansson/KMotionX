// KMotionLocal.cpp: implementation of the CKMotionLocal class.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#ifdef _KMOTIONX
#include "StdAfx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/mman.h>


static const char *memname = "kmxShared";
static const size_t region_size = sysconf(_SC_PAGE_SIZE);
static void *ptr;
static void error_and_die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}
int volatile share=0;
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
  share = *sharePtr;
}

CKMotionLocal::~CKMotionLocal()
{
  (*sharePtr)--;
  share = *sharePtr;
  log_info("share = %d", *sharePtr);
  if((*sharePtr) < 1){
    log_info("DEMAPPING");
    if (munmap(ptr, region_size) != 0)
      perror("munmap");

    if (shm_unlink(memname) != 0)
      perror("shm_unlink");

  } else {
    //if shared memory is not unmapped we might get problems on next run
    log_info("To reset shared memory launch server with reset option 'KMotionServer -reset'");
  }
}
#else
#include "stdafx.h"

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
