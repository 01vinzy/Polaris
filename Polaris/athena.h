#ifndef ATHENA_H
#define ATHENA_H

#include "common.h"

namespace polaris
{
	class Athena
	{
	public:
		Athena();
		~Athena();
	};
}

static polaris::Athena* gpAthena;

#endif // ATHENA_H
