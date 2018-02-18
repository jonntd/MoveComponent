#pragma once

#define kContextName			"moveComponentContext"
#define kCommandName			"moveComponentCmd"
#define kManipName				"moveComponentManip"
#define kManipContainerName		"moveComponentManipContainer"

#define kTitleString			"Move Component Tool"
#define kHelpString				"Use manipulator to transform selected components"

#define kSpaceFlag				"-sp"
#define kSpaceFlagLong			"-space"
#define kOrientationFlag		"-o"
#define kOrientationFlaglong	"-orientation"
#define kModeFlag				"-m"
#define kModeFlagLong			"-mode"
#define kSensitivityFlag		"-s"
#define kSensitivityFlagLong	"-sensitivity"
#define kPresetFlag				"-p"
#define kPresetFlagLong			"-preset"
#define kStepFlag				"-st"
#define kStepFlagLong			"-step"

#define kConeHeight				3.5
#define kConeRadius				0.75
#define kConeDivisions			12
#define	kManipLength			0.35
#define	kNormalManipLength		0.25
#define	kSlideManipLength		0.025

class MoveComponent
{
public:
	enum Direction {
		kXaxis,
		kYaxis,
		kZaxis,
		kNormal,
		kNslide,
		kUslide,
		kVslide,
		kWslide
	};

	enum Mode {
		kTranslate,
		kSlide
	};

	enum Space {
		kObject,
		kWorld,
		kParent,
		kCustom
	};
};