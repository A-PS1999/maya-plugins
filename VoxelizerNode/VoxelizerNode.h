#pragma once

#include <maya/MPxNode.h>
#include <maya/MBoundingBox.h>

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

	static MBoundingBox GetBoundingBox(MObject meshObj);
	static MPointArray GetVoxels(float voxelDistance, MObject meshObj, MBoundingBox boundingBox);
	static void CreateVoxelMesh(MPointArray voxelPositions, float voxelWidth, MObject& outputMeshData);
};
