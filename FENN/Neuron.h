#ifndef FENN_NEURON_H
#define FENN_NEURON_H

#include "Config.h"
#include "RNA.h"

#include <vector>

namespace fenn
{
	enum NeuronType
	{
		NEURONTYPE_INPUT = 0,
		NEURONTYPE_ADDITION = 1,
		NEURONTYPE_SUBTRACTION = 2,
		NEURONTYPE_MULTIPLICATION = 3,
		NEURONTYPE_DIVISION = 4,
		NEURONTYPE_TOTAL = 5,
		NEURONTYPE_UNKNOWN = 6,
	};

	class Neuron 
	{
	public:
		//constructor and destructor
		Neuron(const NeuronAllele &neuronAllele);
		virtual ~Neuron() {};

		//assignment of input and output
		void SetInput(Neuron *pNeuron, LOCAL_INDEX_TYPE nIndex, const ConnectionAllele &connectionAllele);
		void SetValue(VALUE_TYPE value);
		void GetValue(VALUE_TYPE &result);
		virtual bool Fire() = 0;
		virtual NeuronType GetNeuronType() const = 0;

	protected:
		//storage of the calculated/set value and bias
		GLOBAL_INDEX_TYPE m_nIndex;
		VALUE_TYPE		m_fValue;
		BIAS_TYPE		m_fBias;

		//storage of the inputs and outputs
		struct InputEntry 
		{
			Neuron				*pNeuron;
			SLOT_INDEX_TYPE		nSlot;
			WEIGHT_TYPE			fWeight;
		};

		std::vector<InputEntry> m_veInputs;
	};

	//global functions to create/remove neurons (to make it easier to add memory
	//managers later in the development cycle)
	Neuron* CreateNeuron(const NeuronAllele &allele);
	void ReleaseNeuron(Neuron* const pNeuron);
}

#endif