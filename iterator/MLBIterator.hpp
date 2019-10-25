/**
* @file: MLBIterator.cpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-09-10 11:37:06
* @last Modified by:   lenovo
* @last Modified time: 2019-09-10 14:41:18
*/
#include "iterator.hpp"
/**
* @brief Multi-Level Blocks Iterator
*/
class MLBIterator : public Iterator
{
private:
	/// The index of first edge in each block
	label* blockStarts_;
	/// The row index of first edge in each block
	label* cellStarts_;
	/// Mapping from unreorderd to reordered cells
	label* postCellOrder_;
	/// Mapping from unreorderd to reordered edges
	label* postEdgeOrder_;
	/// Total count of sub-blocks
	label* cpeBlockNum_;
	/// Total count of blocks
	label* mshBlockNum_;
	/// Count of sub-blocks in each block
	label* mtxBlockNum_;
public:
	/**
	* @param default constructor
	*/
	MLBIterator();
	/**
	* @brief constructor with coupled operator
	* @param cOpt operator coupled with mesh information and field
	*/
	MLBIterator(coupledOperator& cOpt) : Iterator(cOpt)
	{

	};
	/**
	* @param deconstructor
	*/
	~MLBIterator();
	/**
	* @brief Multi-Level Blocks Iteration
	* @param cOpt operator coupled with mesh information and field
	*/
	void Iteration(coupledOperator& cOpt)
	{

	};
	
};