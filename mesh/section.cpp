/**
* @file: 
* @author: Liu Hongbin
* @brief: 
* @date:   2019-10-14 16:22:22
* @last Modified by:   lenovo
* @last Modified time: 2019-10-14 16:33:09
*/
#include <cstdio>
#include <cstdlib>
#include "section.hpp"

/**
* @brief counts of nodes for each element type
*/
bool Section::compareEleType(const Label secType, const Label meshType_)
{
	bool ltmp = secType==TRI_3 || secType==TRI_6 || secType==TRI_9;
	ltmp = ltmp || secType==QUAD_4 || secType==QUAD_8 || secType==QUAD_9;
	if(ltmp && meshType_==Inner) return false;
	else return true;
}

/**
* @brief counts of faces for each element type
*/
Label Section::nodesNumForEle(const Label eleType)
{
	switch(eleType)
	{
		case TRI_3: return 3;
		case TRI_6: return 6;
		case QUAD_4: return 4;
		case QUAD_8: return 8;
		case TETRA_4: return 4;
		case PYRA_5: return 5;
		case PENTA_6: return 6;
		case HEXA_8: return 8;
		default:
			Terminate("find nodes count for Elements", eleType);
	}
}

/**
* @brief the connectivity of faces and nodes for each element type
*/
Label Section::facesNumForEle(const Label eleType)
{
	switch(eleType)
	{
		case TRI_3: return 0;
		case TRI_6: return 0;
		case QUAD_4: return 0;
		case QUAD_8: return 0;
		case TETRA_4: return 4;
		case PYRA_5: return 5;
		case PENTA_6: return 5;
		case HEXA_8: return 6;
		default:
			Terminate("find faces count for Elements", eleType);
	}
}

/**
* @brief whether the section belongs to the entity through the elements type 
*/
Array<Label> Section::faceNodesForEle(
	Label* conn, const Label eleType, const Label idx)
{
	Array<Label> tmp;
	// printf("%d, %d\n", eleType, TETRA_4);
	if(eleType==TETRA_4)
	{
		/// the first face
		if(idx==0)
		{
			tmp.push_back(conn[0]);
			tmp.push_back(conn[2]);
			tmp.push_back(conn[1]);
		} else if(idx==1)
		/// the second face
		{
			tmp.push_back(conn[0]);
			tmp.push_back(conn[1]);
			tmp.push_back(conn[3]);
		} else if(idx==2)
		/// the third face
		{
			tmp.push_back(conn[1]);
			tmp.push_back(conn[2]);
			tmp.push_back(conn[3]);
		} else if(idx==3)
		/// the fourth face
		{
			tmp.push_back(conn[2]);
			tmp.push_back(conn[0]);
			tmp.push_back(conn[3]);
		}
	} else if(eleType==HEXA_8)
	{
		/// the first face
		if(idx==0)
		{
			tmp.push_back(conn[0]);
			tmp.push_back(conn[3]);
			tmp.push_back(conn[2]);
			tmp.push_back(conn[1]);
		} else if(idx==1)
		/// the second face
		{
			tmp.push_back(conn[0]);
			tmp.push_back(conn[1]);
			tmp.push_back(conn[5]);
			tmp.push_back(conn[4]);
		} else if(idx==2)
		/// the third face
		{
			tmp.push_back(conn[1]);
			tmp.push_back(conn[2]);
			tmp.push_back(conn[6]);
			tmp.push_back(conn[5]);
		} else if(idx==3)
		/// the fourth face
		{
			tmp.push_back(conn[2]);
			tmp.push_back(conn[3]);
			tmp.push_back(conn[7]);
			tmp.push_back(conn[6]);
		} else if(idx==4)
		/// the fourth face
		{
			tmp.push_back(conn[0]);
			tmp.push_back(conn[4]);
			tmp.push_back(conn[7]);
			tmp.push_back(conn[3]);
		} else if(idx==5)
		/// the fourth face
		{
			tmp.push_back(conn[4]);
			tmp.push_back(conn[5]);
			tmp.push_back(conn[6]);
			tmp.push_back(conn[7]);
		}		
	} else
	{
		Terminate("find face nodes for Elements", "The element type is not supported");
	}
	return tmp;
}
