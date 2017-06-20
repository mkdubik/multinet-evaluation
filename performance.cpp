#include "common.h"

#include <iostream>
#include <fstream>


int main() {
	std::string rpath = "results/performance/";

	std::string command = "mkdir -p " + rpath;
	if(system(command.c_str())) {
		return 1;
	}

	std::string dpath = "data/performance/";

	std::vector<int> L = {
		3,
		5,
	};

	std::vector<std::string> N = {
		"1k",
		"2k",
		"3k",
		"4k"
	};

	std::vector<std::string> MU = {"03"};

	/*for (auto l : L) {
		for (auto n: N) {
			for (auto mu: MU) {
				std::string dset = n + "_" + mu;
				std::string dset_full = dpath + std::to_string(l) + "/" + dset;

				mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer(dset_full, dset, ',');
				mlnet::CommunityStructureSharedPtr truth = mlnet_evaluation::read_truth(mnet, dset_full);

				if (l == 5) {
					l = 1;
				}
				mlnet_evaluation::lart(rpath, mnet, truth, 3 * l, 1, 1);
				mlnet_evaluation::glouvain(rpath, mnet, truth, "move", 1, 1, 4000);
				mlnet_evaluation::pmm(rpath, mnet, truth, 40, 140);
			}
		}
	}*/

	std::string dset = "8k";
	mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer("data/" + dset, dset, ',');
	mlnet::CommunityStructureSharedPtr truth = mlnet_evaluation::read_truth(mnet, "data/" + dset);

	mlnet_evaluation::glouvain(rpath, mnet, truth, "move", 1, 1, 4000);
	mlnet_evaluation::pmm(rpath, mnet, truth, 40, 140);

	std::string dset10k = "10k";
	mlnet::MLNetworkSharedPtr mnet10k = mlnet::read_multilayer("data/" + dset10k, dset10k, ',');
	mlnet::CommunityStructureSharedPtr truth10k = mlnet_evaluation::read_truth(mnet10k, "data/" + dset10k);
	mlnet_evaluation::glouvain(rpath, mnet10k, truth10k, "move", 1, 1, 0);
	return 0;
}
