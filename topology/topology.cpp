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
#include "loadBalancer.hpp"


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
* @brief Construct from the connectivity after load balance
*/
void Topology::constructTopology()
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	ArrayArray<Label> cell2Node = this->cell2Node_;
	// if(rank==0)
	// {
	// for (int i = 0; i < cell2Node.num; ++i)
	// {
	// 	printf("The %dth element: ", i+cellStartId_);
	// 	for (int j = cell2Node.startIdx[i]; j < cell2Node.startIdx[i+1]; ++j)
	// 	{
	// 		printf("%d, ", cell2Node.data[j]);
	// 	}
	// 	printf("\n");
	// }
	// }
	Label cellNum = cell2Node.size();
	Array<Array<Label> > faces2NodesTmp;
	Array<Array<Label> > cell2Cell(cellNum);
	for (int i = 0; i < cellNum; ++i)
	{
		Label faceNumTmp = Section::facesNumForEle(cellType_[i]);
		for (int j = 0; j < faceNumTmp; ++j)
		{
			Array<Label> face2NodeTmp = Section::faceNodesForEle(
				&cell2Node.data[cell2Node.startIdx[i]], cellType_[i], j);
			sort(face2NodeTmp.begin(), face2NodeTmp.end());
			face2NodeTmp.push_back(i+cellStartId_);
			faces2NodesTmp.push_back(face2NodeTmp);
		}
	}
	// printf("faceNum: %d, cellNum: %d\n", faces2NodesTmp.size(), cell2Cell.size());
	/// 对所有面单元进行排序
	quicksortArray(faces2NodesTmp, 0, faces2NodesTmp.size()-1);
	/// 由面与node之间关系寻找cell与cell之间关系
	/// 删除重复的内部面
	/// 找到面与cell之间关系
	bool* isInner = new bool[faces2NodesTmp.size()];
	Array<Array<Label> > face2NodeInn, face2NodeBnd;
	Array<Array<Label> > face2CellInn, face2CellBnd;
	Array<Array<Label> > cell2CellArr(cellNum);
	for (int i = 0; i < faces2NodesTmp.size(); ++i){isInner[i] = false;}
	for (int i = 0; i < faces2NodesTmp.size(); ++i)
	{
		if(isInner[i]) continue;
		int end = i+1;
		/// 默认两个面不相等
		bool isEqual = false;
		while(end<faces2NodesTmp.size() && 
			faces2NodesTmp[i][0] == faces2NodesTmp[end][0])
		{
			/// 第一个node相等时，默认相等
			isEqual = true;
			/// 如果维度不相等，则两个面不相等
			if(faces2NodesTmp[i].size()!=faces2NodesTmp[end].size())
			{
				isEqual = false;
				break;
			}
			/// 比较各个维度，不相等则跳出，标记不相等
			for (int j = 0; j < faces2NodesTmp[i].size()-1; ++j)
			{
				if(faces2NodesTmp[i][j]!=faces2NodesTmp[end][j])
				{
					isEqual = false;
					break;
				}
			}
			if(isEqual)
			{
				/// 本面对应cell编号为owner，相等面对应cell编号为neighbor
				Label ownerId 
					= faces2NodesTmp[i][faces2NodesTmp[i].size()-1]-cellStartId_;
				Label neighborId
					= faces2NodesTmp[end][faces2NodesTmp[end].size()-1]-cellStartId_;
				cell2CellArr[ownerId].push_back(neighborId);
				cell2CellArr[neighborId].push_back(ownerId);
				/// 记录面单元左右cell编号
				Array<Label> face2CellTmp;
				face2CellTmp.push_back(ownerId+cellStartId_);
				face2CellTmp.push_back(neighborId+cellStartId_);
				face2CellInn.push_back(face2CellTmp);
				face2NodeInn.push_back(faces2NodesTmp[i]);
				/// 删除相等面
				// faces2NodesTmp.erase(end+faces2NodesTmp.begin());
				isInner[i] = true;
				isInner[end] = true;
				// face2CellTmp.push_back(-1);
				break;
			}
			end++;
		}
		Label cellId = faces2NodesTmp[i][faces2NodesTmp[i].size()-1];
		/// 删除面单元中最后一个元素，即cell编号
		faces2NodesTmp[i].pop_back();
		/// 记录边界面单元对应cell编号，外部进程单元记-1
		if(!isEqual)
		{
			Array<Label> face2CellTmp;
			face2CellTmp.push_back(cellId);
			face2CellTmp.push_back(-1);
			face2CellBnd.push_back(face2CellTmp);
			face2NodeBnd.push_back(faces2NodesTmp[i]);
		}
	}

	setPatchInfo(face2NodeInn, face2NodeBnd, face2CellInn, face2CellBnd);

	/// localization of cell index
	for (int i = 0; i < face2CellInn.size(); ++i)
	{
		for (int j = 0; j < face2CellInn[i].size(); ++j)
		{
			face2CellInn[i][j] -= cellStartId_;
		}
	}
	this->face2Cell_ = transformArray(face2CellInn);
	this->cell2Cell_ = transformArray(cell2CellArr);
	this->face2Node_ = transformArray(face2NodeInn);
}

/*
* @brief generate the face2Cell topology at the boundary face
* @param face2NodeInn face-to-node topology, sorted for the first node index at least
* @param face2NodeBnd face-to-node topology, sorted for the first node index at least
* @param face2CellInn face-to-cell topology, one-to-one corresponding to the face above
* @param face2CellBnd
*/
void Topology::setPatchInfo(Array<Array<Label> > face2NodeInn,
	Array<Array<Label> > face2NodeBnd, Array<Array<Label> > face2CellInn,
	Array<Array<Label> > face2CellBnd)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	Array<Array<Label> > face2CellArr, face2NodeArr;
	face2CellArr.clear();
	face2CellArr.insert(face2CellArr.end(), face2CellBnd.begin(), face2CellBnd.end());
	face2CellArr.insert(face2CellArr.end(), face2CellInn.begin(), face2CellInn.end());
	// printf("%d, %d, %d\n", face2CellArr.size(), face2CellInn.size(), face2CellBnd.size());
	face2NodeArr.clear();
	face2NodeArr.insert(face2NodeArr.end(), face2NodeBnd.begin(), face2NodeBnd.end());
	face2NodeArr.insert(face2NodeArr.end(), face2NodeInn.begin(), face2NodeInn.end());

	Array<Label> face2CellTmp;
	for (int i = 0; i < face2CellArr.size(); ++i)
	{
		face2CellTmp.push_back(face2CellArr[i][0]);
		/// 对于内部面，令其为-1
		if(face2CellArr[i][1]!=-1) face2CellTmp[i] = -1;
	}
	ArrayArray<Label> face2NodeBndTmp = transformArray(face2NodeBnd);
	// if(rank==1)
	// {
	// 	for (int i = 0; i < face2NodeBndTmp.size(); ++i)
	// 	{
	// 		printf("%d, %d, %d\n", i, face2NodeBndTmp.startIdx[i], face2NodeBndTmp.startIdx[i+1]);
	// 	}
	// }
	Array<Label> face2CellNew;
	face2CellNew = LoadBalancer::collectNeighborCell(face2NodeBndTmp,
		face2NodeBnd, face2CellTmp);

	for (int i = 0; i < face2CellNew.size(); ++i)
	{
		if(face2CellNew[i]>=0)
		{
			Array<Label> face2CellPatchTmp;
			face2CellPatchTmp.push_back(face2CellArr[i][0]);
			face2CellPatchTmp.push_back(face2CellNew[i]);
			face2CellPatch_.push_back(face2CellPatchTmp);
		}
	}
	printf("rank %d, patch: %d\n", rank, face2CellPatch_.size());
	// if(rank==1)
	// {
	// 	for (int i = 0; i < face2CellPatch_.size(); ++i)
	// 	{
	// 		printf("The %dth face: %d, %d\n", i, face2CellPatch_[i][0], face2CellPatch_[i][1]);
	// 	}
	// }
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
