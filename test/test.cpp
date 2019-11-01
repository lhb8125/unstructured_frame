/**
* @file: test.cpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-10-09 11:04:42
* @last Modified by:   lenovo
* @last Modified time: 2019-10-23 15:36:52
*/
#include <iostream>
#include <fstream>
#include <string>
#include <yaml.h>
#include <assert.h>
#include <unistd.h>
#include "cstdlib"
#include "mpi.h"
#include "utilities.hpp"
#include "loadBalancer.hpp"
#include "cgnslib.h"
#define OUT std::cout
#define IN std::cin
#define ENDL std::endl
#define String std::string

// #define DEBUG_YAML

void loadRegionTopologyFromYAML(String filePtr, Array<Scalar> &s,
	ArrayArray<Label> &nei, Label procNum);
void operator >> (const YAML::Node& node, Array<Scalar>& s);
void operator >> (const YAML::Node& node, Array<Array<Label> >& nei);
void operator >> (const YAML::Node& node, Array<Label>& regionIdx);
void hdf5ToAdf(char* filePtr, char* desFilePtr);

int main(int argc, char** argv)
{
	LoadBalancer *lb = new LoadBalancer();
	OUT<<"hello world!"<<ENDL;

	/// initialize MPI environment
	printf("initialize MPI environment ......\n");
	int numproces, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numproces);
	printf("This is process %d, %d processes are launched\n", rank, numproces);

	/// construct mesh block topology
	Array<Scalar> s;
	ArrayArray<Label> nei;
	Label procNum = 4;
	loadRegionTopologyFromYAML("regionTopo.yaml",s, nei, procNum);

	// LoadBalancer *lb = new LoadBalancer(s, nei, procNum);
	lb->LoadBalancer_2(s, nei, procNum);
	/// evaluate the result of load balancer
	// ArrayArray<Label> procId = lb->getProcId();
	// OUT<<"Item: region (processes ID)"<<ENDL;
	// procId.display();
	// ArrayArray<Scalar> procLoad = lb->getProcLoad();
	// OUT<<"Item: region (measurement)"<<ENDL;
	// procLoad.display();
	// Scalar* procLoadSum = lb->getProcLoadSum();
	// OUT<<"Item: process (measurement)"<<ENDL;
	// for (int i = 0; i < procNum; ++i)
	// {
	// 	OUT<<"Item: "<<i<<" ("<<procLoadSum[i]<<")"<<ENDL;
	// }

	/// read CGNS file
	Array<Region> regs;

	/// only one region for test.
	Region reg;
	// reg.getMesh().readMesh("data/40W.cgns");
	// printf("writing HDF5 cgns file: ......\n");
	// reg.getMesh().writeMesh("data/hdf5.cgns");
	printf("reading HDF5 cgns file: ......\n");
	// reg.getMesh().initMesh("data/hdf5.cgns");
	// MPI_Barrier(MPI_COMM_WORLD);
	reg.getMesh().readMesh("data/yf17_hdf5.cgns");
	MPI_Barrier(MPI_COMM_WORLD);
	// MPI_Barrier(MPI_COMM_WORLD);

	regs.push_back(reg);

	/// load balance in region
	Label* parts = lb->LoadBalancer_3(regs);

	regs[0].getMesh().writeMesh("data/result.cgns", parts);

	MPI_Finalize();
	return 0;
}

void loadRegionTopologyFromYAML(String filePtr, Array<Scalar> &s,
	ArrayArray<Label> &nei, Label procNum)
{
	OUT<<"reading YAML file: "<<filePtr<<" ......"<<ENDL;
	std::ifstream fin(filePtr.c_str());
	YAML::Parser parse(fin);
	YAML::Node doc;
	parse.GetNextDocument(doc);
	const YAML::Node& measurement = doc["measurement"];
	for (int i = 0; i < measurement.size(); ++i)
	{
		measurement[i] >> s;
	}
	Array<Array<Label> > neiTmp;
	Array<Label> regIdxTmp;
	const YAML::Node& topology = doc["topology"];
	for (int i = 0; i < topology.size(); ++i)
	{
		topology[i] >> neiTmp;
		topology[i] >> regIdxTmp;
	}

	/// transform the vector<vector<int> > to ArrayArray
	nei.num = s.size();
	nei.startIdx = new Label[nei.num+1];
	nei.startIdx[0] = 0;
	for (int i = 0; i < nei.num; ++i)
	{
		nei.startIdx[i+1] = nei.startIdx[i]+neiTmp[regIdxTmp[i]].size();
	}
	nei.data = new Label[nei.startIdx[nei.num]];
	for (int i = 0; i < nei.num; ++i)
	{
		Label k = 0;
		for (int j = nei.startIdx[i]; j < nei.startIdx[i+1]; ++j)
		{
			nei.data[j] = neiTmp[regIdxTmp[i]][k];
			k++;
		}
	}
#ifdef DEBUG_YAML
	/// check
	for (int i = 0; i < nei.num; ++i)
	{
		for (int j = nei.startIdx[i]; j < nei.startIdx[i+1]; ++j)
		{
			OUT<<nei.data[j]<<", ";
		}
	}
	OUT<<ENDL;
#endif
}

void operator >> (const YAML::Node& node, Array<Scalar>& s)
{
	String mea;
	node["mea"] >> mea;
	s.push_back(std::atof(mea.c_str()));
#ifdef DEBUG_YAML
	for (int i = 0; i < s.size(); ++i)
	{
		OUT<<s[i]<<", ";
	}
	OUT<<ENDL;
#endif
}

void operator >> (const YAML::Node& node, Array<Array<Label> >& nei)
{
	// String neighbor;
	// node["neighbor"].as<string>() >> neighbor;
	Array<Label> neiTmp;
	int tmp;
	const YAML::Node& neighbor = node["neighbor"];
	for (int i = 0; i < neighbor.size(); ++i)
	{
		neighbor[i] >> tmp;
		neiTmp.push_back(tmp);
	}
	nei.push_back(neiTmp);

#ifdef DEBUG_YAML
	for (int i = 0; i < nei.size(); ++i)
	{
		OUT<<"(";
		for (int j = 0; j < nei[i].size(); ++j)
		{
			OUT<<nei[i][j]<<", ";
		}
		OUT<<")";
	}
	OUT<<ENDL;
#endif
}

void operator >> (const YAML::Node& node, Array<Label>& regionIdx)
{
	Label tmp;
	node["regionIdx"] >> tmp;
	regionIdx.push_back(tmp);

#ifdef DEBUG_YAML
	for (int i = 0; i < regionIdx.size(); ++i)
	{
		OUT<<regionIdx[i]<<", ";
	}
	OUT<<ENDL;
#endif
}

void hdf5ToAdf(char* filePtr, char* desFilePtr)
{
	int iFile;
	if(cg_open(filePtr, CG_MODE_MODIFY, &iFile))
		Terminate("readCGNSFile", cg_get_error());
	if(cg_save_as(iFile,  desFilePtr, CG_FILE_ADF2, 0))
		Terminate("transformToADF", cg_get_error());
}