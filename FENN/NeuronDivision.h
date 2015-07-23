#ifndef FENN_NEURONDIVISION_H
#define FENN_NEURONDIVISION_H

#include "Config.h"
#include "Neuron.h"

namespace fenn
{
	class NeuronDivision : public Neuron 
	{
	public:
		NeuronDivision(const NeuronAllele &neuronAllele);
		~NeuronDivision();

		bool Fire();
		NeuronType GetNeuronType() const;

	private:
		NeuronDivision(const NeuronDivision &neuron) {};
		void operator = (const NeuronDivision &neuron) {};
	};
}

#endif