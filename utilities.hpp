/**
* @file: utilities.hpp
* @author: Liu Hongbin
* @brief: some useful utilities
* @date:   2019-10-08 15:12:44
* @last Modified by:   lenovo
* @last Modified time: 2019-10-23 15:14:10
*/
#ifndef UTILITIES_HPP
#define UTILITIES_HPP


#include <iostream>
#include <vector>

#define Array std::vector
#define Label long
#define Label32 int
#define Scalar float

#define Inner 0
#define Boco 1

#define Terminate(location, content) {printf("Location: %s, error message: %s\n", location, content); exit(-1);}

template <class T>
class ArrayArray
{
public:
	Label  num;
	Label* startIdx;
	T*     data;
	ArrayArray(){};
	~ArrayArray(){};
	Label size(){return num;};
	void display()
	{
		for (int i = 0; i < num; ++i)
		{
			std::cout<<"Item: "<<i<<" (";
			for (int j = startIdx[i]; j < startIdx[i+1]; ++j)
			{
				std::cout<<data[j];
				if(j<startIdx[i+1]-1) std::cout<<",";
			}
			std::cout<<")"<<std::endl;
		}
	}

};

/// eliminate the duplicate elements
template<class T>
Label filterArray(Array<T> arr){};

/// compare whether the two elements are equal
template<class T>
bool compareArray(T& a, T& b){};

/// transform the Array<Array<>> to ArrayArray
template<class T>
ArrayArray<T> transformArray(Array<Array<T> > arr){};

#endif
