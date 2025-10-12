#include "VoxelizerNode.h"
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>

static const MTypeId TYPE_ID = MTypeId(0x000BEEF6);
static const MString TYPE_NAME = "voxelizernode";

MObject VoxelizerNode::inputMeshObj;
MObject VoxelizerNode::outputMeshObj;
MObject VoxelizerNode::voxelWidthObj;
MObject VoxelizerNode::voxelDistanceObj;

static const float DEFAULT_VOXEL_WIDTH = 0.9f;
static const float DEFAULT_VOXEL_DISTANCE = 1.0f;

VoxelizerNode::VoxelizerNode() : MPxNode() {}
VoxelizerNode::~VoxelizerNode() {}

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
}

MTypeId VoxelizerNode::GetTypeId() {
	return TYPE_ID;
}

MString VoxelizerNode::GetTypeName() {
	return TYPE_NAME;
}
