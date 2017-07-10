#include "common.h"

#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << "usage: " << argv[0] << " aucs/dk" << std::endl;
		std::exit(1);
	}

	std::string test = argv[1];
	std::string dset_n;
	std::string dset_p;

	if (test == "t5") {
		dset_n = "aucs";
	} else if (test == "t6") {
		dset_n = "dk";
	} else {
		std::cout << test << " test not supported" << std::endl;
		std::exit(1);
	}
	dset_p = "data/" + test + "/"+ dset_n;

	mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer(dset_p, dset_n, ',');
	mlnet::CommunityStructureSharedPtr truth = mlnet_evaluation::read_truth(mnet, dset_p);

	mlnet_evaluation::stats s;

	std::vector<int> steps = {3, 6, 9, 12};
	std::vector<double> eps = {0.1, 0.5, 1.0, 1.5};
	std::vector<double> gamma = {0.25, 0.5, 1.0, 1.5, 2.0};


	for (auto t: steps) {
		for (auto e: eps) {
			for (auto g: gamma) {
				s = mlnet_evaluation::lart(mnet, truth, t, e, g);
				std::string write = "results/" + test + "/lart_" + std::to_string(t) + "_" + std::to_string(e) + "_" + std::to_string(g) + "_" + dset_n;
				s.write(write);
				s.write_community(write + "_community");
			}
		}
	}

	std::vector<double> omega = {0.1, 0.5, 1.0};
	for (auto g: gamma) {
		for (auto o: omega) {
			s = mlnet_evaluation::glouvain(mnet, truth, "move", g, o, 4000);
			std::string write = "results/" + test + "/glouvain_" +
				std::to_string(g) + "_" + std::to_string(o) + "_" + dset_n;
			s.write(write);
			s.write_community(write + "_community");
		}
	}

	std::vector<int> k = {3, 5, 10, 15, 20, 30, 40, 50};
	std::vector<int> ell = {100, 110, 120, 130, 140, 150, 160};

	for (auto k_: k) {
		for (auto l: ell) {
			s = mlnet_evaluation::pmm(mnet, truth, k_, l);
			std::string write = "results/" + test + "/pmm_" +
				std::to_string(k_) + "_" + std::to_string(l) + "_" + dset_n;
			s.write(write);
			s.write_community(write + "_community");
		}
	}



	return 0;
}
