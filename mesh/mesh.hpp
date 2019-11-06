/**
* @file: mesh.hpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-09-24 09:25:44
* @last Modified by:   lenovo
* @last Modified time: 2019-10-21 15:27:52
*/
#ifndef MESH_HPP
#define MESH_HPP

#include "utilities.hpp"
#include "section.hpp"
#include "topology.hpp"
#include "nodes.hpp"
/**
* @brief 
*/
class Mesh
{
private:
	Topology topo_;
	/// Coordinates of Nodes
	Nodes nodes_;
	/// section information for CGNS file
	Array<Section> secs_;
	/// boundary section information for CGNS file (temperorary)
	Array<BCSection> BCSecs_;
	/// read mesh file with CGNS format (temporary)
	void readBoundaryCondition(int iFile, int iBase, int iZone);
	/// write mesh file with CGNS format (temporary)
	void writeBoundaryCondition(int iFile, int iBase, int iZone);
	/// Identification for mesh type
	Label meshType_;
	/// count of nodes
	Label nodeNum_;
	/// count of elements
	Label eleNum_;
	/**
	* @brief read mesh file with CGNS format
	*/
	void readCGNSFile(const char* filePtr);
	/**
	* @brief read mesh file with CGNS format, parallel version
	*/
	void readCGNSFilePar(const char* filePtr);
	/**
	* @brief write mesh file with CGNS format, parallel version
	*/
	void writeCGNSFilePar(const char* filePtr, Label* parts);
	/**
	* @brief initialize mesh file with CGNS format, parallel version
	*/
	void initCGNSFilePar(const char* filePtr);
public:
	/**
	* @brief default constructor
	*/
	Mesh()
	{
		this->meshType_ = Inner;
	};
	/**
	* @brief deconstructor
	*/
	virtual ~Mesh(){};
	/**
	* @brief read mesh and construct topology
	*/
	void readMesh(const char* filePtr)
	{
		// readCGNSFile(filePtr);
		readCGNSFilePar(filePtr);
		topo_.constructTopology(this->secs_);
	};
	/**
	* @brief initialize mesh and construct topology
	*/
	void initMesh(const char* filePtr)
	{
		// writeCGNSFile(filePtr);
		initCGNSFilePar(filePtr);
		// topo_ = new Topology(this->secs_);
	};
	/**
	* @brief write mesh and construct topology
	*/
	void writeMesh(const char* filePtr, Label* parts)
	{
		// writeCGNSFile(filePtr);
		writeCGNSFilePar(filePtr, parts);
		// topo_ = new Topology(this->secs_);
	};
	/**
	* @brief get class Topology
	*/
	Topology& getTopology() { return this->topo_;};

	Array<Section>& getSections() {return this->secs_; };

	void setLoadBalancerResult(ArrayArray<Label>& cell2Node,
		Array<Label> cellType, Label cellStartId)
	{
		this->topo_.setCell2Node(cell2Node);
		this->topo_.setCellType(cellType);
		this->topo_.setCellStartId(cellStartId);
	};
	
};

#endif