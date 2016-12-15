/*
 * utils.h
 *
 *  Created on: 22 maj 2015
 *      Author: parhansson
 */

#ifndef KMOTIONX_EXAMPLES_WEBGUI_UTILS_H_
#define KMOTIONX_EXAMPLES_WEBGUI_UTILS_H_

extern struct mg_server * server;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include <stdio.h>
#include <sys/types.h>


struct MappedFile{
  caddr_t addr;
  size_t mapsize;
  size_t filesize;
};

int mmapNamedFile(MappedFile& mmFile, const char * fileName);
int mmapFile(MappedFile& mmFile, FILE *fp);
void unmapFile(MappedFile& mmFile);

void absolutePath(const char * relativePath, char * absolutePath);
size_t expand_non_ascii(char ** dest, const char * source, size_t srclen);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* KMOTIONX_EXAMPLES_WEBGUI_UTILS_H_ */
