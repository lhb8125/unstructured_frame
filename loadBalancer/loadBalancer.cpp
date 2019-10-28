/**
* @file: loadBalancer.cpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-09-23 15:26:27
* @last Modified by:   lenovo
* @last Modified time: 2019-10-14 08:58:35
*/
#include <cstdio>
#include "loadBalancer.hpp"
/**
* @brief find the process with the least measurement load
* @param procId Id of processes for all regions
*/
Label LoadBalancer::findMinProc()
{
	for (int i = 0; i < this->procNum_; ++i)
	{
		procLoadSum_[i] = 0.0;
	}
	Label regionNum = procId_.size();
	for (int i = 0; i < regionNum; ++i)
	{
		for (int j = procId_.startIdx[i]; j < procId_.startIdx[i+1]; ++j)
		{
			if(procId_.data[j]<0) continue;
			procLoadSum_[procId_.data[j]]+=procLoad_.data[j];
		}
	}

	// for (int i = 0; i < this->procNum_; ++i)
	// {
	// 	printf("%f\n", procLoadSum_[i]);
	// }

	Label minProcIdx = -1;
	Scalar minProcLoad = 0x7fffffff;
	for (int i = 0; i < this->procNum_; ++i)
	{
		if(procLoadSum_[i] < minProcLoad)
		{
			minProcIdx = i;
			minProcLoad = procLoadSum_[i];
		}
	}
	// delete[] procLoad;
	return minProcIdx;
}
/*
* @brief find the region with the maximum measurement load
* @param s the measurement load of all regions
*/
Label LoadBalancer::findMaxRegion(const Array<Scalar> s)
{
	Label regionNum = s.size();
	Label maxRegionIdx = -1;
	Label maxRegionLoad = 0;
	for (int i = 0; i < regionNum; ++i)
	{
		if(s[i] > maxRegionLoad)
		{
			maxRegionIdx = i;
			maxRegionLoad = s[i];
		}
	}
	return maxRegionIdx;
}
/**
* @brief default constructor
*/
LoadBalancer::LoadBalancer(){};
/**
* @brief deconstructor
*/
LoadBalancer::~LoadBalancer(){};
/**
* @brief Second Level load balance
* @param s measurement for all regions
* @param nei connectivity among all regions
* @param procNum count of processes
*/
void LoadBalancer::LoadBalancer_2(const Array<Scalar> s, const ArrayArray<Label> nei,
	Label procNum)
{
	this->s_   = s;
	this->nei_ = nei;
	this->procNum_ = procNum;
	procLoadSum_ = new Scalar[this->procNum_];

	Label regionNum = this->s_.size();
	/// ID of processes in each region
	Label **procId = new Label *[regionNum];
	/// proportion of ID of processes in each region
	Scalar **procLoad = new Scalar *[regionNum];
	/// count of processes in each region
	Label *procNumReg = new Label[regionNum];
	Scalar S = 0;
	/// sum up the measurements of all regions
	for (int i = 0; i < regionNum; ++i)
	{
		S += this->s_[i];
	}
	// printf("Total measurement: %f\n", S);
	/// average measurement load for each process
	Scalar phi = Scalar(S/procNum);
	Label procIdx = 0;
	/// Select the private processes for each region
	for (int i = 0; i < regionNum; ++i)
	{
		/// total count of processes
		procNumReg[i]   = ceil(this->s_[i]/phi);
		/// calculate the count of private processes
		Label procNumLower = floor(this->s_[i]/phi);
		procId[i]   = new Label[procNumReg[i]];
		procLoad[i] = new Scalar[procNumReg[i]];
		// printf("%d, %d\n", i, procNumLower);
		for (int j = 0; j < procNumReg[i]; ++j) { procId[i][j]=-1; }
		for (int j = 0; j < procNumLower; ++j)
		{
			procId[i][j]   = procIdx++;
			procLoad[i][j] = phi;
		}
		/// remove the load charged for provate processes
		this->s_[i] = this->s_[i]-phi*procNumLower;
	}

	/// construct the processes ID with ArrayArray type
	this->procId_.num = regionNum;
	this->procId_.startIdx = new Label[regionNum+1];
	this->procLoad_.num = regionNum;
	this->procLoad_.startIdx = new Label[regionNum+1];
	int totalCount = 0;
	this->procId_.startIdx[0] = 0;
	this->procLoad_.startIdx[0] = 0;
	for (int i = 0; i < regionNum; ++i)
	{
		this->procId_.startIdx[i+1]
			= this->procId_.startIdx[i] + procNumReg[i];
		this->procLoad_.startIdx[i+1]
			= this->procLoad_.startIdx[i] + procNumReg[i];
		totalCount += procNumReg[i];
	}
	this->procId_.data = new Label[totalCount];
	this->procLoad_.data = new Scalar[totalCount];
	int k = 0;
	for (int i = 0; i < regionNum; ++i)
	{
		for (int j = 0; j < procNumReg[i]; ++j)
		{
			this->procId_.data[k] = procId[i][j];
			this->procLoad_.data[k] = procLoad[i][j];
			k++;
		}
	}

	/// Select the public processes for each region
	// for (int i = 0; i < this->s_.size(); ++i)
	int i;
	while(true)
	{
	// printf("************************************\n");
	// for (int k = 0; k < regionNum; ++k)
	// {
	// 	for (int j = procId_.startIdx[k]; j < procId_.startIdx[k+1]; ++j)
	// 	{
	// 		printf("region: %d, pid: %d\n", k, procId_.data[j]);
	// 	}
	// }
		i = findMaxRegion(this->s_);
		if(i==-1) break;
		Scalar phiReg = this->s_[i];
		/// The public process must be the last process for each region
		if(procIdx >= procNum)
		{
			/// if all processes are assigned, then find
			/// the process with the least load
			procIdx = findMinProc();
			// printf("%d\n", procIdx);
		}
		this->procId_.data[this->procId_.startIdx[i+1]-1] = procIdx;
		this->procLoad_.data[this->procId_.startIdx[i+1]-1] = phiReg;
		/// 0 indicate the region has been assigned the public process
		this->s_[i] = 0;
		for (int j = this->nei_.startIdx[i]; j < this->nei_.startIdx[i+1]; ++j)
		{
			Label regionId = this->nei_.data[j];
			/// if the neighbor region is waiting for assigning the public
			/// process and the load of this public process does not
			/// exceed the average measurement load.
			phiReg += this->s_[regionId];
			if(phiReg < phi && this->s_[regionId] > 0)
			{
				this->procId_.data[this->procId_.startIdx[regionId+1]-1]
					= procIdx;
				this->procLoad_.data[this->procId_.startIdx[regionId+1]-1]
					= this->s_[regionId];
				/// mark the neighbor region as finished region.
				this->s_[regionId] = 0;
			}
		}
		procIdx++;
	}
	findMinProc();

	delete[] procId;
	delete[] procLoad;
	delete[] procNumReg;

	// for (int i = 0; i < regionNum; ++i)
	// {
	// 	for (int j = procId_.startIdx[i]; j < procId_.startIdx[i+1]; ++j)
	// 	{
	// 		printf("region: %d, pid: %d\n", i, procId_.data[j]);
	// 	}
	// }
};

/**
* @brief Third Level load balance with ParMETIS
* @param reg the collection of regions owned by this processor
*/
void LoadBalancer::LoadBalancer_3(Array<Region>* regs)
{
	int regNum = regs->size();

	for (int i = 0; i < regNum; ++i)
	{
		// int regIdx = regs[i].idx_g;
		/// if this region is private for the process
		/// then it need not to be partitioned
		printf("region: %d, procId_start: %d, procId_end: %d\n", i, procId_.startIdx[i], procId_.startIdx[i+1]);
		if(procId_.startIdx[i+1]-procId_.startIdx[i]==1) return;

		// /// get parameters of ParMetis: vtxlist, xadj, adjncy
		// Label* xadj   = regs[i].getMesh().getTopology().getCell2Cell().startIdx;
		// Label* adjncy = regs[i].getMesh().getTopology().getCell2Cell().data;
		// Label  cStart = regs[i].getMesh().getCellStart();
		// /// gather the start index of cells from other processes
		// mpi_collective(vtxlist);

		// Label* vwgt = NULL;
		// Label* adjwgt = NULL;
		// Label  wgtflag = 0;
		// Label  numflag = 0;
		// Label  ncon = 1;
		// Label  nparts = procId_.startIdx[regIdx+1]-procId_.startIdx[regIdx];
		// Label* tpwgts = new Label[ncon*nparts];
		// for (int j = 0; j < nparts; ++j)
		// {
		// 	tpwgts[j] = procLoad_.data[j+procId_.startIdx[regIdx]];
		// }
		// Label  ubvec = 1.05;
		// Label  options[3] = 0;
		// Label  edgecut;
		// Label* parts = new Label[regs[i].getMesh().getNodesNum()];
		// ParMETIS_V3_PartKway(vtxlist, xadj, adjncy, vwgt, adjwgt, &wgtflag,
		// 	&numflag, &ncon, &nparts, tpwgts, &ubvec, options, &edgecut,
		// 	parts, MPI_COMM_WROLD);

	}
}
