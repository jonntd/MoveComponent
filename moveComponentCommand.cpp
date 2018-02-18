#include "moveComponentCommand.h"
#include "moveComponentConstants.h"
#include "moveComponentData.h"

#include <maya\MToolsInfo.h>

MoveComponentCommand::MoveComponentCommand(){
}

MPxSelectionContext *MoveComponentCommand::makeObj(){
	context = new MoveComponentContext();
	data = context->getData();
	data->setContext(context);

	return context;
}

void *MoveComponentCommand::creator(){
	return new MoveComponentCommand;
}

MStatus MoveComponentCommand::appendSyntax(){
	MStatus status(MStatus::kSuccess);

	MSyntax mySyntax = syntax(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	mySyntax.addFlag(kSpaceFlag, kSpaceFlagLong, MSyntax::kUnsigned);
	mySyntax.addFlag(kOrientationFlag, kOrientationFlaglong, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	mySyntax.addFlag(kModeFlag, kModeFlagLong, MSyntax::kUnsigned);
	mySyntax.addFlag(kSensitivityFlag, kSensitivityFlagLong, MSyntax::kDouble);
	mySyntax.addFlag(kPresetFlag, kPresetFlagLong, MSyntax::kUnsigned);
	mySyntax.addFlag(kStepFlag, kSensitivityFlagLong, MSyntax::kBoolean);

	return MS::kSuccess;
}

MStatus MoveComponentCommand::doEditFlags(){
	MStatus status(MStatus::kSuccess);

	MArgParser argData = parser();
	
	//MStringArray objects;
	//argData.getObjects(objects);
	//data->setName(objects[0]);

	if (argData.isFlagSet(kSpaceFlag)) {
		int value;
		status = argData.getFlagArgument(kSpaceFlag, 0, value);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		data->setSpace(value);
	}
	if (argData.isFlagSet(kOrientationFlag)) {
		double orientation[3];
		status = argData.getFlagArgument(kOrientationFlag, 0, orientation[0]);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		status = argData.getFlagArgument(kOrientationFlag, 1, orientation[1]);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		status = argData.getFlagArgument(kOrientationFlag, 2, orientation[2]);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		data->setOrientation(orientation);
	}
	if (argData.isFlagSet(kModeFlag)) {
		int value;
		status = argData.getFlagArgument(kModeFlag, 0, value);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		data->setMode(value);
	}
	if (argData.isFlagSet(kSensitivityFlag)) {
		double value;
		status = argData.getFlagArgument(kSensitivityFlag, 0, value);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		data->setSensitivity(value);
	}
	if (argData.isFlagSet(kPresetFlag)) {
		int value;
		status = argData.getFlagArgument(kPresetFlag, 0, value);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		data->setPreset(value);
	}
	if (argData.isFlagSet(kStepFlag)) {
		bool value;
		status = argData.getFlagArgument(kStepFlag, 0, value);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		data->setStep(value);
	}

	context->setDirty();
	context->updateManip();
	
	return status;
}

MStatus MoveComponentCommand::doQueryFlags(){
	MStatus status(MStatus::kSuccess);

	MArgParser argData = parser();

	//MStringArray objects;
	//argData.getObjects(objects);
	//data->setName(objects[0]);

	if (argData.isFlagSet(kSpaceFlag))
		setResult((int)data->getSpace());
	else if (argData.isFlagSet(kOrientationFlag)) {
		MDoubleArray orientation = data->getOrientation();
		setResult(MString("") + orientation[0] + " " + orientation[1] + " " + orientation[2]);}
	else if (argData.isFlagSet(kModeFlag))
		setResult((int)data->getMode());
	else if (argData.isFlagSet(kSensitivityFlag))
		setResult((double)data->getSensitivity());
	else if (argData.isFlagSet(kPresetFlag))
		setResult((int)data->getPreset());
	else if (argData.isFlagSet(kStepFlag))
		setResult((bool)data->getStep());

	return status;
}