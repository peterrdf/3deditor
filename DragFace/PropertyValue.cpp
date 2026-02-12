#include "pch.h"
#include "PropertyValue.h"

/// <summary>
/// 
/// </summary>
template <typename TData>
void PropertyValue::GetDataValue(RdfsResource instance, OwlDatatypeProperty prop, std::vector<TData>& value)
{
    value.clear();

    TData*  cvalue = NULL;
    int64_t card = 0;
    GetDatatypeProperty(instance, prop, (void**)&cvalue, &card);

    for (int64_t i = 0; i < card; i++) {
        value.push_back(cvalue[i]);
    }
}

/// <summary>
/// 
/// </summary>
template <typename TData>
void PropertyValue::SetDataValue(RdfsResource instance, OwlDatatypeProperty prop, const std::vector<TData>& value)
{
    SetDatatypeProperty(instance, prop, (void**)value.data(), value.size());
}


/// <summary>
/// 
/// </summary>
void PropertyValue::GetObjectValue(RdfsResource instance, RdfProperty prop)
{
    m_objval.clear();

    RdfsResource* objvalue = NULL;
    int64_t card = 0;
    GetObjectProperty(instance, prop, &objvalue, &card);
    
    for (int64_t i = 0; i < card; i++) {
        m_objval.push_back(objvalue[i]);
    }
}

/// <summary>
/// 
/// </summary>
void PropertyValue::SetObjectValue(RdfsResource instance, RdfProperty prop)
{
    SetObjectProperty(instance, prop, m_objval.data(), m_objval.size());
}

/// <summary>
/// 
/// </summary>
void PropertyValue::GetFromProperty(RdfsResource instance, RdfProperty prop)
{
    m_propertyType = GetPropertyType(prop);

    switch (m_propertyType) {
        case OBJECTTYPEPROPERTY_TYPE:
            GetObjectValue(instance, prop);
            break;
        case DATATYPEPROPERTY_TYPE_DOUBLE:
            GetDataValue(instance, prop, m_dval);
            break;
        case DATATYPEPROPERTY_TYPE_BOOLEAN:
            GetDataValue(instance, prop, m_bval);
            break;
        case DATATYPEPROPERTY_TYPE_INTEGER:
            GetDataValue(instance, prop, m_ival);
            break;
        default:
            assert(!"TODO");
    }
}

/// <summary>
/// 
/// </summary>
void PropertyValue::SetToProperty(RdfsResource instance, RdfProperty prop)
{
    if (GetPropertyType(prop) != m_propertyType) {
        assert(!"Property type mismatch in PropertyValue::SetToProperty");
        return;
    }

    switch (m_propertyType) {
        case OBJECTTYPEPROPERTY_TYPE:
            SetObjectValue(instance, prop);
            break;
        case DATATYPEPROPERTY_TYPE_DOUBLE:
            SetDataValue(instance, prop, m_dval);
            break;
        case DATATYPEPROPERTY_TYPE_BOOLEAN:
            SetDataValue(instance, prop, m_bval);
            break;
        case DATATYPEPROPERTY_TYPE_INTEGER:
            SetDataValue(instance, prop, m_ival);
            break;
        default:
            assert(!"TODO");
    }

}
