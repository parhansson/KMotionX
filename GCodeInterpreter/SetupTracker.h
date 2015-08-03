#pragma once

#include <afxtempl.h>
#include "rs274ngc.h"

typedef struct
{
	__int64 data; 
	int offset;
	bool first;
} CHANGE;


// Keeps track of the Interpreter State Changes for a number of States to be able
// to re-wind the Interpreter after a Halt. Assuming a max number of states (lines of GCode)
// and a typical number of 64-bit changes between states a fixed ring buffer size is declared to 
// hopefully keep enough history.  Basically we need to go back the amount of buffer lookahead
// time which is hard to translate into a numbe of Interpreter State Changes.  If it fails then
// we will not be able to cleanly resume after a Halt

// allow going back this number of state changes
#define MAX_TRACKER 500000

// The data is stored in ring buffer in the form of a number of changes between states.  As each change is
// detected it is placed in the buffer.  The first change will be marked as the first so when we process the
// changes in reverse order we will know a complete state (represeting a sequence number).  There will always
// be at least one change as thesequence number always changes

class CSetupTracker
{
public:
	CSetupTracker(void);
	~CSetupTracker(void);
	// Clear List of changes
	void ClearHistory(void);
	// Save the current state
	int InsertState(setup * CurSetup);
	// Restore the state to what it was at the beg of this line
	int RestoreState(int sequence_number, setup * CurSetup);
	// Advance real-time state to beg of this line (sequence number)
	// This basically tracks the delayed real-time state of the machine
	int CSetupTracker::AdvanceState(int sequence_number);

	setup cur_state;
	setup realtime_state;
	bool m_FirstCall;

	CHANGE Buffer[MAX_TRACKER];
	int nChanges;
	int nApplied;
	int index;  // circular index
	int realtime_index; // circular index
};
