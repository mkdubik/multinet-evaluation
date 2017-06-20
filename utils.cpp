#include "common.h"

namespace mlnet_evaluation
{

double modul(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr c) {
	if ((*c).get_communities().size() == 0) {
		return -1;
	}
	return mlnet::modularity(mnet, c, 1);
}

double comm(mlnet::CommunityStructureSharedPtr c, mlnet::CommunityStructureSharedPtr truth) {
	if ((*c).get_communities().size() == 0) {
		return -1;
	}
	if ((*truth).get_communities().size() == 0) {
		return -1;
	}
	double c_sz = static_cast<double>((*c).get_communities().size());
	double truth_sz = static_cast<double>((*truth).get_communities().size());
	return c_sz / truth_sz;
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

mlnet::CommunityStructureSharedPtr read_truth(mlnet::MLNetworkSharedPtr mnet, std::string tpath) {

	std::fstream file(tpath + "_truth", std::ios_base::in);
	std::string actor;
	std::string community;

	mlnet::hash_map<std::string,std::set<mlnet::NodeSharedPtr> > result;

	for (std::string line; std::getline(file, line); )
	{
		std::istringstream ss(line);
		std::string token;

		std::getline(ss, token, ',');
		actor = token;

		std::getline(ss, token, ',');
		community = token;

		mlnet::ActorSharedPtr a = mnet->get_actor(actor);
		for (mlnet::LayerSharedPtr l: *mnet->get_layers()) {
			mlnet::NodeSharedPtr n = mnet->get_node(a,l);
			if (n) {
				result[community].insert(n);
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

void write2file(mlnet::CommunityStructureSharedPtr c, std::string path) {
	if ((*c).get_communities().size() == 0) {
		return;
	}

	std::ofstream ofs;
	ofs.open(path, std::ofstream::out | std::ofstream::trunc);
	(*c).print(ofs);
	ofs.close();
}


void lart(std::string rpath, mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth, int t, double eps, double gamma) {
	mlnet::lart l;

	mlnet_evaluation::stats s(rpath + mnet->name + "_LART", mnet->get_layers()->size());
	mlnet::CommunityStructureSharedPtr c = l.fit(mnet, t, eps, gamma);

	s.modul_result = mlnet_evaluation::modul(mnet, c);
	s.modul_gt = mlnet_evaluation::modul(mnet, truth);
	s.nmi = mlnet_evaluation::nmi(c, truth, mnet->get_nodes()->size());
	s.community_ratio = mlnet_evaluation::comm(c, truth);
	s.write();
}

void glouvain(std::string rpath, mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth, std::string move, double gamma, double omega, int limit) {
	mlnet::glouvain g;

	mlnet_evaluation::stats s(rpath + mnet->name + "_GLOUVAIN", mnet->get_layers()->size());
	mlnet::CommunityStructureSharedPtr c = g.fit(mnet, move, gamma, omega, limit);

	s.modul_result = mlnet_evaluation::modul(mnet, c);
	s.modul_gt = mlnet_evaluation::modul(mnet, truth);
	s.nmi = mlnet_evaluation::nmi(c, truth, mnet->get_nodes()->size());
	s.community_ratio = mlnet_evaluation::comm(c, truth);
	s.write();
}

void pmm(std::string rpath, mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth, int k, int ell) {
	mlnet::pmm p;

	mlnet_evaluation::stats s(rpath + mnet->name + "_PMM", mnet->get_layers()->size());
	mlnet::CommunityStructureSharedPtr c = p.fit(mnet, k, ell);

	s.modul_result = mlnet_evaluation::modul(mnet, c);
	s.modul_gt = mlnet_evaluation::modul(mnet, truth);
	s.nmi = mlnet_evaluation::nmi(c, truth, mnet->get_nodes()->size());
	s.community_ratio = mlnet_evaluation::comm(c, truth);
	s.write();
}

}