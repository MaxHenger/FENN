#include "NeuronDivision.h"
#include <cassert>

fenn::NeuronDivision::NeuronDivision(const NeuronAllele &neuronAllele)
	: Neuron(neuronAllele)
{
	assert(neuronAllele.eNeuronType == NEURONTYPE_DIVISION);
}

fenn::NeuronDivision::~NeuronDivision()
{
}

bool fenn::NeuronDivision::Fire()
{
	//set the neuron value to a default value, multiply by all neurons connected
	//to slot 1 and divide by all neurons connected to slot 2
	m_fValue = 1.0f;
	VALUE_TYPE fTemp;

	for (const auto &input : m_veInputs) {
		input.pNeuron->GetValue(fTemp);
		fTemp *= input.fWeight;

		if (input.nSlot == 0) {
			//multiply
			m_fValue *= fTemp;
		} else {
			//divide, ensure no division by 0 occurs
			if (-VALUE_EPSILON < fTemp && fTemp < VALUE_EPSILON) {
				//consider the value of fTemp as 0
				return false;
			}

			m_fValue /= fTemp;
		}
	}

	//done
	return true;
}

fenn::NeuronType fenn::NeuronDivision::GetNeuronType() const
{
	return NEURONTYPE_DIVISION;
}