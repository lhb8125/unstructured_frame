/**
* @file: mesh.cpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-09-25 11:21:52
* @last Modified by:   lenovo
* @last Modified time: 2019-10-23 15:38:09
*/
#include <cstdio>
#include <iostream>
#include <typeinfo>
#include <stdlib.h>
#include "mpi.h"
#include "../../../src/pcgnslib.h"
#include "utilities.hpp"
#include "mesh.hpp"
#include "cgnslib.h"

// #define INTEGER32


void Mesh::readCGNSFilePar(const char* filePtr)
{
	int rank, numProcs;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	// printf("This is rank %d in %d processes\n", rank, numProcs);

	int iFile, nBases, cellDim, physDim;
	int iBase=1, iZone=1;
	char basename[20];

	if(cgp_mpi_comm(MPI_COMM_WORLD) != CG_OK)
		Terminate("initCGNSMPI", cg_get_error());
	if(cgp_open(filePtr, CG_MODE_READ, &iFile) ||
		cg_nbases(iFile, &nBases) ||
		cg_base_read(iFile, iBase, basename, &cellDim, &physDim))
		Terminate("readBaseInfo", cg_get_error())
	if(rank==0) printf("nBases: %d, basename: %s, cellDim: %d, physDim: %d\n", nBases, basename, cellDim, physDim);
	MPI_Barrier(MPI_COMM_WORLD);

    int precision;
    cg_precision(iFile, &precision);
    // printf("precision: %d\n", precision);

	char zoneName[20];
	int nZones;
	cgsize_t sizes[3];
	ZoneType_t zoneType;
	if(cg_nzones(iFile, iBase, &nZones) ||
		cg_zone_read(iFile, iBase, iZone, zoneName, sizes) ||
		cg_zone_type(iFile, iBase, iZone, &zoneType) ||
		zoneType != Unstructured)
		Terminate("readZoneInfo", cg_get_error());
	if(rank==0) printf("nZones: %d, zoneName: %s, zoneType: %d, nodeNum, %d, eleNum: %d, bndEleNum: %d\n", nZones, zoneName, zoneType, sizes[0], sizes[1], sizes[2]);
	this->nodeNum_ = sizes[0];
	this->eleNum_  = sizes[1];

	int nCoords;
	if(cg_ncoords(iFile, iBase, iZone, &nCoords) ||
		nCoords != 3)
		Terminate("readNCoords", cg_get_error());
    int nnodes = (nodeNum_ + numProcs - 1) / numProcs;
    cgsize_t start  = nnodes * rank + 1;
    cgsize_t end    = nnodes * (rank + 1);
    if (end > nodeNum_) end = nodeNum_;
    Array<Scalar*> coords;
    printf("The vertices range of processor %d is (%d, %d). \n", rank, start, end);
	DataType_t dataType;
	char coordName[20];
	Scalar* x = new Scalar[nnodes];
	if(cg_coord_info(iFile, iBase, iZone, 1, &dataType, coordName) ||
		// sizeof(dataType)!=sizeof(Scalar) ||
		cgp_coord_read_data(iFile, iBase, iZone, 1, &start, &end, x))
		Terminate("readCoords", cg_get_error());
    if(dataType==RealSingle && sizeof(Scalar)==8)
        Terminate("readCoords","The data type of Scalar does not match");
    if(dataType==RealDouble && sizeof(Scalar)==4)
        Terminate("readCoords","The data type of Scalar does not match");
	Scalar* y = new Scalar[nnodes];
	if(cg_coord_info(iFile, iBase, iZone, 2, &dataType, coordName) ||
		// sizeof(dataType)!=sizeof(Scalar) ||
		cgp_coord_read_data(iFile, iBase, iZone, 2, &start, &end, y))
		Terminate("readCoords", cg_get_error());
	Scalar* z = new Scalar[nnodes];
	if(cg_coord_info(iFile, iBase, iZone, 3, &dataType, coordName) ||
		// sizeof(dataType)!=sizeof(Scalar) ||
		cgp_coord_read_data(iFile, iBase, iZone, 3, &start, &end, z))
		Terminate("readCoords", cg_get_error());
	MPI_Barrier(MPI_COMM_WORLD);
    // printf("%d\n", sizeof(dataType));

	this->nodes_.copy(new Nodes(x, y, z, nnodes));
	this->nodes_.setStart(start);
	this->nodes_.setEnd(end);
    // int i,j,k,n,nn,ne;
    // nn = 0;
    // if(rank==0){
    // for (int i = 0; i < nnodes; ++i)
    // {
    //     printf("x: %lf, ", x[i]);
    //     printf("y: %lf, ", y[i]);
    //     printf("z: %lf\n", z[i]);
    	
    // }
	int nSecs;
	if(cg_nsections(iFile, iBase, iZone, &nSecs))
		Terminate("readNSections", cg_get_error());
	int iSec;
	for (int iSec = 1; iSec <= nSecs; ++iSec)
	{
		char secName[20];
		cgsize_t start, end;
		ElementType_t type;
		int nBnd, parentFlag;
		if(cg_section_read(iFile, iBase, iZone, iSec, secName, 
			&type, &start, &end, &nBnd, &parentFlag))
			Terminate("readSectionInfo", cg_get_error());
		printf("iSec: %d, sectionName: %s, type: %d, start: %d, end: %d, nBnd: %d\n", iSec, secName, type, start, end, nBnd);
        if(! Section::compareEleType(type, this->meshType_)) continue;

    	Section sec;
    	sec.name = new char[20];
    	strcpy(sec.name, secName);
    	sec.type   = type;
    	sec.nBnd   = nBnd;
		/// if the section does not match the type of mesh, then ignore it.
		Label secStart = start-1;
		Label eleNum = end-start+1;
    	int nEles = (eleNum + numProcs - 1) / numProcs;
    	start  = nEles * rank + 1;
    	end    = nEles * (rank + 1);
    	if (end > eleNum) end = eleNum;
    	printf("processor %d will read elements from %d to %d.\n", rank, start+secStart, end+secStart);

    	Label* elements = new Label[nEles*Section::nodesNumForEle(type)];
    	if(cgp_elements_read_data(iFile, iBase, iZone, iSec, start+secStart, end+secStart, elements))
    		Terminate("readElements", cg_get_error());
		MPI_Barrier(MPI_COMM_WORLD);

        sec.iStart = start+secStart;
        sec.iEnd   = end+secStart;
    	sec.num    = end-start+1;
        if(precision==64)
        {
    	   sec.conn   = elements;
        } else if(precision==32)
        {
            int *eles32 = (int*)&elements[0];
            Label* eles64 = new Label[nEles*Section::nodesNumForEle(type)];
            for (int i = 0; i < nEles; ++i)
    	    {
                for (int j = 0; j < Section::nodesNumForEle(type); ++j)
                {
                    eles64[i*Section::nodesNumForEle(type)+j] = (Label)eles32[i*Section::nodesNumForEle(type)+j];
                }
        	}
            sec.conn = eles64;
            delete[] elements;
        }
        this->secs_.push_back(sec);
	}
    MPI_Barrier(MPI_COMM_WORLD);

    readBoundaryCondition(iFile, iBase, iZone);
    MPI_Barrier(MPI_COMM_WORLD);

	if(cgp_close(iFile))
		Terminate("closeCGNSFile",cg_get_error());

}

void Mesh::readBoundaryCondition(int iFile, int iBase, int iZone)
{
    /// read boundary condition
    int nBocos;
    if(cg_nbocos(iFile, iBase, iZone, &nBocos))
        Terminate("readNBocos", cg_get_error());

    char bocoName[2000];
    int normalIndex[3];
    PointSetType_t ptsetType[1];
    Label normalListSize;
    int nDataSet;
    DataType_t normDataType;
    for(int iBoco=1; iBoco<=nBocos; iBoco++)
    {
        BCSection BCSec;
        /* Read the info for this boundary condition. */
        if(cg_boco_info(iFile, iBase, iZone, iBoco, BCSec.name, &BCSec.type,
            BCSec.ptsetType, &BCSec.nBCElems, &normalIndex[0], &normalListSize, &normDataType,
            &nDataSet))
            Terminate("readBocoInfo", cg_get_error());
        if(BCSec.ptsetType[0]!=PointRange || BCSec.ptsetType[0]!=PointList)
        {
            if(BCSec.ptsetType[0]==ElementRange) BCSec.ptsetType[0] = PointRange;
            else if(BCSec.ptsetType[0]==ElementList) BCSec.ptsetType[0] = PointList;
            if(cg_boco_gridlocation_read(iFile, iBase, iZone, iBoco, &BCSec.location))
                Terminate("readGridLocation", cg_get_error());
            if(BCSec.location==CellCenter)
                printf("The boundary condition is defined on CellCenter\n");
            else if(BCSec.location==EdgeCenter)
            {
                Terminate("readGridLocation","The boundary condition is defined on EdgeCenter\n");
            }
            else if(BCSec.location==Vertex)
            {
                BCSec.location = CellCenter;
                printf("The boundary condition is defined on Vertex\n");
            }
            else
                printf("!!!!Error!!!!vertex: %d, FaceCenter: %d, EdgeCenter: %d, location: %d\n", Vertex, FaceCenter, EdgeCenter, BCSec.location);
        }
        BCSec.BCElems = new cgsize_t[BCSec.nBCElems];
        if(cg_boco_read(iFile, iBase, iZone, iBoco, BCSec.BCElems, NULL))
            Terminate("readBocoInfo", cg_get_error());
        if(BCSec.ptsetType[0]==PointRange)
            printf("iBoco: %d, name: %s, type: %d, nEles: %d, start: %d, end: %d\n", iBoco, BCSec.name, BCSec.type, BCSec.nBCElems, BCSec.BCElems[0], BCSec.BCElems[1]);
        else if(BCSec.ptsetType[0]==PointList)
            printf("iBoco: %d, name: %s, type: %d, nEles: %d\n", iBoco, BCSec.name, BCSec.type, BCSec.nBCElems);
        this->BCSecs_.push_back(BCSec);
    }    
}

void Mesh::writeCGNSFilePar(const char* filePtr, Label* parts)
{
	int nodesPerSide = 5;
	int nodeNum = this->nodeNum_;
	int eleNum  = this->eleNum_;

	int rank, numProcs;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	printf("This is rank %d in %d processes\n", rank, numProcs);

	int iFile, nBases, cellDim, physDim, Cx, Cy, Cz;
	int iBase=1, iZone=1;
	char basename[20];

	cgsize_t sizes[3];
	sizes[0] = nodeNum;
	sizes[1] = eleNum;
	sizes[2] = 0;

	if(cgp_mpi_comm(MPI_COMM_WORLD) != CG_OK)
		Terminate("initCGNSMPI", cg_get_error());
	if(cgp_open(filePtr, CG_MODE_WRITE, &iFile) ||
		cg_base_write(iFile, "Base", 3, 3, &iBase) ||
        cg_zone_write(iFile, iBase, "Zone", sizes, Unstructured, &iZone))
		Terminate("writeBaseInfo", cg_get_error());
    /* print info */
    if (rank == 0) {
        printf("writing %d coordinates and %d hex elements to %s\n",
            nodeNum, eleNum, filePtr);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    DataType_t dataType;
    if(sizeof(Scalar)==4) dataType = RealSingle;
    else dataType = RealDouble;
    /* create data nodes for coordinates */
    if (cgp_coord_write(iFile, iBase, iZone, dataType, "CoordinateX", &Cx) ||
        cgp_coord_write(iFile, iBase, iZone, dataType, "CoordinateY", &Cy) ||
        cgp_coord_write(iFile, iBase, iZone, dataType, "CoordinateZ", &Cz))
        Terminate("writeCoordInfo", cg_get_error());
    MPI_Barrier(MPI_COMM_WORLD);

    /* number of nodes and range this process will write */
    int nnodes = this->nodes_.size();
    cgsize_t start  = this->nodes_.getStart();
    cgsize_t end    = this->nodes_.getEnd();
    /* create the coordinate data for this process */
    Scalar* x = new Scalar[nnodes];
    Scalar* y = new Scalar[nnodes];
    Scalar* z = new Scalar[nnodes];
    for (int i = 0; i < nnodes; ++i)
    {
    	x[i] = this->nodes_.getX()[i];
    	y[i] = this->nodes_.getY()[i];
    	z[i] = this->nodes_.getZ()[i];
    	// printf("vertex: %d, x, %f, y, %f, z, %f\n", i, x[i], y[i], z[i]);
    }

    // /* write the coordinate data in parallel */
    if (cgp_coord_write_data(iFile, iBase, iZone, Cx, &start, &end, x) ||
        cgp_coord_write_data(iFile, iBase, iZone, Cy, &start, &end, y) ||
        cgp_coord_write_data(iFile, iBase, iZone, Cz, &start, &end, z))
        Terminate("writeCoords", cg_get_error());
	MPI_Barrier(MPI_COMM_WORLD);

    /// write connectivity
    ArrayArray<Label> conn = this->getTopology().getCell2Node();
    Array<Label> cellType = this->getTopology().getCellType();
    Label *cellStartId = new Label[numProcs+1];
    MPI_Allgather(&conn.num, 1, MPI_LABEL, &cellStartId[1], 1, MPI_LABEL, MPI_COMM_WORLD);
    cellStartId[0] = 0;
    // if(rank==1)
    // {
        for (int i = 0; i < numProcs; ++i)
        {
            cellStartId[i+1] += cellStartId[i];
            // cellStartId[i]++;
        }
        // printf("\n");
    // }
    /*
    * filter section
    * code
    */
    int iSec;
    ElementType_t eleType = (ElementType_t)cellType[0];
    if(cgp_section_write(iFile, iBase, iZone, "Connectivity", eleType, 1, cellStartId[numProcs],
        0, &iSec))
        Terminate("writeSecInfo", cg_get_error());
    // printf("%d, %d\n", start, end);
    if(cgp_elements_write_data(iFile, iBase, iZone, iSec, cellStartId[rank]+1,
        cellStartId[rank+1], conn.data))
        Terminate("writeSecConn", cg_get_error());

    int S, Fs, A;
    if(cg_sol_write(iFile, iBase, iZone, "Solution", CellCenter, &S) ||
        cgp_field_write(iFile, iBase, iZone, S, LongInteger, "CellIndex", &Fs))
        Terminate("writeSolutionInfo", cg_get_error());

    start = cellStartId[rank]+1;
        // for (int i = 0; i < conn.num; ++i)
        // {
        //     if(parts[i]>3) printf("rank: %d, cellIdx: %d, value: %d\n", rank, i, parts[i]);
        // }
    if(cgp_field_write_data(iFile, iBase, iZone, S, Fs, &start,
        &cellStartId[rank+1], parts))
        Terminate("writeSolutionData", cg_get_error());
    // if(cg_goto(iFile, iBase, "Zone_t", 1, NULL) ||
    //     cg_user_data_write("User Data") ||
    //     cg_gorel(iFile, "User Data", 0, NULL) ||
    //     cgp_array_write("CellIndex", LongInteger, 1, &ncells, &A))
    //     Terminate("writeArrayInfo", cg_get_error());
    // if(cgp_array_write_data(A, &start, &end, parts))
    //     Terminate("writeArrayData", cg_get_error());
    // /* create data node for elements */
    // Label nSecs = this->secs_.size();
    // // nSecs = 1;
    // for (int iSec = 0; iSec < nSecs; ++iSec)
    // {
    // 	int iSec_f = iSec+1;
    // 	if (cgp_section_write(iFile, iBase, iZone, secs_[iSec].name, 
    // 		secs_[iSec].type, secs_[iSec].iStart, secs_[iSec].iEnd, 0, &iSec_f))
    //     	Terminate("writeSecInfo", cg_get_error());  
    //     MPI_Barrier(MPI_COMM_WORLD);
    // printf("section: %d, start: %d. end: %d\n", iSec_f, secs_[iSec].iStart, secs_[iSec].iEnd);
    // 	int eleNum = secs_[iSec].iEnd-secs_[iSec].iStart+1;
    // 	int nEles = (eleNum + numProcs - 1) / numProcs;
    // 	cgsize_t start  = nEles * rank + secs_[iSec].iStart;
    // 	cgsize_t end    = nEles * (rank + 1) + secs_[iSec].iStart-1;
    // 	if (end > secs_[iSec].iEnd) end = secs_[iSec].iEnd;
    // printf("processor: %d, start: %d. end: %d\n", rank, start, end);
    // // if(rank==0)
    // // {
    // // 	for (int i = 0; i < secs_[iSec].num; ++i)
    // // 	{
    // // 		printf("The %dth element: ", i+secs_[iSec].iStart);
    // // 		for (int j = 0; j < Section::nodesNumForEle(secs_[iSec].type); ++j)
    // // 		{
    // // 			printf("%d, ", secs_[iSec].conn[i*Section::nodesNumForEle(secs_[iSec].type)+j]);
    // // 		}
    // // 		printf("\n");
    // // 	}
    // // }
	   //  /* write the element connectivity in parallel */
    // 	if (cgp_elements_write_data(iFile, iBase, iZone, iSec_f, 
    // 		start, end, secs_[iSec].conn))
    //     	Terminate("writeSecConn", cg_get_error());  	
    // 	MPI_Barrier(MPI_COMM_WORLD);
    // }

    writeBoundaryCondition(iFile, iBase, iZone);

	if(cgp_close(iFile))
		Terminate("closeCGNSFile",cg_get_error());
}

void Mesh::writeBoundaryCondition(int iFile, int iBase, int iZone)
{
    int nBocos = this->BCSecs_.size();
    int iBoco;
    for (int i = 0; i < nBocos; ++i)
    {
        // if(this->BCSecs_[i].ptsetType[0]==PointRange) printf("PointRange\n");
        // else if(this->BCSecs_[i].ptsetType[0]==ElementRange) printf("ElementRange\n");
        // printf("PointRange: %d, ElementRange: %d\n", PointRange, ElementRange);
        if(cg_boco_write(iFile, iBase, iZone, this->BCSecs_[i].name,
            this->BCSecs_[i].type, this->BCSecs_[i].ptsetType[0], 
            this->BCSecs_[i].nBCElems, this->BCSecs_[i].BCElems, &iBoco))
            Terminate("writeBC", cg_get_error());
        if(cg_boco_gridlocation_write(iFile, iBase, iZone, iBoco, this->BCSecs_[i].location))
            Terminate("writeGridLocation",cg_get_error());
    }
}


void Mesh::initCGNSFilePar(const char* filePtr)
{
	int nodesPerSide = 5;
	int nodeNum = nodesPerSide*nodesPerSide*nodesPerSide;
	int eleNum  = (nodesPerSide-1)*(nodesPerSide-1)*(nodesPerSide-1);
    int faceNum = 6*(nodesPerSide-1)*(nodesPerSide-1);
    // int faceNum = 0;

	int rank, numProcs;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	printf("This is rank %d in %d processes\n", rank, numProcs);

	int iFile, nBases, cellDim, physDim, Cx, Cy, Cz;
	int iBase=1, iZone=1;
	char basename[20];

	cgsize_t sizes[3];
	sizes[0] = nodeNum;
	sizes[1] = eleNum;
	sizes[2] = 0;

	if(cgp_mpi_comm(MPI_COMM_WORLD) != CG_OK)
		Terminate("initCGNSMPI", cg_get_error());
	if(cgp_open(filePtr, CG_MODE_WRITE, &iFile) ||
		cg_base_write(iFile, "Base", 3, 3, &iBase) ||
        cg_zone_write(iFile, iBase, "Zone", sizes, Unstructured, &iZone))
		Terminate("writeBaseInfo", cg_get_error());
    /* print info */
    if (rank == 0) {
        printf("writing %d coordinates and %d hex elements and %d quad elements to %s\n",
            nodeNum, eleNum, faceNum, filePtr);
    }
    /* create data nodes for coordinates */
    DataType_t dataType;
    if(sizeof(Scalar)==4) dataType = RealSingle;
    else dataType = RealDouble;
    if (cgp_coord_write(iFile, iBase, iZone, dataType, "CoordinateX", &Cx) ||
        cgp_coord_write(iFile, iBase, iZone, dataType, "CoordinateY", &Cy) ||
        cgp_coord_write(iFile, iBase, iZone, dataType, "CoordinateZ", &Cz))
        Terminate("writeCoordInfo", cg_get_error());
    /* number of nodes and range this process will write */
    int nnodes = (nodeNum + numProcs - 1) / numProcs;
    cgsize_t start  = nnodes * rank + 1;
    cgsize_t end    = nnodes * (rank + 1);
    if (end > nodeNum) end = nodeNum;
    /* create the coordinate data for this process */
    Scalar* x = (Scalar *)malloc(nnodes * sizeof(Scalar));
    Scalar* y = (Scalar *)malloc(nnodes * sizeof(Scalar));
    Scalar* z = (Scalar *)malloc(nnodes * sizeof(Scalar));
    int i,j,k,n,nn,ne;
    nn = 0;
    for (n = 1, k = 0; k < nodesPerSide; k++) {
        for (j = 0; j < nodesPerSide; j++) {
            for (i = 0; i < nodesPerSide; i++, n++) {
                if (n >= start && n <= end) {
                    x[nn] = (Scalar)i;
                    y[nn] = (Scalar)j;
                    z[nn] = (Scalar)k;
                    nn++;
                }
            }
        }
    }

    /* write the coordinate data in parallel */
    if (cgp_coord_write_data(iFile, iBase, iZone, Cx, &start, &end, x) ||
        cgp_coord_write_data(iFile, iBase, iZone, Cy, &start, &end, y) ||
        cgp_coord_write_data(iFile, iBase, iZone, Cz, &start, &end, z))
        Terminate("writeCoords", cg_get_error());

    /* create data node for elements */
    int iSec=1;
    if (cgp_section_write(iFile, iBase, iZone, "Hex", HEXA_8, 1, eleNum, 0, &iSec))
        Terminate("writeSecInfo", cg_get_error());
 
    /* number of elements and range this process will write */
    int nelems = (eleNum + numProcs - 1) / numProcs;
    start  = nelems * rank + 1;
    end    = nelems * (rank + 1);
    if (end > eleNum) end = eleNum;

    /* create the hex element data for this process */
    cgsize_t* e = (cgsize_t *)malloc(8 * nelems * sizeof(cgsize_t));
    nn = 0;
    for (n = 1, k = 1; k < nodesPerSide; k++) {
        for (j = 1; j < nodesPerSide; j++) {
            for (i = 1; i < nodesPerSide; i++, n++) {
                if (n >= start && n <= end) {
                    ne = i + nodesPerSide*((j-1)+nodesPerSide*(k-1));
                    e[nn++] = ne;
                    e[nn++] = ne + 1;
                    e[nn++] = ne + 1 + nodesPerSide;
                    e[nn++] = ne + nodesPerSide;
                    ne += nodesPerSide * nodesPerSide;
                    e[nn++] = ne;
                    e[nn++] = ne + 1;
                    e[nn++] = ne + 1 + nodesPerSide;
                    e[nn++] = ne + nodesPerSide;
                }
            }
        }
    }

    /* write the element connectivity in parallel */
    if (cgp_elements_write_data(iFile, iBase, iZone, iSec, start, end, e))
        Terminate("writeSecConn", cg_get_error());

    iSec++;
    Label* faces = new Label[4*faceNum];
    /// back face
    i=1;
    int nf=0;
    for (int j = 1; j < nodesPerSide; ++j)
    {
    	for (int k = 1; k < nodesPerSide; ++k)
    	{
    		nn = i+(j-1)*nodesPerSide+(k-1)*nodesPerSide*nodesPerSide;
    		faces[nf++] = nn;
    		faces[nf++] = nn+nodesPerSide*nodesPerSide;
    		faces[nf++] = nn+nodesPerSide*(nodesPerSide+1);
    		faces[nf++] = nn+nodesPerSide;
    	}
    }

    /// front face
    i=nodesPerSide;
    for (int j = 1; j < nodesPerSide; ++j)
    {
    	for (int k = 1; k < nodesPerSide; ++k)
    	{
    		nn = i+(j-1)*nodesPerSide+(k-1)*nodesPerSide*nodesPerSide;
    		faces[nf++] = nn;
    		faces[nf++] = nn+nodesPerSide;
    		faces[nf++] = nn+nodesPerSide*(nodesPerSide+1);
    		faces[nf++] = nn+nodesPerSide*nodesPerSide;
    	}
    }

    /// left face
    j=1;
    for (int i = 1; i < nodesPerSide; ++i)
    {
    	for (int k = 1; k < nodesPerSide; ++k)
    	{
    		nn = i+(j-1)*nodesPerSide+(k-1)*nodesPerSide*nodesPerSide;
    		faces[nf++] = nn;
    		faces[nf++] = nn+1;
    		faces[nf++] = nn+1+nodesPerSide*nodesPerSide;
    		faces[nf++] = nn+nodesPerSide*nodesPerSide;
    	}
    }

    /// right face
    j=nodesPerSide;
    for (int i = 1; i < nodesPerSide; ++i)
    {
    	for (int k = 1; k < nodesPerSide; ++k)
    	{
    		nn = i+(j-1)*nodesPerSide+(k-1)*nodesPerSide*nodesPerSide;
    		faces[nf++] = nn;
    		faces[nf++] = nn+nodesPerSide*nodesPerSide;
    		faces[nf++] = nn+1+nodesPerSide*nodesPerSide;
    		faces[nf++] = nn+1;
    	}
    }

    /// bottom face
    k=1;
    for (int i = 1; i < nodesPerSide; ++i)
    {
    	for (int j = 1; j < nodesPerSide; ++j)
    	{
    		nn = i+(j-1)*nodesPerSide+(k-1)*nodesPerSide*nodesPerSide;
    		faces[nf++] = nn;
    		faces[nf++] = nn+nodesPerSide;
    		faces[nf++] = nn+1+nodesPerSide;
    		faces[nf++] = nn+1;
    	}
    }

    /// top face
    k=nodesPerSide;
    for (int i = 1; i < nodesPerSide; ++i)
    {
    	for (int j = 1; j < nodesPerSide; ++j)
    	{
    		nn = i+(j-1)*nodesPerSide+(k-1)*nodesPerSide*nodesPerSide;
    		faces[nf++] = nn;
    		faces[nf++] = nn+1;
    		faces[nf++] = nn+1+nodesPerSide;
    		faces[nf++] = nn+nodesPerSide;
    	}
    }
    if (cgp_section_write(iFile, iBase, iZone, "Wall", QUAD_4, eleNum+1, eleNum+faceNum, 0, &iSec))
        Terminate("writeSecInfo", cg_get_error());
    int nfaces = (faceNum + numProcs - 1) / numProcs;
    start  = nfaces * rank + 1;
    end    = nfaces * (rank + 1);
    if (end > faceNum) end = faceNum;
    // if(rank==0)
    // {
    printf("start: %d, end: %d\n", start, end);
    	// if(cgp_section_write_data(iFile, iBase, iZone, "Wall", QUAD_4, eleNum+1, eleNum+faceNum, 0, faces, &iSec))
        	// Terminate("writeSecConn", cg_get_error());
    // }
    if (cgp_elements_write_data(iFile, iBase, iZone, iSec, start+eleNum, end+eleNum, &faces[4*(start-1)]))
        Terminate("writeSecConn", cg_get_error());

    int iBC;
    Label range[] = {eleNum+1, eleNum+faceNum};
    if(cg_boco_write(iFile, iBase, iZone, "Walls", BCWall, PointRange, 2, range, &iBC))
    	Terminate("writeBC", cg_get_error());
    if(cg_boco_gridlocation_write(iFile, iBase, iZone, iBC, CellCenter))
    	Terminate("writeBCLocation", cg_get_error());
}

void Mesh::readCGNSFile(const char* filePtr)
{
	// bool is_3D_cal = true;

	// Label iFile,ier;

	// // printf("reading CGNS files: %s ......\n", filePtr);
	// /// open cgns file
 //    if(cg_open(filePtr, CG_MODE_READ, &iFile) != CG_OK)
 //    	Terminate("readGridCGNS", cg_get_error());

	// /// read base information
	// Label nBases;
	// if(cg_nbases(iFile, &nBases) != CG_OK)
	// 	Terminate("readNBases", cg_get_error());
	// if(nBases!=1) 
	// 	Terminate("readNBases", "This example assumes one base");
	// Label iBase = 1;

	// char basename[20];
	// Label cellDim,physDim;
	// if(cg_base_read(iFile, iBase, basename, &cellDim, &physDim) != CG_OK)
	// 	Terminate("readBaseInfo", cg_get_error());
	// printf("nBases: %d, basename: %s, cellDim: %d, physDim: %d\n", nBases, basename, cellDim, physDim);


	// /// read zone information
	// Label nZones;
	// if(cg_nzones(iFile, iBase, &nZones) != CG_OK)
	// 	Terminate("readNZones", cg_get_error());
	// if(nZones!=1) 
	// 	Terminate("readNZones", "This example assumes one zone");
	// Label iZone = 1;
	// ZoneType_t zoneType;
	// if(cg_zone_type(iFile, iBase, iZone, &zoneType) != CG_OK)
 //     	Terminate("readZoneType", cg_get_error());
 //   	if(zoneType != Unstructured)
 //     	Terminate("readZoneType", "Unstructured zone expected");
	// char zoneName[20];
	// /// fixed value for unstructured mesh: 3
	// Label size[3];
	// if(cg_zone_read(iFile, iBase, iZone, zoneName, size) != CG_OK)
	// 	Terminate("readZoneInfo", cg_get_error());

	// Label nodesNum = size[0];
	// Label cellsNum = size[1];
	// Label bocoNum  = size[2];
	// printf("nZones: %d, zoneName: %s, nodesNum: %d,cellsNum, %d, bocoNum: %d\n",
	// 	nZones, zoneName, size[0], size[1], size[2]);


 //    /// read coordinate information
 //    Label nCoords;
 //    if(cg_ncoords(iFile, iBase, iZone, &nCoords) != CG_OK)
 //    	Terminate("readNCoords", cg_get_error());
 //    if(nCoords!=3 && is_3D_cal) 
	// 	Terminate("readNCoords", "This example assumes three dimensions");
	// /// make sure data type is in double precision
	// char coordName[20];
	// DataType_t coordType;
 //    if(cg_coord_info(iFile, iBase, iZone, 1, &coordType, coordName) != CG_OK)
 //    	Terminate("readCoordInfo", cg_get_error());
 //    if(coordType != RealDouble && sizeof(Scalar)==8) 
	// 	Terminate("readCoordInfo", "This example assumes double precision");
 //    if(cg_coord_info(iFile, iBase, iZone, 2, &coordType, coordName) != CG_OK)
 //    	Terminate("readCoordInfo", cg_get_error());
 //    if(coordType != RealDouble && sizeof(Scalar)==8) 
	// 	Terminate("readCoordInfo", "This example assumes double precision");
 //    if(cg_coord_info(iFile, iBase, iZone, 3, &coordType, coordName) != CG_OK)
 //    	Terminate("readCoordInfo", cg_get_error());
 //    if(coordType != RealDouble && sizeof(Scalar)==8) 
	// 	Terminate("readCoordInfo", "This example assumes double precision");

	// /// read coordinates X, Y and Z
	// Scalar *coordX, *coordY, *coordZ, *coordk;
	// coordX = new Scalar[nodesNum];
	// coordY = new Scalar[nodesNum];
	// coordZ = new Scalar[nodesNum];

	// // coordk = new Scalar[nodesNum];
	// Label one = 1;
	// // printf("%p,%p,%p,%p\n", coordX,coordY,coordZ,coordk);
 //    if(cg_coord_read(iFile, iBase, iZone, "CoordinateX", coordType, &one,
 //                    &nodesNum, coordX) != CG_OK)
 //    	Terminate("readCoordX", cg_get_error());
 //    if(cg_coord_read(iFile, iBase, iZone, "CoordinateY", coordType, &one,
 //                    &nodesNum, coordY) != CG_OK)
 //    	Terminate("readCoordY", cg_get_error());
 //    if(cg_coord_read(iFile, iBase, iZone, "CoordinateZ", coordType, &one,
 //                    &nodesNum, coordZ) != CG_OK)
 //    	Terminate("readCoordZ", cg_get_error());
 //    this->nodes_.copy(new Nodes(coordX, coordY, coordZ, nodesNum));

 //    /// read section information
 //    Label nSections;
 //    if(cg_nsections(iFile, iBase, iZone, &nSections) != CG_OK)
 //    	Terminate("readNSection", cg_get_error());
 //    char secName[20];
 //    Label iStart, iEnd, nBnd, parentFlag;
 //    ElementType_t secType;
	// for (int iSec = 1; iSec <= nSections; ++iSec)
	// {
	// 	Section sec;
	// 	if(cg_section_read(iFile, iBase, iZone, iSec, secName,
	// 		&secType, &iStart, &iEnd, &nBnd, &parentFlag) != CG_OK)
	// 		Terminate("readSectionInfo", cg_get_error());
	// 	if(!Section::compareEleType((int)secType, this->meshType_)) continue;
	// 	sec.name = secName;
	// 	sec.type = secType;
	// 	sec.iStart = iStart;
	// 	sec.iEnd = iEnd;
	// 	sec.nBnd = nBnd;
	// 	sec.num  = iEnd-iStart+1;
	// 	sec.conn = new Label[Section::nodesNumForEle(secType)*sec.num];
 //        if(cg_elements_read(iFile, iBase, iZone, iSec, sec.conn, NULL) != CG_OK)
 //       		Terminate("readConnectivity", cg_get_error());
	// 	this->secs_.push_back(sec);
	// 	printf("Section: %d, name: %s, type: %d, start: %d, end: %d\n", iSec, secName, secType, iStart, iEnd);
	// }
 //    if(cg_close(iFile) != CG_OK)
 //    	Terminate("closeGridCGNS", cg_get_error());
}