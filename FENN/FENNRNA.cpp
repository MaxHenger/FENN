#include "FENNRNA.h"
#include "FENNRandom.h"

#include <cassert>
#include <map>

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
	nIndex				= allele.nIndex;
	pParentNeuron		= allele.pParentNeuron;
	pConnectedNeuron	= allele.pConnectedNeuron;
	nConnectedSlot		= allele.nConnectedSlot;
	fWeight				= allele.fWeight;
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
	sName			= allele.sName;
	nIndex			= allele.nIndex;
	eNeuronType		= allele.eNeuronType;
	fBias			= allele.fBias;
	veConnections	= allele.veConnections;
	veConnected		= allele.veConnected;
	setAllConnected = allele.setAllConnected;
}

fenn::RNA::RNA()
	: m_nMaxNumHiddenNeurons(0)
{
}

fenn::RNA::RNA(const RNA &rna)
{
	//call the assignment operator function
	*this = rna;
}

void fenn::RNA::operator = (const RNA &rna) 
{
	//delete any previously allocated data
	ReleaseAll();

	//copy simple variables
	m_nMaxNumHiddenNeurons = rna.m_nMaxNumHiddenNeurons;

	//reserve size for all neurons
	m_veInputNeurons.reserve(rna.m_veInputNeurons.size());
	m_veHiddenNeurons.reserve(rna.m_veHiddenNeurons.size());
	m_veOutputNeurons.reserve(rna.m_veOutputNeurons.size());

	//copy all neurons and create a map such that the connections can be recreated as well
	std::map<GLOBAL_INDEX_TYPE, NeuronAllele*> map;

	for (const auto pNeuron : rna.m_veInputNeurons) {
		NeuronAllele *pNew = CreateNeuronAllele(*pNeuron);
		map.insert(std::pair<GLOBAL_INDEX_TYPE, NeuronAllele*>(pNew->nIndex, pNew));
		m_veInputNeurons.push_back(pNew);
	}

	for (const auto pNeuron : rna.m_veHiddenNeurons) {
		NeuronAllele *pNew = CreateNeuronAllele(*pNeuron);
		map.insert(std::pair<GLOBAL_INDEX_TYPE, NeuronAllele*>(pNew->nIndex, pNew));
		m_veHiddenNeurons.push_back(pNew);
	}

	for (const auto pNeuron : rna.m_veOutputNeurons) {
		NeuronAllele *pNew = CreateNeuronAllele(*pNeuron);
		map.insert(std::pair<GLOBAL_INDEX_TYPE, NeuronAllele*>(pNew->nIndex, pNew));
		m_veOutputNeurons.push_back(pNew);
	}

	//now create the connections
	auto it = rna.m_veInputNeurons.cbegin();
	for (const auto pNeuron : m_veInputNeurons) {
		CreateNeuronAlleleConnections(pNeuron, *it, map);
		++it;
	}

	it = rna.m_veHiddenNeurons.cbegin();
	for (const auto pNeuron : m_veHiddenNeurons) {
		CreateNeuronAlleleConnections(pNeuron, *it, map);
		++it;
	}

	//now create the connection sets
	for (const auto pNeuron : m_veOutputNeurons) {
		CreateNeuronConnectionSets(pNeuron, NULL);
	}
}

fenn::RNA::~RNA()
{
	//delete all data contained in the vectors
	ReleaseAll();
}

unsigned int fenn::RNA::GetStorageSize() const
{
	//TODO: Write this such that it returns the size needed to store data in a bitstring
	//in bytes
	return 0;
}

void fenn::RNA::Create(const unsigned int nInput, const unsigned int nMaxHidden, const unsigned int nOutput)
{
	//resize the vectors such that the new neurons will fit
	ResizeNeuronVector(m_veInputNeurons, nInput);

	for (auto neuron : m_veHiddenNeurons) {
		ReleaseNeuronAllele(neuron);
	}
	m_veHiddenNeurons.clear();

	ResizeNeuronVector(m_veOutputNeurons, nOutput);

	//loop through the output neurons and set their values to default
	//values
	for (auto pNeuron : m_veOutputNeurons) {
		pNeuron->nIndex = world_single::Get().GetNewNeuronIndex();
		pNeuron->eNeuronType = NEURONTYPE_ADDITION;
		pNeuron->fBias = BIAS_DEFAULT;
	}

	//loop through the input neurons and set their values to default
	//values
	for (auto pNeuron : m_veInputNeurons) {
		pNeuron->nIndex = world_single::Get().GetNewNeuronIndex();
		pNeuron->eNeuronType = NEURONTYPE_ADDITION;
		pNeuron->fBias = BIAS_DEFAULT;

		//loop through all output neurons and connect this input neuron
		//to all output neurons in the following loop
		for (auto pOutput : m_veOutputNeurons) {
			ConnectionAllele newConnection;
			newConnection.nIndex			= world_single::Get().GetNewConnectionIndex();
			newConnection.pParentNeuron		= pNeuron;
			newConnection.pConnectedNeuron	= pOutput;
			newConnection.nConnectedSlot	= SLOT_DEFAULT;
			newConnection.fWeight			= WEIGHT_DEFAULT;

			pNeuron->veConnections.push_back(newConnection);
			pOutput->veConnected.push_back(pNeuron);
			pNeuron->setAllConnected.insert(pOutput);
		}
	}
}

void fenn::RNA::Create(RNA* const pParentChampion, RNA* const pParent)
{
	//assert that the parent neurons are of expectedly similar sizes
	assert(pParentChampion->m_veInputNeurons.size() == pParent->m_veInputNeurons.size());
	assert(pParentChampion->m_nMaxNumHiddenNeurons == pParent->m_nMaxNumHiddenNeurons);
	assert(pParentChampion->m_veOutputNeurons.size() == pParent->m_veOutputNeurons.size());

	//now ensure that the current neuron will be large enough to store all data.
	ResizeNeuronVector(m_veInputNeurons, pParentChampion->m_veInputNeurons.size());
	ResizeNeuronVector(m_veHiddenNeurons, pParentChampion->m_veHiddenNeurons.size());
	ResizeNeuronVector(m_veOutputNeurons, pParentChampion->m_veOutputNeurons.size());
	
	//create neurons following the RNA recombining process. Create a map at the 
	//same time to be able to relate the new neurons to the old ones. This map will
	//be used later, when interconnecting the neurons
	std::map<GLOBAL_INDEX_TYPE, NeuronAllele*> map;

	//create the input neurons
	auto itNeuronChampion = pParentChampion->m_veInputNeurons.cbegin();
	auto itNeuron = pParent->m_veInputNeurons.cbegin();

	for (auto pNeuron : m_veInputNeurons) {
		//assert the indices of the neurons are the same, this ensures the RNA class
		//is properly used according to the guidelines of the FENN framework
		assert((*itNeuronChampion)->nIndex == (*itNeuron)->nIndex);
		RecombineNeuronAllele(pNeuron, *itNeuronChampion, *itNeuron);

		//add the newly created neuron to the map
		map.insert(std::pair<GLOBAL_INDEX_TYPE, NeuronAllele*>(pNeuron->nIndex, pNeuron));

		//increment the parent iterators
		++itNeuronChampion;
		++itNeuron;
	}

	//create the hidden neurons
	itNeuronChampion = pParentChampion->m_veHiddenNeurons.cbegin();
	auto itNeuronOld = pParent->m_veHiddenNeurons.cbegin();
	
	for (auto pNeuron : m_veHiddenNeurons) {
		//find the neuron corresponding to the position the iterator of the champion
		//parent is pointing to in the other parent's alleles
		auto itNeuronNew = itNeuronOld;

		while (itNeuronNew < pParent->m_veHiddenNeurons.cend()) {
			if ((*itNeuronNew)->nIndex == pNeuron->nIndex) {
				//we found the matching neuron
				break;
			}
		}

		if (itNeuronNew != pParent->m_veHiddenNeurons.cend()) {
			//matching neuron was found, make a new neuron allele by recombining the 
			//RNA from both parents
			RecombineNeuronAllele(pNeuron, *itNeuronChampion, *itNeuronNew);

			//because we found a matching neuron, continue searching for subsequently
			//matching neurons from the current iteration point
			itNeuronOld = itNeuronNew;
			++itNeuronOld;
		} else {
			//no matching neuron found, simply copy the champion RNA's neuron allele
			const NeuronAllele* const pChampion = *itNeuronChampion;

			pNeuron->sName		= pChampion->sName;
			pNeuron->nIndex		= pChampion->nIndex;
			pNeuron->eNeuronType = pChampion->eNeuronType;
			pNeuron->fBias		= pChampion->fBias;
		}

		//whatever happened, the neuron has its proper description now,
		//add it to the map and process the next one
		map.insert(std::pair<GLOBAL_INDEX_TYPE, NeuronAllele*>(pNeuron->nIndex, pNeuron));
		++itNeuronChampion;
	}

	//create the output neurons
	itNeuronChampion = pParentChampion->m_veOutputNeurons.cbegin();
	itNeuron = pParent->m_veOutputNeurons.cbegin();

	for (auto pNeuron : m_veOutputNeurons) {
		//ensure the neuron indices match to assert that the programmer using the FENN
		//framework is using it in the correct fashion
		assert((*itNeuronChampion)->nIndex == (*itNeuron)->nIndex);

		//recombine the neuron alleles to a new output neuron allele
		RecombineNeuronAllele(pNeuron, *itNeuronChampion, *itNeuron);

		//add the new allele to the map
		map.insert(std::pair<GLOBAL_INDEX_TYPE, NeuronAllele*>(pNeuron->nIndex, pNeuron));

		//increment iterators
		++itNeuronChampion;
		++itNeuron;
	}

	//Now that the input, hidden and output neurons are created, create all the connections
	//between them using the created map
	itNeuronChampion = pParentChampion->m_veInputNeurons.cbegin();
	itNeuron = pParent->m_veInputNeurons.cbegin();

	for (auto pNeuron : m_veInputNeurons) {
		RecombineNeuronAlleleConnections(pNeuron, *itNeuronChampion, *itNeuron, map);

		++itNeuronChampion;
		++itNeuron;
	}

	itNeuronChampion = pParentChampion->m_veHiddenNeurons.cbegin();
	itNeuron = pParent->m_veHiddenNeurons.cbegin();

	for (auto pNeuron : m_veHiddenNeurons) {
		RecombineNeuronAlleleConnections(pNeuron, *itNeuronChampion, *itNeuron, map);

		++itNeuronChampion;
		++itNeuron;
	}
}

void fenn::RNA::Mutate(const RNAMutationRates &rates, std::vector<NeuronMutation> &prevNeuron, std::vector<ConnectionMutation> &prevConnection) {
	//start by mutating all the relatively simple things, mapping functions, biases and weights
	for (auto &pHidden : m_veHiddenNeurons) {
		MutateNeuronAlleleProperties(pHidden, rates);
	}

	for (auto &pOutput : m_veOutputNeurons) {
		MutateNeuronAlleleProperties(pOutput, rates);
	}

	//add connections, this is done on neurons which can be connected to something else. Hence
	//it should occur on input and hidden neurons, which can possibly connect to hidden neurons
	//and output neurons
	for (auto &pInput : m_veInputNeurons) {
		for (auto &pHidden : m_veHiddenNeurons) {
			//ensure the connection will not create a recursive value loop
			NeuronAllele::SET_CONNECTED_NEURONS::const_iterator it = pHidden->setAllConnected.find(pInput);

			if (it != pHidden->setAllConnected.cend() && GetRandomContinuous() < rates.fAddConnection) {
				MutateNeuronAlleleAddConnection(pInput, pHidden, prevConnection);
			}
		}

		for (auto &pOutput : m_veOutputNeurons) {
			//ensure no recursive loop will be created
			NeuronAllele::SET_CONNECTED_NEURONS::const_iterator it = pOutput->setAllConnected.find(pInput);

			if (it != pOutput->setAllConnected.cend() && GetRandomContinuous() < rates.fAddConnection) {
				MutateNeuronAlleleAddConnection(pInput, pOutput, prevConnection);
			}
		}
	}

	for (auto &pHiddenOut : m_veHiddenNeurons) {
		for (auto &pHiddenIn : m_veHiddenNeurons) {
			if (pHiddenOut != pHiddenIn) {
				//ensure no value loop will be created
				NeuronAllele::SET_CONNECTED_NEURONS::const_iterator it = pHiddenIn->setAllConnected.find(pHiddenOut);

				if (it != pHiddenIn->setAllConnected.cend() && GetRandomContinuous() < rates.fAddConnection) {
					MutateNeuronAlleleAddConnection(pHiddenOut, pHiddenIn, prevConnection);
				}
			}
		}

		for (auto &pOutput : m_veOutputNeurons) {
			//ensure the lack of a value loop
			NeuronAllele::SET_CONNECTED_NEURONS::const_iterator it = pOutput->setAllConnected.find(pHiddenOut);

			if (it != pOutput->setAllConnected.cend() && GetRandomContinuous() < rates.fAddConnection) {
				MutateNeuronAlleleAddConnection(pHiddenOut, pOutput, prevConnection);
			}
		}
	}

	//add neurons, this is done on a connection. Hence the type of mutation should
	//be applied both to the input neurons as the hidden neurons
	for (auto &pInput : m_veInputNeurons) {
		for (auto &connection : pInput->veConnections) {
			//check if this connection should be mutated
			if (GetRandomContinuous() < rates.fAddNeuron) {
				//a neuron should be added
				MutateConnectionAlleleAddNeuron(&connection, prevNeuron);
			}
		}
	}

	for (auto &pHidden : m_veHiddenNeurons) {
		for (auto &connection : pHidden->veConnections) {
			//check if this connection should be mutated
			if (GetRandomContinuous() < rates.fAddNeuron) {
				//a neuron should be added
				MutateConnectionAlleleAddNeuron(&connection, prevNeuron);
			}
		}
	}

	//now start removing neurons. As this type of mutation should only occur on neurons which
	//have both neurons connected to them as neurons to which they connect, this type of
	//mutation should only occur on hidden neurons
	for (auto pHidden : m_veHiddenNeurons) {
		if (GetRandomContinuous() < rates.fRemoveNeuron) {
			//a neuron should be removed
			MutateNeuronAlleleRemoveNeuron(pHidden, prevConnection);
		}
	}
}

bool fenn::RNA::ResizeNeuronVector(VECTOR_NEURON_ALLELE &vector, const unsigned int nNewSize) const
{
	//most often the neuron vectors will be of the same size, check this first
	if (vector.size() != nNewSize) {
		if (vector.size() < nNewSize) { 
			//current size is smaller than new size, make array larger
			vector.reserve(nNewSize);
			const unsigned int nNew = nNewSize - vector.size();

			for (unsigned int i = 0; i < nNew; i++) {
				vector.push_back(CreateNeuronAllele());
			}
		} else if (vector.size() > nNewSize) {
			//current size is larger than new size, make array smaller
			for (auto it = vector.begin() + nNewSize; it != vector.end(); ++it) {
				ReleaseNeuronAllele(*it);
			}

			vector.resize(nNewSize);
		}

		return true;
	}

	return false;
}

fenn::NeuronAllele* fenn::RNA::CreateNeuronAllele()
{
	return new NeuronAllele;
}

fenn::NeuronAllele* fenn::RNA::CreateNeuronAllele(const NeuronAllele &allele)
{
	return new NeuronAllele(allele);
}

void fenn::RNA::ReleaseNeuronAllele(NeuronAllele *pAllele)
{
	delete pAllele;
}

void fenn::RNA::ReleaseAll()
{
	//delete all dynamically allocated neurons
	for (auto pNeuron : m_veInputNeurons) {
		ReleaseNeuronAllele(pNeuron);
	}

	m_veInputNeurons.clear();

	for (auto pNeuron : m_veHiddenNeurons) {
		ReleaseNeuronAllele(pNeuron);
	}

	m_veHiddenNeurons.clear();

	for (auto pNeuron : m_veOutputNeurons) {
		ReleaseNeuronAllele(pNeuron);
	}

	m_veOutputNeurons.clear();
}

void fenn::RNA::RecombineNeuronAllele(NeuronAllele *pTarget, NeuronAllele *pParentChampion, NeuronAllele *pParent)
{
	//this function will recombine the neurons based on randomly generated numbers
	const NeuronAllele* const pToUse = GetRandomBinary() ? pParentChampion : pParent;
	pTarget->sName		= pToUse->sName;
	pTarget->nIndex		= pToUse->nIndex;
	pTarget->eNeuronType = pToUse->eNeuronType;
	pTarget->fBias		= pToUse->fBias;
}

void fenn::RNA::CreateNeuronAlleleConnections(NeuronAllele *pTarget, NeuronAllele *pSource, const std::map<GLOBAL_INDEX_TYPE, NeuronAllele*> &map)
{
	//reserve size for connections
	pTarget->veConnections.reserve(pSource->veConnections.size());

	for (const auto &source : pSource->veConnections) {
		//copy all data from the source neuron
		ConnectionAllele newConnection;
		assert(map.find(source.pParentNeuron->nIndex)->second == pTarget);

		newConnection.nIndex = source.nIndex;
		newConnection.pParentNeuron = pTarget;
		newConnection.pConnectedNeuron = map.find(source.pConnectedNeuron->nIndex)->second;
		newConnection.nConnectedSlot = source.nConnectedSlot;
		newConnection.fWeight = source.fWeight;

		pTarget->veConnections.push_back(newConnection);
		newConnection.pConnectedNeuron->veConnected.push_back(pTarget);
	}
}

void fenn::RNA::RecombineNeuronAlleleConnections(NeuronAllele *pTarget, NeuronAllele *pParentChampion, NeuronAllele *pParent, const std::map<GLOBAL_INDEX_TYPE, NeuronAllele*> &map)
{
	//this function will reconstruct the connections for the provided neuron from the parent
	//neurons. The map should contain a map of all neurons in the neural network
	assert(pTarget->veConnections.empty());

	auto pNeuronOld = pParent->veConnections.cbegin();

	for (const auto &pChampion : pParentChampion->veConnections) {
		//look for a connection with an index matching to the champion parent connection's index
		auto pNeuronNew = pNeuronOld;

		while (pNeuronNew != pParent->veConnections.cend() && pNeuronNew->nIndex != pChampion.nIndex) {
			pNeuronNew++;
		}

		//check if a matching neuron was found
		if (pNeuronNew != pParent->veConnections.cend()) {
			//a matching connection was found, the new connection should be a recombination of
			//the two parent neuron's connections.
			ConnectionAllele newConnection;
			newConnection.nIndex = pChampion.nIndex;
			
			//find the parent and connected neuron's new pointer value using the map
			assert(map.find(pChampion.pParentNeuron->nIndex)->second == pTarget);
			newConnection.pParentNeuron = pTarget;
			newConnection.pConnectedNeuron = map.find(pChampion.pConnectedNeuron->nIndex)->second;

			//choose slot and weight on the basis of random numbers
			const ConnectionAllele* const pToUse = GetRandomBinary() ? &pChampion : &(*pNeuronNew);

			newConnection.nConnectedSlot = pToUse->nConnectedSlot;
			newConnection.fWeight = pToUse->fWeight;

			pTarget->veConnections.push_back(newConnection);
			newConnection.pConnectedNeuron->veConnected.push_back(pTarget);

			//update the iterator finding the connections in the non-championing parent
			pNeuronOld = pNeuronNew;
			++pNeuronOld;
		} else {
			//no matching neuron was found, only use the data coming from the neuron
			ConnectionAllele newConnection;
			assert(map.find(pChampion.pParentNeuron->nIndex)->second == pTarget);

			newConnection.nIndex			= pChampion.nIndex;
			newConnection.pParentNeuron		= pTarget;
			newConnection.pConnectedNeuron	= map.find(pChampion.pConnectedNeuron->nIndex)->second;
			newConnection.nConnectedSlot	= pChampion.nConnectedSlot;
			newConnection.fWeight			= pChampion.fWeight;

			pTarget->veConnections.push_back(newConnection);
			newConnection.pConnectedNeuron->veConnected.push_back(pTarget);

			//as we've found no matching neuron, the iterator finding the matching connections
			//in the non-championing parent should not be changed
		}
	}
}

void fenn::RNA::CreateNeuronConnectionSets(NeuronAllele *pNew, const NeuronAllele * const pPrev)
{
	//This function is used to construct the neuron connection sets from scratch. As such, this
	//function should only be called explicitly on the output neurons with pPrev set to NULL.
	//The other neurons will have their connection sets created by the recursive nature of this
	//function. This function will clear any previously existing elements of the connection set.
	pNew->setAllConnected.clear();

	//copy the previous neuron's connection set to the current one and add the previous neuron's
	//as well
	if (pPrev) {
		pNew->setAllConnected = pPrev->setAllConnected;
		pNew->setAllConnected.insert((NeuronAllele*)pPrev);
	}

	//call the same function recursively on all neurons connected to this one
	for (auto pConnected : pNew->veConnected) {
		CreateNeuronConnectionSets(pConnected, pNew);
	}
}

void fenn::RNA::AddToNeuronConnectionSets(NeuronAllele *pToProcess, const NeuronAllele* const pNew)
{
	//add the new neuron allele's pointer to the connection set of the neuron allele being
	//processed
	pToProcess->setAllConnected.insert((NeuronAllele*)pNew);

	//call the function recursively on all neuron allele's feeding into the currently processed
	//one
	for (auto &pConnected : pToProcess->veConnected) {
		AddToNeuronConnectionSets(pConnected, pNew);
	}
}

void fenn::RNA::AddToNeuronConnectionSets(NeuronAllele *pToProcess, const NeuronAllele::SET_CONNECTED_NEURONS &toAdd)
{
	//add the entire passed set to the connection set of the neuron allele being processed
	for (auto &neuron : toAdd) {
		pToProcess->setAllConnected.insert(neuron);
	}

	//call the function recursively on all neuron allele's feeding into the neuron allele
	//currently being processed
	for (auto &pConnected : pToProcess->veConnected) {
		AddToNeuronConnectionSets(pConnected, toAdd);
	}
}

void fenn::RNA::RemoveFromNeuronConnectionSets(NeuronAllele *pToProcess, const NeuronAllele* const pToRemove)
{
	//remove the old neuron allele's point from the currently processed neuron allele connection set
	pToProcess->setAllConnected.erase((NeuronAllele*)pToRemove);

	//do the same recursively for all neuron alleles connected to the currently processed one
	for (auto pConnected : pToProcess->veConnected) {
		RemoveFromNeuronConnectionSets(pConnected, pToRemove);
	}
}

void fenn::RNA::MutateNeuronAlleleProperties(NeuronAllele* const pToMutate, const RNAMutationRates &rates)
{
	//mutate all of the neuron allele properties
	if (GetRandomContinuous() < rates.fMappingFunction) {
		pToMutate->eNeuronType = world_single::Get().GetRandomNeuronType();
	}

	if (GetRandomContinuous() < rates.fBiasRandom) {
		pToMutate->fBias = world_single::Get().GetRandomBias();
	}

	if (GetRandomContinuous() < rates.fBiasAdditive) {
		pToMutate->fBias += world_single::Get().GetRandomBiasAdditive();
	}

	//mutate all of the connection allele properties
	for (auto &connection : pToMutate->veConnections) {
		if (GetRandomContinuous() < rates.fNeuronSlot) {
			connection.nConnectedSlot = world_single::Get().GetRandomSlot();
		}

		if (GetRandomContinuous() < rates.fWeightRandom) {
			connection.fWeight = world_single::Get().GetRandomWeight();
		} 

		if (GetRandomContinuous() < rates.fWeightAdditive) {
			connection.fWeight += world_single::Get().GetRandomWeightAdditive();
		}
	}
}

void fenn::RNA::MutateConnectionAlleleAddNeuron(ConnectionAllele* const pToMutate, std::vector<NeuronMutation> &prevMutations)
{
	//This function will mutate a connection allele such that a neuron will be created
	//between the original source neuron and the original target neuron allele
	NeuronAllele *pNewNeuron = CreateNeuronAllele();
	ConnectionAllele newConnectionTowards;
	ConnectionAllele newConnectionFrom;

	//check if the mutation already ocurred before
	bool bMutationOccurred = false;

	for (const auto &previous : prevMutations) {
		if (previous.nNeuronOrigin == pToMutate->pParentNeuron->nIndex &&
			previous.nNeuronTarget == pToMutate->pConnectedNeuron->nIndex) {
			//mutation already ocurred before
			pNewNeuron->nIndex			= previous.nNewNeuronIndex;
			newConnectionTowards.nIndex = previous.nNewConnectionTowards;
			newConnectionFrom.nIndex	= previous.nNewConnectionFrom;

			bMutationOccurred = true;
			break;
		}
	}

	if (!bMutationOccurred) {
		//mutation did not occur: get new neuron index
		pNewNeuron->nIndex = world_single::Get().GetNewNeuronIndex();
		newConnectionTowards.nIndex = world_single::Get().GetNewConnectionIndex();
		newConnectionFrom.nIndex = world_single::Get().GetNewConnectionIndex();

		//and store the new mutation in the vector for mutations occurring later
		NeuronMutation &&newMutation{pToMutate->pParentNeuron->nIndex,
									 pToMutate->pConnectedNeuron->nIndex,
									 pNewNeuron->nIndex,
									 newConnectionTowards.nIndex,
									 newConnectionFrom.nIndex};
		prevMutations.push_back(newMutation);
	}

	//initialize the new connection going towards the newly created neuron
	newConnectionTowards.pParentNeuron = pToMutate->pParentNeuron;
	newConnectionTowards.pConnectedNeuron = pNewNeuron;
	newConnectionTowards.nConnectedSlot = world_single::Get().GetRandomSlot();
	newConnectionTowards.fWeight = world_single::Get().GetRandomWeight();

	//initialize the new connection coming from the newly created neuron
	newConnectionFrom.pParentNeuron = pNewNeuron;
	newConnectionFrom.pConnectedNeuron = pToMutate->pConnectedNeuron;
	newConnectionFrom.nConnectedSlot = world_single::Get().GetRandomSlot();
	newConnectionFrom.fWeight = world_single::Get().GetRandomWeight();

	//set remaining variables of the newly created neuron
	pNewNeuron->eNeuronType = world_single::Get().GetRandomNeuronType();
	pNewNeuron->fBias = world_single::Get().GetRandomBias();
	pNewNeuron->veConnections.push_back(newConnectionFrom);
	pNewNeuron->veConnected.push_back(pToMutate->pParentNeuron);
	pNewNeuron->setAllConnected = pToMutate->pConnectedNeuron->setAllConnected;
	pNewNeuron->setAllConnected.insert(pToMutate->pConnectedNeuron);

	//update the neuron to which the new neuron is connected:
	// - remove original incoming neuron from vector
	// - add newly created neuron to vector
	bool bFound = false;

	for (auto& pConnected : pToMutate->pConnectedNeuron->veConnected) {
		if (pConnected == pToMutate->pParentNeuron) {
			//found the original connected neuron, set it to the new one
			pConnected = pNewNeuron;
			bFound = true;
			break;
		}
	}

	assert(bFound);

	//update the neuron which is going to be connected to the new neuron
	// - removing original connection to next neuron
	// - add connection to newly created neuron
	// - update the connection set to add the newly added neuron
	AddToNeuronConnectionSets(pToMutate->pParentNeuron, pNewNeuron);

	//TODO: Instead of the following, check if *pToMutate = newConnectionTowards works, it should!
	//for now, work with an assertion
	bFound = false;

	for (auto& pConnection : pToMutate->pParentNeuron->veConnections) {
		if (pConnection.nIndex == pToMutate->nIndex) {
			//replace the connection with the new connection
			assert(&pConnection == pToMutate);
			pConnection = newConnectionTowards;
			bFound = true;
			break;
		}
	}

	assert(bFound);
	//after the foregoing loop, pToMutate is no long valid (it points to newConnectionTowards)

	m_veHiddenNeurons.push_back(pNewNeuron);
}

void fenn::RNA::MutateNeuronAlleleRemoveNeuron(NeuronAllele* const pToMutate, std::vector<ConnectionMutation> &prevMutations) 
{
	//start by looping through all neurons which are currently connected to the neuron which is
	//about to be deleted
	for (auto& pNeuron : pToMutate->veConnected) {
		//for the current neuron, connect to all neurons to which the neuron-to-be-deleted is connected
		for (auto& targetConnection : pToMutate->veConnections) {
			//but only if it isn't already connected to it
			bool bAlreadyConnected = false;

			for (auto& ownConnection : pNeuron->veConnections) {
				if (targetConnection.pConnectedNeuron == ownConnection.pConnectedNeuron) {
					//it is already connected
					bAlreadyConnected = true;
					break;
				}
			}

			if (!bAlreadyConnected) {
				//not already connected, check if the mutation doesn't already exist
				bool bAlreadyMutated = false;
				ConnectionAllele newConnection;

				for (auto &mutation : prevMutations) {
					if (mutation.nNeuronOrigin == pNeuron->nIndex && mutation.nNeuronTarget == targetConnection.pConnectedNeuron->nIndex) {
						//mutation already exists
						newConnection.nIndex = mutation.nNewConnectionIndex;
						bAlreadyMutated = true;
						break;
					}
				}

				if (!bAlreadyMutated) {
					//mutation doesn't already exist, retrieve new connection index
					newConnection.nIndex = world_single::Get().GetNewConnectionIndex();
					
					//create a new mutation type such that it can be added to the list of mutations
					ConnectionMutation &&newMutation{pNeuron->nIndex, 
													 targetConnection.pConnectedNeuron->nIndex,
													 newConnection.nIndex};
					prevMutations.push_back(newMutation);
				}

				//set the remaining connection variables
				newConnection.pParentNeuron = pNeuron;
				newConnection.pConnectedNeuron = targetConnection.pConnectedNeuron;
				newConnection.nConnectedSlot = world_single::Get().GetRandomSlot();
				newConnection.fWeight = world_single::Get().GetRandomWeight();

				//add the new connection
				pNeuron->veConnections.push_back(newConnection);

				//assert that the new connection's connected neuron is already in the connection set
#ifdef _DEBUG
				bool bFound = false;

				for (auto &pSetElement : pNeuron->setAllConnected) {
					if (pSetElement == targetConnection.pConnectedNeuron) {
						bFound = true;
						break;
					}
				}

				assert(bFound);
#endif
			}
		}

		//for the current neuron, connected to the to-be-deleted neuron, remove the original
		//connection to it
		bool bFound = false;

		for (NeuronAllele::VECTOR_CONNECTION_ALLELE::const_iterator it = pNeuron->veConnections.cbegin(); it != pNeuron->veConnections.cend(); it++) {
			if (it->pConnectedNeuron == pToMutate) {
				//remove the current connection
				pNeuron->veConnections.erase(it);
				bFound = true;
				break;
			}
		}

		assert(bFound);

		//remove the to-be-deleted neuron from the connection set
		RemoveFromNeuronConnectionSets(pNeuron, pToMutate);
	}

	//now loop through all the neurons to which the to-be-removed neuron connects
	for (auto &connection : pToMutate->veConnections) {
		//remove the to-be-removed neuron from the connected vector
		NeuronAllele* const pCurNeuron = connection.pConnectedNeuron;
		bool bFound = false;

		for (NeuronAllele::VECTOR_CONNECTED_NEURONS::const_iterator it = pCurNeuron->veConnected.cbegin(); it != pCurNeuron->veConnected.cend(); it++) {
			if ((*it) == pToMutate) {
				//found the connected neuron to remove
				pCurNeuron->veConnected.erase(it);
				bFound = true;
				break;
			}
		}

		assert(bFound);
	}
	
	//finally, remove the to-be-removed neuron from the hidden neuron vector and finally delete the
	//dynamically allocated neuron
	for (VECTOR_NEURON_ALLELE::const_iterator it = m_veHiddenNeurons.cbegin(); it != m_veHiddenNeurons.cend(); it++) {
		if ((*it) == pToMutate) {
			//found the neuron
			m_veHiddenNeurons.erase(it);
			ReleaseNeuronAllele(pToMutate);
			return;
		}
	}

	//if this code is reached, the neuron allele is not found in the hidden neuron vector
	assert(false);
}

void fenn::RNA::MutateNeuronAlleleAddConnection(NeuronAllele* pSource, NeuronAllele* const pTarget, std::vector<ConnectionMutation> &prevMutations)
{
	//check if the mutation has already occurred before
	bool bMutationOccurred = false;
	ConnectionAllele newConnection;

	for (auto &mutation : prevMutations) {
		if (mutation.nNeuronOrigin == pSource->nIndex && mutation.nNeuronTarget == pTarget->nIndex) {
			//mutation already occurred before
			newConnection.nIndex = mutation.nNewConnectionIndex;
			bMutationOccurred = true;
			break;
		}
	}

	if (!bMutationOccurred) {
		//mutation did not occur, get new index
		newConnection.nIndex = world_single::Get().GetNewConnectionIndex();

		//add mutation to the vector of occurred mutations
		ConnectionMutation &&newMutation{pSource->nIndex,
										 pTarget->nIndex,
										 newConnection.nIndex};
		prevMutations.push_back(newMutation);
	}

	//set the remaining connection properties
	newConnection.pParentNeuron = pSource;
	newConnection.pConnectedNeuron = pTarget;
	newConnection.nConnectedSlot = world_single::Get().GetRandomSlot();
	newConnection.fWeight = world_single::Get().GetRandomWeight();

	//add the source neuron to the target neurons connected vector
	pTarget->veConnected.push_back(pSource);

	//add the new connection to the source neuron and update the connection set
	pSource->veConnections.push_back(newConnection);
	AddToNeuronConnectionSets(pSource, pTarget->setAllConnected);
}