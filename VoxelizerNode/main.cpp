#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include "VoxelizerNode.h"

MStatus initializePlugin(MObject pluginObj) {
	const char* vendor = "SAP";
	const char* version = "1.0";
	const char* requiredApiVersion = "Any";

	MStatus status;

	MFnPlugin pluginFn(pluginObj, vendor, version, requiredApiVersion, &status);
	if (!status) {
		MGlobal::displayError("Failed to initialize plugin: " + status.errorString());
		return status;
	}

	status = pluginFn.registerNode(VoxelizerNode::GetTypeName(), 
		VoxelizerNode::GetTypeId(), 
		VoxelizerNode::Creator,
		VoxelizerNode::Initialize,
		VoxelizerNode::kDependNode
	);
	if (!status) {
		MGlobal::displayError("Failed to register VoxelizerNode");
		return status;
	}

	return status;
}

MStatus uninitializePlugin(MObject pluginObj) {
	MStatus status;

	MFnPlugin pluginFn(pluginObj);

	status = pluginFn.deregisterNode(VoxelizerNode::GetTypeId());

	if (!status) {
		MGlobal::displayError("Failed to deregister VoxelizerNode");
		return status;
	}

	return status;
}