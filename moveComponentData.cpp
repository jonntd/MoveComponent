#include "moveComponentData.h"

#include <maya\MGlobal.h>
#include <maya\MItMeshVertex.h>
#include <maya\MFnMesh.h>
#include <maya\MFnSingleIndexedComponent.h>
#include <maya\MRichSelection.h>
#include <maya\MItGeometry.h>
#include <maya\MFnToolContext.h>
#include <maya\MMatrix.h>

MoveComponentData::MoveComponentData(){
	m_space = MoveComponent::Space::kWorld;
	m_orientation[0] = m_orientation[1] = m_orientation[2] = 0.0;
	m_mode = MoveComponent::Mode::kTranslate;
	m_axis = MoveComponent::Direction::kXaxis;
	m_sensitivity = 0.1;
	m_preset = 4;
	m_doStep = false;
}

MoveComponentData::~MoveComponentData(){
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set variables ///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MoveComponentData::setContext(MoveComponentCtx *context){
	m_context = context;
}

void MoveComponentData::setSpace(int space) {
	syncIntOptionVar(MString("space"), space, true);
	m_space = space;
}

void MoveComponentData::setOrientation(double orientation[3]) {
	syncDoubleOptionVar(MString("orientationX"), orientation[0], true);
	syncDoubleOptionVar(MString("orientationY"), orientation[1], true);
	syncDoubleOptionVar(MString("orientationZ"), orientation[2], true);

	m_orientation[0] = orientation[0];
	m_orientation[1] = orientation[1];
	m_orientation[2] = orientation[2];
}

void MoveComponentData::setMode(int mode) {
	syncIntOptionVar(MString("mode"), mode, true);
	m_mode = mode;
}

void MoveComponentData::setAxis(int axis) {
	syncIntOptionVar(MString("axis"), axis, true);
	m_axis = axis;
}

void MoveComponentData::setSensitivity(double sensitivity) {
	MString variable = MString("sensitivity_preset") + m_preset;
	syncDoubleOptionVar(variable, sensitivity, true);
	m_sensitivity = sensitivity;
}

void MoveComponentData::setPreset(int preset) {
	syncIntOptionVar(MString("preset"), preset, true);
	m_preset = preset;
}

void MoveComponentData::setStep(bool doStep) {
	int step = (doStep) ? 1 : 0;
	syncIntOptionVar(MString("step"), step, true);
	m_doStep = doStep;
}

void MoveComponentData::setDrag(bool drag) {
	m_drag = drag;
}

void MoveComponentData::setDelta(double delta) {
	m_delta = delta;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Query variables /////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

MoveComponentCtx *MoveComponentData::getContext(){
	return m_context;
}

double MoveComponentData::getSensitivity() {
	MString variable = MString("sensitivity_preset") + m_preset;
	syncDoubleOptionVar(variable, m_sensitivity);
	return m_sensitivity;
}

int MoveComponentData::getPreset() {
	syncIntOptionVar(MString("preset"), m_preset);
	return m_preset;
}

bool MoveComponentData::getStep() {
	int step = 0; syncIntOptionVar(MString("step"), step); m_doStep = (step == 1) ? true : false;
	return m_doStep;
}

int MoveComponentData::getAxis() {
	syncIntOptionVar(MString("axis"), m_axis);
	return m_axis;
}

int MoveComponentData::getMode() {
	syncIntOptionVar(MString("mode"), m_mode);
	return m_mode;
}

int MoveComponentData::getSpace() {
	syncIntOptionVar(MString("space"), m_space);
	return m_space;
}

MDoubleArray MoveComponentData::getOrientation() {
	syncDoubleOptionVar(MString("orientationX"), m_orientation[0]);
	syncDoubleOptionVar(MString("orientationY"), m_orientation[1]);
	syncDoubleOptionVar(MString("orientationZ"), m_orientation[2]);
	return MDoubleArray(m_orientation, 3);
}

bool MoveComponentData::getDrag() {
	return m_drag;
}

double MoveComponentData::getDelta() {
	return m_delta;
}

void MoveComponentData::getPoints(MPointArray& points){
	if (m_component.isNull())
		return;
	MItGeometry geoIt(m_path, m_component);
	geoIt.allPositions(points, MSpace::kWorld);
}

MDagPath MoveComponentData::getPath(){
	return m_path;
}

MObject MoveComponentData::getComponent(){
	return m_component;
}

MStatus MoveComponentData::getMatrix(MMatrix& matrix){
	MStatus status(MStatus::kSuccess);

	MDagPath path(m_path);
	path.extendToShape();

	switch (m_space) {
	case MoveComponent::Space::kObject:
		matrix = path.inclusiveMatrix(&status);
		break;
	case MoveComponent::Space::kParent:
		matrix = path.exclusiveMatrix(&status);
		break;
	case MoveComponent::Space::kCustom:{
		MTransformationMatrix tMatrix;
		tMatrix.addRotation(m_orientation, MTransformationMatrix::kXYZ, MSpace::kWorld);
		matrix = tMatrix.asMatrix();}
	}

	return status;
}

MStatus MoveComponentData::getLinks(int direction, MIntArray &links){
	MStatus status(MStatus::kSuccess);

	links.clear();
	
	if (m_path.apiType() != MFn::kMesh)
		return MStatus::kFailure;

	MFnMesh meshFn(m_path, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	int numVertices = meshFn.numVertices(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	for (int i = 0; i < numVertices; i++)
		links.append(m_slideDirections[i][direction % 4]);
	
	return status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sync option variables ///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

MString MoveComponentData::fullName(MString& variable) {
	MObject context = MGlobal::currentToolContext();
	MFnToolContext contextFn(context);
	MString name = contextFn.name() + "_" + variable;
	return name;
}

MStatus MoveComponentData::syncIntOptionVar(MString& variable, int& value, bool edit) {
	MStatus status(MStatus::kSuccess);

	if (edit)
		MGlobal::setOptionVarValue(fullName(variable), value);
	if (MGlobal::optionVarExists(fullName(variable)))
		value = MGlobal::optionVarIntValue(fullName(variable));

	return status;
}

MStatus MoveComponentData::syncDoubleOptionVar(MString& variable, double& value, bool edit) {
	MStatus status(MStatus::kSuccess);

	if (edit)
		MGlobal::setOptionVarValue(fullName(variable), value);
	if (MGlobal::optionVarExists(fullName(variable)))
		value = MGlobal::optionVarDoubleValue(fullName(variable));

	return status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Selection change ///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MoveComponentData::selectionChanged(){
	MStatus status;

	m_selection.clear();
	m_component = MObject::kNullObj;
	
	MRichSelection richSelection;
	MGlobal::getRichSelection(richSelection);
	richSelection.getSelection(m_selection);

	if (m_selection.length() == 0)
		return;

	status = m_selection.getDagPath(0, m_path, m_component);
	CHECK_MSTATUS(status);

	sortSlideDirections();
}

void MoveComponentData::sortSlideDirections(){
	MDagPath dpath;
	MObject component;
	m_selection.getDagPath(0, dpath, component);

	m_slideDirections.clear();

	if (component.apiType() == MFn::kMeshVertComponent) {
		MFnMesh meshFn(dpath);
		MItMeshVertex vertexIt(dpath, component);

		int numVertices = meshFn.numVertices();
		for (int i = 0; i < numVertices; i++)
			m_slideDirections[i] = MIntArray(4, -1);

		MFnSingleIndexedComponent componentFn(component);
		MIntArray vtxList;
		componentFn.getElements(vtxList);

		for (vertexIt.reset(); !vertexIt.isDone(); vertexIt.next()) {
			MIntArray connected;
			vertexIt.getConnectedVertices(connected);

			int connection = connected[0];
			int direction = 0;

			iterateSlideDirections(dpath, vtxList, vertexIt.index(), connection, direction);
		}
	}
}

void MoveComponentData::iterateSlideDirections(MDagPath& path, MIntArray& list, int vertex, int connection, int inDirection){
	int elPos;
	if (!intArrayContains(list, vertex, elPos))
		return;

	MItMeshVertex vertexIt(path);
	int dummyIdx;
	vertexIt.setIndex(vertex, dummyIdx);

	MFnMesh meshFn(path);

	MIntArray vertices;
	vertexIt.getConnectedVertices(vertices);
	unsigned numConnections = vertices.length();

	// Find dorection of incoming connection
	int inDirPos = false;
	if (!intArrayContains(vertices, connection, inDirPos))
		return;

	list.remove(elPos);

	bool boundary[3];
	if (numConnections == 3) {
		for (unsigned b = 0; b<numConnections; b++) {
			vertexIt.setIndex(vertices[b], dummyIdx);
			boundary[b] = vertexIt.onBoundary();
		}
	}

	int maxAngleId = -1;
	if (numConnections == 3 && !vertexIt.onBoundary()) {
		MVectorArray edges;
		for (unsigned v = 0; v < numConnections; v++) {
			MPoint opposite;
			meshFn.getPoint(vertices[v], opposite, MSpace::kWorld);
			edges.append(opposite - vertexIt.position(MSpace::kWorld));
		}
		double maxAngle=0;
		for (unsigned v = 0; v < numConnections; v++) {
			double angle = acos(edges[v].normal() * edges[(v + 1) % numConnections].normal());
			if (angle > maxAngle) {
				maxAngle = angle;
				maxAngleId = v + 1;
			}
		}
	}
	if (numConnections == 2 && vertexIt.onBoundary()) {
		MVectorArray edges;
		for (unsigned v = 0; v < numConnections; v++) {
			MPoint opposite;
			meshFn.getPoint(vertices[v], opposite, MSpace::kWorld);
			edges.append(opposite - vertexIt.position(MSpace::kWorld));
		}
		MVector direction = edges[0] ^ edges[1];
		MVector normal;
		vertexIt.getNormal(normal, MSpace::kWorld);
		if (direction*normal < 0)
			maxAngleId = 1;
	}

	for (unsigned v = 0; v < numConnections; v++) {
			
		int mod = 0;
		switch (numConnections){
		case 2: {
			if (!vertexIt.onBoundary())
				mod = 1;
			else if (vertexIt.onBoundary() && v == maxAngleId)
				mod = 2;
			break;}
		case 3: {
			if (vertexIt.onBoundary() && ((boundary[0] && boundary[1] && v == 1) || (!boundary[0] && v == 2)))
				mod = 1;
			else if (!vertexIt.onBoundary() && v == maxAngleId)
				mod = 1;
			break;}
		}

		int outDirection = (inDirection + v%4 + mod - inDirPos + 2) % 4;
		m_slideDirections[vertex].set(vertices[v], outDirection);
		iterateSlideDirections(path, list, vertices[v], vertex, outDirection);
	}
	/*else {
		unsigned set = 0;
		for (unsigned i = 0; i < 4; i++)
			if (m_slideDirections[vertex][i] > -1)
				set++;
		if (set == 4 || set == vertices.length())
			list.remove(elPos);

		m_slideDirections[vertex].set(vertices[inDirPos], (inDirection + 2) % 4);
	}*/
}

bool MoveComponentData::intArrayContains(MIntArray& list, int element, int& index){
	index = -1;
	for (unsigned i = 0; i < list.length(); i++)
		if (list[i] == element) {
			index = i;
			return true;
		}
	return false;
}
