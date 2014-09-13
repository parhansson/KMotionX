/* 
 * Copyright 2014 (C) par.hansson@gmail.com 
 *  
 * Created on : 2014-03-01 
 * Author     : Pär Hansson 
 * 
 */

package com.dynomotion.kmotionx.types;

public enum KMotionLockStatus // KMotionLocked Return Codes
{
	KMOTION_LOCKED, // (and token is locked) if KMotion is available for
						// use
	KMOTION_IN_USE, // if already in use
	KMOTION_NOT_CONNECTED; // if error or not able to connect
	public static KMotionLockStatus get(int ordinal) {
		return KMotionLockStatus.values()[ordinal];
	}

};