#ifndef FENN_RNA_H
#define FENN_RNA_H

#include "FENNConfig.h"
#include "FENNNeuronBase.h"
#include "FENNWorld.h"

#include <string>
#include <vector>
#include <list>
#include <set>

namespace fenn
{
	//forward definitions
	struct NeuronAllele;

	//----------------------------------------------------------------
	// Structure - ConnectionAllele
	//----------------------------------------------------------------
	// This structure represents a neuron connection and is used for
	// the creation of a brain. Specified is the connection index, the
	// indices of the neuron this connection belongs to and the neuron
	// to which it is connected along with the connection slot, and
	// the connection's weight. This structure is kept publically
	// accessible to allow the programmer to inspect the contents of
	// a RNA class. This structure should not be used directly by the
	// programmer to create a new brain.
	//----------------------------------------------------------------
	struct ConnectionAllele
	{
		//(copy) constructors, assignment and destructor
		// Note: The assignment operator and copy constructor are not 
		// strictly necessary, but included such that when the type-
		// definitions change to classes/structures using dynamic
		// allocations, the structure definition can easily be changed
		// accordingly.
		ConnectionAllele();
		ConnectionAllele(const ConnectionAllele &allele);
		void operator = (const ConnectionAllele &allele);
		~ConnectionAllele();

		//structure members
		GLOBAL_INDEX_TYPE		nIndex;
		NeuronAllele*			pParentNeuron;
		NeuronAllele*			pConnectedNeuron;
		SLOT_INDEX_TYPE			nConnectedSlot;
		WEIGHT_TYPE				fWeight;
	};

	//----------------------------------------------------------------
	// Structure - NeuronAllele
	//----------------------------------------------------------------
	// This structure represents a neuron configuration and is used to
	// create a new brain. It specifies a name (only used for the
	// retrieval of the final brain's mathematical functions), a 
	// global index unique identifying the neuron, the mapping 
	// function to use once the neuron is created, the bias associated
	// with the neuron and a list of connection alleles.
	//----------------------------------------------------------------
	struct NeuronAllele
	{
		//typedefinitions
		typedef std::vector<ConnectionAllele> VECTOR_CONNECTION_ALLELE;
		typedef std::vector<NeuronAllele*> VECTOR_CONNECTED_NEURONS;
		typedef std::set<NeuronAllele*> SET_CONNECTED_NEURONS;

		//(copy) constructors, assigment and destructor
		NeuronAllele();
		NeuronAllele(const NeuronAllele &allele);
		void operator = (const NeuronAllele &allele);
		~NeuronAllele();

		//structure members
		std::string				sName;
		GLOBAL_INDEX_TYPE		nIndex;
		NeuronType				eNeuronType;
		BIAS_TYPE				fBias;
		VECTOR_CONNECTION_ALLELE veConnections; //the connection alleles pointing to other neurons
		VECTOR_CONNECTED_NEURONS veConnected; //all neurons which are connected to this neuron
		SET_CONNECTED_NEURONS	setAllConnected; //the indices of neurons that this neuron somehow connects to (indirectly as well)
	};

	//----------------------------------------------------------------
	// Structure - RNAMutationRates
	//----------------------------------------------------------------
	// This structures contains all mutation rates.
	//----------------------------------------------------------------
	struct RNAMutationRates {
		CHANCE_TYPE	fMappingFunction;
		CHANCE_TYPE fAddNeuron;
		CHANCE_TYPE fRemoveNeuron;
		CHANCE_TYPE fNeuronSlot;
		CHANCE_TYPE fBiasRandom;
		CHANCE_TYPE fBiasAdditive;
		CHANCE_TYPE fAddConnection;
		CHANCE_TYPE fWeightRandom;
		CHANCE_TYPE fWeightAdditive;
	};

	//----------------------------------------------------------------
	// Structure - NeuronMutation
	//----------------------------------------------------------------
	// When mutating multiple RNA strings within the same generation
	// the possiblity exists that the exact same mutation occurs
	// twice. In this case the newly created neuron should receive the
	// same global index as the previously generated neuron. This 
	// structure helps in tracking those neuron mutations
	//----------------------------------------------------------------
	struct NeuronMutation
	{
		GLOBAL_INDEX_TYPE		nNeuronOrigin;		//index of the neuron connected to the new neuron
		GLOBAL_INDEX_TYPE		nNeuronTarget;		//index of the neuron that this neuron connects to
		GLOBAL_INDEX_TYPE		nNewNeuronIndex;	//index of the newly created neuron
		GLOBAL_INDEX_TYPE		nNewConnectionTowards; //index of the new connection toward the newly created neuron
		GLOBAL_INDEX_TYPE		nNewConnectionFrom;	//index of the new connection from the newly created neuron
	};

	//----------------------------------------------------------------
	// Structure - ConnectionMutation
	//----------------------------------------------------------------
	// When mutation multiple RNA strings within the same generation
	// the possiblity exists that the exact same connection mutation
	// occurs twice. In this case the second occurrence should not 
	// request a new global connection index but reuse the old one. 
	// This structure helps keeping track of those mutations.
	//----------------------------------------------------------------
	struct ConnectionMutation
	{
		GLOBAL_INDEX_TYPE		nNeuronOrigin;
		GLOBAL_INDEX_TYPE		nNeuronTarget;
		GLOBAL_INDEX_TYPE		nNewConnectionIndex;
	};

	//----------------------------------------------------------------
	// Class - RNA
	//----------------------------------------------------------------
	// The manner in which neurons constitute the brain is described
	// by the RNA class: A graph-based instruction set analogous to
	// RNA in biological cells. The idea for this type of RNA is
	// largely stolen from the NEAT method.
	// This class makes a distinction between input, hidden and output
	// neurons. Creating a string of RNA can be done by calling the
	// Create(...) function with either:
	//		1. The requested brain size. In which case the resulting
	//			brain will simply connect all its input neurons to all
	//			its output neurons, without hidden neurons.
	//		2. Two parents. In which case the two parents will be
	//			recombined to form a new RNA string.
	// Once created, the RNA string can be forced to mutate by calling
	// any of the MutateXXX(...) functions.
	//----------------------------------------------------------------
	class RNA
	{
	public:
		//(copy) constructors, assignment and destructor
		RNA();
		RNA(const RNA &rna);
		void operator = (const RNA &rna);
		~RNA();

		//saving and loading to/from a bitstring
		unsigned int GetStorageSize() const;
		bool Save(unsigned char *pData, unsigned int nMax) const;
		bool Load(unsigned char *pData, const unsigned int nSize);

		//creation of RNA
		void Create(const unsigned int nInput, const unsigned int nMaxHidden, const unsigned int nOutput);
		void Create(RNA* const pParentChampion, RNA* const pParent);

		//mutation of RNA
		// - mutating neurons
		void Mutate(const RNAMutationRates &rates, std::vector<NeuronMutation> &prevNeuron, std::vector<ConnectionMutation> &prevConnection);

	protected:
		//keeping track of the maximum number of neurons
		unsigned int				m_nMaxNumHiddenNeurons;

		//the neuron alleles
		typedef std::vector<NeuronAllele*> VECTOR_NEURON_ALLELE;
		VECTOR_NEURON_ALLELE		m_veInputNeurons;
		VECTOR_NEURON_ALLELE		m_veHiddenNeurons;
		VECTOR_NEURON_ALLELE		m_veOutputNeurons;

	private:
		//resizing arrays efficiently
		bool ResizeNeuronVector(VECTOR_NEURON_ALLELE &vector, const unsigned int nNewSize) const;

		//dynamically allocating elements and releasing elements
		static NeuronAllele* CreateNeuronAllele();
		static NeuronAllele* CreateNeuronAllele(const NeuronAllele &allele);
		static void ReleaseNeuronAllele(NeuronAllele *pAllele);

		void ReleaseAll();

		//reducing code redundancy
		// - recombining neuron allele properties from parents
		static void RecombineNeuronAllele(NeuronAllele *pTarget, NeuronAllele *pParentChampion, NeuronAllele *pParent);
		static void RecombineNeuronAlleleConnections(NeuronAllele *pTarget, NeuronAllele *pParentChampion, NeuronAllele *pParent, const std::map<GLOBAL_INDEX_TYPE, NeuronAllele*> &map);
		
		// - create neuron allele properties from a source RNA neuron allele
		static void CreateNeuronAlleleConnections(NeuronAllele *pTarget, NeuronAllele * pSource, const std::map<GLOBAL_INDEX_TYPE, NeuronAllele*> &map);
		
		// - constructing, adding and removing from the neuron allele connection sets
		static void CreateNeuronConnectionSets(NeuronAllele *pNew, const NeuronAllele * const pPrev);
		static void AddToNeuronConnectionSets(NeuronAllele *pToProcess, const NeuronAllele * const pNew);
		static void AddToNeuronConnectionSets(NeuronAllele *pToProcess, const NeuronAllele::SET_CONNECTED_NEURONS &toAdd);
		static void RemoveFromNeuronConnectionSets(NeuronAllele *pToProcess, const NeuronAllele * const pRemoved);
		
		// - simple neuron allele mutations (weight/bias/etc.) and the more complex ones (adding
		// a connection or neuron, and removing a neuron)
		static void MutateNeuronAlleleProperties(NeuronAllele* const pToMutate, const RNAMutationRates &rates);
		void MutateConnectionAlleleAddNeuron(ConnectionAllele* const pToMutate, std::vector<NeuronMutation> &prevMutations);
		void MutateNeuronAlleleRemoveNeuron(NeuronAllele* const pToMutate, std::vector<ConnectionMutation> &prevMutations);
		static void MutateNeuronAlleleAddConnection(NeuronAllele* pSource, NeuronAllele* const pTarget, std::vector<ConnectionMutation> &prevMutations);
	};
}

#endif