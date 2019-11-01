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
#include "mpi.h"
#include "topology.hpp"
#include "section.hpp"

/**
* @brief constructor
*/
Topology::Topology()
{
	
}

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
void Topology::constructTopology(Array<Section>& secs)
{
	Label secNum = secs.size();
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/// record the type of cells
	cellNum_ = 0;
	Array<Label> tmp;
	for (int i = 0; i < secNum; ++i)
	{
		cellNum_ += secs[i].num;
		// printf("%d, %d, %d\n", i, secs[i].num, secs[i].type);
		for (int j = 0; j < secs[i].num; ++j)
		{
			// printf("%d, %d, %d\n", j, secs[i].num, secs[i].type);
			tmp.push_back(secs[i].type);
		}
	}

	// / construct the topology array: cell2Node
	cell2Node_.startIdx = new Label[cellNum_+1];
	cell2Node_.startIdx[0] = 0;
	Label k=0;
	for (int i = 0; i < secNum; ++i)
	{
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
		int l = 0;
		for (int j = 0; j < secs[i].num; ++j)
		{
			memcpy(&cell2Node_.data[cell2Node_.startIdx[k]],
				&secs[i].conn[l],
				Section::nodesNumForEle(secs[i].type)*sizeof(Label));
			k++;
			l += Section::nodesNumForEle(secs[i].type);
		}
	}

// for (int i = 0; i < cellNum_; ++i)
// {
// 	for (int j = cell2Node_.startIdx[i]; j < cell2Node_.startIdx[i+1]; ++j)
// 	{
// 		printf("%d, ", cell2Node_.data[j]);
// 	}
// 	printf("\n");
// }
	/// construct the topology array: face2Node
	// Array<Array<Label> > faces2NodesBndTmp;
#if 0
	Array<Array<Label> > faces2NodesTmp;
	for (int i = 0; i < cellNum_; ++i)
	{
		Label faceNumTmp = Section::facesNumForEle(cellType_[i]);
		// if(rank==0) printf("cellIdx: %d, faceNumTmp: %d, cellType: %d\n", i, faceNumTmp, cellType_[i]);
		// if the face num equals to 0, then the cell is considered as face.
		// if(faceNumTmp==0)
		// {
		// 	Array<Label> faceEleTmp;
		// 	for (int j = cell2Node_.startIdx[i]; j < cell2Node_.startIdx[i+1]; ++j)
		// 	{
		// 		faceEleTmp.push_back(cell2Node_.data[j]);
		// 	}
		// 	faces2NodesBndTmp.push_back(faceEleTmp);
		// }
		for (int j = 0; j < faceNumTmp; ++j)
		{
			faces2NodesTmp.push_back(
				Section::faceNodesForEle(
					&cell2Node_.data[cell2Node_.startIdx[i]], cellType_[i], j));
		}
		// assert(faceNumTmp==4);
	}
	/// eliminate the duplicate faces and seperate the boundary face and internal face
	// faceNum_b_  = filterArray(faces2NodesBndTmp);
	Label *faceNum = filterArray(faces2NodesTmp);
	faceNum_ = faceNum[0]+faceNum[1];
	faceNum_b_ = faceNum[0];
	faceNum_i_ = faceNum[1];
#endif
	// MPI_Send(&)
// for (int i = 0; i < faces2NodesTmp.size(); ++i)
// {
// 	printf("The %dth face: ", i);
// 	for (int j = 0; j < faces2NodesTmp[i].size(); ++j)
// 	{
// 		printf("%d, ", faces2NodesTmp[i][j]);
// 	}
// 	printf("\n");
// }
#if 0
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
			sort(face2NodesTmp.begin(), face2NodesTmp.end());
			/// find the index of face matching this cell
			bool findFace = false;
			// printf("faceNum_: %d\n", faceNum_);
			for (int k = 0; k < faceNum_; ++k)
			{
				// printf("The %dth face\n", k);
				if(compareArray(faces2NodesTmp[k], face2NodesTmp))
				{
					findFace = true;
					faceIdx = k;
					// if(rank==0) printf("The matching face index of element %d is %d\n", i, faceIdx);
				}
			}
			if(!findFace) 
			{
				printf("%d\n", i);
				Terminate("find faces of elements", "can not find the matching faces");
			}
			cell2FacesTmp.push_back(faceIdx);
		}
		cells2FacesTmp.push_back(cell2FacesTmp);
	}
	cell2Face_ = transformArray(cells2FacesTmp);
#endif
// for (int i = 0; i < cell2Face_.num; ++i)
// {
// 	printf("The %dth cell: ", i);
// 	for (int j = cell2Face_.startIdx[i]; j < cell2Face_.startIdx[i+1]; ++j)
// 	{
// 		printf("%d, ", cell2Face_.data[j]);
// 	}
// 	printf("\n");
// }
#if 0
	/// construct the topology array: face2Cell
	face2Cell_.startIdx = new Label[faceNum_+1];
	face2Cell_.num = faceNum_;
	face2Cell_.data = new Label[faceNum_i_*2+faceNum_b_];
	face2Cell_.startIdx[0] = 0;
	Label *bonus = new Label[faceNum_];
	for (int i = 0; i < faceNum_; ++i) { bonus[i]=0; }
	for (int i = 0; i < faceNum_b_; ++i)
	{
		face2Cell_.startIdx[i+1] = face2Cell_.startIdx[i]+1;
	}
	for (int i = faceNum_b_; i < faceNum_b_+faceNum_i_; ++i)
	{
		face2Cell_.startIdx[i+1] = face2Cell_.startIdx[i]+2;
	}
	for (int i = 0; i < cellNum_; ++i)
	{
		for (int j = cell2Face_.startIdx[i]; j < cell2Face_.startIdx[i+1]; ++j)
		{
			Label idx = cell2Face_.data[j];
			/// if the face is boundary face, then ignore it
			face2Cell_.data[face2Cell_.startIdx[idx]+bonus[idx]] = i;
			bonus[idx]++;
		}
	}
#endif
// for (int i = 0; i < face2Cell_.num; ++i)
// {
// 	printf("The %dth face: ", i);
// 	for (int j = face2Cell_.startIdx[i]; j < face2Cell_.startIdx[i+1]; ++j)
// 	{
// 		printf("%d, ", face2Cell_.data[j]);
// 	}
// 	printf("\n");
// }
	// for (int i = 0; i < faceNum_i_; ++i) { assert(bonus[i]==2); }
	// delete[] bonus;

	/// construct the topology array: node2Cell
	
}

// Label Topology::reorderFace2Node(Array<Array<Label> >& face2NodeTmp, Array<Array<Label> >& face2NodeBndTmp)
// {
// 	Array<Array<Label> > result;
// 	// printf("%d,%d\n", face2NodeTmp.size(), face2NodeBndTmp.size());
// 	face2NodeTmp.insert(face2NodeTmp.begin(), face2NodeBndTmp.begin(),face2NodeBndTmp.end());
// 	// printf("%d,%d\n", face2NodeTmp.size(), face2NodeBndTmp.size());
// 	filterArray(face2NodeTmp);
// 	// printf("%d,%d\n", face2NodeTmp.size(), face2NodeBndTmp.size());
// }
