/**
* @file: topology.hpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-09-09 15:08:19
* @last Modified by:   lenovo
* @last Modified time: 2019-09-20 10:22:13
*/
#include "topology.hpp"
/**
* @brief Coordinates of nodes
*/
class Nodes
{
private:
	/// AoS layout of coordinates
	ArrayArray xyz_;
	/// Coordinate X
	Array x_;
	/// Coordinate Y
	Array y_;
	/// Coordinate Z
	Array z_;
public:
	/**
	* @brief default constructor
	*/
	Nodes();
	/**
	* @param xyz AoS layout of coordinates
	*/
	Nodes(ArrayArray& xyz);
	/** 
	* @param x Coordinate X
	* @param y Coordinate Y
	* @param z Coordinate Z
	*/
	Nodes(Array& x, Array& y, Array& z);
	/**
	* @brief deconstructor
	*/
	~Nodes();
};