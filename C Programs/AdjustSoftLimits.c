// #include this file to add functionality of adjusting softlimits feedhold based 
// on current Velocity and Acceleration.  Call CheckDistToStop for each Axis that
// should be serviced

void CheckDistToStop(CHAN *ch);
void CalcStopDist(double V0, double A0, double Amax, double Jmax, double *TotalDist);
void GetCurVelAccelSafe(CHAN *ch, double *CurVel, double *CurAccel);
void GetCurVelAccel(CHAN *ch, double *CurVel, double *CurAccel);
#define CALC_TIME_TICKS 20  // assume it takes ~2 millisec to calculate

void CheckDistToStop(CHAN *ch)
{
	double Jerk,t0,t1,t2;	
	int Nstates;
	double TotalDist, CurVel, CurAccel, StopDest;
	
	// Get current Velocity and Acceleration for the Axis
	GetCurVelAccelSafe(ch,&CurVel,&CurAccel);
	
	// Calc Dist to Stop
	CalcStopDist(CurVel,CurAccel,ch->Accel,ch->Jerk,&TotalDist);
	
	// Time/Distance Stop uses to calculate decel trajectory
	TotalDist += (CALC_TIME_TICKS+6) * TIMEBASE * CurVel;

	// Compute Destination where Stop should be initiated
	StopDest = ch->Dest + TotalDist;
	if ((StopDest > ch->SoftLimitPos && ch->LastNonZeroDir==1) || 
		(StopDest < ch->SoftLimitNeg && ch->LastNonZeroDir==-1))
		StopCoordinatedMotion();
}

// read state twice until readings match to detect if something changed during read
void GetCurVelAccelSafe(CHAN *ch, double *CurVel, double *CurAccel)
{
	double CurVel2, CurAccel2;
	
	GetCurVelAccel(ch, &CurVel2, &CurAccel2);

	for (;;)
	{
		GetCurVelAccel(ch, CurVel, CurAccel);
	
		if (*CurVel == CurVel2 && *CurAccel == CurAccel2) return;
		CurVel2 = *CurVel;
		CurAccel2 = *CurAccel;
	}
}

// take on-the-fly snapshot of Velocity and Acceleration
void GetCurVelAccel(CHAN *ch, double *CurVel, double *CurAccel)
{	
	TRIP_COEFF *C=ch->pcoeff;
		
	*CurVel  =  0;
	*CurAccel=  0;

	// are we doing a trajectory?
	if (C)
	{
		// yes, calc our current vel & accel

		if (C->trajectory_mode == TRAJECTORY_INDEPENDENT)
   		{
			*CurVel  = (3.0*C->a * ch->t + 2.0*C->b)*ch->t + C->c;
			*CurAccel=  6.0*C->a * ch->t + 2.0*C->b;
		}
		else if (C->trajectory_mode == TRAJECTORY_EXPONENTIAL)
		{
			// for Exponential moves just handle 2nd order changes
			*CurVel  = ch->last_vel;
			*CurAccel=  0;
		}
	}
}



// Determines how to go from an initial velocity and Acceleration
// to zero velocity with zero acceleration given max acceleration
// and jerk constraints.  If max acceleration is required the
// solution will have 3 states otherwise 2 states.
//
// returns the total dist (signed) that will be transversed until
// the final zero velocity is obtained

void CalcStopDist(double V0, double A0, double Amax, double Jmax, double *TotalDist)
{
	double Alim,Apeak,V0p,V1p;
	double dVa=fast_fabs(A0)*A0/(2*Jmax);  // velocity if we ramped the Accel straight to zero
	double dV=-V0;                  // desired final velocity change
	double J, t0, t1, t2;

	// determine if positive or negative jerk is initially required to
	// reach the final velocity.
	 	
	if (dVa > dV)
	{
		J   = -Jmax;
		Alim = -Amax;
	}
	else
	{
		J   =  Jmax;
		Alim =  Amax;
	}	
	// calc time to peak acceleration
	t0 = (-A0 / J + sqrtf(0.5*A0*A0/(J * J) + (-V0) / J));
	
	Apeak = A0 + J * t0;
	
	if (fast_fabs(Apeak) < Amax)
	{
		// calc final vel and dist traveled after 1st phase

		*TotalDist = ((J * t0 * (1.0/6.0) + A0 * (1.0/2.0)) * t0 + V0) * t0;
		V0p =  (J * t0 * (1.0/2.0) + A0) * t0 + V0;
		
		t1 = fast_fabs(Apeak / J);     /* state 1 */
		
		// calc dist traveled after 2nd phase
		
		*TotalDist += ((-J * t1 * (1.0/6.0) + Apeak * (1.0/2.0)) * t1 + V0p) * t1;

		return;
	}
	
	// acceleration limit exceeded add constant accel phase
	
	t0 = (Alim-A0) / J;
	t2 = fast_fabs((A0 + t0 * J) / J);     /* state 1 */
	t1 = (dV - J * t0 * t0 - 2.0 * A0 * t0 - A0*A0/(2 * J))/Alim;

	// calc final vel and dist traveled after 1st phase

	*TotalDist = ((J * t0 * (1.0/6.0) + A0 * (1.0/2.0)) * t0 + V0) * t0;
	V0p =  (J * t0 * (1.0/2.0) + A0) * t0 + V0;

	// calc final vel and dist traveled after 2nd phase

	*TotalDist += (Alim * (1.0/2.0) * t1 + V0p) * t1;
	V1p = Alim * t1 + V0p;

	// calc dist traveled after 3rd phase
	
	*TotalDist += ((-J * t2 * (1.0/6.0) + Alim * (1.0/2.0)) * t2 + V1p) * t2;
}

