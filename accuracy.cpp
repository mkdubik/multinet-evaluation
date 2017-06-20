#include "common.h"

#include <iostream>
#include <fstream>


int main() {
	std::string rpath = "results/accuracy/";

	std::string command = "mkdir -p " + rpath;
	if(system(command.c_str())) {
		return 1;
	}

	std::string dpath = "data/accuracy/";

	std::vector<int> L = {
		3,
		5,
	};

	std::vector<std::string> N = {
		"1k",
		"4k",
	};

	std::vector<std::string> MU = {"03"};

	/*for (auto l : L) {
		for (auto n: N) {
			for (auto mu: MU) {
				std::string dset = n + "_" + mu;
				std::string dset_full = dpath + std::to_string(l) + "/" + dset;

				mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer(dset_full, dset, ',');
				mlnet::CommunityStructureSharedPtr truth = mlnet_evaluation::read_truth(mnet, dset_full);

				mlnet_evaluation::lart(rpath, mnet, truth, 3 * l, 1, 1);
				mlnet_evaluation::glouvain(rpath, mnet, truth, "move", 1, 1);
				mlnet_evaluation::pmm(rpath, mnet, truth, 40, 140);
			}
		}
	}*/


	std::string dset = "10k";
	mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer("data/" + dset, dset, ',');
	mlnet::CommunityStructureSharedPtr truth = mlnet_evaluation::read_truth(mnet, "data/" + dset);

	std::cout << "starting.." << std::endl;
	//mlnet_evaluation::glouvain(rpath, mnet, truth, "move", 1, 1);
	mlnet_evaluation::pmm(rpath, mnet, truth, 40, 140);

	return 0;
}
