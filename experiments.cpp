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

	double c_sz = static_cast<int>((*c).get_communities().size());
	double truth_sz = static_cast<int>((*c).get_communities().size());

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
		cid aid aid aid

		*/
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
			for (mlnet::NodeSharedPtr n : *mnet->get_nodes(((mnet->get_actors()->get_at_index(m - 1))))) {
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
		mlnet::ActorSharedPtr a = mnet->get_actor(std::to_string(actor));
		for (mlnet::LayerSharedPtr l: *mnet->get_layers()) {
			result[community].insert(mnet->get_node(a,l));
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

void tLART(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth, std::vector<double> gamma) {
	mlnet::lart l;
	std::string format = mnet->name + "/LART_";

	std::vector<int> t = {3, 6, 9};
	std::vector<double> eps = {0.5, 1.0};

	double modul_gt = modul(mnet, truth);

	std::cout << modul_gt << std::endl;

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
	std::vector<std::string> method = {"moverandw", "move"};
	std::vector<double> omega = {0.0, 0.5, 1.0};

	double modul_gt = modul(mnet, truth);

	for (size_t i = 0; i < method.size(); i++) {
		for (size_t j = 0; j < gamma.size(); j++) {
			for (size_t k = 0; k < omega.size(); k++) {
				stats s;
				s.path = format +
						std::to_string(gamma[j]) + "_" +
						std::to_string(omega[k]) + "_" +
						method[i];
				mlnet::CommunityStructureSharedPtr c = g.fit(mnet, method[i], gamma[j], omega[k]);

				s.modul_result = modul(mnet, c);
				s.modul_gt = modul_gt;
				s.nmi = nmi(c, truth, mnet->get_nodes()->size());
				s.community_ratio = comm(c, truth);
				s.write();
			}
		}
	}
}

void tPMM(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth, std::vector<double> gamma) {
	mlnet::pmm p;
	std::string format = mnet->name + "/PMM_";

	std::vector<int> k = {5, 10, 20, 40, 60, 100};


	double modul_gt = modul(mnet, truth);

	for (size_t i = 0; i < k.size(); i++) {////
		for (size_t j = 0; j < gamma.size(); j++) {
			stats s;
			s.path = format +
					std::to_string(k[i]) + "_" +
					std::to_string(k[i] * 2) + "_" +
					std::to_string(gamma[j]);
			mlnet::CommunityStructureSharedPtr c = p.fit(mnet, k[i], k[i] * 2, gamma[j]);

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
		std::make_tuple("1k_mix01", " ", true),
		/*std::make_tuple("1k_mix04", " ", true),
		std::make_tuple("3k_mix04", " ", true),
		std::make_tuple("5k_mix025", " ", true),
		std::make_tuple("3k_overlap_2250_5", " ", true),
		std::make_tuple("5k_overlap_3750_5", " ", true),
		std::make_tuple("dk", ",", true),*/
		//std::make_tuple("fftwyt", " ", false),
		//std::make_tuple("eu", " ", false),
		//std::make_tuple("aucs", ",", true),
	};
	std::string ix = "overlap";

	std::vector<double> gamma = {0.5, 1.0, 1.5, 2.0, 2.5};


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



		//tLART(mnet, truth, gamma);
		tGLOUVAIN(mnet, truth, gamma);
		//tPMM(mnet, truth, gamma);

	}
	return 0;
}
