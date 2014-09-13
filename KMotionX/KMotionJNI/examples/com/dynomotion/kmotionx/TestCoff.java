/* 
 * Copyright 2014 (C) par.hansson@gmail.com 
 *  
 * Created on : 2014-03-01 
 * Author     : Pär Hansson 
 * 
 */

package com.dynomotion.kmotionx;

import com.dynomotion.kmotionx.types.BoardType;


public class TestCoff {

	
	public static void main(String[] args){
		KMotion kmx = new KMotion(true);
		kmx.SetErrMsgCallback(new IndexCallbackHandler(1));
		String cFile = "/Users/parhansson/Desktop/test/Gecko3Axis.c";
		String outFile = kmx.convertToOut(1, cFile);
		if(outFile != null){
			outFile = new String(outFile);
			String error = kmx.compile(cFile, outFile, BoardType.BOARD_TYPE_KFLOP, 1);
			if(error != null){
				System.out.println(error);
			}
			
		}
		kmx.CompileAndLoadCoff(cFile, 1);
//		kmx.LoadCoff(1, "/Users/parhansson/Documents/workspaces/KMotionX/KMotionX/Gecko3Axis(1).out");
	}
}
