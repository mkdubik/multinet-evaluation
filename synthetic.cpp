#include "common.h"

#include <iostream>
#include <fstream>
#include <sstream>

template <typename T>
T parse_arg(char *argv) {
	std::istringstream ss(argv);
	T x;
	if (!(ss >> x)) {
		std::cerr << "Invalid conversion " << argv[1] << '\n';
	}

	return x;
}

int main(int argc, char *argv[]) {
	if (argc < 6) {
		std::cout << "usage: " << argv[0] << "<t1/t2/t3/t4> lart <N> <L> <mu> <t> <eps> <gamma>" << std::endl;
		std::cout << "usage: " << argv[0] << "<t1/t2/t3/t4> glouvain <N> <L> <mu> <gamma> <omega> <limit>" << std::endl;
		std::cout << "usage: " << argv[0] << "<t1/t2/t3/t4> pmm <N> <L> <mu> <k> <ell>" << std::endl;
		std::exit(1);
	}

	std::string test = argv[1];
	std::string method = argv[2];
	int N = parse_arg<int>(argv[3]);
	int L = parse_arg<int>(argv[4]);
	std::string mu = argv[5];

	std::string dset_n = std::to_string(N) + "_" + std::to_string(L) + "_" + mu;
	std::string dset_p = "data/" + test + "/"+ dset_n;

	mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer(dset_p, dset_n, ',');
	mlnet::CommunityStructureSharedPtr truth = mlnet_evaluation::read_truth(mnet, dset_p);

	mlnet_evaluation::stats s;
	if (method == "lart") {
		if (argc < 9) {
			std::cout << "usage: " << argv[0] << "<t1/t2/t3/t4> lart <N> <L> <mu> <t> <eps> <gamma>" << std::endl;
			std::exit(1);
		}
		int t = parse_arg<int>(argv[6]);
		double eps = parse_arg<double>(argv[7]);
		double gamma = parse_arg<double>(argv[8]);

		s = mlnet_evaluation::lart(mnet, truth, t, eps, gamma);

	} else if (method == "glouvain") {
		if (argc < 9) {
			std::cout << "usage: " << argv[0] << "<t1/t2/t3/t4> glouvain <N> <L> <mu> <gamma> <omega> <limit>" << std::endl;
			std::exit(1);
		}
		double gamma = parse_arg<double>(argv[6]);
		double omega = parse_arg<double>(argv[7]);
		int limit = parse_arg<int>(argv[8]);

		s = mlnet_evaluation::glouvain(mnet, truth, "move", gamma, omega, limit);

	} else if (method == "pmm") {
		if (argc < 8) {
			std::cout << "usage: " << argv[0] << "<t1/t2/t3/t4> pmm <N> <L> <mu> <k> <ell>" << std::endl;
			std::exit(1);
		}
		int k = parse_arg<int>(argv[6]);
		int ell = parse_arg<int>(argv[7]);

		s = mlnet_evaluation::pmm(mnet, truth, k, ell);

	} else {
		std::cout << "invalid method " << argv[2] << std::endl;
		std::exit(1);
	}

	s.write("results/"+ test + "/" + method + "_" + dset_n);
	return 0;
}
