/*
 * GCodeInterpreterX.h
 *
 *  Created on: 26 feb 2017
 *      Author: parhansson
 */

#ifndef GCODEINTERPRETERX_H_
#define GCODEINTERPRETERX_H_

#include <pthread.h>

//#include <CString.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
//Might be removed since code is changed to use fmin
//#define min(X,Y) fmin(X,Y)
#include <math.h>

#include <KMotionX.h>

#include <locale.h>
#include "canon.h"
#include "rs274ngc.h"
#include "driver.h"
#include "PT2D.h"
#include "PT3D.h"
#include "KMotionDLL.h"
#include "TrajectoryPlanner.h"
#include "Kinematics.h"
#include "Kinematics3Rod.h"
#include "KinematicsGeppetto.h"
#include "KinematicsGeppettoExtrude.h"
#include "Kinematics5AxisGimbalAB.h"
#include "Kinematics5AxisTableAGimbalB.h"
#include "Kinematics5AxisGimbalCB.h"
#include "Kinematics5AxisTableAC.h"
#include "Kinematics5AxisTableAB.h"
#include "Kinematics5AxisTableBC.h"
#include "KinematicsScara.h"
#include "Kinematics2AxisRobot.h"
#include "GCodeInterpreter.h"

#endif /* GCODEINTERPRETERX_H_ */
