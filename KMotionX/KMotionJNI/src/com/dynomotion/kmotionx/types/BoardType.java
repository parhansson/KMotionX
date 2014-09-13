/* 
 * Copyright 2014 (C) par.hansson@gmail.com 
 *  
 * Created on : 2014-03-01 
 * Author     : Pär Hansson 
 * 
 */

package com.dynomotion.kmotionx.types;



public enum BoardType {
	BOARD_TYPE_UNKNOWN, BOARD_TYPE_KMOTION, BOARD_TYPE_KFLOP;
	public static BoardType get(int ordinal) {
		return BoardType.values()[ordinal];
	}
};