#include "NeuronMultiplication.h"
#include <cassert>

fenn::NeuronMultiplication::NeuronMultiplication(const NeuronAllele &neuronAllele)
	: Neuron(neuronAllele)
{
	assert(neuronAllele.eNeuronType == NEURONTYPE_MULTIPLICATION);
}

fenn::NeuronMultiplication::~NeuronMultiplication()
{
}

bool fenn::NeuronMultiplication::Fire()
{
	//loop through all inputs and multiply them
	m_fValue = 1.0f;
	VALUE_TYPE fTemp;

	for (const auto &input : m_veInputs) {
		input.pNeuron->GetValue(fTemp);
		m_fValue *= fTemp;
		m_fValue *= input.fWeight;
	}

	//done
	return true;
}

fenn::NeuronType fenn::NeuronMultiplication::GetNeuronType() const
{
	return NEURONTYPE_MULTIPLICATION;
}