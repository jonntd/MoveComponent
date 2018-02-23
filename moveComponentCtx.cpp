#include "moveComponentCtx.h"
#include "moveComponentConstants.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Context ////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

MoveComponentCtx::MoveComponentCtx(){
	setTitleString(kTitleString);
	setImage("moveComponent.xpm", MPxSelectionContext::kImage1);
}

MoveComponentCtx::~MoveComponentCtx() {
}

void MoveComponentCtx::getClassName(MString& name) const {
	name = kContextName;
}

void MoveComponentCtx::toolOnSetup(MEvent &){
	MStatus status;

	setHelpString(kHelpString);

	selectionChanged(this);
	m_callbackID = MModelMessage::addCallback(MModelMessage::kActiveListModified, selectionChanged, this, &status);

	updateManip();
}
void MoveComponentCtx::toolOffCleanup(){
	MStatus status;

	MModelMessage::removeCallback(m_callbackID);
	deleteManipulators();

	MPxSelectionContext::toolOffCleanup();
}

MStatus MoveComponentCtx::doEnterRegion(MEvent &){
	return setHelpString(kHelpString);
}

void MoveComponentCtx::selectionChanged(void * data){
	MoveComponentCtx *ctxPtr = (MoveComponentCtx *)data;

	if (ctxPtr == NULL)
		return;

	MoveComponentData *toolData = ctxPtr->data();
	toolData->selectionChanged();
	ctxPtr->updateManip();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Tool settings //////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

MoveComponentData* MoveComponentCtx::data() {
	return &m_data;
}

MoveComponentToolCmd* MoveComponentCtx::getToolCommand(){
	return (MoveComponentToolCmd*)newToolCommand();
}

void MoveComponentCtx::updateManip() {
	deleteManipulators();

	if (m_data.getComponent().isNull())
		return;

	MString manipName(kManipContainerName);
	MObject manipObject;
	MoveComponentManipContainer *manip = (MoveComponentManipContainer *)MoveComponentManipContainer::newManipulator(manipName, manipObject);
	if (NULL != manip) {
		addManipulator(manipObject);
		manip->setData(&m_data);
	}
}