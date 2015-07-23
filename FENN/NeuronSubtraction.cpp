#include "NeuronSubtraction.h"
#include <cassert>

fenn::NeuronSubtraction::NeuronSubtraction(const NeuronAllele &neuronAllele)
	: Neuron(neuronAllele)
{
	assert(neuronAllele.eNeuronType == NEURONTYPE_SUBTRACTION);
}

fenn::NeuronSubtraction::~NeuronSubtraction()
{
}

bool fenn::NeuronSubtraction::Fire()
{
	//loop through all neurons, if connected to slot 1 then add the value, if connected
	//to slot 2 then subtract the value
	m_fValue = 0;
	VALUE_TYPE fTemp;

	for (const auto &input : m_veInputs) {
		//retrieve value and modify with weight
		input.pNeuron->GetValue(fTemp);
		fTemp *= input.fWeight;

		if (input.nSlot == 0) {
			m_fValue += fTemp;
		} else {
			m_fValue -= fTemp;
		}
	}

	//return
	return true;
}

fenn::NeuronType fenn::NeuronSubtraction::GetNeuronType() const
{
	return NEURONTYPE_SUBTRACTION;
}