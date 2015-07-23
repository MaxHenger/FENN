#include "RNA.h"

fenn::ConnectionAllele::ConnectionAllele()
	: nIndex(0)
	, pParentNeuron(NULL)
	, pConnectedNeuron(NULL)
	, nConnectedSlot(0)
	, fWeight(0)
{
}

fenn::ConnectionAllele::ConnectionAllele(const ConnectionAllele &allele)
	: nIndex(allele.nIndex)
	, pParentNeuron(allele.pParentNeuron)
	, pConnectedNeuron(allele.pConnectedNeuron)
	, nConnectedSlot(allele.nConnectedSlot)
	, fWeight(allele.fWeight)
{
}

void fenn::ConnectionAllele::operator = (const ConnectionAllele &allele)
{
	//simply copy all information from the passed allele argument
	nIndex = allele.nIndex;
	pParentNeuron = allele.pParentNeuron;
	pConnectedNeuron = allele.pConnectedNeuron;
	nConnectedSlot = allele.nConnectedSlot;
	fWeight = allele.fWeight;
}

fenn::ConnectionAllele::~ConnectionAllele()
{
	//nothing to do
}

fenn::NeuronAllele::NeuronAllele()
	: nIndex(0)
	, eNeuronType(NEURONTYPE_UNKNOWN)
	, fBias(0)
{
}

fenn::NeuronAllele::NeuronAllele(const NeuronAllele &allele)
	: sName(allele.sName)
	, nIndex(allele.nIndex)
	, eNeuronType(allele.eNeuronType)
	, fBias(allele.fBias)
	, veConnections(allele.veConnections)
	, veConnected(allele.veConnected)
	, setAllConnected(allele.setAllConnected)
{
}

void fenn::NeuronAllele::operator = (const NeuronAllele &allele)
{
	sName = allele.sName;
	nIndex = allele.nIndex;
	eNeuronType = allele.eNeuronType;
	fBias = allele.fBias;
	veConnections = allele.veConnections;
	veConnected = allele.veConnected;
	setAllConnected = allele.setAllConnected;
}