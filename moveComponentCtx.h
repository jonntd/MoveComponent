#pragma once

#include "moveComponentManipContainer.h"
#include "moveComponentData.h"

#include <maya\MPxSelectionContext.h>
#include <maya\MModelMessage.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Context ////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class MoveComponentCtx : public MPxSelectionContext
{
public:
	MoveComponentCtx();
	virtual ~MoveComponentCtx();

	virtual void			toolOnSetup(MEvent& event);
	virtual void			toolOffCleanup();
	virtual MStatus			doEnterRegion(MEvent & event);
	virtual void			getClassName(MString & name) const;

	// Util
	static void				selectionChanged(void * data);
	void					updateManip();

	MoveComponentData*		data();
	MoveComponentToolCmd*	getToolCommand();

private:
	MCallbackId				m_callbackID;
	MoveComponentData		m_data;
	MoveComponentToolCmd	*m_cmd;
	
};