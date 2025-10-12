#pragma once

#include <maya/MPxNode.h>

class VoxelizerNode : public MPxNode {

public:
	VoxelizerNode();
	virtual ~VoxelizerNode() override;

	virtual MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

	static void* Creator();
	static MStatus Initialize();

	static MTypeId GetTypeId();
	static MString GetTypeName();

private:
	static MObject voxelWidthObj;
	static MObject voxelDistanceObj;
	static MObject inputMeshObj;
	static MObject outputMeshObj;
};
