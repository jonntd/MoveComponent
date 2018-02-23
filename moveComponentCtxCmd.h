#pragma once

#include "moveComponentCtx.h"

#include <maya\MPxContextCommand.h>

class MoveComponentCtxCmd : public MPxContextCommand
{
public:
	MoveComponentCtxCmd();
	static void* creator();
	virtual MPxSelectionContext * makeObj();

	virtual MStatus     doEditFlags();
	virtual MStatus     doQueryFlags();
	virtual MStatus		appendSyntax();

private:
	MoveComponentCtx	*m_context;
};