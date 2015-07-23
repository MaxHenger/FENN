#include "Neuron.h"
#include <cassert>

//include various neuron implementations
#include "NeuronInput.h"
#include "NeuronAddition.h"
#include "NeuronSubtraction.h"
#include "NeuronMultiplication.h"
#include "NeuronDivision.h"

fenn::Neuron::Neuron(const NeuronAllele &neuronAllele)
	: m_nIndex(neuronAllele.nIndex)
	, m_fValue(0)
	, m_fBias(neuronAllele.fBias)
{
	//allready resize the inputs
	m_veInputs.resize(neuronAllele.veConnected.size());
}

void fenn::Neuron::SetInput(Neuron *pNeuron, LOCAL_INDEX_TYPE nIndex, const ConnectionAllele &connectionAllele)
{
	//assert that the index does not exceed the given limit
	assert(nIndex < m_veInputs.size());
	assert(pNeuron->m_nIndex == connectionAllele.pParentNeuron->nIndex);
	assert(m_nIndex == connectionAllele.pConnectedNeuron->nIndex);

	//set the input data
	InputEntry &entry = m_veInputs[nIndex];
	entry.pNeuron	= pNeuron;
	entry.nSlot		= connectionAllele.nConnectedSlot;
	entry.fWeight	= connectionAllele.fWeight;
}

void fenn::Neuron::SetValue(VALUE_TYPE value)
{
	//set the provided value
	m_fValue = value;
}

void fenn::Neuron::GetValue(VALUE_TYPE &result)
{
	//return the requested value
	result = m_fValue;
}

fenn::Neuron* fenn::CreateNeuron(const NeuronAllele &allele)
{
	switch (allele.eNeuronType) {
		case NEURONTYPE_INPUT:
			return new NeuronInput(allele);
		case NEURONTYPE_ADDITION:
			return new NeuronAddition(allele);
		case NEURONTYPE_SUBTRACTION:
			return new NeuronSubtraction(allele);
		case NEURONTYPE_MULTIPLICATION:
			return new NeuronMultiplication(allele);
		case NEURONTYPE_DIVISION:
			return new NeuronDivision(allele);
	}

	//not any of the known neurons
	return NULL;
}