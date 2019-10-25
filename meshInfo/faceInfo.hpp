/**
* @file: faceInfo.cpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-09-09 14:12:42
* @last Modified by:   lenovo
* @last Modified time: 2019-09-09 14:51:24
*/
#include "meshInfo.hpp"
/**
	Geometry information of face element, organized as SoAoS layout
*/
class FaceInfo : public MeshInfo
{
private:
	/// Area of faces
	scalar* area_;
	/// Relevant velocity of faces
	scalar* relVel_;
	/// Normal vector of faces
	scalar* normVec_;
public:
	/**
	* @brief default constructor
	*/
	FaceInfo();
	/**
	* @brief construct from mesh
	* @param mesh Topology
	*/
	FaceInfo(Mesh& mesh);
	/**
	* @brief deconstructor
	*/
	~FaceInfo();
};