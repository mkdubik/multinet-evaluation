#include "common.h"

#include <iostream>
#include <fstream>


int main() {
	std::string rpath = "results/";

	std::string command = "mkdir -p " + rpath;
	if(system(command.c_str())) {
		return 1;
	}

	std::string dpath = "data/";

	mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer(dpath + "dk", "dk", ',');
	mlnet::CommunityStructureSharedPtr truth = mlnet_evaluation::read_truth(mnet, dpath + "dk");

	mlnet_evaluation::lart(rpath, mnet, truth, 3 * 3, 1, 1);
	mlnet_evaluation::glouvain(rpath, mnet, truth, "move", 1, 1, 4000);
	mlnet_evaluation::pmm(rpath, mnet, truth, 40, 140);

	return 0;
}
