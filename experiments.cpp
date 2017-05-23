#include <iostream>
#include <fstream>

#include <multinet.h>
#include <community.h>
#include <community/lart.h>
#include <community/glouvain.h>
#include <community/pmm.h>

std::string result_path;
std::string data_path;

struct stats
{
	double modul = 0;
	double nmi = 0;
	std::string path;

	void write() {
		std::ofstream ofs;
		ofs.open(result_path + path, std::ofstream::out | std::ofstream::trunc);
		ofs << modul << "," << nmi << std::endl;
		ofs.close();
	}

};

void write(mlnet::CommunityStructureSharedPtr c, std::string path) {
	if ((*c).get_communities().size() == 0) {
		return;
	}

	std::ofstream ofs;
	ofs.open(result_path + path + "_community", std::ofstream::out | std::ofstream::trunc);
	(*c).print(ofs);
	ofs.close();
}

double modul(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr c) {
	return 0;
}

double nmi(mlnet::CommunityStructureSharedPtr c, mlnet::CommunityStructureSharedPtr truth, int n) {
	if ((*c).get_communities().size() == 0) {
		return -1;
	}
	if ((*truth).get_communities().size() == 0) {
		return -1;
	}
	return normalized_mutual_information(c, truth, n);
}

mlnet::CommunityStructureSharedPtr read_truth(mlnet::MLNetworkSharedPtr mnet) {
	std::string truth_path = "truth/";

	std::fstream myfile((truth_path + mnet->name), std::ios_base::in);
	int actor;
	int community;
	std::vector<unsigned int> communities;

	while (myfile >> actor) {
		myfile >> community;
		communities.push_back((unsigned int)community);
	}

	if (communities.size() == mnet->get_actors()->size()) {
		return mlnet::cutils::actors2communities(mnet, communities);
	}

	return mlnet::cutils::nodes2communities(mnet, communities);
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
				stats s;

				s.path = format +
							std::to_string(t[i]) + "_" +
							std::to_string(eps[j]) + "_" +
							std::to_string(gamma[k]);
				mlnet::CommunityStructureSharedPtr c = l.fit(mnet, t[i], eps[j], gamma[k]);

				s.modul = modul(mnet, c);
				s.nmi = nmi(c, truth, mnet->get_nodes()->size());
				s.write();
				write(c, s.path);
			}
		}
	}
}

void tGLOUVAIN(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth) {
	mlnet::glouvain g;
	std::string format = mnet->name + "/GLOUVAIN_";
	std::vector<std::string> method = {"moverandw", "move"};
	std::vector<double> gamma = {0.25, 0.5, 1.0, 1.5, 2.0, 2.5};
	std::vector<double> omega = {0.0, 0.5, 1.0};

	for (size_t i = 0; i < method.size(); i++) {
		for (size_t j = 0; j < gamma.size(); j++) {
			for (size_t k = 0; k < omega.size(); k++) {
				stats s;
				s.path = format +
							std::to_string(gamma[j]) + "_" +
							std::to_string(omega[k]) + "_" +
							method[i];
				mlnet::CommunityStructureSharedPtr c = g.fit(mnet, method[i], gamma[j], omega[k]);
				s.modul = modul(mnet, c);
				s.write();
				write(c, s.path);
			}
		}
	}
}

void tPMM(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth) {
	mlnet::pmm p;
	std::string format = mnet->name + "/PMM_";

	std::vector<int> k = {5, 10, 20, 40, 60};

	for (size_t i = 0; i < k.size(); i++) {
		std::vector<int> ell = {1, k[i] / 2, k[i], k[i] + 2, k[i] * 2};

		for (size_t j = 0; i < ell.size(); j++) {
			if (j > 4 && mnet->name == "aucs") {
				break;
			}

			stats s;
			s.path = format +
						std::to_string(k[i]) + "_" +
						std::to_string(ell[j]);
			mlnet::CommunityStructureSharedPtr c1 = p.fit(mnet, k[i], 1);
			s.modul = modul(mnet, c1);
			s.nmi = nmi(c1, truth, mnet->get_nodes()->size());
			s.write();

			write(c1, s.path);
		}
	}
}

int main() {
	result_path = "results/";
	data_path = "data/";

	if(system(("mkdir -p results/"))) {
		return 1;
	}

	std::vector<std::tuple<std::string, std::string, bool>> datasets = {
		std::make_tuple("aucs", ",", true),
		std::make_tuple("dk", ",", true),
		std::make_tuple("1k_mix025", " ", false),
		std::make_tuple("1k_mix04", " ", true),
		std::make_tuple("3k_mix025", " ", true),
		std::make_tuple("3k_mix04", " ", true),
		std::make_tuple("5k_mix025", " ", true),
		//std::make_tuple("fftwyt", " ", false),
		//std::make_tuple("eu", " ", false),
	};

	for (size_t i = 0; i < datasets.size(); i++) {
		if(system(("mkdir -p " + result_path + std::get<0>(datasets[i])).c_str())) {
			return 1;
		}

		mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer(data_path + std::get<0>(datasets[i]),
			std::get<0>(datasets[i]), std::get<1>(datasets[i])[0]);

		mlnet::CommunityStructureSharedPtr truth;
		if (std::get<2>(datasets[i])) {
			truth = read_truth(mnet);
		} else {
			truth = mlnet::community_structure::create();
		}

		tLART(mnet, truth);
		tGLOUVAIN(mnet, truth);
		tPMM(mnet, truth);

	}
	return 0;
}
