//-----------------------------------
//Created by: Brent Kingma
//Date Version 1.0 Finished: 07/05/18
//-----------------------------------
#pragma once
#include <vector>
#include <functional>
#include <random>
#include <ctime>
enum class SelectionMethod
{
	ROULETTE = 0,
	RANDOM,
	TOURNAMENT
};

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

	std::vector<GeneType> getGenes() { return m_genes; };
	void setGenes(std::vector<GeneType> a_newGenes) { m_genes = a_newGenes; }
	void setGene(int a_pos, GeneType a_newGene) { m_genes[a_pos] = a_newGene; }
	void AddGene(GeneType a_gene) { m_genes.push_back(a_gene); }
	void Mutate(std::function<void(Chromosome<GeneType>&)> a_function)
	{
		a_function(*this);
	}
private:
	std::vector<GeneType> m_genes;
};

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

	Chromosome<GeneType>& GetChromosomeAtPoint(int point) { return m_chromosomes[point]; }
	int GetNumberOfChromosomes() { return m_chromosomes.size(); }

	void AddChromosome(Chromosome<GeneType>& a_chromosome) { m_chromosomes.push_back(a_chromosome); }
	void setFitness(float a_fitness) { m_fitness = a_fitness; }
	float GetFitness() const { return m_fitness; }
private:
	std::vector<Chromosome<GeneType>>	m_chromosomes;
	bool								m_hasBeenSorted = false;
	float								m_fitness = 0.0f;
};

template<typename GeneType>
class Population
{
public:

	Population() {};
	Population(int a_numGenomes, int a_numChromosomes, int a_numGenes, std::function<GeneType()> a_geneFunc)
	{
		srand((unsigned int)time(NULL));

		m_numOfGenes = a_numGenes;
		m_numOfGenomes = a_numGenomes;
		m_numOfChromosomes = a_numChromosomes;
		
		for (int i = 0; i < a_numGenomes; i++)
		{
			m_genomes.push_back(Genome<GeneType>(a_numChromosomes, a_numGenes, a_geneFunc));
		}
	};
	~Population() {};
	//Funtion that calculates the fitness of their Population based on user standard
	//Remember BIGGER IS BETTER
	void SetFitnessFunction(std::function<void(Genome<GeneType>&)> a_pointer)				{ m_fitnessFunc = a_pointer; }
	//Function that the user created to mutate their Population to their standard
	void SetMutateFunction(std::function<void(Chromosome<GeneType>&)> a_pointer)			{ m_mutateFunc = a_pointer; }
	//Function that creates the genes based on the user standards
	void SetGeneFunction(std::function<GeneType()> a_pointer)								{ m_geneFunc = a_pointer; }
	//The array will be sorted by defualt
	void SetDisplayFunction(std::function<void(std::vector<Genome<GeneType>>)> a_pointer)	{ m_displayFunction = a_pointer; }
	void SetNumberOfGenomes(int a_numGenomes)												{ m_numOfGenomes = a_numGenomes; }
	void SetNumberOfChromosomes(int a_numChromosomes)										{ m_numOfChromosomes = a_numChromosomes; }
	void SetNumberOfGenes(int a_numGenes)													{ m_numOfGenes = a_numGenes; }

	void GenerateGenomes()
	{
		for (int i = 0; i < m_numOfGenomes; i++)
		{
			m_genomes.push_back(Genome<GeneType>(m_numOfChromosomes, m_numOfGenes, m_geneFunc));
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




