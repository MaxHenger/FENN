#ifndef FENN_NEATRNA_H
#define FENN_NEATRNA_H

#include "Config.h"
#include "RNA.h"
#include "Neuron.h"
#include "World.h"

#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>

namespace fenn //general framework namespace
{
	namespace neat //neat-implementation namespace
	{
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


			//Retrieving RNA data
			// - input neurons
			unsigned int GetNumInputNeurons() const;
			NeuronAllele* GetInputNeuron(unsigned int i) const;

			// - hidden neurons
			unsigned int GetNumHiddenNeurons() const;
			NeuronAllele* GetHiddenNeuron(unsigned int i) const;

			// - output neurons
			unsigned int GetNumOutputNeurons() const;
			NeuronAllele* GetOutputNeuron(unsigned int i) const;
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
}

#endif