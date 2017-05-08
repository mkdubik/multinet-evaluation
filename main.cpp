#include <multinet.h>
#include <community/lart.h>
#include <community/glouvain.h>
#include <community/pmm.h>

const std::string resultp = "../multinet-evaluation/results/";

int main() {
	const std::string datap = "../multinet-evaluation/data/";
	const std::vector<std::string> datasets = {
		"dk",
		/*"aucs",
		"5k",
		"10k",
		"20k",
		"100k",
		"friendfeed_ita",*/
	};

	for (auto d: datasets) {
		mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer(datap + d, d,',');
		tLART(mnet);
		tGLOUVAIN(mnet);
		tPMM(mnet);
	}

	return 0;
}

void tLART(mlnet::MLNetworkSharedPtr mnet) {
	mlnet::lart l;
	std::string format = mnet->name + "/LART_";
	std::vector<int> t = {3, 6, 9, 12, 20};
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
	std::vector<std::string> method = {"move", "moverandw"};
	std::vector<double> gamma = {0.1, 0.5, 1.0};
	std::vector<double> omega = {0.1, 0.5, 1.0};

	for (size_t i = 0; i < method.size(); i++) {
		for (size_t j = 0; j < gamma.size(); j++) {
			for (size_t k = 0; k < omega.size(); k++) {
				std::string path = format +
									std::to_string(gamma[j]) + "_" +
									std::to_string(omega[k]) + "_" +
									method[i];
				write(g.fit(mnet, gamma[j], omega[k], method[i]));
			}
		}
	}
}

void tPMM(mlnet::MLNetworkSharedPtr mnet) {
	mlnet::pmm p;
	std::string format = mnet->name + "/PMM_";
	std::vector<int> k = {1, 5, 10, 15, 20};
	std::vector<int> ell = {5, 10, 15, 20};

	for (size_t i = 0; i < k.size(); i++) {
		for (size_t j = 0; j < ell.size(); j++) {
			std::string path = format +
								std::to_string(k[i]) + "_" +
								std::to_string(ell[j]);

			write(p.fit(mnet, k[i], ell[j], 1));
		}
	}
}

void write(CommunitySharedPtr c, std::string path) {
	// write here to blabla c.resultp + path
}

