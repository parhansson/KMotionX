/* 
 * Copyright 2014 (C) par.hansson@gmail.com 
 *  
 * Created on : 2014-03-01 
 * Author     : Pär Hansson 
 * 
 */

package com.dynomotion.kmotionx;

public class MotionParams {
	MotionParams(double[] doubleValues, boolean[] booleanValues) {
		this.BreakAngle = doubleValues[0];
		this.CollinearTol = doubleValues[1];
		this.CornerTol = doubleValues[2];
		this.FacetAngle = doubleValues[3];
		this.TPLookahead = doubleValues[4];
		this.RadiusA = doubleValues[5];
		this.RadiusB = doubleValues[6];
		this.RadiusC = doubleValues[7];
		this.MaxAccelX = doubleValues[8];
		this.MaxAccelY = doubleValues[9];
		this.MaxAccelZ = doubleValues[10];
		this.MaxAccelA = doubleValues[11];
		this.MaxAccelB = doubleValues[12];
		this.MaxAccelC = doubleValues[13];
		this.MaxVelX = doubleValues[14];
		this.MaxVelY = doubleValues[15];
		this.MaxVelZ = doubleValues[16];
		this.MaxVelA = doubleValues[17];
		this.MaxVelB = doubleValues[18];
		this.MaxVelC = doubleValues[19];

		this.MaxRapidJerkX = doubleValues[20];
		this.MaxRapidJerkY = doubleValues[21];
		this.MaxRapidJerkZ = doubleValues[22];
		this.MaxRapidJerkA = doubleValues[23];
		this.MaxRapidJerkB = doubleValues[24];
		this.MaxRapidJerkC = doubleValues[25];
		this.MaxRapidAccelX = doubleValues[26];
		this.MaxRapidAccelY = doubleValues[27];
		this.MaxRapidAccelZ = doubleValues[28];
		this.MaxRapidAccelA = doubleValues[29];
		this.MaxRapidAccelB = doubleValues[30];
		this.MaxRapidAccelC = doubleValues[31];
		this.MaxRapidVelX = doubleValues[32];
		this.MaxRapidVelY = doubleValues[33];
		this.MaxRapidVelZ = doubleValues[34];
		this.MaxRapidVelA = doubleValues[35];
		this.MaxRapidVelB = doubleValues[36];
		this.MaxRapidVelC = doubleValues[37];

		this.SoftLimitNegX = doubleValues[38];
		this.SoftLimitNegY = doubleValues[39];
		this.SoftLimitNegZ = doubleValues[40];
		this.SoftLimitNegA = doubleValues[41];
		this.SoftLimitNegB = doubleValues[42];
		this.SoftLimitNegC = doubleValues[43];
		this.SoftLimitPosX = doubleValues[44];
		this.SoftLimitPosY = doubleValues[45];
		this.SoftLimitPosZ = doubleValues[46];
		this.SoftLimitPosA = doubleValues[47];
		this.SoftLimitPosB = doubleValues[48];
		this.SoftLimitPosC = doubleValues[49];

		this.CountsPerInchX = doubleValues[50];
		this.CountsPerInchY = doubleValues[51];
		this.CountsPerInchZ = doubleValues[52];
		this.CountsPerInchA = doubleValues[53];
		this.CountsPerInchB = doubleValues[54];
		this.CountsPerInchC = doubleValues[55];
		this.MaxLinearLength = doubleValues[56];

		this.UseOnlyLinearSegments = booleanValues[0];
		this.ArcsToSegs = booleanValues[1];
		this.DegreesA = booleanValues[2];
		this.DegreesB = booleanValues[3];
		this.DegreesC = booleanValues[4];
	}

	public double[] doubleValues() {
		double[] params = new double[57];

		params[0] = this.BreakAngle;
		params[1] = this.CollinearTol;
		params[2] = this.CornerTol;
		params[3] = this.FacetAngle;
		params[4] = this.TPLookahead;
		params[5] = this.RadiusA;
		params[6] = this.RadiusB;
		params[7] = this.RadiusC;
		params[8] = this.MaxAccelX;
		params[9] = this.MaxAccelY;
		params[10] = this.MaxAccelZ;
		params[11] = this.MaxAccelA;
		params[12] = this.MaxAccelB;
		params[13] = this.MaxAccelC;
		params[14] = this.MaxVelX;
		params[15] = this.MaxVelY;
		params[16] = this.MaxVelZ;
		params[17] = this.MaxVelA;
		params[18] = this.MaxVelB;
		params[19] = this.MaxVelC;

		params[20] = this.MaxRapidJerkX;
		params[21] = this.MaxRapidJerkY;
		params[22] = this.MaxRapidJerkZ;
		params[23] = this.MaxRapidJerkA;
		params[24] = this.MaxRapidJerkB;
		params[25] = this.MaxRapidJerkC;
		params[26] = this.MaxRapidAccelX;
		params[27] = this.MaxRapidAccelY;
		params[28] = this.MaxRapidAccelZ;
		params[29] = this.MaxRapidAccelA;
		params[30] = this.MaxRapidAccelB;
		params[31] = this.MaxRapidAccelC;
		params[32] = this.MaxRapidVelX;
		params[33] = this.MaxRapidVelY;
		params[34] = this.MaxRapidVelZ;
		params[35] = this.MaxRapidVelA;
		params[36] = this.MaxRapidVelB;
		params[37] = this.MaxRapidVelC;

		params[38] = this.SoftLimitNegX;
		params[39] = this.SoftLimitNegY;
		params[40] = this.SoftLimitNegZ;
		params[41] = this.SoftLimitNegA;
		params[42] = this.SoftLimitNegB;
		params[43] = this.SoftLimitNegC;
		params[44] = this.SoftLimitPosX;
		params[45] = this.SoftLimitPosY;
		params[46] = this.SoftLimitPosZ;
		params[47] = this.SoftLimitPosA;
		params[48] = this.SoftLimitPosB;
		params[49] = this.SoftLimitPosC;

		params[50] = this.CountsPerInchX;
		params[51] = this.CountsPerInchY;
		params[52] = this.CountsPerInchZ;
		params[53] = this.CountsPerInchA;
		params[54] = this.CountsPerInchB;
		params[55] = this.CountsPerInchC;
		params[56] = this.MaxLinearLength;

		return params;
	}

	public double BreakAngle;
	public double CollinearTol;
	public double CornerTol;
	public double FacetAngle;
	public double TPLookahead;
	public double RadiusA;
	public double RadiusB;
	public double RadiusC;
	public double MaxAccelX;
	public double MaxAccelY;
	public double MaxAccelZ;
	public double MaxAccelA;
	public double MaxAccelB;
	public double MaxAccelC;
	public double MaxVelX;
	public double MaxVelY;
	public double MaxVelZ;
	public double MaxVelA;
	public double MaxVelB;
	public double MaxVelC;

	public double MaxRapidJerkX; // used for 3rd order Rapids
	public double MaxRapidJerkY;
	public double MaxRapidJerkZ;
	public double MaxRapidJerkA;
	public double MaxRapidJerkB;
	public double MaxRapidJerkC;
	public double MaxRapidAccelX;
	public double MaxRapidAccelY;
	public double MaxRapidAccelZ;
	public double MaxRapidAccelA;
	public double MaxRapidAccelB;
	public double MaxRapidAccelC;
	public double MaxRapidVelX;
	public double MaxRapidVelY;
	public double MaxRapidVelZ;
	public double MaxRapidVelA;
	public double MaxRapidVelB;
	public double MaxRapidVelC;

	public double SoftLimitNegX; // Soft Limits (inches)
	public double SoftLimitNegY;
	public double SoftLimitNegZ;
	public double SoftLimitNegA;
	public double SoftLimitNegB;
	public double SoftLimitNegC;
	public double SoftLimitPosX;
	public double SoftLimitPosY;
	public double SoftLimitPosZ;
	public double SoftLimitPosA;
	public double SoftLimitPosB;
	public double SoftLimitPosC;

	public double CountsPerInchX;
	public double CountsPerInchY;
	public double CountsPerInchZ;
	public double CountsPerInchA;
	public double CountsPerInchB;
	public double CountsPerInchC;
	public double MaxLinearLength;

	public boolean UseOnlyLinearSegments;
	public boolean ArcsToSegs;
	public boolean DegreesA;
	public boolean DegreesB;
	public boolean DegreesC;

	public boolean[] booleanValues() {
		boolean[] values = new boolean[5];
		values[0] = this.UseOnlyLinearSegments;
		values[1] = this.ArcsToSegs;
		values[2] = this.DegreesA;
		values[3] = this.DegreesB;
		values[4] = this.DegreesC;

		return values;
	}
}
