/**
* @file: 
* @author: Liu Hongbin
* @brief: 
* @date:   2019-10-14 16:47:22
* @last Modified by:   lenovo
* @last Modified time: 2019-10-21 14:27:55
*/
#include <cstdio>
#include "nodes.hpp"

/**
* @brief get the count of nodes
*/
Label Nodes::size()
{
	return this->x_.size();
}

Label Nodes::getStart()
{
	return this->start_;
}

Label Nodes::getEnd()
{
	return this->end_;
}

void Nodes::setStart(const Label start)
{
	this->start_ = start;
}

void Nodes::setEnd(const Label end)
{
	this->end_ = end;
}

/**
* @brief default constructor
*/
Nodes::Nodes()
{

}

/**
* @brief default deconstructor
*/
Nodes::~Nodes()
{
	
}

/** 
* @param x Coordinate X
* @param y Coordinate Y
* @param z Coordinate Z
*/
Nodes::Nodes(Scalar* x, Scalar* y, Scalar* z, Label num)
{
	for (int i = 0; i < num; ++i)
	{
		x_.push_back(x[i]);
		y_.push_back(y[i]);
		z_.push_back(z[i]);
	}
}

void Nodes::copy(Nodes* nodes)
{
	int num = nodes->size();
	Array<Scalar> x = nodes->getX();
	Array<Scalar> y = nodes->getY();
	Array<Scalar> z = nodes->getZ();
	for (int i = 0; i < num; ++i)
	{
		x_.push_back(x[i]);
		y_.push_back(y[i]);
		z_.push_back(z[i]);

	}
}

Array<Scalar>& Nodes::getX()
{
	return this->x_;
}

Array<Scalar>& Nodes::getY()
{
	return this->y_;
}

Array<Scalar>& Nodes::getZ()
{
	return this->z_;
}