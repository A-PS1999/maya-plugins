#include "VoxelizerNode.h"
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MPointArray.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>

static const MTypeId TYPE_ID = MTypeId(0x000BEEF6);
static const MString TYPE_NAME = "voxelizernode";

MObject VoxelizerNode::inputMeshObj;
MObject VoxelizerNode::outputMeshObj;
MObject VoxelizerNode::voxelWidthObj;
MObject VoxelizerNode::voxelDistanceObj;

static const float DEFAULT_VOXEL_WIDTH = 0.9f;
static const float DEFAULT_VOXEL_DISTANCE = 1.0f;
static const int NUM_VERTS_PER_VOXEL = 8; // Cube has 8 vertices
static const int NUM_POLYS_PER_VOXEL = 6; // Cube has 6 faces
static const int NUM_VERTS_PER_POLY = 4; // 4 verts per cube face

VoxelizerNode::VoxelizerNode() : MPxNode() {}
VoxelizerNode::~VoxelizerNode() {}

MStatus VoxelizerNode::compute(const MPlug& plug, MDataBlock& dataBlock) {
	
	if (plug == VoxelizerNode::outputMeshObj) {
		float voxelWidth = dataBlock.inputValue(voxelWidthObj).asFloat();
		float voxelDistance = dataBlock.inputValue(voxelDistanceObj).asFloat();
		MObject inputMesh = dataBlock.inputValue(inputMeshObj).asMesh();

		MBoundingBox boundingBox = VoxelizerNode::GetBoundingBox(inputMesh);
		MPointArray voxels = VoxelizerNode::GetVoxels(voxelDistance, inputMesh, boundingBox);

		MFnMeshData meshDataFn;
		MObject newOutputMeshData = meshDataFn.create();

		MObject voxelMesh = VoxelizerNode::CreateVoxelMesh(voxels, voxelWidth, newOutputMeshData);

		MDataHandle outputMeshHandle = dataBlock.outputValue(VoxelizerNode::outputMeshObj);

		return outputMeshHandle.setMObject(newOutputMeshData);
	}
	else {
		return MS::kUnknownParameter;
	}
}

MBoundingBox VoxelizerNode::GetBoundingBox(MObject meshObj) {
	MBoundingBox boundingBox;
	MFnMesh meshFn = MFnMesh(meshObj);
	MPointArray pointArray;

	meshFn.getPoints(pointArray, MSpace::kTransform);

	for (unsigned int i = 0; i < pointArray.length(); i++) {
		MPoint point = pointArray[i];
		boundingBox.expand(point);
	}
	
	return boundingBox;
}

MPointArray VoxelizerNode::GetVoxels(float voxelDistance, MObject meshObj, MBoundingBox boundingBox) {
	MPointArray voxels;
	MFnMesh meshFn = MFnMesh(meshObj, NULL);
	float halfVoxelDist = voxelDistance * 0.5f;
	double distAsDouble = (double)voxelDistance;
	
	// We offset the position of the minimum point to account for the distance between voxels
	MPoint minPoint = boundingBox.min();
	minPoint.x += halfVoxelDist;
	minPoint.y += halfVoxelDist;
	minPoint.z += halfVoxelDist;

	MPoint maxPoint = boundingBox.max();
	maxPoint.x += halfVoxelDist;
	maxPoint.y += halfVoxelDist;
	maxPoint.z += halfVoxelDist;

	double xStart = minPoint.x;
	double xEnd = maxPoint.x;
	double yStart = minPoint.y;
	double yEnd = maxPoint.y;
	double zStart = minPoint.z;
	double zEnd = maxPoint.z;

	MFloatVector rayDir = MFloatVector(0, 0, -1);
	float tolerance = 0.0001f;

	MFloatPointArray hitPts;

	for (double xCoord = xStart; xCoord < xEnd; xCoord += distAsDouble) {
		for (double yCoord = yStart; yCoord < yEnd; yCoord += distAsDouble) {
			for (double zCoord = zStart; zCoord < zEnd; zCoord += distAsDouble) {
				MFloatPoint raySource = MFloatPoint(xCoord, yCoord, zCoord);

				bool rayCheck = meshFn.allIntersections(raySource,
					rayDir,
					nullptr,
					nullptr,
					false,
					MSpace::kTransform,
					float(9999),
					false,
					nullptr,
					false,
					hitPts,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					tolerance
				);

				if (hitPts.length() % 2 == 1) {
					voxels.append(raySource);
				}
			}
		}
	}

	return voxels;
}

MObject VoxelizerNode::CreateVoxelMesh(MPointArray voxelPositions, float voxelWidth, MObject& outputMeshData) {
	int numVoxels = voxelPositions.length();

	int numPolyConnectionsPerVoxel = NUM_POLYS_PER_VOXEL * NUM_VERTS_PER_POLY;

	int totalVerts = numVoxels * NUM_VERTS_PER_VOXEL;
	MFloatPointArray vertArray = MFloatPointArray().setLength(totalVerts);
	int vertIndexOffset = 0;

	int totalPolys = numVoxels * NUM_POLYS_PER_VOXEL;
	MIntArray polyCounts = MIntArray().setLength(totalPolys);
	int polyOffsetIndex = 0;

	int totalPolyConnections = numVoxels * numPolyConnectionsPerVoxel;
	MIntArray polyConnections = MIntArray().setLength(totalPolyConnections);
	int polyConnectionsIndexOffset = 0;

	for (int i = 0; i < numVoxels; i++) {
		MPoint voxelPos = voxelPositions[i];

		VoxelizerNode::CreateCube(voxelPos, voxelWidth, vertArray, vertIndexOffset, polyCounts,
			polyOffsetIndex, polyConnections, polyConnectionsIndexOffset
		);

		vertIndexOffset += NUM_VERTS_PER_VOXEL;
		polyOffsetIndex += NUM_POLYS_PER_VOXEL;
		polyConnectionsIndexOffset += numPolyConnectionsPerVoxel;
	}

	MFnMesh meshFn;
	return meshFn.create(totalVerts, totalPolys, vertArray, polyCounts, polyConnections, outputMeshData);
}

void VoxelizerNode::CreateCube(MPoint voxelPos, float width, MFloatPointArray& vertexArray, int vertexIndexOffset, MIntArray& polyCountsArray,
	int polygonCountIndexOffset, MIntArray& polyConnectionsArray, int polyConnectionsIndexOffset
) {
	double halfWidth = width / 2.0f;

	double floatVerts[][4] = { {-halfWidth + voxelPos.x, -halfWidth + voxelPos.y, -halfWidth + voxelPos.z},
		{halfWidth + voxelPos.x, -halfWidth + voxelPos.y, -halfWidth + voxelPos.z},
		{halfWidth + voxelPos.x, -halfWidth + voxelPos.y,  halfWidth + voxelPos.z},
		{-halfWidth + voxelPos.x, -halfWidth + voxelPos.y,  halfWidth + voxelPos.z},
		{-halfWidth + voxelPos.x,  halfWidth + voxelPos.y, -halfWidth + voxelPos.z},
		{-halfWidth + voxelPos.x,  halfWidth + voxelPos.y,  halfWidth + voxelPos.z},
		{halfWidth + voxelPos.x,  halfWidth + voxelPos.y,  halfWidth + voxelPos.z},
		{halfWidth + voxelPos.x,  halfWidth + voxelPos.y, -halfWidth + voxelPos.z}
	};

	MFloatPointArray vertices = MFloatPointArray(floatVerts, NUM_VERTS_PER_VOXEL);

	const int POLY_CONNECTIONS_ELEM_SIZE = 3;
	int polyConnectionElems[][POLY_CONNECTIONS_ELEM_SIZE] = { {0, 12, 16}, {1, 19, 20}, {2, 9, 23}, {3, 8, 13},
		{4, 15, 17}, {5, 11, 14}, {6, 10, 22}, {7, 18, 21}
	};

	for (int i=0; i < NUM_VERTS_PER_VOXEL; i++) {
		vertexArray[vertexIndexOffset + i] = vertices[i];

		for (int j=0; j < POLY_CONNECTIONS_ELEM_SIZE; j++) {
			polyConnectionsArray[polyConnectionsIndexOffset + polyConnectionElems[i][j]] = vertexIndexOffset + i;
		}
	}

	for (int n = 0; n < NUM_POLYS_PER_VOXEL; n++) {
		polyCountsArray[polygonCountIndexOffset + n] = NUM_VERTS_PER_POLY;
	}
}

void* VoxelizerNode::Creator() {
	return new VoxelizerNode();
}

MStatus VoxelizerNode::Initialize() {
	MFnNumericAttribute numericAttr;
	MFnTypedAttribute typedAttr;

	voxelWidthObj = numericAttr.create("voxelWidth", "voxW", MFnNumericData::kFloat, DEFAULT_VOXEL_WIDTH);
	numericAttr.setWritable(true);
	numericAttr.setReadable(false);
	numericAttr.setStorable(true);
	numericAttr.setHidden(false);
	numericAttr.setMin(0.1f);

	voxelDistanceObj = numericAttr.create("voxelDistance", "voxD", MFnNumericData::kFloat, DEFAULT_VOXEL_DISTANCE);
	numericAttr.setWritable(true);
	numericAttr.setReadable(false);
	numericAttr.setStorable(true);
	numericAttr.setHidden(false);
	numericAttr.setMin(0.1f);

	inputMeshObj = typedAttr.create("inputMesh", "im", MFnData::kMesh);
	typedAttr.setWritable(true);
	typedAttr.setReadable(false);
	typedAttr.setStorable(true);
	typedAttr.setHidden(false);

	outputMeshObj = typedAttr.create("outputMesh", "om", MFnData::kMesh);
	typedAttr.setWritable(false);
	typedAttr.setReadable(true);
	typedAttr.setStorable(false);
	typedAttr.setHidden(false);

	addAttribute(voxelWidthObj);
	addAttribute(voxelDistanceObj);
	addAttribute(inputMeshObj);
	addAttribute(outputMeshObj);

	attributeAffects(voxelWidthObj, outputMeshObj);
	attributeAffects(voxelDistanceObj, outputMeshObj);
	attributeAffects(inputMeshObj, outputMeshObj);

	return MS::kSuccess;
}

MTypeId VoxelizerNode::GetTypeId() {
	return TYPE_ID;
}

MString VoxelizerNode::GetTypeName() {
	return TYPE_NAME;
}
