#include "moveComponentManip.h"
#include "moveComponentConstants.h"
#include "moveComponentCtx.h"
#include "../_library/SCamera.h"

#include <maya\MGlobal.h>
#include <maya\MQuaternion.h>
#include <maya\MVectorArray.h>
#include <maya\MItGeometry.h>
#include <maya\MFnManip3D.h>
#include <maya\MFnCamera.h>
#include <maya\MAngle.h>
#include <maya\MFnMesh.h>
#include <maya\MToolsInfo.h>

MTypeId MoveComponentManip::id(0x00127886);

MoveComponentManip::MoveComponentManip(){
}

MoveComponentManip::~MoveComponentManip(){
}

void* MoveComponentManip::creator() {
	return new MoveComponentManip();
}

MStatus MoveComponentManip::initialize() {
	return MStatus::kSuccess;
}

void MoveComponentManip::draw(M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status) {
	setDirty();
	
	if (m_manipMode != m_mode)
		return;

	m_drawColor = manipColor();
	double handleSize = MFnManip3D::globalSize()*MFnManip3D::handleSize();
	
	view.beginGL();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glLineWidth(MFnManip3D::lineSize());
	glFirstHandle(m_arrowName);

	if(m_manipMode == MoveComponent::Mode::kTranslate){
		if (manipGeometry(m_manipPoint, m_manipVector)) {
			colorAndName(view, m_arrowName, true, m_drawColor);
			(m_direction == MoveComponent::Direction::kNormal)	? drawArrow(view, m_manipPoint, m_manipVector, handleSize*kNormalManipLength, false, "N")
																: drawArrow(view, m_manipPoint, m_manipVector, handleSize*kManipLength);
		}
	}
	else if(m_manipMode == MoveComponent::Mode::kSlide){
		if (manipGeometry(m_manipPoints, m_manipVectors)) {
			for (unsigned i = 0; i < m_manipPoints.length(); i++) {
				if (m_manipVectors[i].length() == 0)
					continue;
				colorAndName(view, m_arrowName + i, true, m_drawColor);
				drawArrow(view, m_manipPoints[i], m_manipVectors[i], handleSize*kSlideManipLength);
			}
		}
	}

	glPopAttrib();
	view.endGL();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Viewport 2.0 ///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void MoveComponentManip::preDrawUI(const M3dView &view){
	bool drawSelected = false;

	m_view = view;

	switch (m_mode){
	case MoveComponent::Mode::kTranslate: {
		manipGeometry(m_manipPoint, m_manipVector);
		shouldDrawHandleAsSelected(m_arrowName, drawSelected);
		break;}
	case MoveComponent::Mode::kSlide: {
		m_manipPoints.clear(); m_manipVectors.clear();
		manipGeometry(m_manipPoints, m_manipVectors);

		for (unsigned i = 0; i < m_manipPoints.length(); i++){
			bool selected = false;
			shouldDrawHandleAsSelected(m_arrowName+i, selected);
			if (selected) {
				drawSelected = true;
				break;
			}
		}
		break;}
	}

	m_drawColor = (drawSelected) ? selectedColor() : manipColor();
}

void MoveComponentManip::drawUI(MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext) const{
	if (m_manipMode != m_mode)
		return;

	double handleSize = MFnManip3D::globalSize()*MFnManip3D::handleSize();

	if (m_manipMode == MoveComponent::Mode::kTranslate) {
		drawManager.beginDrawable(m_arrowName, true);
		drawManager.setLineWidth(MFnManip3D::lineSize());
		drawManager.setColorIndex(m_drawColor);

		double scale = -SCamera::scaleFactor(M3dView(m_view), MPoint(m_manipPoint));
		double length = (m_direction == MoveComponent::Direction::kNormal) ? handleSize*kNormalManipLength : handleSize*kManipLength;
		MVector normal = m_manipVector.normal();
		MPoint pointA = m_manipPoint;
		MPoint pointB = pointA + normal * length * scale;

		drawManager.line(pointA, pointB);
		(m_direction != MoveComponent::Direction::kNormal)	? drawManager.cone(pointB, normal, kConeRadius*scale, kConeHeight*scale, true)
															: drawManager.text(pointB, "N");
		drawManager.endDrawable();
	}
	else if (m_manipMode == MoveComponent::Mode::kSlide) {
		for (unsigned i = 0; i < m_manipPoints.length(); i++) {
			if (m_manipVectors[i].length() == 0)
				continue;

			drawManager.beginDrawable(m_arrowName+i, true);
			drawManager.setLineWidth(MFnManip3D::lineSize());
			drawManager.setColorIndex(m_drawColor);

			double scale = -SCamera::scaleFactor(M3dView(m_view), MPoint(m_manipPoints[i]));
			double length = handleSize*kSlideManipLength;
			MVector normal = m_manipVectors[i].normal();
			MPoint pointA = m_manipPoints[i];
			MPoint pointB = pointA + normal * length * scale;

			drawManager.line(pointA, pointB);
			drawManager.cone(pointB, normal, kConeRadius*scale, kConeHeight*scale, true);
			drawManager.endDrawable();
		}
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Event handling /////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

MStatus MoveComponentManip::doPress(M3dView& view){
	if ((m_mode == MoveComponent::Mode::kSlide && m_component.apiType() != MFn::kMeshVertComponent) || m_data->getComponent() == MObject::kNullObj)
		return MS::kUnknownParameter;

	// Get start point for measuring delta
	setPlane();
	MPoint point;
	MVector	ray;
	double param;
	mouseRayWorld(point, ray);
	m_plane.intersect(point, ray, m_start, param);

	// Set context parameters
	m_data->setDrag(true);
	m_data->setMode(m_manipMode);
	m_data->setAxis(m_direction);

	MoveComponentCtx* context = m_data->getContext();
	MToolsInfo::setDirtyFlag(*context);

	// Create and set tool command
	m_cmd = context->getToolCommand();
	m_cmd->setData(m_data);
	
	return MS::kUnknownParameter;
}

MStatus MoveComponentManip::doDrag(M3dView& view){
	if ((m_mode == MoveComponent::Mode::kSlide && m_component.apiType() != MFn::kMeshVertComponent) || m_data->getComponent() == MObject::kNullObj)
		return MS::kUnknownParameter;

	// Get current drag point
	MPoint point, last;
	MVector ray;
	double param;
	mouseRayWorld(point, ray);
	m_plane.intersect(point, ray, last, param);

	// Measure delta
	MVector direction = last - m_start;
	double delta = direction.length();
	m_start = last;

	double angle = m_vector.normal()*direction.normal();
	if (angle < 0)
		delta *= -1;

	// Execute tool command
	m_data->setDelta(delta);
	m_cmd->redoIt();

	return MS::kUnknownParameter;
}

MStatus MoveComponentManip::doRelease(M3dView& view){
	if ((m_mode == MoveComponent::Mode::kSlide && m_component.apiType() != MFn::kMeshVertComponent) || m_data->getComponent() == MObject::kNullObj)
		return MS::kUnknownParameter;

	// Finalize tool command
	m_data->setDrag(false);
	m_cmd->finalize();

	return MS::kUnknownParameter;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Set manipulator ////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void MoveComponentManip::setData(MoveComponentData *data){
	m_data = data;
	setDirty();
}

void MoveComponentManip::setDirection(int direction) {
	m_direction = direction;
}

void MoveComponentManip::setMode(int mode) {
	m_manipMode = mode;
}

void MoveComponentManip::setDirty() {
	m_mode = m_data->getMode();
	m_data->getMatrix(m_matrix);
	m_path = m_data->getPath();
	m_component = m_data->getComponent();
	m_axis = m_data->getAxis();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions ///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool MoveComponentManip::manipGeometry(MPoint& point, MVector& vector) {
		point = averagePosition();

		switch (m_direction){
		case MoveComponent::Direction::kXaxis:
			vector = MVector::xAxis*m_matrix;
			break;
		case MoveComponent::Direction::kYaxis:
			vector = MVector::yAxis*m_matrix;
			break;
		case MoveComponent::Direction::kZaxis:
			vector = MVector::zAxis*m_matrix;
			break;
		case MoveComponent::Direction::kNormal:
			vector = MVector::one*m_matrix;
			break;
		default:
			return false;
			break;
		}

		return true;
}

bool MoveComponentManip::manipGeometry(MPointArray &points, MVectorArray &vectors) {
	points.clear(); vectors.clear();
	
	if (m_component.apiType() != MFn::kMeshVertComponent || m_path.apiType()!=MFn::kMesh)
		return false;
	
	MIntArray links;
	m_data->getLinks(m_direction, links);
	
	MFnMesh meshFn(m_path);
	MPointArray meshPoints;
	meshFn.getPoints(meshPoints, MSpace::kWorld);

	MItGeometry geoIt(m_path, m_component);

	for (geoIt.reset(); !geoIt.isDone(); geoIt.next()){
		MPoint pointA, pointB;
		pointA = pointB = geoIt.position(MSpace::kWorld);
		int indexB = links[geoIt.index()];
		if(indexB>0)
			pointB = meshPoints[indexB];
		MVector slideDirection = pointB - pointA;
		points.append(pointA);
		vectors.append(slideDirection.normal());
	}

	return true;
}

int MoveComponentManip::manipColor(){
	M3dView view = M3dView::active3dView();

	int color;
	switch (m_direction){
	case MoveComponent::Direction::kXaxis:
		color = xColor();
		break;
	case MoveComponent::Direction::kYaxis:
		color = yColor();
		break;
	case MoveComponent::Direction::kZaxis:
		color = zColor();
		break;
	case MoveComponent::Direction::kNormal:
		color = mainColor();
		break;
	case MoveComponent::Direction::kNslide:
		color = view.userDefinedColorIndex(0);
		break;
	case MoveComponent::Direction::kUslide:
		color = view.userDefinedColorIndex(1);
		break;
	case MoveComponent::Direction::kVslide:
		color = view.userDefinedColorIndex(2);
		break;
	case MoveComponent::Direction::kWslide:
		color = view.userDefinedColorIndex(3);
		break;
	}
	return color;
}

void MoveComponentManip::setPlane(){
	MPoint	point;
	MVector v, ray, normal;
	mouseRayWorld(point, ray);

	switch (m_manipMode){
	case MoveComponent::Mode::kTranslate:
		manipGeometry(point, m_vector);
		break;
	case MoveComponent::Mode::kSlide:{
		MGLuint activeHandle;
		glActiveName(activeHandle);
		
		MPointArray points;
		MVectorArray vectors;
		manipGeometry(points, vectors);

		point = points[activeHandle - m_arrowName];
		m_vector = vectors[activeHandle - m_arrowName];
		break;}
	}

	v = m_vector^ray;
	normal = m_vector^v;

	m_plane.setOrigin(point);
	m_plane.setNormal(normal);
}

MPoint MoveComponentManip::averagePosition() {
	MPointArray selectedPoints;
	m_data->getPoints(selectedPoints);
	MVector sumPosition;

	for (unsigned i = 0; i < selectedPoints.length(); i++)
		sumPosition += selectedPoints[i];

	sumPosition /= selectedPoints.length();

	return sumPosition;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Open gl functions /////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MoveComponentManip::drawArrow(M3dView &view, MPoint& point, MVector& direction, double length, bool drawCone, char * label) {
	double scale = -SCamera::scaleFactor(view, point);

	MVector normal = direction.normal();

	MPoint pointA = point;
	MPoint pointB = pointA + normal * length * scale;
	MPoint pointC = pointB + normal * kConeHeight * scale;

	double start[4];
	double end[4];
	pointA.get(start);
	pointB.get(end);

	glBegin(GL_LINES);
	glVertex4dv(start);
	glVertex4dv(end);
	glEnd();

	if(drawCone){
		MQuaternion rotation = MVector::zAxis.rotateTo(normal);

		glBegin(GL_TRIANGLE_FAN);

		double top[4];
		pointC.get(top);
		glVertex4dv(top);

		for (int i = 0; i <= kConeDivisions; i++) {
			float angle = 360 / kConeDivisions*float(i);
			MAngle agl(angle, MAngle::kDegrees);
			MVector vector(sin(agl.asRadians()) * kConeRadius, cos(agl.asRadians()) * kConeRadius, -kConeHeight);
			vector = vector.rotateBy(rotation);
			MPoint basePoint = pointC + vector * scale;
			double base[4];
			basePoint.get(base);
			glVertex4dv(base);
		}
	}
	if (label != ""){
		M3dView view = M3dView::active3dView();
		view.drawText(label, pointB, M3dView::TextPosition::kLeft);
	}

	glEnd();
}