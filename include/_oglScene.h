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

	virtual void _getDimensions(int& /*iWidth*/, int& /*iHeight*/) {}

	virtual void _prepare(
		bool /*bPerspecitve*/,
		int /*iViewportX*/, int /*iViewportY*/,
		int /*iViewportWidth*/, int /*iViewportHeight*/,
		float /*fXmin*/, float /*fXmax*/,
		float /*fYmin*/, float /*fYmax*/,
		float /*fZmin*/, float /*fZmax*/,
		bool /*bClear*/,
		bool /*bTranslate*/) {
	}
};