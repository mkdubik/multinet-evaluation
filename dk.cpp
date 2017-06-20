#include "common.h"

const std::string result_path = "results/";

/*void lart(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth) {
	mlnet::lart l;
	std::string format = mnet->name + "/LART_";

	int t = 9;
	if (mnet->get_actors()->size() >= 5000) {
		t = 3;
	}

	double eps = 1.0;
	double gamma = 1.0;

	mlnet_evaluation::stats s(
		result_path + format + std::to_string(t) + "_" + std::to_string(eps) + "_" + std::to_string(gamma));
	mlnet::CommunityStructureSharedPtr c = l.fit(mnet, t, eps, gamma);

	s.modul_result = mlnet_evaluation::modul(mnet, c);
	s.modul_gt = mlnet_evaluation::modul(mnet, truth);
	s.nmi = mlnet_evaluation::nmi(c, truth,mnet->get_nodes()->size());
	s.community_ratio = mlnet_evaluation::comm(c, truth);
	mlnet_evaluation::write2file(c, result_path + "dk/lart_community");
	s.write();
}

void glouvain(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth) {
	mlnet::glouvain g;

	std::string format = mnet->name + "/GLOUVAIN_";
	std::string method = "move";

	double gamma = 1.0;
	double omega = 1.0;

	mlnet_evaluation::stats s(
		result_path + format + std::to_string(gamma) + "_" + std::to_string(omega) + "_" + method);

	mlnet::CommunityStructureSharedPtr c = g.fit(mnet, method, gamma, omega, 4000);

	s.modul_result = mlnet_evaluation::modul(mnet, c);
	s.modul_gt = mlnet_evaluation::modul(mnet, truth);
	s.nmi = mlnet_evaluation::nmi(c, truth, mnet->get_nodes()->size());
	s.community_ratio = mlnet_evaluation::comm(c, truth);

	mlnet_evaluation::write2file(c, result_path + "dk/glouvain_community");
	s.write();
}

void pmm(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth) {
	mlnet::pmm p;
	std::string format = mnet->name + "/PMM_";

	int k = 40;
	int ell = 100;

	mlnet_evaluation::stats s(
		result_path + format + std::to_string(k) + "_" + std::to_string(ell));
	mlnet::CommunityStructureSharedPtr c = p.fit(mnet, k, ell);

	s.modul_result = mlnet_evaluation::modul(mnet, c);
	s.modul_gt = mlnet_evaluation::modul(mnet, truth);


	size_t t = 0;
	for (size_t i = 0; i < c->get_communities().size(); i++) {
		t += c->get_community(i)->get_nodes().size();
	}

	s.nmi = mlnet_evaluation::nmi(c, truth,
		mnet->get_actors()->size() * mnet->get_layers()->size());
	s.community_ratio = mlnet_evaluation::comm(c, truth);
	s.write();
	mlnet_evaluation::write2file(c, result_path + "dk/pmm_community");
}


int main(){
	std::string data_path = "data/";

	std::string command = "mkdir -p " + result_path;
	if(system(command.c_str())) {
		return 1;
	}

	if(system(("mkdir -p "+ result_path + "dk").c_str())) {
		return 1;
	}

	std::string dataset = "dk";
	mlnet::MLNetworkSharedPtr mnet = mlnet::read_multilayer(
		data_path + dataset, dataset, ',');

	mlnet::CommunityStructureSharedPtr truth = mlnet_evaluation::read_truth(mnet);

	lart(mnet, truth);
	glouvain(mnet, truth);
	pmm(mnet, truth);

}*/

int main(){}