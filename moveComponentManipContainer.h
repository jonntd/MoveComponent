#pragma once

#include "moveComponentTool.h"
#include "moveComponentManip.h"
#include "moveComponentData.h"

#include <maya\MPxManipContainer.h>
#include <map>

class MoveComponentManipContainer : public MPxManipContainer
{
public:
	MoveComponentManipContainer();
	virtual ~MoveComponentManipContainer();

	static void *	creator();
	static MStatus	initialize();
	virtual MStatus createChildren();
	virtual MStatus connectToDependNode(const MObject &node);
	virtual void	draw(M3dView &view, const MDagPath &path, M3dView::DisplayStyle style, M3dView::DisplayStatus status);
	
	//Viewport 2.0 override
	virtual void	preDrawUI(const M3dView& view);
	virtual void	drawUI(MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext) const;
	
	void	setData(MoveComponentData *data);
	MoveComponentManip* createMoveManip(MString& manipName, int mode = MoveComponent::Mode::kTranslate, int direction = MoveComponent::Direction::kXaxis);

public:
	static MTypeId		id;
	
	MoveComponentData	*m_data;
	MoveComponentTool	*m_cmd;
	MoveComponentManip	*m_Xptr,
						*m_Yptr,
						*m_Zptr,
						*m_normalPtr,
						*m_Nptr,
						*m_Uptr,
						*m_Vptr,
						*m_Wptr;
};