/**
* @file: topology.hpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-09-09 15:08:19
* @last Modified by:   lenovo
* @last Modified time: 2019-10-21 14:28:16
*/
#ifndef NODES_HPP
#define NODES_HPP
#include "topology.hpp"
/**
* @brief Coordinates of nodes
*/
class Nodes
{
private:
	/// AoS layout of coordinates
	ArrayArray<Scalar> xyz_;
	/// Coordinate X
	Array<Scalar> x_;
	/// Coordinate Y
	Array<Scalar> y_;
	/// Coordinate Z
	Array<Scalar> z_;
	/// the global start index of nodes
	Label start_;
	/// the global end index of nodes
	Label end_;
public:
	/**
	* @brief default constructor
	*/
	Nodes();
	/**
	* @param xyz AoS layout of coordinates
	*/
	Nodes(ArrayArray<Scalar>& xyz);
	/** 
	* @param x Coordinate X
	* @param y Coordinate Y
	* @param z Coordinate Z
	*/
	Nodes(Array<Scalar>& x, Array<Label>& y, Array<Label>& z);
	/** 
	* @param x Coordinate X
	* @param y Coordinate Y
	* @param z Coordinate Z
	*/
	Nodes(Scalar* x, Scalar* y, Scalar* z, Label num);
	/**
	* @brief deconstructor
	*/
	~Nodes();
	/**
	* @brief get the count of nodes
	*/
	Label size();
	/**
	* @brief copy constructor
	*/
	void copy(Nodes* nodes);

	Array<Scalar>& getX();
	Array<Scalar>& getY();
	Array<Scalar>& getZ();
	Label getStart();
	Label getEnd();
	void setStart(const Label start);
	void setEnd(const Label end);
};

#endif