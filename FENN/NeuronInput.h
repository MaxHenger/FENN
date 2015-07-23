#ifndef NEURONINPUT_H
#define NEURONINPUT_H

#include "Config.h"
#include "Neuron.h"

namespace fenn
{
	class NeuronInput : public Neuron
	{
	public:
		//simple constructor and destructor
		NeuronInput(const NeuronAllele &neuronAllele);
		~NeuronInput();

		//implement the Fire(...) function to assert it will never occur
		bool Fire();
		NeuronType GetNeuronType() const;

	private:
		NeuronInput(const NeuronInput &neuron) {};
		void operator = (const NeuronInput &neuron) {};
	};
}

#endif