/**
* @file: 
* @author: Liu Hongbin
* @brief: 
* @date:   2019-10-14 16:27:19
* @last Modified by:   lenovo
* @last Modified time: 2019-10-14 17:12:43
*/
#include <cstdio>
#include <assert.h>
#include "topology.hpp"
#include "section.hpp"

/**
* @brief deconstructor
*/
Topology::~Topology()
{
	
}

/**
* @brief Construct from CGNS file
* @param cell2Node Connectivity between cells and nodes
*/
Topology::Topology(Array<Section>& secs)
{
	Label secNum = secs.size();

	/// record the type of cells
	for (int i = 0; i < secNum; ++i)
	{
		for (int j = 0; j < secs[i].num; ++j)
		{
			cellType_.push_back(secs[i].type);
		}
	}

	/// construct the topology array: cell2Node
	cell2Node_.startIdx = new Label[secNum+1];
	cell2Node_.startIdx[0] = 0;
	Label k=0;
	for (int i = 0; i < secNum; ++i)
	{
		cellNum_ += secs[i].num;
		for (int j = 0; j < secs[i].num; ++j)
		{
			cell2Node_.startIdx[k+1]
				= cell2Node_.startIdx[k] + Section::nodesNumForEle(secs[i].type);
			k++;
		}
	}
	cell2Node_.data = new Label[cell2Node_.startIdx[cellNum_]];
	k=0;
	for (int i = 0; i < secNum; ++i)
	{
		for (int j = 0; j < secs[i].num; ++j)
		{
			memcpy(&cell2Node_.data[cell2Node_.startIdx[k]],
				&secs[i].conn[j],
				Section::nodesNumForEle(secs[i].type)*sizeof(Label));
			k++;
		}
	}

	/// construct the topology array: face2Node
	Array<Array<Label> > faces2NodesTmp;
	for (int i = 0; i < cellNum_; ++i)
	{
		Label faceNumTmp = Section::facesNumForEle(cellType_[i]);
		for (int j = 0; j < faceNumTmp; ++j)
		{
			faces2NodesTmp.push_back(
				Section::faceNodesForEle(
					&cell2Node_.data[cell2Node_.startIdx[i]], cellType_[i], j));
		}
	}
	/// eliminate the duplicate faces
	/// and divide the internal faces and boundary faces
	faceNum_b_ = filterArray(faces2NodesTmp);
	faceNum_   = faces2NodesTmp.size();
	faceNum_i_ = faceNum_-faceNum_b_;
	face2Node_ = transformArray(faces2NodesTmp);

	/// construct the topology array: cell2Face
	Array<Array<Label> > cells2FacesTmp;
	for (int i = 0; i < cellNum_; ++i)
	{
		Array<Label> cell2FacesTmp;
		Label faceIdx;
		Label faceNumTmp = Section::facesNumForEle(cellType_[i]);
		for (int j = 0; j < faceNumTmp; ++j)
		{
			Array<Label> face2NodesTmp
				= Section::faceNodesForEle(
					&cell2Node_.data[cell2Node_.startIdx[i]], cellType_[i], j);
			/// find the index of face matching this cell
			bool ltmp = true;
			for (int k = 0; k < faceNum_; ++k)
			{
				if(compareArray(faces2NodesTmp[k], face2NodesTmp))
				{
					ltmp = false;
					faceIdx = k;
				}
			}
			if(ltmp) Terminate("find face matching this cell", ltmp);
			cell2FacesTmp.push_back(faceIdx);
		}
		cells2FacesTmp.push_back(cell2FacesTmp);
	}
	cell2Face_ = transformArray(cells2FacesTmp);

	/// construct the topology array: face2Cell
	face2Cell_.startIdx = new Label[faceNum_i_+1];
	face2Cell_.num = faceNum_i_;
	face2Cell_.data = new Label[faceNum_i_*2];
	face2Cell_.startIdx[0] = 0;
	Label *bonus = new Label[faceNum_i_];
	for (int i = 0; i < faceNum_i_; ++i) { bonus[i]=0; }
	for (int i = 0; i < faceNum_i_; ++i)
	{
		face2Cell_.startIdx[i+1] = face2Cell_.startIdx[i]+2;
	}
	for (int i = 0; i < cellNum_; ++i)
	{
		for (int j = cell2Face_.startIdx[i]; j < cell2Face_.startIdx[i+1]; ++j)
		{
			Label idx = cell2Face_.data[i]-faceNum_b_;
			/// if the face is boundary face, then ignore it
			if(idx<0) continue;
			face2Cell_.data[(idx-1)*2+bonus[idx]] = i;
			bonus[idx]++;
		}
	}
	for (int i = 0; i < faceNum_i_; ++i) { assert(bonus[i]==2); }
	delete[] bonus;

	/// construct the topology array: node2Cell
	
}
