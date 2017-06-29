#include <multinet.h>
#include <community.h>
#include <community/lart.h>
#include <community/glouvain.h>
#include <community/pmm.h>

namespace mlnet_evaluation
{

struct stats
{
	double modul_result = 0;
	double modul_gt = 0;
	double nmi = 0;
	double community_ratio = 0.0;

	mlnet::CommunityStructureSharedPtr community_result;

	void write(std::string path) {
		std::ofstream ofs;
		ofs.open(path, std::ofstream::out | std::ofstream::trunc);
		ofs << modul_result << "," << modul_gt << "," << nmi << "," << community_ratio << std::endl;
		ofs.close();
	}

	void write_community(std::string path) {
		if (this->community_result->get_communities().size() == 0) {
			return;
		}

		std::ofstream ofs;
		ofs.open(path, std::ofstream::out | std::ofstream::trunc);
		community_result->print(ofs);
		ofs.close();
	}
};

double modul(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr c);
double comm(mlnet::CommunityStructureSharedPtr c, mlnet::CommunityStructureSharedPtr truth);
double nmi(mlnet::CommunityStructureSharedPtr c, mlnet::CommunityStructureSharedPtr truth, int n);

mlnet::CommunityStructureSharedPtr read_truth(mlnet::MLNetworkSharedPtr mnet, std::string data_path);

stats lart(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth, int t, double eps, double gamma);
stats glouvain(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth, std::string move, double gamma, double omega, int limit);
stats pmm(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth, int k, int ell);



}


