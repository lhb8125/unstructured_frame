/**
* @file: 
* @author: Liu Hongbin
* @brief: 
* @date:   2019-09-09 14:12:42
* @last Modified by:   lenovo
* @last Modified time: 2019-10-08 10:19:25
*/

/**
	Geometry information of mesh, storaged as class Field
*/
class MeshInfo
{
private:
	/// mapping between name and arrays
	Table<Word, Field*> info_;
public:
	/**
	* @brief default constructor
	*/
	MeshInfo();
	/**
	* @brief construct from mesh
	* @param mesh Topology
	*/
	MeshInfo(Field& field)
	{
		info_.insert({string("area"), Area});
	};
	/**
	* @brief deconstructor
	*/
	~MeshInfo();
};