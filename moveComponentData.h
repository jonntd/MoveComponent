#pragma once

#include "moveComponentConstants.h"

#include <maya\MString.h>
#include <maya\MDoubleArray.h>
#include <maya\MSelectionList.h>
#include <maya\MDagPath.h>
#include <maya\MIntArray.h>
#include <maya\MPointArray.h>
#include <maya\MVectorArray.h>
#include <maya\MString.h>
#include <map>

class MoveComponentCtx;

class MoveComponentData
{
public:
	MoveComponentData();
	~MoveComponentData();

	// Set data
	void					setContext(MoveComponentCtx *context);
	void					setSpace(int space);
	void					setOrientation(double orientation[3]);
	void					setMode(int mode);
	void					setAxis(int axis);
	void					setSensitivity(double sensitivity);
	void					setPreset(int preset);
	void					setStep(bool doStep);
	void					setDrag(bool drag);
	void					setDelta(double delta);

	// Get data
	MoveComponentCtx*	getContext();
	int						getSpace();
	MDoubleArray			getOrientation();
	int						getMode();
	int						getAxis();
	double					getSensitivity();
	int						getPreset();
	bool					getStep();
	bool					getDrag();
	double					getDelta();
	void					getPoints(MPointArray& points);
	MDagPath				getPath();
	MObject					getComponent();
	MStatus					getMatrix(MMatrix& matrix);
	MStatus					getLinks(int direction, MIntArray &links);

	// Sync option variables
	MString					fullName(MString& variable);
	MStatus					syncIntOptionVar(MString& variable, int& value, bool edit = false);
	MStatus					syncDoubleOptionVar(MString& variable, double& value, bool edit = false);

	// Sort slide directions
	void					selectionChanged();
	void					sortSlideDirections();
	void					iterateSlideDirections(MDagPath& path, MIntArray& list, int vertex, int connection, int inDirection);
	static bool				intArrayContains(MIntArray& list, int element, int& index);

private:
	MoveComponentCtx *m_context;

	// Settings	
	double	m_orientation[3],
			m_sensitivity;
	int		m_mode,
			m_axis,
			m_space,
			m_preset;
	bool	m_doStep;

	// Runtime
	bool	m_drag;
	double	m_delta;

	// Selection
	MSelectionList	m_selection;
	MDagPath		m_path;
	MObject			m_component;
	std::map <unsigned, MIntArray> m_slideDirections;
};

