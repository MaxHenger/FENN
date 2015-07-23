#ifndef FENN_NEURONADDITION_H
#define FENN_NEURONADDITION_H

#include "Config.h"
#include "Neuron.h"

namespace fenn
{
	class NeuronAddition : public Neuron
	{
	public:
		NeuronAddition(const NeuronAllele &neuronAllele);
		~NeuronAddition();

		bool Fire();
		NeuronType GetNeuronType() const;

	private:
		NeuronAddition(const NeuronAddition &neuron) {};
		void operator = (const NeuronAddition &neuron) {};
	};
}

#endif