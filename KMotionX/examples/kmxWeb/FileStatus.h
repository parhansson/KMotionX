/*
 * FileStatus.h
 *
 *  Created on: 2 jul. 2016
 *      Author: parhansson
 */

#ifndef KMOTIONX_KMOTIONXCNC_SERVER_FILESTATUS_H_
#define KMOTIONX_KMOTIONXCNC_SERVER_FILESTATUS_H_

class FileStatus {
  public:
  FileStatus();
  virtual ~FileStatus();
  int Update();
  /*
  *Returns true if the file was changed
  */
  bool SetFile(const char * file);

  char * path;
  int timestamp;
  
  private:
  int allocated;
};


#endif /*KMOTIONX_KMOTIONXCNC_SERVER_FILESTATUS_H_*/