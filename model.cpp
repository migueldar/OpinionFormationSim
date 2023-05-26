#include "model.hpp"

int Model::getCols() {
	return conf.n_cols;
}

int Model::getRows() {
	return conf.n_rows;
}

void Model::setOptions(ModelConfig config) {
	conf = config;
}

void Model::generateOpinions() {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<double> dis(0.0, 1.0);

	opinions.clear();
	opinions.resize(conf.n_rows, vector<double>(conf.n_cols));
	for (int i = 0; i < conf.n_rows; i++)
		for (int j = 0; j < conf.n_cols; j++)
			opinions[i][j] = dis(gen);
}

void Model::generateConnections() {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<double> dis(0.0, 1.0);
	int startj2;

	connections.clear();
	for (int i1 = 0; i1 < conf.n_rows; i1++) {
		for (int j1 = 0; j1 < conf.n_cols; j1++) {
			for (int i2 = i1; i2 < conf.n_rows; i2++) {
				startj2 = 0;
				if (i2 == i1)
					startj2 = j1 + 1;
				for (int j2 = startj2; j2 < conf.n_cols; j2++)
					if (dis(gen) <= pow(abs(i1 - i2) + abs(j1 - j2), -2))
						connections.insert({{i1, j1}, {i2, j2}});
			}
		}
	}
}

void Model::singleInteraction(pair<int, int> pair1, pair<int, int> pair2) {
	int i1 = pair1.first, j1 = pair1.second;
	int i2 = pair2.first, j2 = pair2.second;
	double aux;

	if (abs(opinions[i1][j1] - opinions[i2][j2]) <= conf.interaction_max) {
		aux = opinions[i1][j1];
		opinions[i1][j1] += conf.convergence_parameter * (opinions[i2][j2] - opinions[i1][j1]);
		opinions[i2][j2] += conf.convergence_parameter * (aux - opinions[i2][j2]);
	}
}

// generate interactions, randomize their order and execute them
void Model::interact() {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<double> dis(0.0, 1.0);
	vector<pair<pair<int, int>, pair<int, int>>> interactions;

	for (set<pair<pair<int, int>, pair<int, int>>>::iterator it = connections.begin(); it != connections.end(); it++)
		if (dis(gen) < conf.p_interaction)
			interactions.push_back(*it);

	shuffle(interactions.begin(), interactions.end(), gen);

	for (vector<pair<pair<int, int>, pair<int, int>>>::iterator it = interactions.begin(); it != interactions.end(); it++)
		singleInteraction(it->first, it->second);
}

vector<int> Model::pictureArray() {
	vector<int> ret(conf.n_cols * conf.n_rows * 25);
	int red_intensity, blue_intensity;

	for (int i = 0; i < conf.n_rows; i++) {
		for (int j = 0; j < conf.n_cols; j++) {
			red_intensity = opinions[i][j] * 0x100;
			if (red_intensity == 0x100)
				red_intensity = 0xFF;
			blue_intensity = 0xFF - red_intensity;
			for (int x = 0; x < 5; x++)
				for (int y = 0; y < 5; y++)
					ret[(5 * i + x) * 5 * conf.n_cols + 5 * j + y] = (0xFF << 24) + (blue_intensity << 16) + (0 << 8) + red_intensity;
		}
	}
	return ret;
}

void Model::updateImage(GLuint texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, conf.n_cols * 5, conf.n_rows * 5, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->pictureArray().data());
	glBindTexture(GL_TEXTURE_2D, 0);
}