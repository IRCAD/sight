#include <boost/assign/list_of.hpp>
#include <fwTools/Type.hpp>

namespace fwTools
{

const std::string Type::s_unspecifiedTypeName = "UNSPECIFIED TYPE";
const Type Type::s_unspecifiedType;

const Type::TypeMapType Type::s_typeMap = ::boost::assign::map_list_of
    ("uint8" , Type::create< ::boost::uint8_t>())
    ("uint16", Type::create< ::boost::uint16_t>())
    ("uint32", Type::create< ::boost::uint32_t>())
    ("uint64", Type::create< ::boost::uint64_t>())
    ("int8" ,  Type::create< ::boost::int8_t>())
    ("int16",  Type::create< ::boost::int16_t>())
    ("int32",  Type::create< ::boost::int32_t>())
    ("int64",  Type::create< ::boost::int64_t>())
    ("float",  Type::create<float>())
    ("double", Type::create<double>())
    ;

template<> const std::string Type::typeToString< ::boost::uint8_t  >() {return "uint8";}
template<> const std::string Type::typeToString< ::boost::uint16_t >() {return "uint16";}
template<> const std::string Type::typeToString< ::boost::uint32_t >() {return "uint32";}
template<> const std::string Type::typeToString< ::boost::uint64_t >() {return "uint64";}

template<> const std::string Type::typeToString< ::boost::int8_t  > () {return "int8";}
template<> const std::string Type::typeToString< ::boost::int16_t > () {return "int16";}
template<> const std::string Type::typeToString< ::boost::int32_t > () {return "int32";}
template<> const std::string Type::typeToString< ::boost::int64_t > () {return "int64";}

template<> const std::string Type::typeToString< float  >  () {return "float";}
template<> const std::string Type::typeToString< double >  () {return "double";}



//------------------------------------------------------------------------------

Type::Type()
{
    m_name = s_unspecifiedTypeName;
    m_sizeof = 0;
    m_isSigned = false;
    m_isFixedPrecision = false;
}

//------------------------------------------------------------------------------

bool Type::operator==(const Type &_other) const
{
    return m_name == _other.m_name;
}

//------------------------------------------------------------------------------

bool Type::operator!=(const Type &_other) const
{
    return m_name != _other.m_name;
}

//------------------------------------------------------------------------------


bool Type::operator<( const Type& _other) const
{
    return m_name < _other.m_name;
}

//------------------------------------------------------------------------------

unsigned char Type::sizeOf() const
{
    return m_sizeof;
}

//------------------------------------------------------------------------------


const std::string &Type::string() const
{
    return m_name;
}

//------------------------------------------------------------------------------

bool Type::isFixedPrecision() const
{
    return m_isFixedPrecision;
}

//------------------------------------------------------------------------------

bool Type::isSigned() const
{
    return m_isSigned;
}

//-----------------------------------------------------------------------------

Type Type::create(std::string name)
{
    TypeMapType::const_iterator iter = s_typeMap.find(name);
    if (iter != s_typeMap.end())
    {
        return iter->second;
    }
    return s_unspecifiedType;
}

}

