/* 
 * Copyright 2014 (C) par.hansson@gmail.com 
 *  
 * Created on : 2014-03-01 
 * Author     : Pär Hansson 
 * 
 */

package com.dynomotion.kmotionx.callback;

public interface CompleteCallbackHandler {

	void handleComplete(int status, int line_no, int sequence_number, String err);

}
