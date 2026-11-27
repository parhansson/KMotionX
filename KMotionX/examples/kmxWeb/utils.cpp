/*
 * utils.cpp
 *
 *  Created on: 22 maj 2015
 *      Author: parhansson
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <unistd.h> // getpagesize
#include "utils.h"
#include "mongoose.h"

#include "dbg.h"
#include "KMotionX.h"
#include "MachinePaths.h"

int mmapNamedFile(MappedFile& mmFile, const char * fileName){
  FILE *fp = (FILE *) fopen (fileName, "r");
  if (!fp) return -1;
  int result = mmapFile(mmFile, fp);
  fclose(fp);
  return result;
}

int mmapFile(MappedFile& mmFile, FILE *fp){

  if (fp == NULL) {
    debug("Failed to open file");
    return -1;
  }

  struct stat st;

  int pagesize, offset, fd;

  fd = fileno(fp);
  fstat(fd, &st);
  mmFile.filesize = st.st_size;


  offset = 0;
  pagesize = getpagesize(); // should use sysconf(_SC_PAGE_SIZE) instead.
  mmFile.mapsize = (mmFile.filesize/pagesize)*pagesize+pagesize; // align memory allocation with pagesize
  //memory map tmp file and parse it.
  mmFile.addr = (char*)mmap((caddr_t)0, mmFile.mapsize, PROT_READ, MAP_PRIVATE, fd, offset);
  return 0;
}


void unmapFile(MappedFile& mmFile){
  munmap(mmFile.addr, mmFile.mapsize);
}


bool absolutePath(const char * relativePath, char * actualpath){
  actualpath[0] = '\0';
  std::string resolved;
  if (!relativePath || !resolveMachinePath(kmx::getMachineDataPath(), relativePath, resolved))
    return false;
  snprintf(actualpath, 256, "%s", resolved.c_str());
  return true;
}

size_t expand_non_ascii(char ** dest, const char * source, size_t srclen){
    char * dstPtr;
    unsigned char c;
    int i = 0;
    size_t dstlen;
    dstlen = srclen*2;
    //TODO This might have to be even bigger,
    //malloc less than 2 times and realloc if needed
    dstPtr = (char*) malloc(sizeof(char) * ( dstlen + 1));
    *dest = dstPtr;

    while (i < srclen) {
        c = source[i++];
        if (/*c > 0x1F &&*/ c < 0x80){  // singleton byte (high bit is 0, code is same as 7-bit ascii)
            *dstPtr++ = c;
        } else {
          //TODO. use snprintf
            dstPtr += sprintf(dstPtr,"<%#x>", c & 0xff);
        }
    }
    *dstPtr = 0;
    return dstPtr-*dest;
}
