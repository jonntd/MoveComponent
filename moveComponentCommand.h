#pragma once

#include "moveComponentContext.h"

#include <maya\MPxContextCommand.h>

class MoveComponentCommand : public MPxContextCommand
{
public:
	MoveComponentCommand();
	static void* creator();
	virtual MPxSelectionContext * makeObj();

	virtual MStatus     doEditFlags();
	virtual MStatus     doQueryFlags();
	virtual MStatus		appendSyntax();

private:
	MoveComponentContext	*context;
	MoveComponentData		*data;
};