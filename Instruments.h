#include <stdint.h>

#define MAX_NOTES 100
#define MAX_SONGS 5

struct Song{
	uint32_t numNotes;
	uint32_t* note;
	uint32_t* noteDuration;
}Song;

#define oboe 0
#define guitar 1
#define trumpet 2

const uint16_t Oboe[64] = { 1024,
1024,1014,1008,1022,
1065,1093,1006,858,
711,612,596,672,
806,952,1074,1154,
1191,1202,1216,1236,
1255,1272,1302,1318,
1299,1238,1140,1022,
910,827,779,758,
757,782,856,972,
1088,1177,1226,1232,
1203,1157,1110,1067,
1028,993,958,929,
905,892,900,940,
1022,1125,1157,1087,
965,836,783,816,
895,971,1017};

const uint16_t Guitar[64] = {
1024,1024,1026,1001,
935,833,730,647,
619,666,782,964,
1172,1337,1461,1536,
1558,1503,1396,1235,
1040,864,755,758,
877,1087,1358,1613,
1789,1846,1772,1625,
1454,1285,1173,1096,
1052,1034,1027,1036,
1079,1137,1184,1247,
1264,1250,1182,1081,
974,865,801,779,
810,844,859,859,
851,849,875,922,
977,1024,1024,1024
};

const uint16_t Trumpet[64] = {
987,1049,1090,1110,
1134,1160,1139,1092,
1070,1042,1035,1029,
1008,1066,1150,1170,
1087,915,679,372,
151,558,1014,1245,
1260,1145,1063,984,
934,960,1027,1077,
1081,1074,1064,1042,
1010,974,968,974,
994,1039,1094,1129,
1125,1092,1056,1056,
1082,1059,1046,1058,
1061,1045,1034,1050,
1094,1112,1092,1063,
1053,1065,1052,992
};

//note: some of these have been corrected from Valvanoware to http://www.vibrationdata.com/tutorials2/piano.pdf

#define C_2 38223   // 65.406 Hz
#define DF2 36077   // 69.296 Hz
#define D2 34052   // 73.416 Hz
#define EF2 32141   // 77.782 Hz
#define E2 30337   // 82.407 Hz
#define F2 28635   // 87.307 Hz
#define GF2 27027   // 92.499 Hz
#define G2 25511   // 97.999 Hz
#define AF2 24079   // 103.826 Hz
#define A2 22727   // 110.000 Hz
#define A2s 21452   // 116.541 Hz
#define B2 20248   // 123.471 Hz
#define C3 19111   // 130.813 Hz
#define C3s 18039   // 138.591 Hz
#define D3 17026   // 146.832 Hz
#define EF3 16071   // 155.563 Hz
#define E3 15169   // 164.814 Hz
#define F3 14317   // 174.614 Hz
#define F3s 13514   // 184.997 Hz
#define G3 12755   // 195.998 Hz
#define G3s 12039   // 207.652 Hz
#define A3 11364   // 220.000 Hz
#define A3s 10726   // 233.082 Hz
#define B3 10124   // 246.942 Hz
#define C4 9556   // 261.626 Hz
#define C4s 9019   // 277.183 Hz
#define D4 8513   // 293.665 Hz
#define D4s 8035   // 311.127 Hz
#define E4 7584   // 329.628 Hz
#define F4 7159   // 349.228 Hz
#define F4s 6757   // 369.994 Hz
#define G4 6378   // 391.995 Hz
#define G4s 6020   // 415.305 Hz
#define A4 5682   // 440.000 Hz
#define A4s 5363   // 466.164 Hz
#define B4 5062   // 493.883 Hz
#define C5 4778   // 523.251 Hz
#define C5s 4510   // 554.365 Hz
#define D5 4257   // 587.330 Hz
#define EF5 4018   // 622.254 Hz
#define E5 3792   // 659.255 Hz
#define F5 3579   // 698.456 Hz
#define F5s 3378   // 739.989 Hz
#define G5 3189   // 783.991 Hz
#define G5s 3010   // 830.609 Hz
#define A5 2841   // 880.000 Hz
#define A5s 2681   // 932.328 Hz
#define B5 2531   // 987.767 Hz
#define C6 2389   // 1046.502 Hz
#define C6s 2255   // 1108.731 Hz
#define D6 2128   // 1174.659 Hz
#define D6s 2009   // 1244.508 Hz
#define E6 1896   // 1318.510 Hz
#define F6 1790   // 1396.913 Hz
#define F6s 1689   // 1479.978 Hz
#define G6 1594   // 1567.982 Hz
#define G6s 1505   // 1661.219 Hz
#define A6 1420   // 1760.000 Hz
#define A6s 1341   // 1864.655 Hz
#define B6 1265   // 1975.533 Hz
#define C7 1194   // 2093.005 Hz

#define eit 10000000 //eighth note
#define eitdot 15000000 //eighth note
#define qtr 20000000 //quarter note
#define qtrdot 30000000 //quarter note
#define hlf 40000000 //half note
#define hlfdot 60000000 //half note
#define whole 80000000 //whole note


/* SONGS are below this line ------------------------------
* I'm really good at documenting my code guys.
*/

// Song - Together We Ride, Fire Emblem
// Tracks - 4 total, 2 treble, 2 bass
// Reminder that if short for memory, only use the "main" arrays from treble and bass
// Arranged by Maxwell Archibald

const uint32_t songBattleTrebleMainNotes[152] = {
		A5, 0, A5, 0, A5, 0, A5,		// Intro Sequence 1
		A5s, 0, A5s, 0, A5s, 0, A5s,
		C6, 0, C6, 0, C6, 0, C6,
		A5s, 0, A5s, 0, A5s, 0, A5s,
		A5, 0, A5, 0, A5, 0, A5,
		A5, 0, A5, 0, A5, 0, A5,
		
		A5, 0, A5, 0, A5, 0, A5,		// Intro Sequence 2
		A5s, 0, A5s, 0, A5s, 0, A5s,
		C6, 0, C6, 0, C6, 0, C6,
		A5s, 0, A5s, 0, A5s, 0, A5s,
		A5, 0, A5, 0, A5, 0, A5,
		A5, 0, E6, 0, E6, 0,
		
		E6, D6, E6, F6, 0,				// Theme Sequence 1
		E6, D6, E6, F6, 0,
		E6, D6, E6, F6, 0,
		G6, A6, G6, F6, E6, D6, C6s, E6,
		
		E6, D6, E6, A6, 0,				// Theme Sequence 2
		E6, D6, E6, A6, 0,
		E6, D6, E6, A6, 0,
		A6, A6s, A6, G6, A6, G6, F6, E6,
		
		F6, E6, D6, A5, 0,				// Theme Sequence 3
		F6, E6, D6, A5, 0,
		F6, E6, D6, C6s, 0, E6, 0,
		E6, 0, D6, 0, A6, 0
};

const uint32_t songBattleTrebleMainDur[152] = {
		eit/2, eit, eit/2, eit/2, eit/2, eit/2, eit/2,	// Intro Sequence 1
		eit/2, eit, eit/2, eit/2, eit/2, eit/2, eit/2,
		eit/2, eit, eit/2, eit/2, eit/2, eit/2, eit/2,
		eit/2, eit, eit/2, eit/2, eit/2, eit/2, eit/2,
		eit/2, eit, eit/2, eit/2, eit/2, eit/2, eit/2,
		eit/2, eit, eit/2, eit/2, eit/2, eit/2, eit/2,
		
		eit/2, eit, eit/2, eit/2, eit/2, eit/2, eit/2,	// Intro Sequence 2
		eit/2, eit, eit/2, eit/2, eit/2, eit/2, eit/2,
		eit/2, eit, eit/2, eit/2, eit/2, eit/2, eit/2,
		eit/2, eit, eit/2, eit/2, eit/2, eit/2, eit/2,
		eit/2, eit, eit/2, eit/2, eit/2, eit/2, eit/2,
		eit/2, (eit * 3)/2, eit/2, eit/2, eit/2, eit/2,
		
		eit/2, eit/2, eit/2, eit/2, qtr,				// Theme Sequence 1
		eit/2, eit/2, eit/2, eit/2, qtr,
		eit/2, eit/2, eit/2, eit/2, qtr,
		eit/2, eit/2, eit/2, eit/2, eit/2, eit/2, eit/2, eit/2, 
		
		eit/2, eit/2, eit/2, eit/2, qtr,				// Theme Sequence 2
		eit/2, eit/2, eit/2, eit/2, qtr,
		eit/2, eit/2, eit/2, eit/2, qtr,
		eit/2, eit/2, eit/2, eit/2, eit/2, eit/2, eit/2, eit/2, 
		
		eit/2, eit/2, eit/2, eit/2, qtr,				// Theme Sequence 3
		eit/2, eit/2, eit/2, eit/2, qtr,
		eit/2, eit/2, eit/2, eit/2, qtr,
		eit/2, eit/2, eit/2, eit/2, eit/2, eit/2, eit/2, eit/2
};

const uint32_t songBattleBassMainNotes[84] = {
		A2, E3, A3,						// Intro Sequence 1
		A2s, F3, A3s,
		C3, G3, C4,
		A2s, F3, A3s,
		A2, E3, A3,
		A2, E3, A3,
		
		A3, E4, A4,						// Intro Sequence 2
		A3s, F4, A4s,
		C4, G4, C5,
		A3s, F4, A4s,
		A3, E4, A4,
		A3, 0, 0,
		
		D4, A4, A3, A4,					// Theme Sequence 1
		A3s, F4, F3, F4,
		C4, G4, G3, G4,
		A3, E4, E3, E4,
		
		A3s, F4, F3, F4,				// Theme Sequence 2
		C4, G4, G3, G4,
		A3, E4, E3, E4,
		D4, A4, A3, A4,
		
		A3s, F4, F3, F4,				// Theme Sequence 3
		C4, G4, G3, G4,
		A3, E4, E3, E4,
		D4, A4, A3, A4
};

const uint32_t songBattleBassMainDur[84] = {
		eitdot, eitdot, eit,			// Intro Sequence 1
		eitdot, eitdot, eit,
		eitdot, eitdot, eit,
		eitdot, eitdot, eit,
		eitdot, eitdot, eit,
		eitdot, eitdot, eit,
		
		eitdot, eitdot, eit,			// Intro Sequence 2
		eitdot, eitdot, eit,
		eitdot, eitdot, eit,
		eitdot, eitdot, eit,
		eitdot, eitdot, eit,
		eit/2, eitdot, qtr,
		
		eitdot, eit, eit/2, eit,		// Theme Sequence 1
		eitdot, eit, eit/2, eit,
		eitdot, eit, eit/2, eit,
		eitdot, eit, eit/2, eit,
		
		eitdot, eit, eit/2, eit,		// Theme Sequence 2
		eitdot, eit, eit/2, eit,
		eitdot, eit, eit/2, eit,
		eitdot, eit, eit/2, eit,
		
		eitdot, eit, eit/2, eit,		// Theme Sequence 3
		eitdot, eit, eit/2, eit,
		eitdot, eit, eit/2, eit,
		eitdot, eit, eit/2, eit
};


const uint32_t scalesNotes[40] = {
	A2, B2, C3, D3,
	E3, F3, G3, A3,
	A3s, 0, B3, 0,
	0, 0, 0, 0,
	A5, 0, B5, 0,
	C6, 0, D6, 0,
	E6, 0, F6, 0,
	G6, 0, A6, 0,
	A6s, 0, B6, 0,
	0, 0, 0, 0
};

const uint32_t scalesDur[40] = {
		eit, eit, eit, eit,
		eit, eit, eit, eit,
		eit, eit, eit, eit,
		whole, whole, whole, whole,
		eit/2, eit, eit/2, eit,
		eit/2, eit, eit/2, eit,
		eit/2, eit, eit/2, eit,
		eit/2, eit, eit/2, eit,	
		eit/2, eit, eit/2, eit,	
		whole, whole, whole, whole
};

/* song structs */
const struct Song Main_Treble = { 152, (uint32_t *) songBattleTrebleMainNotes, (uint32_t *) songBattleTrebleMainDur };
const struct Song Main_Bass = { 84, (uint32_t *) songBattleBassMainNotes, (uint32_t *) songBattleBassMainDur };

const struct Song Scales = { 40, (uint32_t *) scalesNotes, (uint32_t *) scalesDur };

