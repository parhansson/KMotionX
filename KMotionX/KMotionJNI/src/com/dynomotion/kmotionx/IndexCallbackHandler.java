/* 
 * Copyright 2014 (C) par.hansson@gmail.com 
 *  
 * Created on : 2014-03-01 
 * Author     : Pär Hansson 
 * 
 */

package com.dynomotion.kmotionx;

import com.dynomotion.kmotionx.callback.CompleteCallbackHandler;
import com.dynomotion.kmotionx.callback.ConsoleCallbackHandler;
import com.dynomotion.kmotionx.callback.ErrorCallbackHandler;
import com.dynomotion.kmotionx.callback.StatusCallbackHandler;
import com.dynomotion.kmotionx.callback.UserCallbackHandler;
import com.dynomotion.kmotionx.callback.UserMCodeCallbackHandler;

public class IndexCallbackHandler implements ConsoleCallbackHandler,
		ErrorCallbackHandler, CompleteCallbackHandler, StatusCallbackHandler,
		UserMCodeCallbackHandler, UserCallbackHandler {
	private int index;

	private String indexName(String name) {
		return name + "(" + index + ") : ";
	}

	public IndexCallbackHandler(int index) {
		this.index = index;
	}

	@Override
	public void handleConsole(String message) {
		System.out.println(indexName("CONSOLE") + message);

	}

	@Override
	public void handleError(String message) {
		System.out.println(indexName("ERROR") + message);

	}

	@Override
	public int handleUser(String msg) {
		System.out.println(indexName("USER") + msg);
		return 0;
	}

	@Override
	public int handleUserMCode(int mcode) {
		System.out.println(indexName("USER_M_CODE") + mcode);
		return 0;
	}

	@Override
	public void handleStatus(int line_no, String msg) {

		System.out.print(String.format(indexName("STATUS") + "line: %d\n%s",
				line_no, msg));

	}

	@Override
	public void handleComplete(int status, int line_no, int sequence_number,
			String err) {
		System.out.println(String.format(
				indexName("COMPLETE") + "%d line: %d  error: %s", status,
				line_no, err).trim());
		// wait = false;
		// this.notify();
	}

}