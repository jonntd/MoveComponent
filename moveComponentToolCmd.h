#pragma once
#include "moveComponentData.h"

#include <maya\MPxToolCommand.h>
#include <maya\MPointArray.h>
#include <maya\MMatrix.h>

class MoveComponentToolCmd : public MPxToolCommand
{
public:
	MoveComponentToolCmd();
	virtual         ~MoveComponentToolCmd();
	static void*    creator();

	MStatus         doIt(const MArgList& args);
	MStatus         redoIt();
	MStatus         undoIt();
	bool            isUndoable() const;
	MStatus         finalize();

	MStatus			transformComponent();

	void			setData(MoveComponentData* toolData);

private:
	MoveComponentData *m_data;

	MPointArray		m_oldPoints,
					m_newPoints;
	MVectorArray	m_vectors;
	bool			m_didOnce,
					m_doStep;
	int				m_mode,
					m_axis;
	double			m_sensitivity;
	MMatrix			m_matrix;
	MDagPath		m_path;
	MObject			m_component;
};