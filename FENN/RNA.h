#ifndef FENN_RNA_H
#define FENN_RNA_H

#include "Config.h"
#include "Neuron.h"

#include <vector>
#include <string>
#include <set>
#include <utility>

namespace fenn
{
	//forward definitions
	struct ConnectionAllele;
	struct NeuronAllele;
	struct RNAMutationRates;
	struct NeuronMutation;
	struct ConnectionMutation;

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
		typedef std::pair<LOCAL_INDEX_TYPE, NeuronAllele*> PAIR_CONNECTED;
		typedef std::vector<PAIR_CONNECTED> VECTOR_CONNECTED_NEURONS;
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
}

#endif