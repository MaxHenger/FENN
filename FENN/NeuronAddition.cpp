#include "NeuronAddition.h"
#include <cassert>

fenn::NeuronAddition::NeuronAddition(const NeuronAllele &neuronAllele)
	: Neuron(neuronAllele)
{
	assert(neuronAllele.eNeuronType == NEURONTYPE_ADDITION);
}

fenn::NeuronAddition::~NeuronAddition()
{
}

bool fenn::NeuronAddition::Fire()
{
	//loop through all values and add them
	m_fValue = 0;
	VALUE_TYPE fTemp;

	for (const auto &input : m_veInputs) {
		input.pNeuron->GetValue(fTemp);
		fTemp *= input.fWeight;
		m_fValue += fTemp;
	}

	return true;
}

fenn::NeuronType fenn::NeuronAddition::GetNeuronType() const
{
	return NEURONTYPE_ADDITION;
}