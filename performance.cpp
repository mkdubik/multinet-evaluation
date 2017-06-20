#include "common.h"

#include <iostream>
#include <fstream>

const std::string result_path = "results/performance/";
const std::string data_path = "data/performance/";


mlnet::CommunityStructureSharedPtr read_truth(mlnet::MLNetworkSharedPtr mnet) {

	std::fstream file(data_path + mnet->name + "_truth", std::ios_base::in);
	std::string actor;
	int community;

	mlnet::hash_map<int, std::set<mlnet::NodeSharedPtr> > result;

	for (std::string line; std::getline(file, line); )
	{
		std::istringstream ss(line);
		std::string token;

		std::getline(ss, token, ',');
		actor = token;

		std::getline(ss, token, ',');
		community = std::stoi(token);

		mlnet::ActorSharedPtr a = mnet->get_actor(actor);
		for (mlnet::LayerSharedPtr l: *mnet->get_layers()) {
			mlnet::NodeSharedPtr n = mnet->get_node(a,l);
			if (n) {
				result[community].insert(n);
			} else {
				mlnet::NodeSharedPtr node_ptr(new mlnet::node(0, a, l));
				result[community].insert(node_ptr);
			}
		}
	}

	mlnet::CommunityStructureSharedPtr communities = mlnet::community_structure::create();

	for (auto pair: result) {
		mlnet::CommunitySharedPtr c = mlnet::community::create();
		for (mlnet::NodeSharedPtr node: pair.second) {
			c->add_node(node);
		}
		communities->add_community(c);
	}

	return communities;
}

void lart(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth) {
	mlnet::lart l;
	int t = 9;
	double eps = 1.0;
	double gamma = 1.0;

	mlnet_evaluation::stats s(result_path + mnet->name + "_LART");
	mlnet::CommunityStructureSharedPtr c = l.fit(mnet, t, eps, gamma);

	s.modul_result = mlnet_evaluation::modul(mnet, c);
	s.modul_gt = mlnet_evaluation::modul(mnet, truth);
	s.nmi = mlnet_evaluation::nmi(c, truth, mnet->get_nodes()->size());
	s.community_ratio = mlnet_evaluation::comm(c, truth);
	s.write();
}

void glouvain(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth) {
	mlnet::glouvain g;
	std::string method = "move";
	double gamma = 1.0;
	double omega = 1.0;

	mlnet_evaluation::stats s(result_path + mnet->name + "_GLOUVAIN");
	mlnet::CommunityStructureSharedPtr c = g.fit(mnet, method, gamma, omega, 4000);

	s.modul_result = mlnet_evaluation::modul(mnet, c);
	s.modul_gt = mlnet_evaluation::modul(mnet, truth);
	s.nmi = mlnet_evaluation::nmi(c, truth, mnet->get_nodes()->size());
	s.community_ratio = mlnet_evaluation::comm(c, truth);
	s.write();
}

void pmm(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth) {
	mlnet::pmm p;

	int k = 40;
	int ell = 160;

	mlnet_evaluation::stats s(result_path + mnet->name + "_PMM");
	mlnet::CommunityStructureSharedPtr c = p.fit(mnet, k, ell);

	s.modul_result = mlnet_evaluation::modul(mnet, c);
	s.modul_gt = mlnet_evaluation::modul(mnet, truth);
	s.nmi = mlnet_evaluation::nmi(c, truth, mnet->get_nodes()->size());
	s.community_ratio = mlnet_evaluation::comm(c, truth);
	s.write();
}

int main() {
	std::string command = "mkdir -p " + result_path;
	if(system(command.c_str())) {
		return 1;
	}

	std::vector<std::string> N = {
		"1k",
		//"2k", "3k", "4k"
	};
	std::vector<std::string> MU = {"03"};

	for (auto n: N) {
		for (auto mu: MU) {
			std::string dset = n + "_" + mu;
			mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer(data_path + dset, dset, ',');
			mlnet::CommunityStructureSharedPtr truth = read_truth(mnet);

			lart(mnet, truth);
			glouvain(mnet, truth);
			pmm(mnet, truth);
		}
	}


	/*std::string dset = "20k_02";
	mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer(data_path + dset, dset, ',');
	mlnet::CommunityStructureSharedPtr truth = read_truth(mnet);

	glouvain(mnet, truth);
	pmm(mnet, truth);*/

	return 0;
}
