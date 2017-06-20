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

	std::string result_path;
	stats(std::string result_path, int layers) {
		this->result_path = result_path + "_" + std::to_string(layers);
	}

	void write() {
		std::ofstream ofs;
		ofs.open(result_path, std::ofstream::out | std::ofstream::trunc);
		ofs << modul_result << "," << modul_gt << "," << nmi << "," << community_ratio << std::endl;
		ofs.close();
	}

};

double modul(mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr c);
double comm(mlnet::CommunityStructureSharedPtr c, mlnet::CommunityStructureSharedPtr truth);
double nmi(mlnet::CommunityStructureSharedPtr c, mlnet::CommunityStructureSharedPtr truth, int n);
void write2file(mlnet::CommunityStructureSharedPtr c, std::string path);

mlnet::CommunityStructureSharedPtr read_truth(mlnet::MLNetworkSharedPtr mnet, std::string data_path);

void lart(std::string rpath, mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth, int t, double eps, double gamma);
void glouvain(std::string rpath, mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth, std::string move, double gamma, double omega, int limit);
void pmm(std::string rpath, mlnet::MLNetworkSharedPtr mnet, mlnet::CommunityStructureSharedPtr truth, int k, int ell);



}


