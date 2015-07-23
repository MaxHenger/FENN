#ifndef FENN_WORLD_H
#define FENN_WORLD_H

#include "Config.h"

#include <COMTSingleton.h>

namespace fenn
{
	//forward definitions
	enum NeuronType;

	class World
	{
	public:
		World();
		~World();

		//retrieving globally tracked indices
		GLOBAL_INDEX_TYPE	GetNewNeuronIndex();
		void				ResetNeuronIndices();

		GLOBAL_INDEX_TYPE	GetNewConnectionIndex();
		void				ResetConnectionIndices();

		//retrieving specifc random numbers (neuron mapping function, 
		NeuronType			GetRandomNeuronType();
		WEIGHT_TYPE			GetRandomWeight();
		WEIGHT_TYPE			GetRandomWeightAdditive();
		BIAS_TYPE			GetRandomBias();
		BIAS_TYPE			GetRandomBiasAdditive();
		SLOT_INDEX_TYPE		GetRandomSlot();

	protected:
		GLOBAL_INDEX_TYPE	m_nNeuronIndex;
		GLOBAL_INDEX_TYPE	m_nConnectionIndex;
	};

	typedef comt::SingletonLazy<World> world_single;
}

#endif