/**
* @file: 
* @author: Liu Hongbin
* @brief: 
* @date:   2019-10-14 09:17:17
* @last Modified by:   lenovo
* @last Modified time: 2019-10-14 09:18:49
*/
#include "mesh.hpp"

class Region
{
private:
	Mesh mesh;
public:
	Region();
	~Region();
	Mesh& getMesh(){return this->mesh;};
	
};