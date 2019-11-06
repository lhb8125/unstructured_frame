/**
* @file: loadBalancer.hpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-10-09 14:25:14
* @last Modified by:   lenovo
* @last Modified time: 2019-10-14 10:00:20
*/
#ifndef LOADBALANCER_HPP
#define LOADBALANCER_HPP

#include <math.h>
#include <assert.h>
#include <limits.h>
#include "utilities.hpp"
#include "region.hpp"
/**
* @brief load balancer for regions
*/
class LoadBalancer
{
private:
	/// measurement of regions;
	Array<Scalar> s_;
	/// neighbors of regions;
	ArrayArray<Label> nei_;
	/// results of assigned processes for all regions
	ArrayArray<Label> procId_;
	/// proportion of assigned processes for all regions
	ArrayArray<Scalar> procLoad_;
	/// measurement for all processes
	Scalar* procLoadSum_;
	/// count of processes
	Label procNum_;
	/// start index of cells in every processors
	Label* cellStartId_;
	/**
	* @brief find the process with the least measurement load
	* @param procId Id of processes for all regions
	*/
	Label findMinProc();
	/*
	* @brief find the region with the maximum measurement load
	* @param s the measurement load of all regions
	*/
	Label findMaxRegion(const Array<Scalar> s);
	/*
	* @brief distribute the cells to other processors according to 
	         the result of PARMETIS
	*/
	ArrayArray<Label> distributeCellsToProcs(ArrayArray<Label>& cell2Node,
		Label* parts);
	/*
	* @brief distribute the cell infomation (type) to other processors
	*/
	Array<Label> distributeCellInfoToProcs(Array<Label>& cellInfo, Label* parts);
public:
	/**
	* @brief default constructor
	*/
	LoadBalancer();
	/**
	* @brief deconstructor
	*/
	~LoadBalancer();
	/**
	* @brief Second Level load balance
	* @param s measurement for all regions
	* @param nei connectivity among all regions
	* @param procNum count of processes
	*/
	void LoadBalancer_2(const Array<Scalar> s, const ArrayArray<Label> nei,
		Label procNum);
	/**
	* @brief communicate the region based on the results of loadBalancer_2
	*/
	void exchangeRegion(Region& regs);
	/**
	* @brief Third Level load balance with parmetis
	* @param reg the collection of regions owned by this processor
	*/
	Label* LoadBalancer_3(Array<Region>& reg);
	/*
	* @brief collect the neighbor cell index through mpi
	*/
	static Array<Label> collectNeighborCell(ArrayArray<Label>& bndFaceList,
		Array<Array<Label> >& bndFaceArr, Array<Label>& face2CellArr);

	ArrayArray<Label>& getProcId() {return this->procId_;}
	ArrayArray<Scalar>& getProcLoad() {return this->procLoad_;}
	Scalar* getProcLoadSum() {return this->procLoadSum_;}
};

#endif