#include<iostream>
#include<vector>
#include<algorithm>
#include<queue>
#include<ctime>
using namespace std;

// THE POPULATION SIZE SHOULD BE AN EVEN NUMBER

class TSP_genetic {
private:
	int n; // the number of cities
	int num_iterations_of_the_algorithm; // the number of iterations of the genetic algorithm
	int population_size; // the size of the population
	int mutatation_percentage; // the number of individuals that will mutate somehow (should be a number between 0 and 100 showing the number of mutations in the new generation)
	vector<vector<double>> distance_matrix; // the distance between each two cities
	vector<pair<int, int>> cities; // the random coordinates of the cities
	vector<vector<int>> population; // the current population
	vector<vector<int>> breeding_selection; // the individuals from the current population that are chosen to breed
	vector<vector<int>> children; // the generated children
	vector<vector<int>> best_individuals; // the best individuals of the current population
	int size_of_best_individuals; // dynamically calculated, we do not know the size of the population, i.e. it will be caluclated in selection

	// set the coordinates of the cities
	void set_cities() {
		for (int i = 0; i < n; ++i) {
			cities[i].first = rand(); // random number between 0 and 32767
			cities[i].second = rand(); // random number båtween 0 and 32767
		}
	}

	// computes the distance between the random generated cities
	void compute_distance() {
		for (int i = 0; i < n - 1; ++i) {
			for (int j = i + 1; j < n; ++j) {
				int xi_xj_2 = (cities[i].first - cities[j].first) * (cities[i].first - cities[j].first);
				int yi_yj_2 = (cities[i].second - cities[j].second) * (cities[i].second - cities[j].second);
				double distance_x_y = sqrt(xi_xj_2 + yi_yj_2);
				distance_matrix[i][j] = distance_x_y; // distance from point i to point j
				distance_matrix[j][i] = distance_x_y; // distance from point j to point i 
			}
		}

		for (int i = 0; i < n; ++i) {
			distance_matrix[i][i] = 0.0; // the distance from the city to itself is 0
		}
	}

	// this function generates a random population of population_size individuals; population_size is set in the constructor
	void generate_population() {
		vector<int> individual(n, 0); // the size of an individual is the number of cities
		for (int i = 0; i < n; ++i) { // initially individual contains the numbers 0, 1, 2, ..., n - 1
			individual[i] = i;
		}

		for (int i = 0; i < population_size; ++i) { // generate a population of the given size
			random_shuffle(individual.begin(), individual.end()); // generate a random permutation to represent an individual
			population[i] = individual; // add the randomly generated individual to the population
		}
		// here we have a randomly generated population of population_size individuals
	}

	// computes how fit a given individual (permutation) is
	double fitness(const vector<int>& individual) {
		double fitness_value = 0;

		for (int i = 0; i < n - 1; ++i) {
			fitness_value += distance_matrix[individual[i]][individual[i + 1]];
		}

		return fitness_value;
	}

	// this functor is used for the construction of the priority_squeue
	struct comp {
		bool operator()(pair<vector<int>, double> a, pair<vector<int>, double> b) {
			return a.second > b.second;
		}
	};

	// this function selects individuals from the current population for breeding
	double selection() {
		priority_queue <pair<vector<int>, double>, vector<pair<vector<int>, double>>, comp> sorted_on_fitness;

		for (int i = 0; i < population.size(); ++i) {
			sorted_on_fitness.push({ population[i], fitness(population[i]) });
		}

		double best_individual_from_current_population = sorted_on_fitness.top().second;

		// we will not allow the 20% worst individuals from the current population to take part in breeding and instead the 20% best individuals will participate twice in breeding
		int worst = population_size / 5;
		int cnt_best = 0;
		int b_idx = 0;

		while (sorted_on_fitness.size() > worst) {
			if (cnt_best < worst) { // we put the best 20% of the individuals in the vector
				best_individuals[cnt_best] = sorted_on_fitness.top().first;
				++cnt_best;
			}
			breeding_selection[b_idx] = sorted_on_fitness.top().first; // this individual will be a parent
			++b_idx;
			sorted_on_fitness.pop();
		}

		size_of_best_individuals = cnt_best;
		// copy the best individuals that are kept in the vector best to the breeding_selection
		for (int i = 0; i < cnt_best; ++i) {
			breeding_selection[b_idx] = best_individuals[i];
			++b_idx;
			if (b_idx == population_size) break; // we have filled breeding_selection
		}

		int breeding_selection_better = b_idx / 2; // get random individuals from the better part of breeding_selection (if we do not have enough elements yet)
		while (b_idx < population_size) {
			breeding_selection[b_idx] = breeding_selection[rand() % (breeding_selection_better + 1)];
			++b_idx;
		}

		// after this function we should have 20% x 2 of the best individuals, 40% average and 20% below average, i.e. we totally ignore the worst 20% and they do not participate
		// in the breeding process

		random_shuffle(breeding_selection.begin(), breeding_selection.end()); // make random pairs of parents
		// the selected for breeding individuals are in breeding_selection vector

		// return the value of the best individual
		return best_individual_from_current_population;
	}

	// helper function for the chosen crossover - cycle crossover
	int idx_of_elem_in_parent1(int elem, const vector<int>& p1) {
		for (int i = 0; i < p1.size(); ++i) {
			if (p1[i] == elem) return i;
		}
	}

	// the crossover - the chosen method is cycle crossover
	void crossover(const vector<int>& p1, const vector<int>& p2, vector<int>& ch1, vector<int>& ch2) {
		vector<bool> visited(n, false); // initially no element is visited
		int num_visited = 0;
		bool flag = true; // this flag tells us where to put element - in p1 or in p2
		vector<int> cycle_indices(n, -1); // keeps the indices of the found cycle
		int cycle_indices_size = 0; // current size of cycle_indices vector

		while (true) {
			cycle_indices_size = 0; // now we should find the next cycle, so erase the last found from the vector
			if (num_visited == n) break; // we have found all the cycles, so stop

			// find the index from which to start a cycle
			int idx_to_start = -1;
			for (int i = 0; i < n; ++i) {
				if (!visited[i]) {
					idx_to_start = i;
					++num_visited;
					visited[i] = true;
					break;
				}
			}

			// starting point of the cycle is found
			while (true) {
				cycle_indices[cycle_indices_size] = idx_to_start;
				++cycle_indices_size;
				int elem_idx = idx_of_elem_in_parent1(p2[idx_to_start], p1);
				idx_to_start = elem_idx; // reset the start index
				if (!visited[idx_to_start]) {
					visited[idx_to_start] = true;
					++num_visited;
				}
				else break; // we have formed the cycyle, so exit the loop
			}

			// if flag == true -> the elements stay in the corresonding children (p1 -> ch1, p2 -> ch2), else they are swapped (p1 -> ch2, p2 -> ch1)
			for (int i = 0; i < cycle_indices_size; ++i) {
				if (flag) {
					ch1[cycle_indices[i]] = p1[cycle_indices[i]];
					ch2[cycle_indices[i]] = p2[cycle_indices[i]];
				}
				else {
					ch1[cycle_indices[i]] = p2[cycle_indices[i]];
					ch2[cycle_indices[i]] = p1[cycle_indices[i]];
				}
			}

			flag = !flag; // swap the positions of the elements in the next cycle
		}
		// after the selection the individuals that are chosen to breed are in breeding_selection
	}

	// creates children
	void generate_children() {
		// breed them by pairs and generate kids
		vector<int> ch1(n, -1);
		vector<int> ch2(n, -1);

		int child_idx = 0;

		// each two consecutive individuals in breeding_selection create two children
		for (int i = 0; i < population_size - 1; i += 2) {
			crossover(breeding_selection[i], breeding_selection[i + 1], ch1, ch2);
			// store the generated children in the children vector
			children[child_idx] = ch1; ++child_idx;
			children[child_idx] = ch2; ++child_idx;
		}

		// after this function all the generated children are kept in children vector
		// now we should mutate the children
	}

	// helper function, mutates a single child (mutation metohod used - swaps)
	void mutate_child(vector<int>& ch) {
		int idx1 = rand() % n; // get random index 
		int idx2 = rand() % n; // get random index

		swap(ch[idx1], ch[idx2]); // swap the genes at the corresponding indices and thus create a mutation
	}

	// mutation_percantage field in the class determines the number of children that will mutate
	void mutate_children() {
		double coefficient = mutatation_percentage / 100.0;
		int number_of_mutated_individuals = coefficient * population_size; // we calculated the number of individuals that will mutate in the new generation

		vector<int> individuals_to_mutate(population_size, 0);
		for (int i = 0; i < population_size; ++i) {
			individuals_to_mutate[i] = i;
		}

		random_shuffle(individuals_to_mutate.begin(), individuals_to_mutate.end()); // shuffle the vector randomly -> the values of the first number_of_mutated_individuals will be mutated

		int ind_to_mut_idx = 0; // used to traverse inividuals_to_mutate vector and to get the next random individual to mutate

		// in the loop below we mutate part of the new generation
		for (int i = 0; i < number_of_mutated_individuals; ++i) {
			mutate_child(children[individuals_to_mutate[ind_to_mut_idx]]);
			++ind_to_mut_idx;
		}

		// after this function we have mutated part of the new generation
		// now we have 2 * population_size individuals (kept in children and population vectors)
		// now we should reduce the number of individuals so that the population size is again population_size
	}

	// This function decides which individuals from the old generation and new generation to keep so that we have population_size individuals again. The new population will be kept
	// in population vector. After the execution of this function we repeat the whole process again but this time we do not call generate_population because create_population would have
	// already generated the poulation for the next iteration of the genetic algorithm.
	// Method to create the next population: we keep 20% of the best individuals from the current population and the other 80% are the best individuals in the new generation
	void create_population() {
		// best 20% of the current population are kept in best_individuals vector
		for (int i = 0; i < size_of_best_individuals; ++i) {
			population[i] = best_individuals[i];
		}

		// up to here the best 20% of the current generation are added to the next population.

		priority_queue <pair<vector<int>, double>, vector<pair<vector<int>, double>>, comp> best_children; // 80% of the best children will go to the next population

		for (int i = 0; i < population_size; ++i) {
			best_children.push({ children[i], fitness(children[i]) }); // add all the children to the priority queue together with fitness; the priority queue sorts on the fitness level
		}

		int number_of_ind_in_next_pop = size_of_best_individuals; // number of individuals already included in the next population

		// add the remaining 80% from the children in the next population
		while (number_of_ind_in_next_pop < population_size) {
			population[number_of_ind_in_next_pop] = best_children.top().first;
			++number_of_ind_in_next_pop;
			best_children.pop();
		}

		// after this function the new population is constructed and we are ready for the next iteration of the genetic algorithm
	}

public:
	TSP_genetic(int _n, int _population_size, int _mutation_percentage, int _num_iterations_of_the_algorithm) {
		n = _n;
		population_size = _population_size;
		mutatation_percentage = _mutation_percentage;
		num_iterations_of_the_algorithm = _num_iterations_of_the_algorithm;

		distance_matrix.resize(n, vector<double>(n, -1.0));
		cities.resize(n, { -1,-1 });
		population.resize(population_size, vector<int>(n, -1));
		breeding_selection.resize(population_size, vector<int>(n, -1));
		children.resize(population_size, vector<int>(n, -1));
		best_individuals.resize(population_size, vector<int>(n, -1));
	}

	// prints the best individual initially and the best one of each population after every iteration of the algorithm
	void solve1() {
		set_cities();
		compute_distance();
		generate_population();

		for (int i = 0; i < num_iterations_of_the_algorithm; ++i) {
			if (i == 0) cout << "\nThe length of the shortest path (best individual) in the initially generated population is: ";
			else cout << "Length of the shortest path after " << i << " iterations: ";
			cout << fixed << selection() << '\n';
			generate_children();
			mutate_children();
			create_population();
		}
		cout << "The length of the shortest path (best individual) after the last iteration of the genetic algorithm is: ";
		cout << selection() << '\n';
	}

	// The number of iterations should be larger than 10
	// Is is best if the number of (iterations - 10) is divisible by 4 -> the printing of the best indiviual will be more regular
	void solve2() {
		set_cities();
		compute_distance();
		generate_population();

		int step = (num_iterations_of_the_algorithm - 10) / 4;
		int coef = 0;

		for (int i = 0; i < num_iterations_of_the_algorithm; ++i) {
			double best_individual = selection();

			if (i == 0) {
				cout << "\nThe length of the shortest path (best individual) in the initially generated population is: " << fixed << best_individual << '\n';
			}
			else if ((10 + coef * step == i) && coef < 4) {
				++coef;
				cout << "The length of the shortest path (best individual) after " << i << " iterations is: " << best_individual << '\n';
			}
			generate_children();
			mutate_children();
			create_population();
		}
		cout << "The length of the shortest path (best individual) after the last iteration of the genetic algorithm is: ";
		cout << selection() << '\n';
	}
};

int main() {

	// tsp(number of cities, number of individuals in the popuation, percentage of mutation, number of iteration of the genetic algorithm)

	int number_of_cities;
	int number_of_individuals_in_population;
	int mutation_percentage;
	int num_iter_of_genetic_alg;

	cout << "Enter number of cities\n";
	cin >> number_of_cities;
	cout << "Enter number of individuals in population\n";
	cin >> number_of_individuals_in_population;
	cout << "Enter mutation percentage\n";
	cin >> mutation_percentage;
	cout << "Enter number of iterations of the algorithm\n";
	cin >> num_iter_of_genetic_alg;

	TSP_genetic tsp(number_of_cities, number_of_individuals_in_population, mutation_percentage, num_iter_of_genetic_alg);
	tsp.solve2();

	return 0;
}