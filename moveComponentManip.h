#pragma once

#include "moveComponentData.h"
#include "moveComponentTool.h"
#include "SPlane.h"

#include <maya\MPxManipulatorNode.h>
#include <maya\MVectorArray.h>
#include <maya\MPointArray.h>

class MoveComponentManip : MPxManipulatorNode
{
public:
	MoveComponentManip();
	virtual			~MoveComponentManip();

	static void*	creator();
	static MStatus	initialize();

	virtual void	draw(M3dView &view, const MDagPath &path, M3dView::DisplayStyle style, M3dView::DisplayStatus status);
	static void		drawArrow(M3dView &view, MPoint &point, MVector &direction, double length, bool drawCone = true, char* label = "");

	// Viewport 2.0 Override
	virtual void	preDrawUI(const M3dView &view);
	virtual void	drawUI(MHWRender::MUIDrawManager &drawManager, const MHWRender::MFrameContext &frameContext) const;

	void			setData(MoveComponentData *data);
	void			setDirection(int direction);
	void			setMode(int mode);
	void			setDirty();

	void			setPlane();
	MPoint			averagePosition();
	bool			manipGeometry(MPoint &point, MVector &vector);
	bool			manipGeometry(MPointArray &points, MVectorArray &vectors);
	int				manipColor();

	virtual MStatus	doPress(M3dView &veiw);
	virtual MStatus	doDrag(M3dView &view);
	virtual MStatus	doRelease(M3dView &view);

	static MTypeId	id;
private:
	MoveComponentData	*m_data;
	MoveComponentTool	*m_cmd;

	MPoint			m_start;
	SPlane			m_plane;
	MVector			m_vector;

	MGLuint			m_arrowName;
	int				m_direction;
	int				m_manipMode;
	short			m_drawColor;
	int				m_mode,
					m_axis;
	MMatrix			m_matrix;
	MDagPath		m_path;
	MObject			m_component;

	MPoint			m_manipPoint;
	MPointArray		m_manipPoints;
	MVector			m_manipVector;
	MVectorArray	m_manipVectors;
	M3dView			m_view;
};

