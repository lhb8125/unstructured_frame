/**
* @file: boundary.cpp
* @author: Liu Hongbin
* @brief: 
* @date:   2019-09-26 09:25:10
* @last Modified by:   lenovo
* @last Modified time: 2019-09-29 08:45:07
*/
#include "boundary.hpp"

void Boundary::readBoundaryCondition(const char* filePtr)
{
	int iFile;

	/// open cgns file
    if(cg_open(filePtr, CG_MODE_READ, &iFile))
    	Terminate("readGridCGNS", cg_get_error());
    Label iBase = 1;
    Label iZone = 1;

    if(cg_goto(iFile, iBase, "Zone", 1, "ZoneBC_t", 1, "BC_t", 1, "end"))
    	Terminate("goZoneBC", cg_get_error());
	/// read boundary information
	int nBocos;
   	if(cg_nbocos(iFile, iBase, iZone, &nBocos))
    	Terminate("readNBocos", cg_get_error());

    char* bocoName;
    Label bocoType, ptsetType, nBCElems, normalIndex, normListFlag;
    Label normDataType, nDataSet;
    // for(int iBoco=1; iBoco<=nBocos; iBoco++)
    // {
    // 	BCSection BCSec;
    //  	/* Read the info for this boundary condition. */
    // 	if(cg_boco_info(iFile, iBase, iZone, iBoco, BCSec.name, &BCSec.type,
    // 		&ptsetType, &BCSec.nBCElems, &normalIndex, &normListFlag, &normDataType,
    //         &nDataSet) != CG_OK)
    //    		Terminate("readBocoInfo", cg_get_error());

    //    	BCSec.BCElems = new Label(nBCElems);
    // 	if(cg_boco_read(iFile, iBase, iZone, iBoco, BCSec.BCElems, NULL) != CG_OK)
    //    		Terminate("readBocoInfo", cg_get_error());
    //    	this->BCSecs_.push_back(BCSec);
    // }
}