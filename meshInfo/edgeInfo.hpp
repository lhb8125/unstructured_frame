/**
* @file: edgeInfo.cpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-09-09 14:51:55
* @last Modified by:   lenovo
* @last Modified time: 2019-09-09 14:53:18
*/
#include "meshInfo.hpp"
/**
	Geometry information of face element, organized as SoAoS layout
*/
class EdgeInfo : public MeshInfo
{
private:
	/// Length of edges
	scalar* length_;
	/// Relevant velocity of edges
	scalar* relVel_;
	/// Normal vector of edges
	scalar* normVec_;
public:
	/**
	* @brief default constructor
	*/
	EdgeInfo();
	/**
	* @brief construct from mesh
	* @param mesh Topology
	*/
	EdgeInfo(Mesh& mesh);
	/**
	* @brief deconstructor
	*/
	~EdgeInfo();
};