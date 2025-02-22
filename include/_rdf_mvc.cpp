#include "_host.h"
#include "_rdf_mvc.h"
#include "_ptr.h"

// ************************************************************************************************
_rdf_model::_rdf_model()
	: _model()
	, m_mapTextures()
{
}

/*virtual*/ _rdf_model::~_rdf_model()
{
	for (auto itTexture : m_mapTextures)
	{
		delete itTexture.second;
	}
}

_texture* _rdf_model::getTexture(const wstring& strTexture)
{
	if (!m_strPath.empty())
	{
		if (m_mapTextures.find(strTexture) != m_mapTextures.end())
		{
			return m_mapTextures.at(strTexture);
		}

		fs::path pthFile = m_strPath;
		fs::path pthTexture = pthFile.parent_path();
		pthTexture.append(strTexture);

		if (fs::exists(pthTexture))
		{
			auto pTexture = new _texture();
			pTexture->load(pthTexture.wstring().c_str());

			m_mapTextures[strTexture] = pTexture;

			return pTexture;
		}
	} // if (!m_strModel.empty())

	return getDefaultTexture();
}

// ************************************************************************************************
_rdf_view::_rdf_view()
	: _view()
{
}

/*virtual*/ _rdf_view::~_rdf_view()
{
}

_rdf_controller* _rdf_view::getRDFController() const
{
	return _ptr<_rdf_controller>(m_pController);
}

// ************************************************************************************************
_rdf_controller::_rdf_controller()
	: _controller()
{
}

/*virtual*/ _rdf_controller::~_rdf_controller()
{
}
