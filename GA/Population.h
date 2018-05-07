#pragma once
#include "Genome.h"
#include <vector>
#include <functional>
#include <ctime>
#include <random>
enum class SelectionMethod
{
	ROULETTE = 0,
	RANDOM,
	TOURNAMENT
};

template<typename GeneType>
class Population
{
public:

	Population() {};
	Population(int a_numGenomes, int a_numChromosomes, int a_numGenes, std::function<GeneType()> a_geneFunc)
	{
		m_numOfGenes = a_numGenes;
		m_numOfGenomes = a_numGenomes;
		m_numOfChromosomes = a_numChromosomes;
		
		for (int i = 0; i < a_numGenomes; i++)
		{
			m_genomes.push_back(Genome<GeneType>(a_numChromosomes, a_numGenes, a_geneFunc));
		}
	};
	~Population() {};
	//Remember BIGGER IS BETTER
	void SetFitnessFunction(std::function<void(Genome<GeneType>&)> a_pointer)				{ m_fitnessFunc = a_pointer; }
	void SetMutateFunction(std::function<void(Chromosome<GeneType>&)> a_pointer)			{ m_mutateFunc = a_pointer; }
	void SetGeneFunction(std::function<GeneType()> a_pointer)								{ m_geneFunc = a_pointer; }
	//The array will be sorted by defualt
	void SetDisplayFunction(std::function<void(std::vector<Genome<GeneType>>)> a_pointer)	{ m_displayFunction = a_pointer; }

	void GenerateGenomes(int a_numGenomes, int a_numChromosomes, int a_numGenes)
	{
		m_numOfGenes = a_numGenes;
		for (int i = 0; i < a_numGenomes; i++)
		{
			m_genomes.push_back(Genome<DataType>(a_numChromosomes, a_numGenes, m_geneFunc));
		}
	}
	void SelectionAndBreeding(SelectionMethod a_selection)
	{
		for (size_t i = 0; i < m_genomes.size(); i++)
		{
			Genome<GeneType> temp = Genome<GeneType>();
			for (size_t j = 0; j < m_genomes[i].GetNumberOfChromosomes(); j++)
			{
				if (a_selection == SelectionMethod::ROULETTE)
				{
					float totalFitness = GetTotalFitness();
					int randFitnessPoint = rand() % (int)totalFitness;
					int randFitnessPoint2 = rand() % (int)totalFitness;

					auto parent1 = GetGenomeWithinFitness(randFitnessPoint);
					auto parent2 = GetGenomeWithinFitness(randFitnessPoint2);

					Crossover(parent1, parent2, temp);
				}
				if (a_selection == SelectionMethod::RANDOM)
				{
					int randParentNum1 = rand() % m_genomes.size();
					int randParentNum2 = rand() % m_genomes.size();

					auto parent1 = m_genomes[randParentNum1];
					auto parent2 = m_genomes[randParentNum2];

					Crossover(parent1, parent2, temp);
				}
				if (a_selection == SelectionMethod::TOURNAMENT)
				{
					Genome<GeneType> parent1 = Genome<GeneType>();
					Genome<GeneType> parent2 = Genome<GeneType>();

					int numToSelect = m_genomes.size() * 0.05;
					m_genomesForTournament.reserve(numToSelect);
					m_genomesForTournament.resize(numToSelect);
					
					//Gets the first parent from the selected genomes
					for (size_t i = 0; i < numToSelect; i++)
					{
						int num = rand() % m_numOfGenomes;
						m_genomesForTournament[i] = m_genomes[num];
					}					
					parent1 = m_genomesForTournament[0];
					for (size_t i = 1; i < numToSelect; i++)
					{
						if (m_genomesForTournament[i].GetFitness() > parent1.GetFitness())
						{
							parent1 = m_genomesForTournament[i];
						}
					}
					//Gets the second parent from the selected genomes
					for (size_t i = 0; i < numToSelect; i++)
					{
						int num = rand() % m_numOfGenomes;
						m_genomesForTournament[i] = m_genomes[num];
					}					
					parent2 = m_genomesForTournament[0];
					for (size_t i = 1; i < numToSelect; i++)
					{
						if (m_genomesForTournament[i].GetFitness() > parent2.GetFitness())
						{
							parent2 = m_genomesForTournament[i];
						}
					}

					Crossover(parent1, parent2, temp);
				}
			}
			m_nextGen.push_back(temp);
		}
		m_genomes = m_nextGen;
		m_nextGen.clear();
		m_genomesForTournament.clear();
	}
	void CalculateFitness()
	{
		for (size_t i = 0; i < m_genomes.size(); i++)
		{
			m_fitnessFunc(m_genomes[i]);
		}
	}
	void Mutate()
	{
		for (size_t i = 0; i < m_genomes.size(); i++)
		{
			for (size_t j = 0; j < m_genomes[i].GetNumberOfChromosomes(); j++)
			{
				m_genomes[i].GetChromosomeAtPoint(j).Mutate(m_mutateFunc);
			}
		}
		m_sorted = false;
	}
	void DisplayFitest()
	{
		SortGenomes();
		m_displayFunction(m_genomes);
	}
	Genome<GeneType> GetFittestGenome()
	{
		SortGenomes();
		return m_genomes[0];
	}

private:
	std::vector<Genome<GeneType>>							m_genomes;
	std::vector<Genome<GeneType>>							m_nextGen;
	std::vector<Genome<GeneType>>							m_genomesForTournament;
	std::function<void(Genome<GeneType>&)>					m_fitnessFunc;
	std::function<void(Chromosome<GeneType>&)>				m_mutateFunc;
	std::function<void(std::vector < Genome<GeneType>>)>	m_displayFunction;
	std::function<GeneType()>								m_geneFunc;
	int														m_numOfGenes;
	int														m_numOfChromosomes;
	int														m_numOfGenomes;
	bool													m_sorted = false;

	Genome<GeneType> GetGenomeWithinFitness(int a_randFitness)
	{
		int randFitness = a_randFitness;;
		for (int i = 0; i < m_numOfGenomes; i++)
		{
			randFitness -= (int)m_genomes[i].GetFitness();
			if (randFitness < 0.0f)
			{
				return m_genomes[i];
			}
		}
	}
	void Crossover(Genome<GeneType> a_parent1, Genome<GeneType> a_parent2, Genome<GeneType>& a_nextGen)
	{
		Genome<GeneType> tempGenome = Genome<GeneType>();
		int halfwayGenes = m_numOfChromosomes * 0.5f;

		for (size_t i = 0; i < halfwayGenes; i++)
		{
			tempGenome.AddChromosome(a_parent1.GetChromosomeAtPoint(i));
		}
		for (size_t i = halfwayGenes; i < m_numOfChromosomes; i++)
		{
			tempGenome.AddChromosome(a_parent2.GetChromosomeAtPoint(i));
		}

		a_nextGen = tempGenome;
	}
	void SortGenomes()
	{
		std::sort(m_genomes.begin(), m_genomes.end(), [](const Genome<GeneType>& a, const Genome<GeneType>& b) -> bool
		{
			return a.GetFitness() > b.GetFitness();
		});
	}
	float GetTotalFitness()
	{
		float score = 0;
		for (size_t i = 0; i < m_genomes.size(); i++)
		{
			score += m_genomes[i].GetFitness();
		}
		return score;
	}
};

