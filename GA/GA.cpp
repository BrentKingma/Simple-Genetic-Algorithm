// GA.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Population.h"
#include <string>
#include <random>
#include <algorithm>
#include <ctime>
#include <iostream>

const int NUM_OF_GENOMES = 100;		//Number of solutions
const int NUM_OF_CHROMOSOMES = 5;	//Number of components in solution
const int NUM_OF_GENES = 1;			//Number of lowest level components in chromosomes

std::string word = "brent";

char GeneFunc()
{
	int num = rand() % 26 + 97;
	return (char)num;
}

void FitnessFunc(Genome<char>& a_genome)
{
	float score = 0;

	for (size_t i = 0; i < a_genome.GetNumberOfChromosomes(); i++)
	{
		score += std::abs((int)word[i] - (int)a_genome.GetChromosomeAtPoint(i).getGenes()[0]);
	}
	a_genome.setFitness(130 - score);
}

void MutateFunc(Chromosome<char>& a_chromosome)
{
	int size = a_chromosome.getGenes().size();
	int charData;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(-1, 1);
	for (size_t i = 0; i < size; i++)
	{
		int chance = rand() % 101;
		if (chance >= 0 && chance <= 50)
		{
			charData = (int)a_chromosome.getGenes()[0];
			charData += dis(gen);
			charData = std::clamp(charData, 97, 122);

			a_chromosome.setGene(0, (char)charData);
		}
	}
}

void DisplayFunction(std::vector<Genome<char>> a_genomes)
{
	int size = a_genomes[0].GetNumberOfChromosomes();
	for (int i = 0; i < size; i++)
	{
		std::cout << a_genomes[0].GetChromosomeAtPoint(i).getGenes()[0];
	}
	std::cout << "		" << a_genomes[0].GetFitness() << std::endl;
}

int main()
{
	///Method 1:
	Population<char> m_pop = Population<char>(NUM_OF_GENOMES, 5, 1, GeneFunc);
	m_pop.SetFitnessFunction(FitnessFunc);
	m_pop.SetMutateFunction(MutateFunc);
	m_pop.SetDisplayFunction(DisplayFunction);
	///Method 2:
	/*Population<char> m_pop = Population<char>();
	
	m_pop.SetNumberOfGenomes(NUM_OF_GENOMES);
	m_pop.SetNumberOfChromosomes(NUM_OF_CHROMOSOMES);
	m_pop.SetNumberOfGenes(NUM_OF_GENES);

	m_pop.SetGeneFunction(GeneFunc);

	m_pop.SetFitnessFunction(FitnessFunc);
	m_pop.SetMutateFunction(MutateFunc);
	m_pop.SetDisplayFunction(DisplayFunction);

	m_pop.GenerateGenomes();*/

	int numOfGeneration = 0;

	

	while (true)
	{
		m_pop.CalculateFitness();

		if (m_pop.GetFittestGenome().GetFitness() == 130.0f)
		{
			std::cout << "--------------------------------------" << std::endl;
			m_pop.DisplayFitest();
			std::cout << "Number of Generations: " << numOfGeneration << std::endl;
			break;
		}

		m_pop.DisplayFitest();		

		m_pop.SelectionAndBreeding(SelectionMethod::TOURNAMENT);

		m_pop.Mutate();

		numOfGeneration++;
	}

	
	system("PAUSE");
    return 0;
}

