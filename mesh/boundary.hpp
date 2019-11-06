/**
* @file: boundary.cpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-09-26 09:25:10
* @last Modified by:   lenovo
* @last Modified time: 2019-09-29 08:45:07
*/
#ifndef BOUNDARY_HPP
#define BOUNDARY_HPP

#include "utilities.hpp"
#include "mesh.hpp"

/**
* @brief store the boundary elements
*/
class Boundary : public Mesh
{
private:
	/// Identification for mesh type
	Label meshType_;
	/// Boundary condition section information for CGNS file
	Array<BCSection> BCSecs_;
	/// read mesh file with CGNS format
	void readBoundaryCondition(const char* filePtr);
public:
	/**
	* @brief default constructor
	*/
	Boundary()
	{
		this->meshType_ = Boco;
	};
	/**
	* @brief deconstructor
	*/
	virtual ~Boundary(){};
};

// struct BCSection
// {
// 	char* name;
// 	Label type;
// 	Label nBCElems;
// 	Label* BCElems_;
// }

#endif