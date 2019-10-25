/**
* @file: cellInfo.cpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-09-09 14:12:42
* @last Modified by:   lenovo
* @last Modified time: 2019-09-09 14:51:29
*/
#include "meshInfo.hpp"
/**
	Geometry information of cell element, organized as SoAoS layout
*/
class CellInfo : public MeshInfo
{
private:
	/// Volume of elements
	scalar* volume_;
	/// Relevant velocity of elements
	scalar* relVel_;
public:
	/**
	* @brief default constructor
	*/
	CellInfo();
	/**
	* @brief construct from mesh
	* @param mesh Topology
	*/
	CellInfo(Mesh& mesh);
	/**
	* @brief deconstructor
	*/
	~CellInfo();
};