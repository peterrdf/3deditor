#pragma once

//To keep value of property of any type

class PropertyValue
{
public:
    //init this PropetyValue from instance property
    void GetFromProperty(RdfsResource instance, RdfProperty prop);

    //set this PropertyValue to instance property
    void SetToProperty(RdfsResource instance, RdfProperty prop);

private:
    void GetObjectValue(RdfsResource instance, RdfProperty prop);
    void SetObjectValue(RdfsResource instance, RdfProperty prop);

    template <typename TData> void GetDataValue(RdfsResource instance, OwlDatatypeProperty prop, std::vector<TData>& value);
    template <typename TData> void SetDataValue(RdfsResource instance, OwlDatatypeProperty prop, const std::vector<TData>& value);

private:
    RdfPropertyType          m_propertyType = 0;

    //union of possible property values
    std::vector<double>         m_dval;
    std::vector<int64_t>        m_ival;
    std::vector<char>           m_bval;
    std::vector<RdfsResource>   m_objval;
};

