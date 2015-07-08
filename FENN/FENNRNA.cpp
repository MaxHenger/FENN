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
	, setConnected(allele.setConnected)
{
}

void fenn::NeuronAllele::operator = (const NeuronAllele &allele)
{
	sName			= allele.sName;
	nIndex			= allele.nIndex;
	eNeuronType		= allele.eNeuronType;
	fBias			= allele.fBias;
	veConnections	= allele.veConnections;
	setConnected	= allele.setConnected;
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
		pNeuron->nIndex = g_world.Get().GetNewNeuronIndex();
		pNeuron->eNeuronType = NEURONTYPE_ADDITION;
		pNeuron->fBias = BIAS_DEFAULT;
	}

	//loop through the input neurons and set their values to default
	//values
	for (auto pNeuron : m_veInputNeurons) {
		pNeuron->nIndex = g_world.Get().GetNewNeuronIndex();
		pNeuron->eNeuronType = NEURONTYPE_ADDITION;
		pNeuron->fBias = BIAS_DEFAULT;

		//loop through all output neurons and connect this input neuron
		//to all output neurons in the following loop
		for (auto pOutput : m_veOutputNeurons) {
			ConnectionAllele newConnection;
			newConnection.nIndex			= g_world.Get().GetNewConnectionIndex();
			newConnection.pParentNeuron		= pNeuron;
			newConnection.pConnectedNeuron	= pOutput;
			newConnection.nConnectedSlot	= SLOT_DEFAULT;
			newConnection.fWeight			= WEIGHT_DEFAULT;

			pNeuron->veConnections.push_back(newConnection);
			pNeuron->setConnected.insert(pNeuron);
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
	//copy the previous neuron's connection set to the current one and add the previous neuron's
	//as well
	pNew->setAllConnected = pPrev->setAllConnected;
	pNew->setAllConnected.insert((NeuronAllele*)pPrev);

	//call the same function recursively on all neurons connected to this one
	for (auto pConnected : pNew->veConnected) {
		CreateNeuronConnectionSets(pConnected, pNew);
	}
}