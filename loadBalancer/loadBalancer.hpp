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
	void LoadBalancer_3(Array<Region>* reg);

	ArrayArray<Label>& getProcId() {return this->procId_;}
	ArrayArray<Scalar>& getProcLoad() {return this->procLoad_;}
	Scalar* getProcLoadSum() {return this->procLoadSum_;}
};

#endif