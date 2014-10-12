/*
Copyright (c) 2014, P.Hansson
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.
 */

//#include <jni.h>
#include <iostream>
#include "stdlib.h"
//#include "../../KMotionDLL/KMotionDLL.h"  // KMOtion DLL Header
#include "../../GCodeInterpreter/StdAfx.h"

//This needs to be included after CString from StdAfx due to an UNUSED macro
//in OpenJDK 7
#include  "com_dynomotion_kmotionx_KMotion.h"
using namespace std;

CKMotionDLL *KM = new CKMotionDLL(0);
CCoordMotion *CM = new CCoordMotion(KM);

//CM->m_Simulate =true;

CGCodeInterpreter *Interpreter = new CGCodeInterpreter(CM);
//static jmethodID errorHandlerCallback;
// cached refs for later callbacks
JavaVM * g_vm;

typedef struct
{
	jobject handler;
	jmethodID method;

} JCALLBACK;


//KMotion Callback
JCALLBACK * errorCH = 0;
JCALLBACK * consoleCH = 0;

//Interpreter callbacks
JCALLBACK * userCH = 0;
JCALLBACK * userMCodeCH = 0;
JCALLBACK * completeCH = 0;
JCALLBACK * statusCH = 0;

//KMotion Callbacks
int ConsoleCallback(const char*);
void ErrMsgCallback(const char*);
void MessageCallback(const char *buf, JCALLBACK * ch);
//Interpreter callbacks
void CompleteCallback(int status, int line_no, int sequence_number, const char *err);
void StatusCallback(int line_no, const char *msg);
int UserCallback(const char *msg);
int UserMCodeCallback(int mcode);

int CreateCallBackReference(JNIEnv *, JCALLBACK * , jobject , const char* , const char* );
int checkJavaEnv(void** ,int);

int checkJavaEnv(void** env, int getEnvStat){
	// double check it's all ok
	int result = getEnvStat;
	if (getEnvStat == JNI_EDETACHED) {
		//std::cout << "Debug GetEnv: not attached" << std::endl;
		if (g_vm->AttachCurrentThread(env, NULL) != 0) {
			std::cout << "Failed to attach" << std::endl;
		} else {
			//std::cout << "Attached successfully" << std::endl;
			result = JNI_OK; //return OK to indicate that we are attached ok
		}
	} else if (getEnvStat == JNI_OK) {
		//std::cout << "Already Attached" << std::endl;
	} else if (getEnvStat == JNI_EVERSION) {
		std::cout << "GetEnv: version not supported" << std::endl;
	}
	//std::cout << "CheckJava "<< result << std::endl;
	return result;
}

void CompleteCallback(int status, int line_no, int sequence_number, const char *err)
{
	JNIEnv * env;
	int getEnvStat = g_vm->GetEnv((void **)&env, JNI_VERSION_1_6);
	if(checkJavaEnv((void **)&env,getEnvStat)){
		return;
	}

	jstring message = env->NewStringUTF(err);
	env->CallVoidMethod(completeCH->handler, completeCH->method, status, line_no, sequence_number, message);

	if (env->ExceptionCheck()) {
		env->ExceptionDescribe();
	}

	if (getEnvStat == JNI_EDETACHED) {
		g_vm->DetachCurrentThread();
	}
}

void StatusCallback(int line_no, const char *msg)
{
	//printf("LineNo %d\n", line_no);
	JNIEnv * env;
	int getEnvStat = g_vm->GetEnv((void **)&env, JNI_VERSION_1_6);
	if(checkJavaEnv((void **)&env,getEnvStat)){
		return;
	}

	jstring message = env->NewStringUTF(msg);

	env->CallVoidMethod(statusCH->handler, statusCH->method, line_no, message);

	if (env->ExceptionCheck()) {
		env->ExceptionDescribe();
	}
	if (getEnvStat == JNI_EDETACHED) {
		g_vm->DetachCurrentThread();
	}
}

int UserCallback(const char *msg)
{
	JNIEnv * env;
	int getEnvStat = g_vm->GetEnv((void **)&env, JNI_VERSION_1_6);
	if(checkJavaEnv((void **)&env,getEnvStat)){
		return 1;
	}

	jstring message = env->NewStringUTF(msg);
	jint result;
	result = env->CallIntMethod(userCH->handler, userCH->method, message);

	if (env->ExceptionCheck()) {
		env->ExceptionDescribe();
	}

	if (getEnvStat == JNI_EDETACHED) {
		g_vm->DetachCurrentThread();

	}
	return result;
}

int UserMCodeCallback(int mcode)
{
	JNIEnv * env;
	int getEnvStat = g_vm->GetEnv((void **)&env, JNI_VERSION_1_6);
	if(checkJavaEnv((void **)&env,getEnvStat)){
		return 1;
	}

	jint result;
	result = env->CallIntMethod(userMCodeCH->handler, userMCodeCH->method, mcode);

	if (env->ExceptionCheck()) {
		env->ExceptionDescribe();
	}

	if (getEnvStat == JNI_EDETACHED) {
		g_vm->DetachCurrentThread();
	}
	return result;
}

int ConsoleCallback(const char *buf){
	MessageCallback(buf, consoleCH);
	return 0;
}
void ErrMsgCallback(const char *buf) {
	MessageCallback(buf, errorCH);
}

void MessageCallback(const char *buf, JCALLBACK * ch){

	JNIEnv * env;
	int getEnvStat = g_vm->GetEnv((void **)&env, JNI_VERSION_1_6);
	if(checkJavaEnv((void **)&env,getEnvStat)){
		return ;
	}

	jstring message = env->NewStringUTF(buf);
	env->CallVoidMethod(ch->handler, ch->method, message);

	if (env->ExceptionCheck()) {
		env->ExceptionDescribe();
	}

	if (getEnvStat == JNI_EDETACHED) {
		g_vm->DetachCurrentThread();
	}

}

int CreateCallBackReference(JNIEnv *env, JCALLBACK * jcallback, jobject callbackInterface, const char* methodName, const char* methodSignature){

	//TODO
	/*
	if(jcallback){
		//send double pointer as parameter might help
		printf("Deregister callback first\n");
		//deregister jcallback->handler global ref and jcallback->method?
	} else {
		jcallback = new JCALLBACK;
	}
	*/

	// convert local to global reference
		// (local will die after this method call)
	jcallback->handler = env->NewGlobalRef(callbackInterface);
	// save refs for callback
	jclass clazz = env->GetObjectClass(jcallback->handler);
	if (clazz == NULL) {
		printf("%s:%d CreateCallBackReference. Failed to find class\n",__FILE__,__LINE__);
		return 1;
	}
	jcallback->method = env->GetMethodID(clazz, methodName, methodSignature);
	if (jcallback->method == NULL) {
		printf("%s:%d CreateCallBackReference. Unable to get method ref: %s %s\n",__FILE__,__LINE__,methodName, methodSignature);
		return 1;
	}
	return 0;
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    init
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_dynomotion_kmotionx_KMotion_init
	(JNIEnv *env, jobject thisObj, jboolean simulate){
	env->GetJavaVM(&g_vm);
	// configure the Action for MCode 3 to do a Callback
	Interpreter->McodeActions[3].Action = M_Action_Callback;
	// configure the Action for MCode 105 to do a Callback
	Interpreter->McodeActions[MCODE_ACTIONS_M100_OFFSET+5].Action = M_Action_Callback;

	strcpy(Interpreter->ToolFile,"");
	strcpy(Interpreter->SetupFile,"");
	CM->m_Simulate = simulate;
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    WriteLineReadLine
 * Signature: (Ljava/lang/String;[B)I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_WriteLineReadLine
	(JNIEnv *env, jobject thisObj, jstring request, jbyteArray response){
   // Step 1: Convert the JNI String (jstring) into C-String (char*)
   const char *Request = env->GetStringUTFChars(request, NULL);
   if (NULL == Request) return 0;

	int result;
	jbyte* jResponse = env->GetByteArrayElements(response, NULL);
	char* Response;

	Response = (char*)jResponse;
	result = KM->WriteLineReadLine(Request, Response);
	env->ReleaseStringUTFChars(request, Request);  // release resources
	env->ReleaseByteArrayElements(response, jResponse, 0);
	return result;
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    WriteLine
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_WriteLine
	(JNIEnv *env, jobject thisObj, jstring request){
	// Step 1: Convert the JNI String (jstring) into C-String (char*)
   const char *Request = env->GetStringUTFChars(request, NULL);
   if (NULL == Request) return 0;
   int result = KM->WriteLine(Request);
   env->ReleaseStringUTFChars(request, Request);  // release resources
   return result;

}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    WriteLineWithEcho
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_WriteLineWithEcho
	(JNIEnv *env, jobject thisObj, jstring request){
	// Step 1: Convert the JNI String (jstring) into C-String (char*)
   const char *Request = env->GetStringUTFChars(request, NULL);
   if (NULL == Request) return 0;
   int result = KM->WriteLineWithEcho(Request);
   env->ReleaseStringUTFChars(request, Request);  // release resources
   return result;


}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    WaitToken
 * Signature: (ZI)I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_WaitToken
  (JNIEnv *env, jobject thisObj, jboolean displayMsg, jint timeOutMs){
	return KM->WaitToken(displayMsg, timeOutMs);
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    KMotionLock
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_KMotionLock
  (JNIEnv *env, jobject thisObj){
	return KM->KMotionLock();
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    USBLocation
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_USBLocation
  (JNIEnv *env, jobject thisObj){
	return KM->USBLocation();
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    KMotionLockRecovery
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_KMotionLockRecovery
  (JNIEnv *env, jobject thisObj){
	return KM->KMotionLockRecovery();
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    ReleaseToken
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_dynomotion_kmotionx_KMotion_ReleaseToken
  (JNIEnv *env, jobject thisObj){
	KM->ReleaseToken();
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    Failed
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_Failed
  (JNIEnv *env, jobject thisObj){
	return KM->Failed();
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    Disconnect
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_Disconnect
  (JNIEnv *env, jobject thisObj){
	return KM->Disconnect();
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    FirmwareVersion
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_FirmwareVersion
  (JNIEnv *env, jobject thisObj){
	return KM->FirmwareVersion();
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    CheckForReady
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_CheckForReady
  (JNIEnv *env, jobject thisObj){
	return KM->CheckForReady();
}


/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    LoadCoff
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_LoadCoff
  (JNIEnv *env, jobject thisObj, jint threadId, jstring name){

	// Step 1: Convert the JNI String (jstring) into C-String (char*)
	const char *Name = env->GetStringUTFChars(name, NULL);
	if (NULL == Name) return 0;

	int result;
	result = KM->LoadCoff(threadId, Name, 0);
	env->ReleaseStringUTFChars(name, Name);  // release resources
	return result;

}


/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    CompileAndLoadCoff
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_CompileAndLoadCoff__Ljava_lang_String_2I
  (JNIEnv *env, jobject thisObj, jstring name, jint threadId){
	// Step 1: Convert the JNI String (jstring) into C-String (char*)
	const char *Name = env->GetStringUTFChars(name, NULL);
	if (NULL == Name) return 0;

	int result;
	result = KM->CompileAndLoadCoff(Name, threadId);
	env->ReleaseStringUTFChars(name, Name);  // release resources
	return result;
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    CompileAndLoadCoff
 * Signature: (Ljava/lang/String;I[BI)I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_CompileAndLoadCoff__Ljava_lang_String_2I_3BI
  (JNIEnv *env, jobject thisObj, jstring name, jint threadId, jbyteArray err, jint maxErrLength){
	// Step 1: Convert the JNI String (jstring) into C-String (char*)
	const char *Name = env->GetStringUTFChars(name, NULL);
	if (NULL == Name) return 0;

	int result;
	jbyte* jErr = env->GetByteArrayElements(err, NULL);
	char* Err;

	//To do this in a nice C++ way with arrays:
	//jbyte memory_buffer[nr_pixels];
	//unsigned char* pixels = reinterpret_cast<unsigned char*>(memory_buffer);
	//or the C way:
	Err = (char*)jErr;

	result = KM->CompileAndLoadCoff(Name, threadId, Err, maxErrLength);
	env->ReleaseByteArrayElements(err, jErr, 0);
	env->ReleaseStringUTFChars(name, Name);  // release resources
	return result;
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    Compile
 * Signature: (Ljava/lang/String;Ljava/lang/String;II[BI)I
 */
//int CKMotionDLL::Compile(const char *Name, const char *OutFile, const int BoardType, int Thread, char *Err, int MaxErrLen)
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_Compile
  (JNIEnv *env, jobject thisObj, jstring name, jstring outFile, jint boardType, jint threadId, jbyteArray err, jint maxErrLength){
	// Step 1: Convert the JNI String (jstring) into C-String (char*)
	const char *Name = env->GetStringUTFChars(name, NULL);
	if (NULL == Name) return 0;

	const char *OutFile = env->GetStringUTFChars(outFile, NULL);
	if (NULL == OutFile) return 0;

	int result;
	jbyte* jErr = env->GetByteArrayElements(err, NULL);
	char* Err;

	//To do this in a nice C++ way with arrays:
	//jbyte memory_buffer[nr_pixels];
	//unsigned char* pixels = reinterpret_cast<unsigned char*>(memory_buffer);
	//or the C way:
	Err = (char*)jErr;

	result = KM->Compile(Name, OutFile, boardType, threadId, Err, maxErrLength);
	env->ReleaseByteArrayElements(err, jErr, 0);
	env->ReleaseStringUTFChars(name, Name);  // release resources
	env->ReleaseStringUTFChars(outFile, OutFile);  // release resources
	return result;
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    GetLoadAddress
 * Signature: (II)I
 */
JNIEXPORT jlong JNICALL Java_com_dynomotion_kmotionx_KMotion_GetLoadAddress
  (JNIEnv *env, jobject thisObj, jint threadId, jint boardType){
	return KM->GetLoadAddress(threadId, boardType);
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    ConvertToOut
 * Signature: (ILjava/lang/String;[BI)V
 */
//void CKMotionDLL::ConvertToOut(int thread, const char *InFile, char *OutFile, int MaxLength)
JNIEXPORT void JNICALL Java_com_dynomotion_kmotionx_KMotion_ConvertToOut
  (JNIEnv *env, jobject thisObj, jint threadId, jstring inFile, jbyteArray outFile, jint maxLength){
	// Step 1: Convert the JNI String (jstring) into C-String (char*)
	const char *InFile = env->GetStringUTFChars(inFile, NULL);
	if (NULL == InFile) return;

	jbyte* jOutFile = env->GetByteArrayElements(outFile, NULL);
	char* OutFile;

	//To do this in a nice C++ way with arrays:
	//jbyte memory_buffer[nr_pixels];
	//unsigned char* pixels = reinterpret_cast<unsigned char*>(memory_buffer);
	//or the C way:
	OutFile = (char*)jOutFile;

	KM->ConvertToOut(threadId, InFile, OutFile, maxLength);
	env->ReleaseByteArrayElements(outFile, jOutFile, 0);
	env->ReleaseStringUTFChars(inFile, InFile);  // release resources
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    ServiceConsole
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_ServiceConsole
  (JNIEnv *env, jobject thisObj){
	return KM->ServiceConsole();
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    SetConsoleCallback
 * Signature: (Lcom/dynomotion/kmotionx/CallbackHandler;)I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_SetConsoleCallback
	(JNIEnv *env, jobject thisObj,jobject callbackInterface){

	consoleCH = new JCALLBACK;

	if(CreateCallBackReference(env, consoleCH, callbackInterface, "handleConsole", "(Ljava/lang/String;)V")){
		return 0;
	}

	return KM->SetConsoleCallback(ConsoleCallback);
}


/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    SetErrMsgCallback
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_SetErrMsgCallback
	(JNIEnv *env, jobject thisObj,jobject callbackInterface){

	errorCH = new JCALLBACK;
	if(CreateCallBackReference(env, errorCH, callbackInterface, "handleError", "(Ljava/lang/String;)V")){
		return 0;
	}
	KM->SetErrMsgCallback(ErrMsgCallback);

	return 1;//(jboolean)returnValue;

}


/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    CheckKMotionVersion
 * Signature: (IZ)I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_CheckKMotionVersion
  (JNIEnv *env, jobject thisObj, jint type, jboolean getBoardTypeOnly){
	//TODO type is outputparameter
	//return KM->CheckKMotionVersion(&type, getBoardTypeOnly);
	return 0;
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    ExtractCoffVersionString
 * Signature: (Ljava/lang/String;[B)I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_ExtractCoffVersionString
  (JNIEnv *env, jobject thisObj, jstring inFile, jbyteArray version){

   // Step 1: Convert the JNI String (jstring) into C-String (char*)
   const char *InFile = env->GetStringUTFChars(inFile, NULL);
   if (NULL == InFile) return 0;

	int result;
	jbyte* jVersion = env->GetByteArrayElements(version, NULL);
	char* Version;

	//To do this in a nice C++ way with arrays:
	//jbyte memory_buffer[nr_pixels];
	//unsigned char* pixels = reinterpret_cast<unsigned char*>(memory_buffer);
	//or the C way:
	Version = (char*)jVersion;
	result = KM->ExtractCoffVersionString(InFile, Version);
	env->ReleaseStringUTFChars(inFile, InFile);  // release resources
	env->ReleaseByteArrayElements(version, jVersion, 0);
	return result;


}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    DoErrMsg
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_dynomotion_kmotionx_KMotion_DoErrMsg
  (JNIEnv *env, jobject thisObj, jstring message){

   // Step 1: Convert the JNI String (jstring) into C-String (char*)
   const char *Message = env->GetStringUTFChars(message, NULL);
   if (NULL == Message) return;
   KM->DoErrMsg(Message);
   env->ReleaseStringUTFChars(message, Message);  // release resources
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    GetMotionParams
 * Signature: ([D)V
 */
JNIEXPORT void JNICALL Java_com_dynomotion_kmotionx_KMotion_GetMotionParams
(JNIEnv *env, jobject thisObj, jdoubleArray params, jbooleanArray bools){

	MOTION_PARAMS *p = Interpreter->GetMotionParams();

	jdouble* jParams = env->GetDoubleArrayElements(params, NULL);
//	memcpy(jParams,p, 57);

	jParams[0] = p->BreakAngle;
	jParams[1] = p->CollinearTol;
	jParams[2] = p->CornerTol;
	jParams[3] = p->FacetAngle;
	jParams[4] = p->TPLookahead;
	jParams[5] = p->RadiusA;
	jParams[6] = p->RadiusB;
	jParams[7] = p->RadiusC;
	jParams[8] = p->MaxAccelX;
	jParams[9] = p->MaxAccelY;
	jParams[10] = p->MaxAccelZ;
	jParams[11] = p->MaxAccelA;
	jParams[12] = p->MaxAccelB;
	jParams[13] = p->MaxAccelC;

	jParams[14] = p->MaxVelX;
	jParams[15] = p->MaxVelY;
	jParams[16] = p->MaxVelZ;
	jParams[17] = p->MaxVelA;
	jParams[18] = p->MaxVelB;
	jParams[19] = p->MaxVelC;

	jParams[20] = p->MaxRapidJerkX;  // used for 3rd order Rapids
	jParams[21] = p->MaxRapidJerkY;
	jParams[22] = p->MaxRapidJerkZ;
	jParams[23] = p->MaxRapidJerkA;
	jParams[24] = p->MaxRapidJerkB;
	jParams[25] = p->MaxRapidJerkC;
	jParams[26] = p->MaxRapidAccelX;
	jParams[27] = p->MaxRapidAccelY;
	jParams[28] = p->MaxRapidAccelZ;
	jParams[29] = p->MaxRapidAccelA;
	jParams[30] = p->MaxRapidAccelB;
	jParams[31] = p->MaxRapidAccelC;
	jParams[32] = p->MaxRapidVelX;
	jParams[33] = p->MaxRapidVelY;
	jParams[34] = p->MaxRapidVelZ;
	jParams[35] = p->MaxRapidVelA;
	jParams[36] = p->MaxRapidVelB;
	jParams[37] = p->MaxRapidVelC;

	jParams[38] = p->SoftLimitNegX;  // Soft Limits (inches)
	jParams[39] = p->SoftLimitNegY;
	jParams[40] = p->SoftLimitNegZ;
	jParams[41] = p->SoftLimitNegA;
	jParams[42] = p->SoftLimitNegB;
	jParams[43] = p->SoftLimitNegC;
	jParams[44] = p->SoftLimitPosX;
	jParams[45] = p->SoftLimitPosY;
	jParams[46] = p->SoftLimitPosZ;
	jParams[47] = p->SoftLimitPosA;
	jParams[48] = p->SoftLimitPosB;
	jParams[49] = p->SoftLimitPosC;

	jParams[50] = p->CountsPerInchX;
	jParams[51] = p->CountsPerInchY;
	jParams[52] = p->CountsPerInchZ;
	jParams[53] = p->CountsPerInchA;
	jParams[54] = p->CountsPerInchB;
	jParams[55] = p->CountsPerInchC;
	jParams[56] = p->MaxLinearLength;
	env->ReleaseDoubleArrayElements(params, jParams, 0);

	jboolean* jBools = env->GetBooleanArrayElements(bools,NULL);

	jBools[0] = p->UseOnlyLinearSegments;
	jBools[1] = p->ArcsToSegs;
	jBools[2] = p->DegreesA;
	jBools[3] = p->DegreesB;
	jBools[4] = p->DegreesC;

	env->ReleaseBooleanArrayElements(bools, jBools,0);

}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    SetMotionParams
 * Signature: ([D)V
 */
JNIEXPORT void JNICALL Java_com_dynomotion_kmotionx_KMotion_SetMotionParams
	(JNIEnv *env, jobject thisObj, jdoubleArray params, jbooleanArray bools){


	MOTION_PARAMS *p = Interpreter->GetMotionParams();

	jdouble* jParams = env->GetDoubleArrayElements(params, NULL);

	p->BreakAngle = jParams[0];
	p->CollinearTol = jParams[1];
	p->CornerTol = jParams[2];
	p->FacetAngle = jParams[3];
	p->TPLookahead = jParams[4];

	p->RadiusA = jParams[5];
	p->RadiusB = jParams[6];
	p->RadiusC = jParams[7];
	p->MaxAccelX = jParams[8];
	p->MaxAccelY = jParams[9];
	p->MaxAccelZ = jParams[10];
	p->MaxAccelA = jParams[11];
	p->MaxAccelB = jParams[12];
	p->MaxAccelC = jParams[13];
	p->MaxVelX = jParams[14];
	p->MaxVelY = jParams[15];
	p->MaxVelZ = jParams[16];
	p->MaxVelA = jParams[17];
	p->MaxVelB = jParams[18];
	p->MaxVelC = jParams[19];

	p->MaxRapidJerkX = jParams[20];
	p->MaxRapidJerkY = jParams[21];
	p->MaxRapidJerkZ = jParams[22];
	p->MaxRapidJerkA = jParams[23];
	p->MaxRapidJerkB = jParams[24];
	p->MaxRapidJerkC = jParams[25];
	p->MaxRapidAccelX = jParams[26];
	p->MaxRapidAccelY = jParams[27];
	p->MaxRapidAccelZ = jParams[28];
	p->MaxRapidAccelA = jParams[29];
	p->MaxRapidAccelB = jParams[30];
	p->MaxRapidAccelC = jParams[31];
	p->MaxRapidVelX = jParams[32];
	p->MaxRapidVelY = jParams[33];
	p->MaxRapidVelZ = jParams[34];
	p->MaxRapidVelA = jParams[35];
	p->MaxRapidVelB = jParams[36];
	p->MaxRapidVelC = jParams[37];

	p->SoftLimitNegX = jParams[38];
	p->SoftLimitNegY = jParams[39];
	p->SoftLimitNegZ = jParams[40];
	p->SoftLimitNegA = jParams[41];
	p->SoftLimitNegB = jParams[42];
	p->SoftLimitNegC = jParams[43];
	p->SoftLimitPosX = jParams[44];
	p->SoftLimitPosY = jParams[45];
	p->SoftLimitPosZ = jParams[46];
	p->SoftLimitPosA = jParams[47];
	p->SoftLimitPosB = jParams[48];
	p->SoftLimitPosC = jParams[49];

	p->CountsPerInchX = jParams[50];
	p->CountsPerInchY = jParams[51];
	p->CountsPerInchZ = jParams[52];
	p->CountsPerInchA = jParams[53];
	p->CountsPerInchB = jParams[54];
	p->CountsPerInchC = jParams[55];
	p->MaxLinearLength = jParams[56];
	env->ReleaseDoubleArrayElements(params, jParams, 0);

	jboolean* jBools = env->GetBooleanArrayElements(bools,NULL);

	p->UseOnlyLinearSegments = jBools[0];
	p->ArcsToSegs = jBools[1];
	p->DegreesA = jBools[2];
	p->DegreesB = jBools[3];
	p->DegreesC = jBools[4];

	env->ReleaseBooleanArrayElements(bools, jBools,0);
	//Update TrajectoryPlanner with new parameters
	//CM->SetTPParams();
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    SetUserMcodeCallback
 * Signature: (Lcom/dynomotion/kmotionx/UserMcodeCallbackHandler;)I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_SetUserMCodeCallback
	(JNIEnv *env, jobject thisObj,jobject callbackInterface){

	userMCodeCH = new JCALLBACK;
	if(CreateCallBackReference(env, userMCodeCH, callbackInterface, "handleUserMCode", "(I)I")){
		return 0;
	}

	Interpreter->SetUserMCodeCallback(UserMCodeCallback);

	return 1;//(jboolean)returnValue;
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    SetUserCallback
 * Signature: (Lcom/dynomotion/kmotionx/UserCallbackHandler;)I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_SetUserCallback
	(JNIEnv *env, jobject thisObj,jobject callbackInterface){

	userCH = new JCALLBACK;
	if(CreateCallBackReference(env, userCH, callbackInterface, "handleUser", "(Ljava/lang/String;)I")){
		return 0;
	}
	Interpreter->SetUserCallback(UserCallback);

	return 1;//(jboolean)returnValue;
}

/*
 * Class:     com_dynomotion_kmotionx_KMotion
 * Method:    Interpret
 * Signature: (ILjava/lang/String;IIILcom/dynomotion/kmotionx/StatusCallbackHandler;Lcom/dynomotion/kmotionx/CompleteCallbackHandler;)I
 */
JNIEXPORT jint JNICALL Java_com_dynomotion_kmotionx_KMotion_Interpret
	(JNIEnv *env, jobject thisObj,jint boardType, jstring fileName, jint start, jint end , jint restart, jobject statusCallbackInterface, jobject completeCallbackInterface){

	statusCH = new JCALLBACK;
	if(CreateCallBackReference(env, statusCH, statusCallbackInterface, "handleStatus", "(ILjava/lang/String;)V")){
		return 0;
	}

	completeCH = new JCALLBACK;
	if(CreateCallBackReference(env, completeCH, completeCallbackInterface, "handleComplete", "(IIILjava/lang/String;)V")){
		return 0;
	}

	int result;
	const char *FileName = env->GetStringUTFChars(fileName, NULL);
	if (NULL == FileName) return 0;

	result = Interpreter->Interpret(boardType,FileName,start,end,restart,StatusCallback,CompleteCallback);
	env->ReleaseStringUTFChars(fileName,FileName);
	return result;
}
