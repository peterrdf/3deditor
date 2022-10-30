#pragma once

#include <string>

class _color
{

private: // Members

	float m_fR;
	float m_fG;
	float m_fB;

public: // Methods

	_color()
	{
		init(0.f, 0.f, 0.f);
	}

	_color(float fR, float fG, float fB)
	{
		init(fR, fG, fB);
	}

	void init(float fR, float fG, float fB)
	{
		m_fR = fR;
		m_fG = fG;
		m_fB = fB;
	}

	void init(unsigned int iColor)
	{
		m_fR = (float)(iColor & ((unsigned int)255 * 256 * 256 * 256)) / (256 * 256 * 256);
		m_fR /= 255.f;

		m_fG = (float)(iColor & (255 * 256 * 256)) / (256 * 256);
		m_fG /= 255.f;

		m_fB = (float)(iColor & (255 * 256)) / 256;
		m_fB /= 255.f;
	}

	float r() const
	{
		return m_fR;
	}

	float g() const
	{
		return m_fG;
	}

	float b() const
	{
		return m_fB;
	}

	bool operator == (const _color& c) const
	{
		return (m_fR == c.m_fR) && (m_fG == c.m_fG) && (m_fB == c.m_fB);
	}

	bool operator < (const _color& c) const
	{
		if (m_fR < c.m_fR)
		{
			return true;
		}

		if (m_fR > c.m_fR)
		{
			return false;
		}

		if (m_fG < c.m_fG)
		{
			return true;
		}

		if (m_fG > c.m_fG)
		{
			return false;
		}

		if (m_fB < c.m_fB)
		{
			return true;
		}

		if (m_fB > c.m_fB)
		{
			return false;
		}

		return false;
	}

	bool operator > (const _color& c) const
	{
		if (m_fR > c.m_fR)
		{
			return true;
		}

		if (m_fR < c.m_fR)
		{
			return false;
		}

		if (m_fG > c.m_fG)
		{
			return true;
		}

		if (m_fG < c.m_fG)
		{
			return false;
		}

		if (m_fB > c.m_fB)
		{
			return true;
		}

		if (m_fB < c.m_fB)
		{
			return false;
		}

		return false;
	}
};

class _material
{

private: // Members

	_color m_clrAmbient;
	_color m_clrDiffuse;
	_color m_clrEmissive;
	_color m_clrSpecular;

	float m_fA;

	std::wstring m_strTexture;

public: // Methods

	_material()
	{
		init(0, 0, 0, 0, 0.f, nullptr);
	}

	_material(
		unsigned int iAmbientColor, 
		unsigned int iDiffuseColor, 
		unsigned int iEmissiveColor, 
		unsigned int iSpecularColor, 
		float fTransparency)
	{
		init(iAmbientColor, iDiffuseColor, iEmissiveColor, iSpecularColor, fTransparency, nullptr);
	}

	void init(
		unsigned int iAmbientColor,
		unsigned int iDiffuseColor,
		unsigned int iEmissiveColor,
		unsigned int iSpecularColor,
		float fTransparency,
		const wchar_t* szTexture)
	{
		if ((iAmbientColor == 0) && (iDiffuseColor == 0) && (iEmissiveColor == 0) && (iSpecularColor == 0) && (fTransparency == 0.f))
		{
			// There is no material - use non-transparent black
			m_clrAmbient.init(0);
			m_clrDiffuse.init(0);
			m_clrEmissive.init(0);
			m_clrSpecular.init(0);

			m_fA = 1.f;
		}
		else
		{
			m_clrAmbient.init(iAmbientColor);
			m_clrDiffuse.init(iDiffuseColor == 0 ? iAmbientColor : iDiffuseColor);
			m_clrEmissive.init(iEmissiveColor);
			m_clrSpecular.init(iSpecularColor);

			m_fA = fTransparency;
		}

		m_strTexture = szTexture != nullptr ? szTexture : L"";
	}

	void init(
		float fAmbientR, float fAmbientG, float fAmbientB,
		float fDiffuseR, float fDiffuseG, float fDiffuseB,
		float fEmissiveR, float fEmissiveG, float fEmissiveB,
		float fSpecularR, float fSpecularG, float fSpecularB,
		float fTransparency, 
		const wchar_t* szTexture)
	{
		m_clrAmbient.init(fAmbientR, fAmbientG, fAmbientB);
		m_clrDiffuse.init(fDiffuseR, fDiffuseG, fDiffuseB);
		m_clrEmissive.init(fEmissiveR, fEmissiveG, fEmissiveB);
		m_clrSpecular.init(fSpecularR, fSpecularG, fSpecularB);

		m_fA = fTransparency;

		m_strTexture = (szTexture != nullptr ? szTexture : L"");
	}

	const _color& getAmbientColor() const
	{
		return m_clrAmbient;
	}

	const _color& getDiffuseColor() const
	{
		return m_clrDiffuse;
	}

	const _color& getEmissiveColor() const
	{
		return m_clrEmissive;
	}

	const _color& getSpecularColor() const
	{
		return m_clrSpecular;
	}

	float getA() const
	{
		return m_fA;
	}	

	const std::wstring& texture() const
	{
		return m_strTexture;
	}

	bool hasTexture() const
	{
		return m_strTexture.empty();
	}
};

