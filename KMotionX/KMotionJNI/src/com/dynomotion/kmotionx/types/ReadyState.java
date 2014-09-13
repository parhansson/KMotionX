/* 
 * Copyright 2014 (C) par.hansson@gmail.com 
 *  
 * Created on : 2014-03-01 
 * Author     : Pär Hansson 
 * 
 */

package com.dynomotion.kmotionx.types;

public enum ReadyState {
	KMOTION_OK, KMOTION_TIMEOUT, KMOTION_READY, KMOTION_ERROR;

	public static ReadyState get(int ordinal) {
		return ReadyState.values()[ordinal];
	}
};