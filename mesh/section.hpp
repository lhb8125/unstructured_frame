/*
* @Author: Liu Hongbin
* @Date:   2019-10-14 10:05:10
* @Last Modified by:   lenovo
* @Last Modified time: 2019-10-14 14:14:36
*/
#ifndef SECTION_H
#define SECTION_H
#include "cgnslib.h"
#include "utilities.hpp"

/**
* @brief Elements with same type are stored in one section
*/
class Section
{
public:
	char*  name;
	ElementType_t  type;
	/// global start index of the section
	Label  iStart;
	/// global end index of the section
	Label  iEnd;
	/// count of elements of the section
	Label  num;
	Label  nBnd;
	Label* conn;
	/**
	* @brief counts of nodes for each element type
	*/
	static Label nodesNumForEle(const Label eleType);
	/**
	* @brief counts of faces for each element type
	*/
	static Label facesNumForEle(const Label eleType);
	/**
	* @brief the connectivity of faces and nodes for each element type
	*/
	static Array<Label> faceNodesForEle(Label* conn, const Label eleType, const Label idx);
	/**
	* @brief whether the section belongs to the entity through the elements type 
	*/
	static bool compareEleType(const Label secType, const Label meshType_);
};

struct BCSection
{
	char name[40];
	BCType_t type;
	cgsize_t nBCElems;
	GridLocation_t location;
	PointSetType_t ptsetType[1];
	cgsize_t* BCElems;
};

#endif