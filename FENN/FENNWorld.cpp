#include "FENNWorld.h"
#include "FENNNeuronBase.h"
#include "FENNRandom.h"

#include <cstdlib> //for RAND_MAX

fenn::World::World()
	: m_nNeuronIndex(0)
	, m_nConnectionIndex(0)
{
}

fenn::World::~World()
{
	//nothing to do yet
}

GLOBAL_INDEX_TYPE fenn::World::GetNewNeuronIndex()
{
	const GLOBAL_INDEX_TYPE nCur = m_nNeuronIndex;
	m_nNeuronIndex++;
	return nCur;
}

void fenn::World::ResetNeuronIndices()
{
	m_nNeuronIndex = 0;
}

GLOBAL_INDEX_TYPE fenn::World::GetNewConnectionIndex()
{
	const GLOBAL_INDEX_TYPE nCur = m_nConnectionIndex;
	m_nConnectionIndex++;
	return nCur;
}

void fenn::World::ResetConnectionIndices()
{
	m_nConnectionIndex = 0;
}

fenn::NeuronType fenn::World::GetRandomNeuronType()
{
	return (NeuronType)(GetRandom() % (unsigned int)(fenn::NEURONTYPE_TOTAL));
}

WEIGHT_TYPE fenn::World::GetRandomWeight()
{
	return (WEIGHT_TYPE)(GetRandom()) / RAND_MAX * WEIGHT_RANGE + WEIGHT_MIN;
}

WEIGHT_TYPE fenn::World::GetRandomWeightAdditive()
{
	return (WEIGHT_TYPE)(GetRandom()) / RAND_MAX * WEIGHT_ADDITIVE_RANGE + WEIGHT_ADDITIVE_MIN;
}

BIAS_TYPE fenn::World::GetRandomBias()
{
	return (BIAS_TYPE)(GetRandom()) / RAND_MAX * BIAS_RANGE + BIAS_MIN;
}

BIAS_TYPE fenn::World::GetRandomBiasAdditive()
{
	return (BIAS_TYPE)(GetRandom()) / RAND_MAX * BIAS_ADDITIVE_RANGE + BIAS_ADDITIVE_MIN;
}

SLOT_INDEX_TYPE fenn::World::GetRandomSlot()
{
	return (SLOT_INDEX_TYPE)(GetRandom() % SLOT_MAX);
}