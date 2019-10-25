/**
* @file: topology.hpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-09-09 15:08:19
* @last Modified by:   lenovo
* @last Modified time: 2019-10-14 16:33:43
*/
#ifndef TOPOLOGY_HPP
#define TOPOLOGY_HPP
#include <string.h>
#include "utilities.hpp"
#include "section.hpp"
/**
* @brief Topology information of mesh
*/
class Topology
{
private:
	/// count of nodes;
	Label nodeNum_;
	/// count of cells;
	Label cellNum_;
	/// count of edges;
	Label edgeNum_;
	/// count of faces;
	Label faceNum_;
	/// count of boundary faces;
	Label faceNum_b_;
	/// count of internal faces;
	Label faceNum_i_;
	/// Connectivity between nodes and nodes
	ArrayArray<Label> node2Node_;
	/// Connectivity between nodes and edges
	ArrayArray<Label> node2Edge_;
	/// Connectivity between edges and nodes
	ArrayArray<Label> edge2Node_;
	/// Connectivity between edges and cells
	ArrayArray<Label> edge2Cell_;
	/// Connectivity between faces and nodes
	ArrayArray<Label> face2Node_;
	/// Connectivity between faces and cells
	ArrayArray<Label> face2Cell_;
	/// Connectivity between faces and edges
	ArrayArray<Label> face2Edge_;
	/// Connectivity between cells and cells
	ArrayArray<Label> cell2Cell_;
	/// Connectivity between cells and nodes
	ArrayArray<Label> cell2Node_;
	/// Connectivity between cells and faces
	ArrayArray<Label> cell2Face_;
	/// Connectivity between cells and edges
	ArrayArray<Label> cell2Edge_;
	/// type of cells
	Array<Label> cellType_;
	/// type of faces
	Array<Label> faceType_;
public:
	/**
	* @brief default constructor
	*/
	Topology();
	/**
	* @brief Construct from CGNS file
	* @param cell2Node Connectivity between cells and nodes
	*/
	Topology(Array<Section>& secs);
	/**
	* @brief deconstructor
	*/
	~Topology();
	/**
	* @brief get the count of nodes
	*/
	Label getNodesNum(){return this->nodeNum_;};
	/**
	* @brief get the count of cells
	*/
	Label getCellsNum(){return this->cellNum_;};
	/**
	* @brief get the count of faces
	*/
	Label getFacesNum(){return this->faceNum_;};
	/**
	* @brief get the count of edges
	*/
	Label getEdgesNum(){return this->edgeNum_;};
};

#endif