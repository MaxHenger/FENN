#ifndef FENN_NEURONSUBTRACTION_H
#define FENN_NEURONSUBTRACTION_H

#include "Config.h"
#include "Neuron.h"

namespace fenn
{
	class NeuronSubtraction : public Neuron
	{
	public:
		NeuronSubtraction(const NeuronAllele &neuronAllele);
		~NeuronSubtraction();

		bool Fire();
		NeuronType GetNeuronType() const;

	private:
		NeuronSubtraction(const NeuronSubtraction &neuron) {};
		void operator = (const NeuronSubtraction &neuron) {};
	};
}

#endif