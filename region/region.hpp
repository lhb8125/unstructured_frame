/**
* @file: 
* @author: Liu Hongbin
* @brief: 
* @date:   2019-10-14 09:17:17
* @last Modified by:   lenovo
* @last Modified time: 2019-10-14 17:10:30
*/
#ifndef REGION_HPP
#define REGION_HPP

#include "mesh.hpp"
#include "boundary.hpp"

class Region
{
private:
	Mesh mesh_;
	Boundary boundary_;
public:
	Region(){};
	~Region(){};
	Mesh& getMesh(){return this->mesh_;};
	Boundary& getBoundary(){return this->boundary_;};
	
};

#endif