#include "moveComponentContext.h"
#include "moveComponentConstants.h"

#include <maya\MModelMessage.h>
#include <maya\MToolsInfo.h>
#include <maya\MFnComponent.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Context ////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

MoveComponentContext::MoveComponentContext(){
	setTitleString(kTitleString);
	setImage("moveComponent.xpm", MPxSelectionContext::kImage1);
	m_data = new MoveComponentData();
}

void MoveComponentContext::toolOnSetup(MEvent &){
	MStatus status;

	setHelpString(kHelpString);

	selectionChanged(this);
	m_callbackID = MModelMessage::addCallback(MModelMessage::kActiveListModified, selectionChanged, this, &status);

	updateManip();
}

void MoveComponentContext::getClassName(MString& name) const{
	name = kContextName;
}

void MoveComponentContext::toolOffCleanup(){
	MStatus status(MStatus::kSuccess);

	status = MModelMessage::removeCallback(m_callbackID);
	deleteManipulators();
	MPxSelectionContext::toolOffCleanup();
}

MStatus MoveComponentContext::doEnterRegion(MEvent &){
	return setHelpString(kHelpString);
}

void MoveComponentContext::selectionChanged(void * data){
	MStatus stat = MStatus::kSuccess;

	MoveComponentContext *ctxPtr = (MoveComponentContext *)data;
	MoveComponentData *toolData = ctxPtr->getData();
	toolData->selectionChanged();
	ctxPtr->updateManip();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Common event handling //////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

MStatus MoveComponentContext::doPressCommon(MEvent & event){
	MStatus status(MStatus::kSuccess);

	// Handle selection
	if (event.mouseButton() == MEvent::kLeftMouse){
		if (event.isModifierShift() || event.isModifierControl()) {
			if (event.isModifierShift()) {
				if (event.isModifierControl())
					m_listAdjustment = MGlobal::kAddToList;
				else
					m_listAdjustment = MGlobal::kXORWithList;
			}
			else if (event.isModifierControl())
				m_listAdjustment = MGlobal::kRemoveFromList;
		}
		else
			m_listAdjustment = MGlobal::kReplaceList;
		beginMarquee(event);
	}
	
	return status;
}

MStatus MoveComponentContext::doDragCommon(MEvent & event){
	MStatus status(MStatus::kSuccess);

	if (event.mouseButton() == MEvent::kLeftMouse)
		dragMarquee(event);

	return status;
}

MStatus MoveComponentContext::doReleaseCommon(MEvent & event){
	MStatus status(MStatus::kSuccess);

	// Handle selection
	if (event.mouseButton() == MEvent::kLeftMouse) {
		short top, left, bottom, right;
		releaseMarquee(event, top, left, bottom, right);

		MSelectionList incomingList, marqueeList;
		MGlobal::getActiveSelectionList(incomingList);

		if (abs(left - right) < 2 && abs(top - bottom) < 2)
			MGlobal::selectFromScreen(left, top, MGlobal::kReplaceList, MGlobal::selectionMethod());
		else
			MGlobal::selectFromScreen(left, top, right, bottom, MGlobal::kReplaceList, MGlobal::kWireframeSelectMethod);

		MGlobal::getActiveSelectionList(marqueeList);
		MGlobal::setActiveSelectionList(incomingList, MGlobal::kReplaceList);
		MGlobal::selectCommand(marqueeList, m_listAdjustment);
	}

	return status;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Tool settings //////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

MoveComponentData* MoveComponentContext::getData() {
	return m_data;
}

MoveComponentTool* MoveComponentContext::getToolCommand(){
	return (MoveComponentTool*)newToolCommand();
}

void MoveComponentContext::setDirty(){
	MToolsInfo::setDirtyFlag(*this);
}

void MoveComponentContext::updateManip() {
	deleteManipulators();

	if (m_data->getComponent()==MObject::kNullObj)
		return;

	MString manipName(kManipContainerName);
	MObject manipObject;
	MoveComponentManipContainer *manip = (MoveComponentManipContainer *)MoveComponentManipContainer::newManipulator(manipName, manipObject);
	if (NULL != manip) {
		addManipulator(manipObject);
		manip->setData(m_data);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Legacy viewport event handling /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


MStatus MoveComponentContext::doPress(MEvent & event){
	return doPressCommon(event);
}

MStatus MoveComponentContext::doDrag(MEvent & event){	
	return doDragCommon(event);
}


MStatus MoveComponentContext::doRelease(MEvent & event){
	return doReleaseCommon(event);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Viewport 2.0 event handling ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

MStatus MoveComponentContext::doPress(MEvent & event, MHWRender::MUIDrawManager& drawMgr, const MHWRender::MFrameContext& context){
	return doPressCommon(event);
}

MStatus MoveComponentContext::doRelease(MEvent & event, MHWRender::MUIDrawManager& drawMgr, const MHWRender::MFrameContext& context){
	return doReleaseCommon(event);
}

MStatus MoveComponentContext::doDrag(MEvent & event, MHWRender::MUIDrawManager& drawMgr, const MHWRender::MFrameContext& context){
	return doDragCommon(event);
}