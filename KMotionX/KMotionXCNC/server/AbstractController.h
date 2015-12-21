/*
 * AbstractController.h
 *
 *  Created on: 19 dec. 2015
 *      Author: parhansson
 */

#ifndef KMOTIONX_KMOTIONXCNC_SERVER_ABSTRACTCONTROLLER_H_
#define KMOTIONX_KMOTIONXCNC_SERVER_ABSTRACTCONTROLLER_H_
#include <stdlib.h>
class AbstractController {
public:
  AbstractController();
  virtual ~AbstractController();
  virtual int PushClientData(const char *data) = 0;
  virtual int PushClientData(const char *data , size_t data_len) = 0;
};

#endif /* KMOTIONX_KMOTIONXCNC_SERVER_ABSTRACTCONTROLLER_H_ */
