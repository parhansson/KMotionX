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
	nChanges=index=0;
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
		cur_state = *CurSetup;
		m_FirstCall=false;
	}
	else
	{
		int n;
		int i,k = 0;
		__int64 *s=(__int64 *)CurSetup;   // new state
		__int64 *d=(__int64 *)&cur_state; // previous state

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
			if (s[i] != d[i])  // Changed?
			{
				C.data = d[i];  // save previous state
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

	    for (int n = 0; n < CurSetup->parameter_occurrence; n++)
		{	
			i = (__int64 *)(&CurSetup->parameters[_setup.parameter_numbers[n]]) - s;
			if (s[i] != d[i])  // Changed?
			{
				C.data = d[i];  // save previous state
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

	    for (int n = 0; n < CurSetup->n_ParamChanges; n++)
		{	
			i = (__int64 *)(&CurSetup->parameters[_setup.ParamChanges[n]]) - s;
			if (s[i] != d[i])  // Changed?
			{
				C.data = d[i];  // save previous state
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
			d[C.offset] = C.data;
		}
		while (!C.first);
	}
}
