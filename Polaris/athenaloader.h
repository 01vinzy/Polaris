/*
 * [irma] This class is unfinished, but I plan to have a system here that loads the main Polaris behaviour when the player readies up
*/

#ifndef ATHENALOADER_H
#define ATHENALOADER_H

#include "common.h"

namespace polaris 
{
	class AthenaLoader 
	{
	public:
		AthenaLoader();
		~AthenaLoader();
	};
}

static polaris::AthenaLoader* gpFrontendManager;

#endif // ATHENALOADER_H
