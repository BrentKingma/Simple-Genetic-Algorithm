#pragma once
#include <vector>
#include <functional>
#include "Population.h"

template<typename GeneType>
struct Chromosome
{
public:
	Chromosome() {};
	Chromosome(int a_numGenes, std::function<GeneType()> a_geneFunc)
	{
		m_genes.reserve(a_numGenes);
		for (int i = 0; i < a_numGenes; i++)
		{
			if (a_geneFunc == nullptr)
			{
				m_genes.push_back(GeneType());
			}
			else
			{
				m_genes.push_back(a_geneFunc());
			}			
		}
	};
	~Chromosome() {};

	std::vector<GeneType> getGenes()								{ return m_genes; };
	void setGenes(std::vector<GeneType> a_newGenes)					{ m_genes = a_newGenes; }
	void setGene(int a_pos, GeneType a_newGene)						{ m_genes[a_pos] = a_newGene; }
	void AddGene(GeneType a_gene)									{ m_genes.push_back(a_gene); }
	void Mutate(std::function<void(Chromosome<GeneType>&)> a_function)
	{
		a_function(*this);
	}
private:
	std::vector<GeneType> m_genes;
};

