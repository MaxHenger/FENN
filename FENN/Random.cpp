#include "Random.h"
#include <cstdlib> //for RAND_MAX

#include <random>

void fenn::InitializeRandom(const unsigned int nSeed)
{
	srand(nSeed);
}

void fenn::ReleaseRandom()
{
	//this function doesn't do anything. But perhaps, if I would implement
	//C++11-like random number generators, this will be useful
}

unsigned int fenn::GetRandom()
{
	return rand();
}

bool fenn::GetRandomBinary()
{
	return (rand() > ((RAND_MAX - 1) >> 2));
}

CHANCE_TYPE fenn::GetRandomContinuous()
{
	return (CHANCE_TYPE)(rand()) / RAND_MAX;
}