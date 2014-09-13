/* 
 * Copyright 2014 (C) par.hansson@gmail.com 
 *  
 * Created on : 2014-03-01 
 * Author     : Pär Hansson 
 * 
 */
package com.dynomotion.kmotionx;

import java.io.IOException;

import com.dynomotion.kmotionx.callback.CompleteCallbackHandler;
import com.dynomotion.kmotionx.callback.ConsoleCallbackHandler;
import com.dynomotion.kmotionx.callback.StatusCallbackHandler;
import com.dynomotion.kmotionx.callback.UserCallbackHandler;
import com.dynomotion.kmotionx.callback.UserMCodeCallbackHandler;
import com.dynomotion.kmotionx.types.BoardType;
import com.dynomotion.kmotionx.types.ReadyState;

public class KMotion {

	static {
		System.loadLibrary("KMotionXjni");
	}
//	private boolean wait;
	private static int MAX_LINE = 2560;

	private native void init(boolean simulate);

	// int WriteLineReadLine(const char *s, char *response);
	public native int WriteLineReadLine(String s, byte[] response);
	
	public String WriteLineReadLine(String request){
		byte[] response = new byte[MAX_LINE];
		if(WriteLineReadLine(request,response) == 0){
			return new String(response);	
		} 
		return null;
	}
	

	// int WriteLine(const char *s);
	public native int WriteLine(String s);

	// int WriteLineWithEcho(const char *s);
	public native int WriteLineWithEcho(String s);

	// int ReadLineTimeOut(char *buf, int TimeOutms=1000000);
	// public native int ReadLineTimeOut(char *buf, int TimeOutms=1000000);

	// int ListLocations(int *nlocations, int *list);
	// public native int ListLocations(int *nlocations, int *list);

	// int WaitToken(bool display_msg=true, int TimeOut_ms=1000000);
	public native int WaitToken(boolean display_msg, int TimeOut_ms);

	public int WaitToken() {
		return WaitToken(true, 1000000);
	}

	public native int KMotionLock();

	public native int USBLocation();

	public native int KMotionLockRecovery();

	public native void ReleaseToken();

	public native int Failed();

	public native int Disconnect();

	public native int FirmwareVersion();

	public native int CheckForReady();
	
	public ReadyState checkForReady(){
		return ReadyState.get(CheckForReady());
	}

	// Note: ALL User Thread Numbers start with 1

	// int LoadCoff(int Thread, const char *Name, int PackToFlash=0);
	// //PackToFlash 0=normal,1=NewVersion,2=bootload
	public native int LoadCoff(int Thread, String Name);

	// int CompileAndLoadCoff(const char *Name, int Thread);
	public native int CompileAndLoadCoff(String Name, int Thread);
	
	// int CompileAndLoadCoff(const char *Name, int Thread, char *Err, int
	// MaxErrLen);
	public native int CompileAndLoadCoff(String Name, int Thread, byte[] Err,
			int MaxErrLen);

	// int Compile(const char *Name, const char *OutFile, const int board_type,
	// int Thread, char *Err, int MaxErrLen);
	public native int Compile(String Name, String OutFile, int board_type,
			int Thread, byte[] Err, int MaxErrLen);

	public String compile(String inFile,String outFile,BoardType boardType,
			int Thread){
		int maxLength = 512;
		byte[] err = new byte[maxLength];
		if(Compile(inFile,outFile,boardType.ordinal(),Thread,err,maxLength) == 0){
			return 	null;
		} else {
			return new String(err);
		}
	}
	
	// int CheckCoffSize(const char *Name, int *size_text, int *size_bss, int *size_data, int *size_total); // return size of sections and total (including padding)
	// public native int CheckCoffSize(String Name, int *size_text, int *size_bss, int *size_data, int *size_total); // return size of sections and total (including padding)

	// unsigned int GetLoadAddress(int thread, int BoardType);
	public native long GetLoadAddress(int thread, int BoardType);

	// void ConvertToOut(int thread, const char *InFile, char *OutFile, int
	// MaxLength);
	public native void ConvertToOut(int thread, String InFile, byte[] OutFile,
			int MaxLength);

	public String convertToOut(int thread, String inFile){
		int maxLength = 512;
		byte[] outFile = new byte[maxLength];
		ConvertToOut(thread,inFile,outFile,maxLength);
		//need to check for 
		for (int i = 0; i < outFile.length; i++) {
			if(outFile[i] == 0){
				return new String(outFile,0,i);
			}
			
		}
		return(new String(outFile));
	}
	
	
	public native int ServiceConsole();

	// int SetConsoleCallback(CONSOLE_HANDLER *ch);
	public native int SetConsoleCallback(ConsoleCallbackHandler ch);

	// int SetErrMsgCallback(ERRMSG_HANDLER *eh);
	public native int SetErrMsgCallback(ConsoleCallbackHandler ch);

	// int CheckKMotionVersion(int *type=NULL, bool GetBoardTypeOnly=false);
	// TODO First parameter is actually out parameter
	public native int CheckKMotionVersion(int type, boolean GetBoardTypeOnly);

	public int CheckKMotionVersion() {
		// TODO First parameter is actually out parameter
		return CheckKMotionVersion(BoardType.BOARD_TYPE_UNKNOWN.ordinal(),
				false);
	}

	// int ExtractCoffVersionString(const char *InFile, char *Version);
	public native int ExtractCoffVersionString(String InFile, byte[] Version);

	// int GetStatus(MAIN_STATUS& status, bool lock);
	//public native int GetStatus(MAIN_STATUS& status, bool lock);

	// void DoErrMsg(const char *s);
	public native void DoErrMsg(String s);

	// Interpreter calls
	public native void GetMotionParams(double[] params, boolean[] booleans);

	public native void SetMotionParams(double[] params, boolean[] booleans);

	public void SetMotionParams(MotionParams params) {
		SetMotionParams(params.doubleValues(), params.booleanValues());
	}

	public MotionParams GetMotionParams() {
		double[] doubles = new double[57];
		boolean[] booleans = new boolean[5];
		GetMotionParams(doubles, booleans);
		return new MotionParams(doubles, booleans);
	}

	public native int SetUserMCodeCallback(UserMCodeCallbackHandler ch);

	public native int SetUserCallback(UserCallbackHandler ch);

	// int CGCodeInterpreter::Interpret(int board_type,const char *fname,int
	// start, int end,int restart,G_STATUS_CALLBACK
	// *StatusFn,G_COMPLETE_CALLBACK *CompleteFn)
	public native int Interpret(int board_type, String fname, int start,
			int end, int restart, StatusCallbackHandler sch,
			CompleteCallbackHandler cch);

	public KMotion (boolean simulate){
		init(simulate);
	}
	public static void main(String[] args) {
		KMotion kmx = new KMotion(true);
		IndexCallbackHandler cb  = new IndexCallbackHandler(2);
		kmx.DoErrMsg("Error message. No callback registered");
		kmx.SetErrMsgCallback(new IndexCallbackHandler(1));
		kmx.DoErrMsg("Error message. Callback registered");
		kmx.SetErrMsgCallback(cb);
		kmx.DoErrMsg("Error message. New callback registered");
		
		kmx.SetConsoleCallback(new IndexCallbackHandler(3));
		kmx.SetUserCallback(cb);
		kmx.SetUserMCodeCallback(cb);
		/*
		kmx.CheckKMotionVersion();
		System.out.println(kmx.FirmwareVersion());
		int lockstat;
		if ((lockstat = kmx.KMotionLock()) == KMotionLockStatus.KMOTION_LOCKED.ordinal())  // see if we can get access
		{
			// Get the firmware date from the KMotion Card which
			// will be in PT (Pacific Time)
			kmx.ReleaseToken();
			byte[] Version = new byte[80];
			if(kmx.WriteLineReadLine("Version",Version) == 0){
				System.out.println(new String(Version));	
			} else {
				System.out.println("Failed");
			}
		} else {
			
			System.out.println(KMotionLockStatus.get(lockstat));
		}
		*/
		//if(true) return;
		// String InFile
		// ="/Users/parhansson/git/kflop/KMotionX/DSP_KFLOP/DSPKFLOP.out";
		// byte[] Version = new byte[40];
		//
		// if(hello.ExtractCoffVersionString(InFile , Version )==0){
		// System.out.println("OK " + new String(Version));
		//
		// } else {
		// System.out.println("Failed");
		// }
		MotionParams params = kmx.GetMotionParams();
		// for (double d : params.toArray()) {
		// System.out.println(d);
		// }
		params.BreakAngle = 17;
		//TODO föra över booleans och testa om ARC_FEED försvinner
		params.UseOnlyLinearSegments = true;
		kmx.SetMotionParams(params);
		params = kmx.GetMotionParams();
		// for (double d : params.toArray()) {
		// System.out.println(d);
		// }
		// int result = kmx.KMotionLock();
		// System.out.println(KMotionLocked.values()[result].name());
		// kmx.ReleaseToken();
		// kmx.SetConsoleCallback(new IndexCallbackHandler(3));
		// kmx.ServiceConsole();
		
		
		

		int start = 0;
		int end = -1;
		int restart = 1;
		kmx.Interpret(BoardType.BOARD_TYPE_KFLOP.ordinal(),
				"/Users/parhansson/Desktop/SimpleCircle.ngc", start, end,
				restart, cb
				, cb);
		for (int i = 0; i < 4; i++) {
			//kmx.ServiceConsole();
			//System.out.println("FirmwareVersion=" +kmx.FirmwareVersion());
			try {
				Thread.sleep(200);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		try {
			System.in.read();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
//	     synchronized (kmx) {
//	    	 kmx.wait = true;
//	         while (kmx.wait){
//	        	 try {
//					kmx.wait();
//				} catch (InterruptedException e) {
//					e.printStackTrace();
//				}
//	        	 
//	         }
//	         
//	         // Perform action appropriate to condition
//	     }
		System.out.println("Slut");
		kmx.ServiceConsole();
		// hello.WriteLine("CheckDone");
		// System.out.println(hello.GetLoadAddress(1, 1));
		// byte[] outFile = new byte[300];
		// hello.ConvertToOut(2,
		// "/Users/parhansson/git/kflop/KMotionX/DSP_KFLOP/DSPKFLOP.c", outFile,
		// outFile.length);
		// System.out.println(new String(outFile));

		// hello.sayHi(args[0], Integer.parseInt(args[1])); //3
	}
}
