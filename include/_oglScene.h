#pragma once

// ************************************************************************************************
class _oglScene
{

public: // Methods

	_oglScene()
	{
	}

	virtual ~_oglScene()
	{
	}

	virtual void _update(
		int iViewportX, int iViewportY,
		int iViewportWidth, int iViewportHeight,
		bool bClear,
		bool bTranslate) {}
};