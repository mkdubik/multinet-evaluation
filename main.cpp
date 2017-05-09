#include <iostream>
#include <fstream>

#include <multinet.h>
#include <community/lart.h>
#include <community/glouvain.h>
#include <community/pmm.h>

const std::string resultp = "../multinet-evaluation/results/";

void write(mlnet::CommunitiesSharedPtr c, std::string path) {
	std::ofstream ofs;
	ofs.open(resultp + path, std::ofstream::out | std::ofstream::trunc);
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
				write(l.fit(mnet, t[i], eps[j], gamma[k]), path);
			}
		}
	}
}

void tGLOUVAIN(mlnet::MLNetworkSharedPtr mnet) {
	mlnet::glouvain g;
	std::string format = mnet->name + "/GLOUVAIN_";
	std::vector<std::string> method = {"move"};
	std::vector<double> gamma = {0.1, 0.5, 1.0};
	std::vector<double> omega = {0.1, 0.5, 1.0};

	for (size_t i = 0; i < method.size(); i++) {
		for (size_t j = 0; j < gamma.size(); j++) {
			for (size_t k = 0; k < omega.size(); k++) {
				if (gamma[j] == 1.0 && omega[k] == 1.0){
					break;
				}
				std::string path = format +
									std::to_string(gamma[j]) + "_" +
									std::to_string(omega[k]) + "_" +
									method[i];
				write(g.fit(mnet, method[i], gamma[j], omega[k]), path);
			}
		}
	}
}

void tPMM(mlnet::MLNetworkSharedPtr mnet) {
	mlnet::pmm p;
	std::string format = mnet->name + "/PMM_";

	std::vector<int> k = {3, 5, 10, 20};

	for (size_t i = 0; i < k.size(); i++) {
		std::string path = format +
							std::to_string(k[i]) + "_" +
							std::to_string(k[i]);

		write(p.fit(mnet, k[i], k[i] + 2, 1), path);
	}
}

int main() {
	const std::string datap = "../multinet-evaluation/data/";
	const std::vector<std::string> datasets = {
		"dk",
		//"aucs",
		//"5k",
		"10k",
		//"20k",
		/*"100k",*/
		"friendfeed_ita",
	};


	for (auto d: datasets) {
		mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer(datap + d, d,',');
		tLART(mnet);
		tGLOUVAIN(mnet);
		tPMM(mnet);
	}

	return 0;
}
