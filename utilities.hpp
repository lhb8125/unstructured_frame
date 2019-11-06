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
#include "mpi.h"

#define Array std::vector
#define Label long
#define Label32 int
#define Scalar double

#define Inner 0
#define Boco 1

#define Terminate(location, content) {printf("Location: %s, error message: %s\n", location, content); exit(-1);}

#if(Label==long)
#define MPI_LABEL MPI_LONG
#else
#define MPI_LABEL MPI_INT
#endif

// #define DEBUG_METIS


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

template<class T>
int partition(Array<Array<T> >& arr, int l , int r)
{
	int k=l,pivot = arr[r][0];
	for (int i = l; i < r; ++i)
	{
		if(arr[i][0]<=pivot)
		{
			arr[i].swap(arr[k]);
			k++;
		}
	}
	arr[k].swap(arr[r]);
	return k;
}

template<class T>
void quicksortArray(Array<Array<T> >& arr, int l, int r)
{
	if(l<r)
	{
		int pivot = partition(arr, l, r);
		quicksortArray(arr, l, pivot-1);
		quicksortArray(arr, pivot+1, r);
	}
}

/// eliminate the duplicate elements
template<class T>
Label* filterArray(Array<Array<T> >& arr)
{
	int num = arr.size();
	quicksortArray(arr, 0, num-1);
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
		// printf("%d, %d\n", end, num);
		if(isInner[end]) {end++; continue;}
		// for (int i = end+1; i < num; ++i)
		int i = end+1;
		while(i<num && arr[i][0]==arr[end][0])
		{
			if(compareArray(arr[i],arr[end]))
			{
				isInner[i] = true;
				isInner[end] = true;
				innArr.push_back(arr[end]);
				break;
			}
			i++;
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
		// printf("%d, %d\n", a[i],b[i]);
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

template<class T>
Label findArray(Array<Array<T> >& arr, Array<T>& value)
{
	int num = arr.size();
	// printf("%d\n", num);
	// printf("*****************************************************\n");
	for (int i = 0; i < num; ++i)
	{
		// for (int j = 0; j < value.size(); ++j)
		// {
			// printf("(%d, %d)", arr[i][j], value[j]);
		// }
		// printf("\n");
		if(compareArray(arr[i], value)) {return i;};
	}
	// printf("*****************************************************\n");
	return -1;
}

template<class T>
Label findSortedArray(Array<Array<T> >& arr, Array<T>& value, Label l, Label r)
{
	Label num = arr.size();
	bool isFinded = false;
	Label m;
	while(l<r)
	{
		m = (l+r)/2;
		if(l==m)
		{
			if(arr[r][0]==value[0]) {isFinded=true; m=r;}
			break;
		}
		if(arr[m][0]<value[0]) l=m;
		else if(arr[m][0]>value[0]) r=m;
		else {isFinded = true; break;}
	}
	if(!isFinded) return -1;
	int i=m;
	while(i<num && arr[i][0]==value[0])
	{
		if(compareArray(arr[i], value)) return i;
		i++;
	}
	i=m;
	while(i>=0 && arr[i][0]==value[0])
	{
		if(compareArray(arr[i], value)) return i;
		i--;
	}
	// // printf("%d\n", num);
	// // printf("*****************************************************\n");
	// for (int i = 0; i < num; ++i)
	// {
	// 	// printf("%d, %d\n", i, num);
	// 	if(arr[i][0]!=value[0]) continue;
	// 	// if(compareArray(arr[i], value)) {return i;};
	// }
	// // printf("*****************************************************\n");
	return -1;
}

#endif
