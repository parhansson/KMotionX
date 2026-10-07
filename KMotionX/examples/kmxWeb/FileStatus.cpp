/*
 * FileStatus.cpp
 *
 *  Created on: 2 jul. 2016
 *      Author: parhansson
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "FileStatus.h"

FileStatus::FileStatus() {
  allocated = 32;
  path = (char*) malloc (allocated * sizeof(char));
  if(path == NULL) {
    printf("Error allocating memory");
    exit (1);
  } 
  path[0] = '\0';
  timestamp = 0;
}

FileStatus::~FileStatus() {
  if(path != NULL) {
    free(path);
  }
}

bool FileStatus::SetFile(const char * file) {
  void * newarray;
  if (strcmp(path,file)) {
    int new_size = strlen(file)+1;    
    
    if(new_size > allocated) {
      newarray = realloc(path, new_size * sizeof(char));
      if (path == NULL) {
        printf("Error reallocating memory\n");
        exit (1);
      } else {
        //debug("Reallocated memory to new sizefor file %s\n", file);
        path = (char*) newarray;
      }
      allocated = new_size; 
    }
    strcpy(path, file);
    
    Update();

    return true;
  } else {
    return false;
  }

}

int FileStatus::Update(){
  struct stat st;
  int ierr = stat (path, &st);

  if (ierr != 0) {
    printf("Error %d\n", ierr);
  }
  
  timestamp = st.st_mtime;
  
  return timestamp;
}