#ifndef FENN_RANDOM_H
#define FENN_RANDOM_H

#include "FENNConfig.h"

namespace fenn
{
	void InitializeRandom(const unsigned int nSeed);
	void ReleaseRandom();

	unsigned int GetRandom();
	bool GetRandomBinary();
	CHANCE_TYPE GetRandomContinuous();
};

#endif