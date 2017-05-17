#include <iostream>
#include <fstream>

#include <multinet.h>
#include <community/lart.h>
#include <community/glouvain.h>
#include <community/pmm.h>

std::string result_path;
std::string data_path;

void write(mlnet::CommunityStructureSharedPtr c, std::string path) {
	std::ofstream ofs;
	ofs.open(result_path + path, std::ofstream::out | std::ofstream::trunc);
	(*c).print(ofs);
	ofs.close();
}

void tLART(mlnet::MLNetworkSharedPtr mnet) {
	mlnet::lart l;
	std::string format = mnet->name + "/LART_";
	std::vector<int> t = {3, 6, 9};
	std::vector<double> eps = {0.1, 0.5, 1.0};
	std::vector<double> gamma = {0.25, 0.5, 1.0, 1.5, 2.0, 2.5};

	for (size_t i = 0; i < t.size(); i++) {
		for (size_t j = 0; j < eps.size(); j++) {
			for (size_t k = 0; k < gamma.size(); k++) {
				std::string path = format +
									std::to_string(t[i]) + "_" +
									std::to_string(eps[j]) + "_" +
									std::to_string(gamma[k]);

				std::cout << path << std::endl;

				write(l.fit(mnet, t[i], eps[j], gamma[k]), path);
			}
		}
	}
}

void tGLOUVAIN(mlnet::MLNetworkSharedPtr mnet) {
	std::vector<std::string> method = {"moverandw", "move"};
	std::vector<double> gamma = {0.1, 0.2, 0.5, 1.0};
	std::vector<double> omega = {0.1, 0.2, 0.5, 1.0};

	std::string format = mnet->name + "/GLOUVAIN_";

	for (size_t i = 0; i < method.size(); i++) {
		for (size_t j = 0; j < gamma.size(); j++) {
			for (size_t k = 0; k < omega.size(); k++) {
				mlnet::glouvain g;
				std::string path = format +
									std::to_string(gamma[j]) + "_" +
									std::to_string(omega[k]) + "_" +
									method[i];

				std::cout << path << std::endl;

				write(g.fit(mnet, method[i], gamma[j], omega[k]), path);

			}
		}
	}
}

void tPMM(mlnet::MLNetworkSharedPtr mnet) {
	mlnet::pmm p;
	std::string format = mnet->name + "/PMM_";

	std::vector<int> k = {3, 5, 10, 15};

	for (size_t i = 0; i < k.size(); i++) {
		std::string path = format +
							std::to_string(k[i]) + "_" +
							std::to_string(k[i]);

		std::cout << path << std::endl;

		write(p.fit(mnet, k[i], k[i] + 2), path);
	}
}

int main() {
	result_path = "../multinet-evaluation/results/";
	data_path = "../multinet-evaluation/data/";

	/* !!WARNING!! Calling rm -rf on wrong path can result in data loss!!! */
	/* Hardcoded for safety */
	if(system(("rm -rf ../multinet-evaluation/results/"))) {
		return 1;
	}
	if(system(("mkdir -p ../multinet-evaluation/results/"))) {
		return 1;
	}
	std::vector<std::pair<std::string, std::string>> datasets = {
		{"aucs", ","},
		{"dk", ","},
		{"monastery", ","},
		{"1k_mix025", " "},
		{"3k_mix025", " "},
		{"5k_mix025", " "},
		{"fftwyt", ","}
	};

	for (size_t i = 0; i < datasets.size(); i++) {
		if(system(("mkdir -p " + result_path + datasets[i].first).c_str())) {
			return 1;
		}
		mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer(data_path + datasets[i].first, datasets[i].first, datasets[i].second[0]);
		tLART(mnet);
		tGLOUVAIN(mnet);
		tPMM(mnet);

	}
	return 0;
}
