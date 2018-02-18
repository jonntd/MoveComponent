#pragma once

#include "moveComponentManipContainer.h"
#include "moveComponentData.h"
#include "moveComponentTool.h"

#include <maya\MSelectionList.h>
#include <maya\MGlobal.h>
#include <maya\MPxSelectionContext.h>
#include <maya\MCallbackIdArray.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Context ////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class MoveComponentContext : public MPxSelectionContext
{
public:
	MoveComponentContext();

	virtual void			toolOnSetup(MEvent& event);
	virtual void			toolOffCleanup();
	virtual MStatus			doEnterRegion(MEvent & event);
	virtual void			getClassName(MString & name) const;

	// Legacy viewport
	virtual MStatus			doPress(MEvent & event);
	virtual MStatus			doDrag(MEvent & event);
	virtual MStatus			doRelease(MEvent & event);

	// Viewport 2.0
	virtual MStatus			doPress(MEvent & event, MHWRender::MUIDrawManager& drawMgr, const MHWRender::MFrameContext& context);
	virtual MStatus			doRelease(MEvent & event, MHWRender::MUIDrawManager& drawMgr, const MHWRender::MFrameContext& context);
	virtual MStatus			doDrag(MEvent & event, MHWRender::MUIDrawManager& drawMgr, const MHWRender::MFrameContext& context);

	// Util
	static void				selectionChanged(void * data);
	void					setDirty();
	void					updateManip();

	MoveComponentData*		getData();
	MoveComponentTool*		getToolCommand();

private:
	//Tool data
	MoveComponentData				*m_data;
	MoveComponentTool				*m_cmd;

	// Common event handling
	MStatus                 doPressCommon(MEvent& event);
	MStatus                 doDragCommon(MEvent& event);
	MStatus                 doReleaseCommon(MEvent& event);

	// Component handling
	MCallbackId				m_callbackID;

	short                   m_startX,
							m_startY;
	MGlobal::ListAdjustment m_listAdjustment;
};