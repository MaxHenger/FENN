#include "NeuronInput.h"
#include <cassert>

fenn::NeuronInput::NeuronInput(const NeuronAllele &neuronAllele)
	: Neuron(neuronAllele)
{
	assert(neuronAllele.eNeuronType == NEURONTYPE_INPUT);
}

fenn::NeuronInput::~NeuronInput()
{
}

bool fenn::NeuronInput::Fire()
{
	//this should never happen, as this is an input neuron
	assert(false);
}

fenn::NeuronType fenn::NeuronInput::GetNeuronType() const
{
	return NEURONTYPE_INPUT;
}