#include<iostream>
#include<algorithm>
#include<vector>
using namespace std;

struct neuron {
	double value;
	double delta;
	double input_function_val;
	vector<pair<int, double>> neighbours;
};

// the bias neuron will always be the 0th neuron un the corresponding array
class neural_xor {
	private:
		vector<neuron> input_layer; // contains 2 neurons + 1 bias neuron
		vector<neuron> hidden_layer; // can contain any number of neurons + 1 bias neuron
		vector<neuron> output_layer; // contains 1 neuron
		int num_neurons_in_hidden_layer;
		const double learning_rate = 0.9;
		const double bias = -1.0; // this is the value of the bias neuron
		const int max_epochs = 50000;
	
		const vector<pair<int, int>> inputs = { {0, 0}, {0, 1}, {1, 0}, {1, 1} };
		const vector<int> expected_result_xor = { 0, 1, 1, 0 };

		double g(double x) {
			return 1.0 / (1.0 + exp(-x));
		}

		double g_prim(double x) { // the derivative of the sigmoid function
			return g(x) * (1.0 - g(x));
		}

		double get_random_edge_weight() {
			return (double)rand() / RAND_MAX * 2.0 - 1.0;
		}

		void create_network() {
			input_layer[0].value = bias; // bias is at index 0 in the input layer
			hidden_layer[0].value = bias; // bias is at index 0 in the hidden layer

			// create edges between input layer and the hidden layer
			for (int i = 0; i < input_layer.size(); ++i) {
				for (int j = 0; j < hidden_layer.size(); ++j) {
					double rand_edge_val = get_random_edge_weight();
					input_layer[i].neighbours.push_back({ j, rand_edge_val });
				}
			}

			// create edges between hidden layer and the output_layer
			for (int i = 0; i < hidden_layer.size(); ++i) {
				for (int j = 0; j < output_layer.size(); ++j) {
					double rand_edge_val = get_random_edge_weight();
					hidden_layer[i].neighbours.push_back({ j, rand_edge_val });
				}
			}
		}

		void train_neural_network() {
			int i = 0;

			while (i < max_epochs) {
				for (int k = 0; k < inputs.size(); ++k) {
					// set the values in the input layer
					input_layer[1].value = inputs[k].first;
					input_layer[2].value = inputs[k].second;

					// calculate the values in the hidden layer
					for (int j = 0; j < hidden_layer.size(); ++j) {
						double in_output = 0.0;

						// calculate the value of each neuron in the hidden layer
						for (int t = 0; t < input_layer.size(); ++t) {
							for (int l = 0; l < input_layer[t].neighbours.size(); ++l) {
								if (input_layer[t].neighbours[l].first == j) {
									in_output += (input_layer[t].value * input_layer[t].neighbours[l].second);
								}
							}
						}

						hidden_layer[j].value = g(in_output); // keep the computed value of each neuron from the hidden layer
						hidden_layer[j].input_function_val = in_output; // keep the input function value only (without the g function applied to it)
					}

					// computing the value of the only output neuron together with the delta for the neuron
					double in_output = 0.0;
					for (int j = 0; j < hidden_layer.size(); ++j) {
						// here every node from the hidden layer has only one neighbour -> the only output node, so it is the neighbour at index 0
						in_output += (hidden_layer[j].value * hidden_layer[j].neighbours[0].second);
					}

					output_layer[0].value = g(in_output); // store the value in the output neuron
					output_layer[0].input_function_val = in_output; // keep only the value of the input function

					double error = expected_result_xor[k] - output_layer[0].value;
					output_layer[0].delta = error * g_prim(in_output);
				
					// update the edges between the hidden layer and the output layer
					for (int j = 0; j < hidden_layer.size(); ++j) {
						// every node in the hidden layer has only one neighbour -> the node from the output layer
						hidden_layer[j].neighbours[0].second = hidden_layer[j].neighbours[0].second + learning_rate * hidden_layer[j].value * output_layer[0].delta;
					}

					// compute delta for each neuron in the hidden layer
					for (int j = 0; j < hidden_layer.size(); ++j) {
						// there is only one neuron in the output layer
						hidden_layer[j].delta = g_prim(hidden_layer[j].input_function_val) * hidden_layer[j].neighbours[0].second * output_layer[0].delta;
					}

					// update the edges between the input and the hidden layer
					for (int j = 0; j < input_layer.size(); ++j) {
						for (int t = 0; t < input_layer[j].neighbours.size(); ++t) {
							input_layer[j].neighbours[t].second = input_layer[j].neighbours[t].second + learning_rate * input_layer[j].value * hidden_layer[input_layer[j].neighbours[t].first].delta;
						}
					}
				}

				++i;
			}
		}

		void test_neural_network() {
			cout << "Boolean function XOR:\n";

			for (int k = 0; k < inputs.size(); ++k) {
				input_layer[1].value = inputs[k].first;
				input_layer[2].value = inputs[k].second;

				// compute values of the neurons in the hidden layer
				for (int j = 0; j < hidden_layer.size(); ++j) {
					double in_output = 0.0;

					// calculate the value of each neuron in the hidden layer
					for (int t = 0; t < input_layer.size(); ++t) {
						for (int l = 0; l < input_layer[t].neighbours.size(); ++l) {
							if (input_layer[t].neighbours[l].first == j) {
								in_output += (input_layer[t].value * input_layer[t].neighbours[l].second);
							}
						}
					}

					hidden_layer[j].value = g(in_output);
				}

				// compute the values of the neurons in the output layer (only one neuron)
				double in_output = 0.0;
				for (int j = 0; j < hidden_layer.size(); ++j) {
					in_output += (hidden_layer[j].value * hidden_layer[j].neighbours[0].second);
				}

				cout << "x = " << inputs[k].first << " y = " << inputs[k].second
					<< " Actual result = " << expected_result_xor[k] << " Neural net result = " << g(in_output) << '\n';
			}
		}

	public:
		neural_xor(int _num_hidden): num_neurons_in_hidden_layer(_num_hidden) {
			input_layer.resize(3); // 2 neurons + 1 bias
			hidden_layer.resize(num_neurons_in_hidden_layer + 1); // entered number of neurons + 1 bias
			output_layer.resize(1);
			create_network();
		}

		void train_nn_test_nn_funtion_xor() {
			train_neural_network();
			test_neural_network();
		}
};

int main() {
	neural_xor nn = neural_xor(4);
	nn.train_nn_test_nn_funtion_xor();

	return 0;
}