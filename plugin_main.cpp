#include "moveComponentCtx.h"
#include "moveComponentCtxCmd.h"
#include "moveComponentToolCmd.h"
#include "moveComponentManip.h"
#include "moveComponentManipContainer.h"
#include "moveComponentConstants.h"

#include <maya/MFnPlugin.h>

MStatus initializePlugin(MObject obj)
{
	MStatus status;

	MFnPlugin plugin(obj, "Stepan Jirka", "1.0", "Any");
	status = plugin.registerContextCommand(
		kContextName,
		&MoveComponentCtxCmd::creator,
		kCommandName,
		&MoveComponentToolCmd::creator);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.registerNode(
		"moveComponentManipContainer",
		MoveComponentManipContainer::id,
		&MoveComponentManipContainer::creator,
		&MoveComponentManipContainer::initialize,
		MPxNode::kManipContainer);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.registerNode(kManipName,
		MoveComponentManip::id,
		&MoveComponentManip::creator,
		&MoveComponentManip::initialize,
		MPxNode::kManipulatorNode);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;
}
MStatus uninitializePlugin(MObject obj)
{
	MStatus status;

	MFnPlugin plugin(obj);
	status = plugin.deregisterContextCommand(kContextName, kCommandName);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	status = plugin.deregisterNode(MoveComponentManipContainer::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = plugin.deregisterNode(MoveComponentManip::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;
}