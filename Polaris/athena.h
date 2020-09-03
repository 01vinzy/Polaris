#ifndef ATHENA_H
#define ATHENA_H

#include "common.h"
#include "playerpawn_polaris.h"

namespace polaris
{
	class Athena
	{
	public:
		PlayerPawnPolaris* m_pPlayerPawnPolaris;

		Athena();
		~Athena();
	};
}

static polaris::Athena* gpAthena;

#endif // ATHENA_H
