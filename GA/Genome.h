#pragma once
#include <vector>
#include <functional>
#include "Chromosome.h"
#include <iostream>

template<typename GeneType>
struct Genome
{
public:
	Genome() 
	{
		m_chromosomes.reserve(250);
	};
	Genome(int a_numChromosomes, int a_numGenes, std::function<GeneType()> a_geneFunc)
	{
		m_chromosomes.reserve(a_numChromosomes);
		for (int i = 0; i < a_numChromosomes; i++)
		{
			m_chromosomes.push_back(Chromosome<GeneType>(a_numGenes, a_geneFunc));
		}
	};
	~Genome() {};

	Chromosome<GeneType>& GetChromosomeAtPoint(int point)	{ return m_chromosomes[point]; }
	int GetNumberOfChromosomes()							{ return m_chromosomes.size(); }
	
	void AddChromosome(Chromosome<GeneType>& a_chromosome)	{ m_chromosomes.push_back(a_chromosome); }
	void setFitness(float a_fitness)						{ m_fitness = a_fitness; }
	float GetFitness() const								{ return m_fitness; }
private:
	std::vector<Chromosome<GeneType>>	m_chromosomes;
	bool								m_hasBeenSorted = false;
	float								m_fitness = 0.0f;
};

