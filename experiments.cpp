#include <iostream>
#include <fstream>

#include <multinet.h>
#include <community.h>
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

mlnet::CommunityStructureSharedPtr read_truth(mlnet::MLNetworkSharedPtr mnet) {
	std::string truth_path = "truth/";

	std::ifstream infile((truth_path + mnet->name).c_str());
	int a, b;
	std::vector<unsigned int> v;
	while (infile >> a >> b) {
		v.push_back(b);
	}

	if (v.size() == mnet->get_actors()->size()) {
		std::cout << "ground truth" << std::endl;
		return mlnet::cutils::actors2communities(mnet, v);
	}

	return mlnet::cutils::nodes2communities(mnet, v);
}

void tLART(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth) {
	mlnet::lart l;
	std::string format = mnet->name + "/LART_";
	std::vector<int> t = {2, 3, 6, 9};
	std::vector<double> eps = {0.1, 0.5, 1.0};
	std::vector<double> gamma = {0.25, 0.5, 1.0, 1.5, 2.0, 2.5};

	for (size_t i = 0; i < t.size(); i++) {
		for (size_t j = 0; j < eps.size(); j++) {
			for (size_t k = 0; k < gamma.size(); k++) {
				std::string path = format +
									std::to_string(t[i]) + "_" +
									std::to_string(eps[j]) + "_" +
									std::to_string(gamma[k]);
				mlnet::CommunityStructureSharedPtr c = l.fit(mnet, t[i], eps[j], gamma[k]);
				std::vector<mlnet::CommunitySharedPtr> cptr = (*c).get_communities();
				std::vector<mlnet::CommunitySharedPtr> truth_cptr = (*truth).get_communities();

				for (auto m : cptr) {
					for(auto o: truth_cptr)  {
						std::cout << community_jaccard(m, o) << " ";
					}
					std::cout << std::endl;
				}

			}
		}
	}
}

void tGLOUVAIN(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth) {
	std::vector<std::string> method = {"moverandw", "move"};
	std::vector<double> gamma = {0.25, 0.5, 1.0, 1.5, 2.0, 2.5};
	std::vector<double> omega = {0.0, 0.5, 1.0};

	std::string format = mnet->name + "/GLOUVAIN_";
	int size = mnet->get_nodes()->size();

	for (size_t i = 0; i < method.size(); i++) {
		for (size_t j = 0; j < gamma.size(); j++) {
			for (size_t k = 0; k < omega.size(); k++) {
				mlnet::glouvain g;
				std::string path = format +
									std::to_string(gamma[j]) + "_" +
									std::to_string(omega[k]) + "_" +
									method[i];
				mlnet::CommunityStructureSharedPtr c = g.fit(mnet, method[i], gamma[j], omega[k]);
				std::vector<mlnet::CommunitySharedPtr> cptr = (*c).get_communities();
				std::vector<mlnet::CommunitySharedPtr> truth_cptr = (*truth).get_communities();
				//std::cout << "NMI " << path << " " << normalized_mutual_information(c, truth, 1000) << std::endl;

			}
		}
	}
}

void tPMM(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth) {
	mlnet::pmm p;
	std::string format = mnet->name + "/PMM_";

	std::vector<int> k = {5, 10, 20, 40, 60};

	for (size_t i = 0; i < k.size(); i++) {
		std::string path1 = format +
							std::to_string(k[i]) + "_" +
							std::to_string(1);
		std::string path2 = format +
							std::to_string(k[i]) + "_" +
							std::to_string(k[i]);
		std::string path3 = format +
							std::to_string(k[i]) + "_" +
							std::to_string(k[i] + 2);
		std::string path4 = format +
							std::to_string(k[i]) + "_" +
							std::to_string(k[i] * 2);

		mlnet::CommunityStructureSharedPtr c1 = p.fit(mnet, k[i], 1);
		mlnet::CommunityStructureSharedPtr c2 = p.fit(mnet, k[i], k[i]);
		mlnet::CommunityStructureSharedPtr c3 = p.fit(mnet, k[i], k[i] + 2);
		mlnet::CommunityStructureSharedPtr c4 = p.fit(mnet, k[i], k[i] * 2);
	}
}

int main() {
	result_path = "results/";
	data_path = "data/";

	if(system(("mkdir -p results/"))) {
		return 1;
	}

	std::vector<std::pair<std::string, std::string>> datasets = {
		//{"aucs", ","},
		//{"monastery", ","},
		{"dk", ","},
		//{"1k_mix025", " "},
		//{"1k_mix04", " "},
		//{"3k_mix025", " "},
		//{"3k_mix04", " "},
		//{"5k_mix025", " "},
		//{"fftwyt", ","}
	};



	for (size_t i = 0; i < datasets.size(); i++) {
		if(system(("mkdir -p " + result_path + datasets[i].first).c_str())) {
			return 1;
		}

		mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer(data_path + datasets[i].first, datasets[i].first, datasets[i].second[0]);
		mlnet::CommunityStructureSharedPtr truth = read_truth(mnet);
		//tLART(mnet, truth);
		tGLOUVAIN(mnet, truth);
		//tPMM(mnet, truth);

	}
	return 0;
}
