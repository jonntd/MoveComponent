#include "moveComponentManipContainer.h"
#include "moveComponentConstants.h"
#include "../_library/SCamera.h"

#include <maya\MSelectionList.h>
#include <maya\MRichSelection.h>
#include <maya\MGlobal.h>
#include <maya\MFnMesh.h>
#include <maya\MItMeshVertex.h>
#include <maya\MFnSingleIndexedComponent.h>

MTypeId MoveComponentManipContainer::id(0x00127885);

MoveComponentManipContainer::MoveComponentManipContainer(){
}

MoveComponentManipContainer::~MoveComponentManipContainer(){
}

void *MoveComponentManipContainer::creator(){
	return new MoveComponentManipContainer();
}

MStatus MoveComponentManipContainer::initialize(){
	MStatus status;
	status = MPxManipContainer::initialize();
	return status;
}

MStatus MoveComponentManipContainer::createChildren(){
	MStatus status(MStatus::kSuccess);
	
	// Translate
	m_Xptr = createMoveManip(MString("XmoveManip"), MoveComponent::Mode::kTranslate, MoveComponent::Direction::kXaxis);
	m_Yptr = createMoveManip(MString("YmoveManip"), MoveComponent::Mode::kTranslate, MoveComponent::Direction::kYaxis);
	m_Zptr = createMoveManip(MString("ZmoveManip"), MoveComponent::Mode::kTranslate, MoveComponent::Direction::kZaxis);
	
	// Normal
	m_normalPtr = createMoveManip(MString("normalMoveManip"), MoveComponent::Mode::kTranslate, MoveComponent::Direction::kNormal);
	
	// Slide
	m_Nptr = createMoveManip(MString("NslideManip"), MoveComponent::Mode::kSlide, MoveComponent::Direction::kNslide);
	m_Uptr = createMoveManip(MString("UslideManip"), MoveComponent::Mode::kSlide, MoveComponent::Direction::kUslide);
	m_Vptr = createMoveManip(MString("VslideManip"), MoveComponent::Mode::kSlide, MoveComponent::Direction::kVslide);
	m_Wptr = createMoveManip(MString("WslideManip"), MoveComponent::Mode::kSlide, MoveComponent::Direction::kWslide);
	
	return status;
}

MStatus MoveComponentManipContainer::connectToDependNode(const MObject &node){
	return MStatus::kSuccess;
}

void MoveComponentManipContainer::draw(M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status){
	MPxManipContainer::draw(view, path, style, status);
}

void MoveComponentManipContainer::preDrawUI(const M3dView &view){

}

void MoveComponentManipContainer::drawUI(MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext) const{
	
}

void MoveComponentManipContainer::setData(MoveComponentData* data){
	m_data = data;
	
	m_Xptr->setData(data);
	m_Yptr->setData(data);
	m_Zptr->setData(data);
	m_normalPtr->setData(data);
	m_Nptr->setData(data);
	m_Uptr->setData(data);
	m_Vptr->setData(data);
	m_Wptr->setData(data);
}

MoveComponentManip* MoveComponentManipContainer::createMoveManip(MString& manipName, int mode, int direction)
{
	MStatus status(MStatus::kSuccess);

	MPxManipulatorNode *proxyManip = 0;
	status = addMPxManipulatorNode(kManipName, manipName, proxyManip);
	CHECK_MSTATUS(status);
	MoveComponentManip *ptr = (MoveComponentManip*)proxyManip;
	if (ptr) {
		ptr->setMode(mode);
		ptr->setDirection(direction);
	}

	return ptr;
}