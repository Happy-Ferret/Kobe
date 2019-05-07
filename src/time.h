/*-----------------------------

 [time.h]
 - Alexander Brandt 2018
-----------------------------*/

#ifndef KO_TIME_H
#define KO_TIME_H

	#include "kobe.h"

	unsigned koInitTimeModule();
	void koStopTimeModule();

	uint32_t koGetTime();
	void koSleep(uint32_t ms);

#endif
