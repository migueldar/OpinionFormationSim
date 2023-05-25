#pragma once

#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdlib>
#include <random>
#include <set>
#include <utility>
#include <array>
#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>

using namespace std;

// probability of creating long distance links is calculated using pow(10, p_link_creation)
typedef struct s_model_config {
	int n_rows = 10, n_cols = 10;
	float p_interaction = 0.1, p_link_creation = -3.0; 
    float interaction_max = 0.3, convergence_parameter = 0.1;
} ModelConfig;

class Model {
	private:
		set<pair<pair<int, int>, pair<int, int>>> connections;
		ModelConfig conf;
		vector<vector<double>> opinions;

	public:
		Model() {}

		int getCols();
		int getRows();
		void setOptions(ModelConfig conf);
		void generateOpinions();
		void generateConnections();
		void singleInteraction(pair<int, int> pair1, pair<int, int> pair2);
		void interact();
		vector<int> pictureArray();
		void updateImage(GLuint texture);
};