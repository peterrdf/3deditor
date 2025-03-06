#include "_host.h"
#include "_attribute.h"

// ************************************************************************************************
_attribute::_attribute(SdaiAttr sdaiAttr)
	: m_sdaiAttr(sdaiAttr)
{
	assert(m_sdaiAttr != 0);
}

/*virtual*/ _attribute::~_attribute()
{}

SdaiPrimitiveType _attribute::getSdaiPrimitiveType() const
{
	SdaiPrimitiveType sdaiPrimitiveType = engiGetAttrType(m_sdaiAttr);
	if ((sdaiPrimitiveType & engiTypeFlagAggr) ||
		(sdaiPrimitiveType & engiTypeFlagAggrOption))
	{
		sdaiPrimitiveType = sdaiAGGR;
	}

	return sdaiPrimitiveType;
}

// ************************************************************************************************
_attribute_provider::_attribute_provider()
	: m_mapInstanceAttributes()
{}

/*virtual*/ _attribute_provider::~_attribute_provider()
{
	for (auto& itInstanceAttributes : m_mapInstanceAttributes)
	{
		for (auto pAttribute : itInstanceAttributes.second)
		{
			delete pAttribute;
		}
	}
}

const vector<_attribute*>& _attribute_provider::getInstanceAttributes(SdaiInstance sdaiInstance)
{
	assert(sdaiInstance != 0);

	auto itInstanceAttributes = m_mapInstanceAttributes.find(sdaiInstance);
	if (itInstanceAttributes != m_mapInstanceAttributes.end())
	{
		return itInstanceAttributes->second;
	}

	SdaiEntity sdaiEntity = sdaiGetInstanceType(sdaiInstance);
	assert(sdaiEntity != 0);

	vector<_attribute*> vecAttributes;
	loadInstanceAttributes(sdaiEntity, sdaiInstance, vecAttributes);

	m_mapInstanceAttributes[sdaiInstance] = vecAttributes;

	return m_mapInstanceAttributes.at(sdaiInstance);
}

void _attribute_provider::loadInstanceAttributes(SdaiEntity sdaiEntity, SdaiInstance sdaiInstance, vector<_attribute*>& vecAttributes)
{
	if (sdaiEntity == 0)
	{
		return;
	}

	assert(sdaiInstance != 0);

	loadInstanceAttributes(engiGetEntityParent(sdaiEntity), sdaiInstance, vecAttributes);

	SdaiInteger iIndex = 0;
	SdaiAttr sdaiAttr = engiGetEntityAttributeByIndex(
		sdaiEntity,
		iIndex++,
		false,
		true);

	while (sdaiAttr != nullptr)
	{
		auto pIFCAttribute = new _attribute(sdaiAttr);
		vecAttributes.push_back(pIFCAttribute);

		sdaiAttr = engiGetEntityAttributeByIndex(
			sdaiEntity,
			iIndex++,
			false,
			true);
	}
}