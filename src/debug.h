#pragma once

// For all debugs ; be careful, many text
#define DEBUG_ALL	0

// Generic debug
#define DEBUG			0

// Resolution
#define DEBUG_EVALUATION	1
#define DEBUG_PARCOURS_COORDS	0

/* define all debug if DEBUG_ALL defined to 1 */
#if DEBUG_ALL
    #define DEBUG		1
    #define DEBUG_EVALUATION	1
#endif
