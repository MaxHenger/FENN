#include "NEATNetwork.h"

fenn::neat::Network::Network()
{
}

fenn::neat::Network::~Network()
{
	ReleaseAll();
}

bool fenn::neat::Network::Initialize(const RNA &rna)
{
	//release all previously allocated neurons
	ReleaseAll();

	//prepare a map to map all neurons on
	std::map<GLOBAL_INDEX_TYPE, Neuron*> neuronMap;

	//call the recursive creation function on the input neurons
	for (unsigned int i = 0; i < rna.GetNumInputNeurons(); i++) {
		NeuronAllele* const pCurAllele = rna.GetInputNeuron(i);

		if (neuronMap.find(pCurAllele->nIndex) == neuronMap.cend()) {
			ConstructNeurons(rna.GetInputNeuron(i), neuronMap);
		}
	}

	//some assertions to check if the construction was succesfull
	assert(m_veInputNeurons.size() == rna.GetNumInputNeurons());
	assert(m_veHiddenNeurons.size() == rna.GetNumHiddenNeurons());
	assert(m_veOutputNeurons.size() == rna.GetNumOutputNeurons());

	return true;
}

unsigned int fenn::neat::Network::GetNumInputNeurons() const
{
	return m_veInputNeurons.size();
}

unsigned int fenn::neat::Network::GetNumHiddenNeurons() const
{
	return m_veHiddenNeurons.size();
}

unsigned int fenn::neat::Network::GetNumOutputNeurons() const
{
	return m_veOutputNeurons.size();
}

void fenn::neat::Network::ConstructNeurons(NeuronAllele *pToProcess, std::map<GLOBAL_INDEX_TYPE, Neuron*> &map)
{
	//This function will recursively create all neurons on the basis of a single starting
	//Neuron allele. It might be necessary to call this function on the input neurons of
	//the neural network that is going to be created.
	//An important note: Only call this function on neurons that are not yet created 
	//(this will be asserted)

	//for the current neuron, assert if it hasn't already been added to the map
	assert(map.find(pToProcess->nIndex) == map.cend());
	
	//this neuron has not been added yet, loop through all neurons connecting to this 
	//neuron, create them and add them as inputs to this neuron.
	Neuron *pNewNeuron = CreateNeuron(*pToProcess);

	unsigned int nInputCounter = 0;
	for (auto &connection : pToProcess->veConnected) {
		auto itInput = map.find(connection.second->nIndex);

		if (itInput == map.cend()) {
			//the neuron feeding into this one is not connected yet, create it
			ConstructNeurons(connection.second, map);
				
			//after the function finishes assert that it can be found
			itInput = map.find(connection.second->nIndex);
			assert(itInput != map.cend());
		}

		//set the input of the neuron
		pNewNeuron->SetInput(itInput->second, 
								nInputCounter,
								connection.second->veConnections[connection.first]);

		nInputCounter++;
	}

	//at this point all the neurons feeding into the new neuron have been
	//created. Add it to one of the storage vectors depending on the neuron 
	//type (this is a bit hacky, I know)
	if (pNewNeuron->GetNeuronType() == NEURONTYPE_INPUT) {
		m_veInputNeurons.push_back((NeuronInput*)pNewNeuron);
	} else if (pToProcess->veConnections.size() == 0) {
		//almost sure this is an output neuron (god, that sounds horrible
		//for a programmer)
		m_veOutputNeurons.push_back(pNewNeuron);
	} else {
		m_veHiddenNeurons.push_back(pNewNeuron);
	}

	//this is a recursive function, so call the same function on all outputs.
	for (auto &connection : pToProcess->veConnections) {
		if (map.find(connection.pConnectedNeuron->nIndex) == map.cend()) {
			ConstructNeurons(connection.pConnectedNeuron, map);
		}
	}
}

void fenn::neat::Network::ReleaseAll() 
{
	for (auto &pNeuron : m_veInputNeurons) {
		assert(pNeuron);
		ReleaseNeuron(pNeuron);
	}

	for (auto &pNeuron : m_veHiddenNeurons) {
		assert(pNeuron);
		ReleaseNeuron(pNeuron);
	}

	for (auto &pNeuron : m_veOutputNeurons) {
		assert(pNeuron);
		ReleaseNeuron(pNeuron);
	}
}