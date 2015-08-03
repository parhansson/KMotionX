#include "stdafx.h"
#include "SetupTracker.h"
#include "HiResTimer.h"

CSetupTracker::CSetupTracker(void)
{
	m_FirstCall=true;
}

CSetupTracker::~CSetupTracker(void)
{
}

// Clear List of changes
void CSetupTracker::ClearHistory(void)
{
	nChanges=nApplied=index=realtime_index=0;
	m_FirstCall=true;
}

// Save the current state

// note: sequence_number is incremented when we read a line of GCode from the file
//       we Insert the State before we Read/Execute the line of GCode

int CSetupTracker::InsertState(setup *CurSetup)
{
	CHANGE C;

	if (m_FirstCall)
	{
		realtime_state = cur_state = *CurSetup;
		m_FirstCall=false;
	}
	else
	{
		int n;
		int i,k = 0;
		__int64 *s=(__int64 *)CurSetup;   // new state
		__int64 *d=(__int64 *)&cur_state; // previous state
		__int64 X;

		// only compare the first part of the structure that
		// has all the misc stuff.  The interpreter keeps track
		// of each parameter that gets changed so only check
		// the parameter table if it detected a change
		
//		if (CurSetup->parameter_occurrence > 0)
//			n = offsetof(setup,line_length)/sizeof(__int64);
//		else
		
		n = offsetof(setup,parameters)/sizeof(__int64);
		
		C.first = true;
		for (i=0; i<n; i++)
		{
			X = s[i] ^ d[i];  // xor words
			if (X)  // Changed?
			{
				C.data = X;  // save changes
				C.offset = i;	// save offset
				Buffer[index++] = C;
				if (index>=MAX_TRACKER) index=0;
				nChanges++;
				C.first = false;
				d[i] = s[i];    // update with the change
				k++;
			}
		}

		// Interpreter keeps track if any parameters changed so check those also

/* tktk - which vars are changed per line should not need to be tracked, only the final values
	    for (int n = 0; n < CurSetup->parameter_occurrence; n++)
		{	
			i = (__int64 *)(&CurSetup->parameters[_setup.parameter_numbers[n]]) - s;
			X = s[i] ^ d[i];  // xor words
			if (X)  // Changed?
			{
				C.data = X;  // save changes
				C.offset = i;	// save offset
				Buffer[index++] = C;
				if (index>=MAX_TRACKER) index=0;
				nChanges++;
				C.first = false;
				d[i] = s[i];    // update with the change
				k++;
			}
		}
*/

		// Interpreter keeps track if any parameters changed so check those also

	    for (int n = 0; n < CurSetup->n_ParamChanges; n++)
		{	
			i = (__int64 *)(&CurSetup->parameters[_setup.ParamChanges[n]]) - s;
			X = s[i] ^ d[i];  // xor words
			if (X)  // Changed?
			{
				C.data = X;  // save changes
				C.offset = i;	// save offset
				Buffer[index++] = C;
				if (index>=MAX_TRACKER) index=0;
				nChanges++;
				C.first = false;
				d[i] = s[i];    // update with the change
				k++;
			}
		}
	}
	return 0;
}

// Restore the state to what it was at the beg of this line (sequence number)
int CSetupTracker::RestoreState(int sequence_number, setup *CurSetup)
{
	CHANGE C;
	__int64 *d=(__int64 *)CurSetup;

	*CurSetup = cur_state;  // restore to last known state 

	for (;;)
	{
		// are we at the correct sequence number?
		if (CurSetup->sequence_number == sequence_number) return 0;

		// any changes remaining?
		if (nChanges <= 0) return 1;  // no, error

		// yes, apply one set
		do
		{
			index--;
			nChanges--;
			if (index < 0) index=MAX_TRACKER-1;

			C = Buffer[index];
			d[C.offset] ^= C.data;
		}
		while (!C.first);
	}
}


// Advance real-time state to beg of this line (sequence number)
// This basically tracks the delayed real-time state of the machine
int CSetupTracker::AdvanceState(int sequence_number)
{
	CHANGE C;
	__int64 *d=(__int64 *)&realtime_state;

	if (realtime_state.sequence_number <= sequence_number)  // need to advance forward?
	{
		for (;;)
		{
			// are we at the correct sequence number?
			if (realtime_state.sequence_number >= sequence_number) return 0;

			// any changes remaining?
			if (nApplied >= nChanges) return 1;  // no, error

			// yes, apply one set
			// loop until the next is a "first" or the end of the list
			C = Buffer[realtime_index];
			do
			{
				d[C.offset] ^= C.data;
				nApplied++;
				realtime_index++;
				if (realtime_index >= MAX_TRACKER) realtime_index=0;
				if (nApplied >= nChanges) break;  // no more
				C = Buffer[realtime_index];
			}
			while (!C.first);
		}
	}
	else
	{
		// need to advance backward
		for (;;)
		{
			// are we at the correct sequence number?
			if (realtime_state.sequence_number == sequence_number) return 0;

			// any changes remaining?
			if (nApplied <= 0) return 1;  // no, error

			// yes, apply one set
			do
			{
				realtime_index--;
				nApplied--;
				if (realtime_index < 0) realtime_index=MAX_TRACKER-1;

				C = Buffer[realtime_index];
				d[C.offset] ^= C.data;
			}
			while (!C.first);
		}
	}
}

