/**
* @file: iterator.hpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-09-10 10:19:24
* @last Modified by:   lenovo
* @last Modified time: 2019-10-08 09:51:49
*/
/**
* @brief Iterator
*/
class Iterator
{
private:
	/// Count of mesh information arrays
	Label meshInfoNum_;
	/// Count of field arrays
	Label fieldNum_;
	/// mesh information arrays
	Array<Scalar> meshInfo_;
	/// field arrays
	Array<Scalar> field_;
	/// topology array: cell2face, face2cell, ...
	ArrayArray<Label> topologyArray_;
	/**
	* @brief get topology array through translator
	*/
	void getTopologyArray(coupledOperator& cOpt);
	/**
	* @brief get operator through translator
	*/
	void getOperator(coupledOperator& cOpt);
public:
	/**
	* @param default constructor
	*/
	Iterator()
	{
		meshInfoNum_ = 0;
		fieldNum_    = 0;
	};
	/**
	* @brief constructor with coupled operator
	* @param cOpt operator coupled with mesh information and field
	*/
	Iterator(coupledOperator* cOpt)
	{
		meshInfoNum_ = 0;
		for (int i = 0; i < meshInfoNum_; ++i)
		{
			meshInfo_.push_back(cOpt.meshInfo[i]);
			meshInfoNum_++;
		}

		fieldNum_ = 0;
		for (int i = 0; i < fieldNum_; ++i)
		{
			field_[i].push_back(cOpt.field[i]);
			fieldNum_++;
		}

		getTopologyArray(cOpt);
		getOperator(cOpt);
	};
	/**
	* @param deconstructor
	*/
	~Iterator();
	/**
	* @brief get topology array corresponding to the information
	* @param cOpt operator coupled with mesh information and field
	*/
	void getTopologyArray(cOpt)
	{
		if(cOpt.discInfo==cell2face)
		{
			topologyArray_ = cOpt.mesh.getTopology().getCell2Face();
		}
		/*
		...
		*/
	};
	/**
	* @brief Iteration
	* @param cOpt operator coupled with mesh information and field
	*/
	void Iteration(coupledOperator& cOpt)
	{
		ops(meshInfo_, field_, topologyArray_);
	};
	
};