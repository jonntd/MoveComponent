#include "moveComponentToolCmd.h"
#include "moveComponentConstants.h"

#include <maya\MArgList.h>
#include <maya\MItGeometry.h>
#include <maya\MGlobal.h>
#include <maya\MMatrix.h>
#include <maya\MWeight.h>
#include <maya\MFnMesh.h>

MoveComponentToolCmd::MoveComponentToolCmd(){
	setCommandString(kCommandName);
	m_didOnce = false;
}

MoveComponentToolCmd::~MoveComponentToolCmd(){
}

void* MoveComponentToolCmd::creator(){
	return new MoveComponentToolCmd;
}

bool MoveComponentToolCmd::isUndoable() const{
	return true;
}

MStatus MoveComponentToolCmd::finalize(){
	MStatus status(MStatus::kSuccess);
	MArgList command;
	status = command.addArg(commandString());
	CHECK_MSTATUS_AND_RETURN_IT(status);
	status = MPxToolCommand::doFinalize(command);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	return status;
}

MStatus MoveComponentToolCmd::doIt(const MArgList& args){
	return MStatus::kSuccess;
}

MStatus MoveComponentToolCmd::undoIt(){
	MStatus status(MStatus::kSuccess);

	if (m_oldPoints.length() == 0)
		return MS::kFailure;
	
	MItGeometry geoIt(m_path, m_component, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = geoIt.setAllPositions(m_oldPoints, MSpace::kWorld);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MStatus::kSuccess;
}

MStatus MoveComponentToolCmd::redoIt(){
	MStatus status(MStatus::kSuccess);

	if (m_data->getDrag())
		if (!(m_doStep && m_didOnce)) {
			transformComponent();
			m_didOnce = true;
		}
	else{
		if (m_newPoints.length() == 0)
			return MS::kFailure;

		MItGeometry geoIt(m_path, m_component, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = geoIt.setAllPositions(m_newPoints, MSpace::kWorld);
		CHECK_MSTATUS_AND_RETURN_IT(status);
	}
	return MStatus::kSuccess;
}

// Tool Settings
void MoveComponentToolCmd::setData(MoveComponentData* toolData) {
	m_data = toolData;
	m_data->getPoints(m_oldPoints);
	m_data->getMatrix(m_matrix);
	m_mode = m_data->getMode();
	m_axis = m_data->getAxis();
	m_sensitivity = m_data->getSensitivity();
	m_path = m_data->getPath();
	m_component = m_data->getComponent();
	m_doStep = m_data->getStep();

	if (m_mode == MoveComponent::Mode::kSlide)
	{
		MIntArray links;
		m_data->getLinks(m_axis, links);

		MFnMesh meshFn(m_path);
		MPointArray meshPoints;
		meshFn.getPoints(meshPoints, MSpace::kWorld);

		m_vectors.clear();

		for (unsigned i = 0; i<meshPoints.length(); i++) {
			MPoint pointA, pointB;
			pointA = pointB = meshPoints[i];
			int indexB = links[i];
			if (indexB>0)
				pointB = meshPoints[indexB];
			MVector slideDirection = pointB - pointA;
			m_vectors.append(slideDirection.normal());
		}
	}
}

MStatus MoveComponentToolCmd::transformComponent()
{
	MStatus status(MStatus::kSuccess);

	MItGeometry geoIt(m_path, m_component, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	m_newPoints.clear();
	double delta = m_data->getDelta();
	delta = (m_doStep) ? delta / abs(delta) : delta;

	for (geoIt.reset(); !geoIt.isDone(); geoIt.next()){
		MPoint oldPosition = geoIt.position(MSpace::kWorld, &status);
		MVector vector;

		switch (m_mode)
		{
		case MoveComponent::Mode::kTranslate: {
			switch (m_axis) {
			case MoveComponent::Direction::kXaxis:
				vector = MVector::xAxis;
				break;
			case MoveComponent::Direction::kYaxis:
				vector = MVector::yAxis;
				break;
			case MoveComponent::Direction::kZaxis:
				vector = MVector::zAxis;
				break;
			case MoveComponent::Direction::kNormal:
				vector = geoIt.normal(MSpace::kWorld).normal();
				break;
			}
			if(m_axis!=MoveComponent::Direction::kNormal)
				vector *= m_matrix;
			break;
		}
		case MoveComponent::Mode::kSlide: {
			vector = m_vectors[geoIt.index()];
			break;}
		}

		MWeight weight = geoIt.weight();
		float influence = weight.influence();

		MPoint newPoint = oldPosition + vector * delta * m_sensitivity * influence;
		m_newPoints.append(newPoint);
	}

	status = geoIt.setAllPositions(m_newPoints, MSpace::kWorld);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;
}