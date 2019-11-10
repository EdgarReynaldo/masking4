////////////////////////////////////////////////////////////////////////////////
//         MASkinG - Miran Amon's Skinnable GUI Library for Allegro           //
//    Copyright (c) 2002-2005 Miran Amon (miranamon@users.sourceforge.net)    //
//          Project websites: http://ferisrv5.uni-mb.si/~ma0747               //
//                  http://sourceforge.net/projects/masking                   //
////////////////////////////////////////////////////////////////////////////////

#include "../include/MASkinG/timer.h"
#include "../include/MASkinG/settings.h"
#include <allegro.h>

static const int nTimers = 8;

/* user timer counters */
static volatile int MAS_timer[] = { 0,0,0,0,0,0,0,0 };
static volatile int MAS_skip[] = { 0,0,0,0,0,0,0,0 };

/* user timers */
static void MAS_timer0_f(void) { MAS_timer[0]++; } END_OF_STATIC_FUNCTION(MAS_timer0_f);
static void MAS_timer1_f(void) { MAS_timer[1]++; } END_OF_STATIC_FUNCTION(MAS_timer1_f);
static void MAS_timer2_f(void) { MAS_timer[2]++; } END_OF_STATIC_FUNCTION(MAS_timer2_f);
static void MAS_timer3_f(void) { MAS_timer[3]++; } END_OF_STATIC_FUNCTION(MAS_timer3_f);
static void MAS_timer4_f(void) { MAS_timer[4]++; } END_OF_STATIC_FUNCTION(MAS_timer4_f);
static void MAS_timer5_f(void) { MAS_timer[5]++; } END_OF_STATIC_FUNCTION(MAS_timer5_f);
static void MAS_timer6_f(void) { MAS_timer[6]++; } END_OF_STATIC_FUNCTION(MAS_timer6_f);
static void MAS_timer7_f(void) { MAS_timer[7]++; } END_OF_STATIC_FUNCTION(MAS_timer7_f);

/* have we already locked the timer callbacks and counters? */
static bool lockedTimers = false;
static int timers[] = { 0,0,0,0,0,0,0,0 };


////////////////////////////////////////////////////////////////////////////////
// Locks the timer callback functions and the timer counters
void MAS::Timer::Lock() {
	if (!lockedTimers) {
		LOCK_VARIABLE(MAS_timer[0]);
		LOCK_VARIABLE(MAS_timer[1]);
		LOCK_VARIABLE(MAS_timer[2]);
		LOCK_VARIABLE(MAS_timer[3]);
		LOCK_VARIABLE(MAS_timer[4]);
		LOCK_VARIABLE(MAS_timer[5]);
		LOCK_VARIABLE(MAS_timer[6]);
		LOCK_VARIABLE(MAS_timer[7]);
		LOCK_FUNCTION(MAS_timer0_f);
		LOCK_FUNCTION(MAS_timer1_f);
		LOCK_FUNCTION(MAS_timer2_f);
		LOCK_FUNCTION(MAS_timer3_f);
		LOCK_FUNCTION(MAS_timer4_f);
		LOCK_FUNCTION(MAS_timer5_f);
		LOCK_FUNCTION(MAS_timer6_f);
		LOCK_FUNCTION(MAS_timer7_f);
		lockedTimers = true;
	}
}


////////////////////////////////////////////////////////////////////////////////
// Finds an empty timer slot and tries to install a timer function.
// Returns ID of the timer on success or -1 on failure.
int MAS::Timer::Install(int interval) {
	if (timers[0] == 0) {
		if (install_int(MAS_timer0_f, interval) < 0) return -1;
		else {
			timers[0] = interval;
			return 0;
		}
	}
	if (timers[1] == 0) {
		if (install_int(MAS_timer1_f, interval) < 0) return -1;
		else {
			timers[1] = interval;
			return 1;
		}
	}
	if (timers[2] == 0) {
		if (install_int(MAS_timer2_f, interval) < 0) return -1;
		else {
			timers[2] = interval;
			return 2;
		}
	}
	if (timers[3] == 0) {
		if (install_int(MAS_timer3_f, interval) < 0) return -1;
		else {
			timers[3] = interval;
			return 3;
		}
	}
	if (timers[4] == 0) {
		if (install_int(MAS_timer4_f, interval) < 0) return -1;
		else {
			timers[4] = interval;
			return 4;
		}
	}
	if (timers[5] == 0) {
		if (install_int(MAS_timer5_f, interval) < 0) return -1;
		else {
			timers[5] = interval;
			return 5;
		}
	}
	if (timers[6] == 0) {
		if (install_int(MAS_timer6_f, interval) < 0) return -1;
		else {
			timers[6] = interval;
			return 6;
		}
	}
	if (timers[7] == 0) {
		if (install_int(MAS_timer7_f, interval) < 0) return -1;
		else {
			timers[7] = interval;
			return 7;
		}
	}

	return -1;
}


int MAS::Timer::InstallEx(int bps) {
	if (timers[0] == 0) {
		if (install_int_ex(MAS_timer0_f, BPS_TO_TIMER(bps)) < 0) return -1;
		else {
			timers[0] = BPS_TO_TIMER(bps);
			return 0;
		}
	}
	if (timers[1] == 0) {
		if (install_int_ex(MAS_timer1_f, BPS_TO_TIMER(bps)) < 0) return -1;
		else {
			timers[1] = BPS_TO_TIMER(bps);
			return 1;
		}
	}
	if (timers[2] == 0) {
		if (install_int_ex(MAS_timer2_f, BPS_TO_TIMER(bps)) < 0) return -1;
		else {
			timers[2] = BPS_TO_TIMER(bps);
			return 2;
		}
	}
	if (timers[3] == 0) {
		if (install_int_ex(MAS_timer3_f, BPS_TO_TIMER(bps)) < 0) return -1;
		else {
			timers[3] = BPS_TO_TIMER(bps);
			return 3;
		}
	}
	if (timers[4] == 0) {
		if (install_int_ex(MAS_timer4_f, BPS_TO_TIMER(bps)) < 0) return -1;
		else {
			timers[4] = BPS_TO_TIMER(bps);
			return 4;
		}
	}
	if (timers[5] == 0) {
		if (install_int_ex(MAS_timer5_f, BPS_TO_TIMER(bps)) < 0) return -1;
		else {
			timers[5] = BPS_TO_TIMER(bps);
			return 5;
		}
	}
	if (timers[6] == 0) {
		if (install_int_ex(MAS_timer6_f, BPS_TO_TIMER(bps)) < 0) return -1;
		else {
			timers[6] = BPS_TO_TIMER(bps);
			return 6;
		}
	}
	if (timers[7] == 0) {
		if (install_int_ex(MAS_timer7_f, BPS_TO_TIMER(bps)) < 0) return -1;
		else {
			timers[7] = BPS_TO_TIMER(bps);
			return 7;
		}
	}

	return -1;
}


////////////////////////////////////////////////////////////////////////////////
// Changes the speed of a timer that was already installed with the
// Install() function.
void MAS::Timer::Adjust(int timerID, int interval) {
	switch (timerID) {
		case 0:		install_int(MAS_timer0_f, interval);	break;
		case 1:		install_int(MAS_timer1_f, interval);	break;
		case 2:		install_int(MAS_timer2_f, interval);	break;
		case 3:		install_int(MAS_timer3_f, interval);	break;
		case 4:		install_int(MAS_timer4_f, interval);	break;
		case 5:		install_int(MAS_timer5_f, interval);	break;
		case 6:		install_int(MAS_timer6_f, interval);	break;
		case 7:		install_int(MAS_timer7_f, interval);	break;
		default:	return;								break;
	}

	timers[timerID] = interval;
}


void MAS::Timer::AdjustEx(int timerID, int bps) {
	switch (timerID) {
		case 0:		install_int_ex(MAS_timer0_f, BPS_TO_TIMER(bps));	break;
		case 1:		install_int_ex(MAS_timer1_f, BPS_TO_TIMER(bps));	break;
		case 2:		install_int_ex(MAS_timer2_f, BPS_TO_TIMER(bps));	break;
		case 3:		install_int_ex(MAS_timer3_f, BPS_TO_TIMER(bps));	break;
		case 4:		install_int_ex(MAS_timer4_f, BPS_TO_TIMER(bps));	break;
		case 5:		install_int_ex(MAS_timer5_f, BPS_TO_TIMER(bps));	break;
		case 6:		install_int_ex(MAS_timer6_f, BPS_TO_TIMER(bps));	break;
		case 7:		install_int_ex(MAS_timer7_f, BPS_TO_TIMER(bps));	break;
		default:	return;								break;
	}

	timers[timerID] = bps;
}


////////////////////////////////////////////////////////////////////////////////
// Removes a timer
void MAS::Timer::Kill(int timerID) {
	switch (timerID) {
		case 0:		remove_int(MAS_timer0_f);	break;
		case 1:		remove_int(MAS_timer1_f);	break;
		case 2:		remove_int(MAS_timer2_f);	break;
		case 3:		remove_int(MAS_timer3_f);	break;
		case 4:		remove_int(MAS_timer4_f);	break;
		case 5:		remove_int(MAS_timer5_f);	break;
		case 6:		remove_int(MAS_timer6_f);	break;
		case 7:		remove_int(MAS_timer7_f);	break;
		default:	return;						break;
	}

	timers[timerID] = 0;
}


////////////////////////////////////////////////////////////////////////////////
// Checks whether a timer variable is positive. If it is it returns its index
// to indicate a timer event has occured otherwise returns -1.
int MAS::Timer::Check() {
	int i;
	for (i=0; i<nTimers; i++) {
		if (MAS_timer[i] > 0) {
			++MAS_skip[i];
			if (MAS_skip[i] > MAS::Settings::maxFrameSkip) {
				MAS_timer[i] = 0;
				MAS_skip[i] = 0;
				continue;
			}
			return i;
		}
	}
	
	return -1;
}


////////////////////////////////////////////////////////////////////////////////
// Updates the passed timer's tick counter by decrementing it
void MAS::Timer::Update(int timerID) {
	if (timerID >= 0 && MAS_timer[timerID] > 0) {
		--MAS_timer[timerID];
	}
}


////////////////////////////////////////////////////////////////////////////////
// Reset the frame skip counters
void MAS::Timer::ResetFrameSkip() {
	for (int i=0; i<nTimers; i++) {
		MAS_skip[i] = 0;
	}
}
