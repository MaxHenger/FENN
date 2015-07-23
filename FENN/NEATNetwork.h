#ifndef FENN_NEATNETWORK_H
#define FENN_NEATNETWORK_H

#include "Config.h"
#include "Neuron.h"
#include "NeuronInput.h"
#include "NEATRNA.h"

#include <vector>
#include <map>

namespace fenn
{
	namespace neat
	{
		class Network
		{
		public:
			Network();
			~Network();

			//initializing the network using an RNA string
			bool Initialize(const RNA &rna);

			//retrieving basic network information
			unsigned int GetNumInputNeurons() const;
			unsigned int GetNumHiddenNeurons() const;
			unsigned int GetNumOutputNeurons() const;

			//processing data using the network
			void SetInput(unsigned int i, VALUE_TYPE input);
			void SetInputs(VALUE_TYPE *pInput);
			bool Process();
			void GetOutput(unsigned int i, VALUE_TYPE &output);
			void GetOutputs(VALUE_TYPE *pOutputs);

		protected:
			std::vector<NeuronInput*> m_veInputNeurons;
			std::vector<Neuron*> m_veHiddenNeurons;
			std::vector<Neuron*> m_veOutputNeurons;
		
		private:
			//recursive function to determine call order
			void ConstructNeurons(NeuronAllele *pProcess, std::map<GLOBAL_INDEX_TYPE, Neuron*> &map);
			void ReleaseAll();
		};
	}
}

#endif