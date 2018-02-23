#include "moveComponentCtxCmd.h"
#include "moveComponentConstants.h"
#include "moveComponentData.h"

#include <maya\MToolsInfo.h>
#include <maya\MStringArray.h>

MoveComponentCtxCmd::MoveComponentCtxCmd(){
}

MPxSelectionContext *MoveComponentCtxCmd::makeObj(){
	return m_context = new MoveComponentCtx();
}

void *MoveComponentCtxCmd::creator(){
	return new MoveComponentCtxCmd;
}

MStatus MoveComponentCtxCmd::appendSyntax(){
	MStatus status(MStatus::kSuccess);

	MSyntax mySyntax = syntax(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	mySyntax.addFlag(kSpaceFlag, kSpaceFlagLong, MSyntax::kLong);
	mySyntax.addFlag(kOrientationFlag, kOrientationFlaglong, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	mySyntax.addFlag(kModeFlag, kModeFlagLong, MSyntax::kLong);
	mySyntax.addFlag(kSensitivityFlag, kSensitivityFlagLong, MSyntax::kDouble);
	mySyntax.addFlag(kStepFlag, kStepFlagLong, MSyntax::kBoolean);

	return MS::kSuccess;
}

MStatus MoveComponentCtxCmd::doEditFlags(){
	MStatus status(MStatus::kSuccess);

	MArgParser argData = parser();
	
	MStringArray objects;
	argData.getObjects(objects);

	if (argData.isFlagSet(kSpaceFlag)) {
		int value;
		status = argData.getFlagArgument(kSpaceFlag, 0, value);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		m_context->data()->setSpace(value);
	}
	if (argData.isFlagSet(kOrientationFlag)) {
		double orientation[3];
		for(unsigned int i=0; i<3; i++)
			argData.getFlagArgument(kOrientationFlag, i, orientation[i]);
		m_context->data()->setOrientation(orientation);
	}
	if (argData.isFlagSet(kModeFlag)) {
		int value;
		status = argData.getFlagArgument(kModeFlag, 0, value);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		m_context->data()->setMode(value);
	}
	if (argData.isFlagSet(kSensitivityFlag)) {
		double value;
		status = argData.getFlagArgument(kSensitivityFlag, 0, value);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		m_context->data()->setSensitivity(value);
	}
	if (argData.isFlagSet(kStepFlag)) {
		bool value;
		status = argData.getFlagArgument(kStepFlag, 0, value);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		m_context->data()->setStep(value);
	}

	MToolsInfo::setDirtyFlag(*m_context);
	m_context->updateManip();
	
	return status;
}

MStatus MoveComponentCtxCmd::doQueryFlags(){
	MStatus status(MStatus::kSuccess);

	MArgParser argData = parser();

	MStringArray objects;
	argData.getObjects(objects);

	if (argData.isFlagSet(kSpaceFlag))
		setResult((int)m_context->data()->getSpace());
	else if (argData.isFlagSet(kOrientationFlag)) {
		MDoubleArray orientation = m_context->data()->getOrientation();
		setResult(MString("") + orientation[0] + " " + orientation[1] + " " + orientation[2]);}
	else if (argData.isFlagSet(kModeFlag))
		setResult((int)m_context->data()->getMode());
	else if (argData.isFlagSet(kSensitivityFlag))
		setResult((double)m_context->data()->getSensitivity());
	else if (argData.isFlagSet(kStepFlag))
		setResult((bool)m_context->data()->getStep());

	return status;
}