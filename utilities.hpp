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
#include <algorithm>

#define Array std::vector
#define Label long
#define Label32 int
#define Scalar double

#define Inner 0
#define Boco 1

#define Terminate(location, content) {printf("Location: %s, error message: %s\n", location, content); exit(-1);}

template<class T> bool compareArray(Array<T>& a, Array<T>& b);

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
Label* filterArray(Array<Array<T> >& arr)
{
	int num = arr.size();
	// for(int i=0;i<tmp;i++) printf("%d\n", arr[0][i]);
	sort(arr[0].begin(), arr[0].end());
	int eraseNum = 0;
	Array<Array<T> > bndArr,innArr;
	for (int i = 0; i < num; ++i)
	{
		sort(arr[i].begin(), arr[i].end());
	}
	int end = 0;
	bool *isInner = new bool[num];
	for (int i = 0; i < num; ++i) { isInner[i] = false; }
	while(end < num)
	{
		// printf("%dth elements in %d\n", end, num);
		if(isInner[end]) {end++; continue;}
		for (int i = end+1; i < num; ++i)
		{
			if(compareArray(arr[i],arr[end]))
			{
				isInner[i] = true;
				isInner[end] = true;
				innArr.push_back(arr[end]);
				break;
			}
		}
		if(!isInner[end]) bndArr.push_back(arr[end]);
		end++;
	}
	// printf("old Array Num: %d, new Array Num: %d\n", arr.size(), newArr.size());
	arr.clear();
	arr.insert(arr.end(), bndArr.begin(), bndArr.end());
	arr.insert(arr.end(), innArr.begin(), innArr.end());
	printf("arr: %d, bndArr: %d, innArr: %d\n", arr.size(), bndArr.size(), innArr.size());
	Label *faceNum = new Label[2];
	faceNum[0] = bndArr.size();
	faceNum[1] = innArr.size();
	return faceNum;
};

template<class T>
bool compareArray(Array<T>& a, Array<T>& b)
{
	int num_a = a.size();
	int num_b = b.size();
	if(num_a!=num_b) return false;
	for (int i = 0; i < num_a; ++i)
	{
		if(a[i]!=b[i]) return false;
	}
	return true;
};


/// transform the Array<Array<>> to ArrayArray
template<class T>
ArrayArray<T> transformArray(Array<Array<T> > arr)
{
	ArrayArray<T> res;
	int cellNum = arr.size();
	res.num = cellNum;
	res.startIdx = new Label[cellNum+1];
	res.startIdx[0] = 0;
	for (int i = 0; i < cellNum; ++i)
	{
		res.startIdx[i+1] = res.startIdx[i]+arr[i].size();	
	}
	// printf("cellNum: %d, nodeNum: %d\n", cellNum, res.startIdx[cellNum]);
	res.data = new T[res.startIdx[cellNum]];
	for (int i = 0; i < cellNum; ++i)
	{
		int k=0;
		for (int j = res.startIdx[i]; j < res.startIdx[i+1]; ++j)
		{
			res.data[j] = arr[i][k];
			k++;
		}
	}
	return res;
};

#endif
