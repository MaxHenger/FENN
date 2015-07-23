#ifndef FENN_NEURONMULTIPLICATION_H
#define FENN_NEURONMULTIPLICAITON_H

#include "Config.h"
#include "Neuron.h"

namespace fenn
{
	class NeuronMultiplication : public Neuron
	{
	public:
		NeuronMultiplication(const NeuronAllele &allele);
		~NeuronMultiplication();

		bool Fire();
		NeuronType GetNeuronType() const;

	private:
		NeuronMultiplication(const NeuronMultiplication &neuron) {};
		void operator = (const NeuronMultiplication &neuron) {};
	};
}

#endif