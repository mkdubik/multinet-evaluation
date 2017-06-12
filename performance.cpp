#include <iostream>
#include <fstream>

#include <multinet.h>
#include <community.h>
#include <community/lart.h>
#include <community/glouvain.h>
#include <community/pmm.h>

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


int main() {
	result_path = "results/";
	data_path = "data/";

	//if(system(("mkdir -p results/"))) {
	//	return 1;
	//}

	std::vector<std::tuple<std::string, std::string>> datasets = {
		std::make_tuple("1k_mix01", " "),
		std::make_tuple("1k_mix025", " "),
		std::make_tuple("1k_mix04", " "),
		std::make_tuple("3k_mix01", " "),
		std::make_tuple("3k_mix025", " "),
		std::make_tuple("3k_mix04", " "),
		std::make_tuple("5k_mix01", " "),
		std::make_tuple("5k_mix025", " "),
		std::make_tuple("5k_mix04", " "),
	};

	for (size_t i = 0; i < datasets.size(); i++) {
		//if(system(("mkdir -p " + result_path + std::get<0>(datasets[i])).c_str())) {
		//	return 1;
		//}

		std::string dataset = std::get<0>(datasets[i]);
		mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer(data_path + dataset,
			std::get<0>(datasets[i]), std::get<1>(datasets[i])[0]);

		mlnet::CommunityStructureSharedPtr truth = read_truth(mnet);

		mlnet::lart l;
		std::string format = mnet->name + "/LART_";

		int t = 9;
		if (mnet->get_actors->size() > 3000) {
			t = 3;
		}

		double eps = 1.0;
		double gamma = 1.0;

		stats s;
		s.path = format +
					std::to_string(t) + "_" +
					std::to_string(eps) + "_" +
					std::to_string(gamma);
		mlnet::CommunityStructureSharedPtr c = l.fit(mnet, t[i], eps[j], gamma[k]);

		s.modul_result = modul(mnet, c);
		s.modul_gt = modul(mnet, truth);
		s.nmi = nmi(c, truth, mnet->get_nodes()->size());

		s.community_ratio = comm(c, truth);
		s.write();
	}

	return 0;
}