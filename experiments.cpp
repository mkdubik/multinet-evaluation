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
	double modul_result = 0;
	double modul_gt = 0;
	double nmi = 0;
	double community_ratio = 0.0;
	std::string path;

	void write() {
		std::ofstream ofs;
		ofs.open(result_path + path, std::ofstream::out | std::ofstream::trunc);
		ofs << modul_result << "," << modul_gt << "," << nmi << "," << community_ratio << std::endl;
		ofs.close();
	}

};


void write2file(mlnet::CommunityStructureSharedPtr c, std::string path) {
	std::cout << "writing " <<  c->get_communities().size()<< std::endl;
	if ((*c).get_communities().size() == 0) {
		return;
	}


	std::ofstream ofs;
	ofs.open("dk_community", std::ofstream::out | std::ofstream::trunc);
	(*c).print(ofs);
	ofs.close();
}

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


mlnet::CommunityStructureSharedPtr read_truth_overlap(mlnet::MLNetworkSharedPtr mnet) {
	std::string truth_path = "truth/";

	std::fstream file((truth_path + mnet->name), std::ios_base::in);
	std::map<unsigned int, std::vector<unsigned int>> mapped;

	std::string str;
	while(getline(file, str))
	{
		std::istringstream ss(str);
		unsigned int num;

		int actor_id = 0;
		/*
		map from:
		aid cid cid cid
		to:
		cid aid aid aid */

		while(ss >> num) {
			if (actor_id == 0){
				actor_id = num;
			} else {
				mapped[num].push_back(actor_id);
			}
		}

	}

	mlnet::CommunityStructureSharedPtr communities = mlnet::community_structure::create();
	for(const auto &p : mapped) {
		mlnet::CommunitySharedPtr c = mlnet::community::create();

		for (auto m: p.second) {
			for (mlnet::NodeSharedPtr n : *mnet->get_nodes(((mnet->get_actor(std::to_string(m)))))) {
				(*c).add_node(n);
			}
		}
		(*communities).add_community(c);
	}

	return communities;
}

mlnet::CommunityStructureSharedPtr read_truth(mlnet::MLNetworkSharedPtr mnet) {
	std::string truth_path = "truth/";

	std::fstream myfile((truth_path + mnet->name), std::ios_base::in);
	int actor;
	int community;

	mlnet::hash_map<long,std::set<mlnet::NodeSharedPtr> > result;

	while (myfile >> actor) {
		myfile >> community;

		mlnet::ActorSharedPtr a;
		if(mnet->name == "aucs" || mnet->name == "dk") {
			a = mnet->get_actors()->get_at_index(actor);
		} else {
			a = mnet->get_actor(std::to_string(actor));
		}

		for (mlnet::LayerSharedPtr l: *mnet->get_layers()) {
			mlnet::NodeSharedPtr n = mnet->get_node(a,l);
			if (n){
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


void tLART(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth) {
	mlnet::lart l;
	std::string format = mnet->name + "/LART_";

	std::vector<int> t = {15};
	std::vector<double> eps = {1.0};


	std::vector<double> gamma = {0.5, 0.75, 1.0, 1.25, 1.5};


	double modul_gt = modul(mnet, truth);

	for (size_t i = 0; i < t.size(); i++) {
		for (size_t j = 0; j < eps.size(); j++) {
			for (size_t k = 0; k < gamma.size(); k++) {
				stats s;

				s.path = format +
							std::to_string(t[i]) + "_" +
							std::to_string(eps[j]) + "_" +
							std::to_string(gamma[k]);
				mlnet::CommunityStructureSharedPtr c = l.fit(mnet, t[i], eps[j], gamma[k]);

				s.modul_result = modul(mnet, c);
				s.modul_gt = modul_gt;
				s.nmi = nmi(c, truth, mnet->get_nodes()->size());

				s.community_ratio = comm(c, truth);
				s.write();

			}
		}
	}
}

void tGLOUVAIN(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth, std::vector<double> gamma) {
	mlnet::glouvain g;
	std::string format = mnet->name + "/GLOUVAIN_";
	std::vector<std::string> method = {"move"};
	std::vector<double> omega = {1.0};


	double modul_gt = modul(mnet, truth);

	for (size_t i = 0; i < method.size(); i++) {
		for (size_t j = 0; j < gamma.size(); j++) {
			for (size_t k = 0; k < omega.size(); k++) {
				stats s;
				s.path = format +
						std::to_string(gamma[j]) + "_" +
						std::to_string(omega[k]) + "_" +
						method[i];
				mlnet::CommunityStructureSharedPtr c = g.fit(mnet, method[i], gamma[j], omega[k], 4000);

				s.modul_result = modul(mnet, c);
				s.modul_gt = modul_gt;
				s.nmi = nmi(c, truth, mnet->get_nodes()->size());
				s.community_ratio = comm(c, truth);
				s.write();
			}
		}
	}
}

void tPMM(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth) {
	mlnet::pmm p;
	std::string format = mnet->name + "/PMM_";

	std::vector<int> k = {20, 30};


	double modul_gt = modul(mnet, truth);

	for (size_t i = 0; i < k.size(); i++) {
		std::vector<int> ell = {k[i], k[i] * 2, k[i] * 3};
		for (size_t l = 0; l < ell.size(); l++) {
			stats s;
			s.path = format +
					std::to_string(k[i]) + "_" +
					std::to_string(ell[l]);
			mlnet::CommunityStructureSharedPtr c = p.fit(mnet, k[i], ell[l]);

			s.modul_result = modul(mnet, c);
			s.modul_gt = modul_gt;

			s.nmi = nmi(c, truth, mnet->get_nodes()->size());
			s.community_ratio = comm(c, truth);
			s.write();
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
		//std::make_tuple("aucs", ",", true),
		//std::make_tuple("dk", ",", true),
		std::make_tuple("1k_mix01", " ", true),
		std::make_tuple("1k_mix025", " ", true),
		std::make_tuple("1k_mix04", " ", true),
		std::make_tuple("3k_mix01", " ", true),
		std::make_tuple("3k_mix025", " ", true),
		std::make_tuple("3k_mix04", " ", true),
		std::make_tuple("5k_mix01", " ", true),
		std::make_tuple("5k_mix025", " ", true),
		std::make_tuple("5k_mix04", " ", true),
		//std::make_tuple("1000_10coms_overlap500_5", " ", true),
		//std::make_tuple("1000_10coms_overlap800_5", " ", true),
		//std::make_tuple("eu", " ", false),
	};
	std::string ix = "overlap";


	std::vector<double> gamma = {1.0};


	for (size_t i = 0; i < datasets.size(); i++) {
		if(system(("mkdir -p " + result_path + std::get<0>(datasets[i])).c_str())) {
			return 1;
		}

		std::string dataset = std::get<0>(datasets[i]);
		mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer(data_path + dataset,
			std::get<0>(datasets[i]), std::get<1>(datasets[i])[0]);

		mlnet::CommunityStructureSharedPtr truth;
		if (std::get<2>(datasets[i])) {
			if (dataset.find(ix) != std::string::npos) {
				truth = read_truth_overlap(mnet);
			} else {
				truth = read_truth(mnet);
			}
		} else {
			truth = mlnet::community_structure::create();
		}



		//tLART(mnet, truth);
		tGLOUVAIN(mnet, truth, gamma);
		tPMM(mnet, truth);

	}
	return 0;
}
