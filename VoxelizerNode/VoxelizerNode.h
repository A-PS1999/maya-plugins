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
	static MObject CreateVoxelMesh(MPointArray voxelPositions, float voxelWidth, MObject& outputMeshData);
	static void CreateCube(MPoint voxelPos, float width, MFloatPointArray& vertexArray, int vertexIndexOffset, MIntArray& polyCountsArray, 
		int polygonCountIndexOffset, MIntArray& polyConnectionsArray, int polyConnectionsIndexOffset);
};
