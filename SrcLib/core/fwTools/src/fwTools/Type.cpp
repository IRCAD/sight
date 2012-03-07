#include <boost/assign/list_of.hpp>

#include "fwTools/Type.hpp"


namespace std
{
    std::ostream& operator<< (std::ostream& os, const ::fwTools::Type& type)
    {
        os << type.string();
        return os;
    }
}

namespace fwTools
{

const std::string Type::s_UNSPECIFIED_TYPENAME("UNSPECIFIED TYPE");

const std::string Type::s_INT8_TYPENAME("int8");
const std::string Type::s_INT16_TYPENAME("int16");
const std::string Type::s_INT32_TYPENAME("int32");
const std::string Type::s_INT64_TYPENAME("int64");

const std::string Type::s_UINT8_TYPENAME("uint8");
const std::string Type::s_UINT16_TYPENAME("uint16");
const std::string Type::s_UINT32_TYPENAME("uint32");
const std::string Type::s_UINT64_TYPENAME("uint64");

const std::string Type::s_FLOAT_TYPENAME("float");
const std::string Type::s_DOUBLE_TYPENAME("double");



const Type::TypeMapType Type::s_TYPEMAP = ::boost::assign::map_list_of
    (Type::s_INT8_TYPENAME  , Type::create< ::fwTools::Type::Int8Type >())
    (Type::s_INT16_TYPENAME , Type::create< ::fwTools::Type::Int16Type >())
    (Type::s_INT32_TYPENAME , Type::create< ::fwTools::Type::Int32Type >())
    (Type::s_INT64_TYPENAME , Type::create< ::fwTools::Type::Int64Type >())

    (Type::s_UINT8_TYPENAME , Type::create< ::fwTools::Type::UInt8Type >())
    (Type::s_UINT16_TYPENAME, Type::create< ::fwTools::Type::UInt16Type >())
    (Type::s_UINT32_TYPENAME, Type::create< ::fwTools::Type::UInt32Type >())
    (Type::s_UINT64_TYPENAME, Type::create< ::fwTools::Type::UInt64Type >())

    (Type::s_FLOAT_TYPENAME , Type::create< ::fwTools::Type::FloatType >())
    (Type::s_DOUBLE_TYPENAME, Type::create< ::fwTools::Type::DoubleType >())
    ;



const Type Type::s_UNSPECIFIED_TYPE;

const Type Type::s_INT8  = ::fwTools::Type::create< ::fwTools::Type::Int8Type  >();
const Type Type::s_INT16 = ::fwTools::Type::create< ::fwTools::Type::Int16Type >();
const Type Type::s_INT32 = ::fwTools::Type::create< ::fwTools::Type::Int32Type >();
const Type Type::s_INT64 = ::fwTools::Type::create< ::fwTools::Type::Int64Type >();

const Type Type::s_UINT8  = ::fwTools::Type::create< ::fwTools::Type::UInt8Type  >();
const Type Type::s_UINT16 = ::fwTools::Type::create< ::fwTools::Type::UInt16Type >();
const Type Type::s_UINT32 = ::fwTools::Type::create< ::fwTools::Type::UInt32Type >();
const Type Type::s_UINT64 = ::fwTools::Type::create< ::fwTools::Type::UInt64Type >();

const Type Type::s_FLOAT  = ::fwTools::Type::create< ::fwTools::Type::FloatType  >();
const Type Type::s_DOUBLE = ::fwTools::Type::create< ::fwTools::Type::DoubleType >();




template<> const std::string &Type::typeToString< ::fwTools::Type::Int8Type  > () {return Type::s_INT8_TYPENAME; }
template<> const std::string &Type::typeToString< ::fwTools::Type::Int16Type > () {return Type::s_INT16_TYPENAME;}
template<> const std::string &Type::typeToString< ::fwTools::Type::Int32Type > () {return Type::s_INT32_TYPENAME;}
template<> const std::string &Type::typeToString< ::fwTools::Type::Int64Type > () {return Type::s_INT64_TYPENAME;}

template<> const std::string &Type::typeToString< ::fwTools::Type::UInt8Type  >() {return Type::s_UINT8_TYPENAME; }
template<> const std::string &Type::typeToString< ::fwTools::Type::UInt16Type >() {return Type::s_UINT16_TYPENAME;}
template<> const std::string &Type::typeToString< ::fwTools::Type::UInt32Type >() {return Type::s_UINT32_TYPENAME;}
template<> const std::string &Type::typeToString< ::fwTools::Type::UInt64Type >() {return Type::s_UINT64_TYPENAME;}

template<> const std::string &Type::typeToString< ::fwTools::Type::FloatType  >() {return ::fwTools::Type::s_FLOAT_TYPENAME; }
template<> const std::string &Type::typeToString< ::fwTools::Type::DoubleType >() {return ::fwTools::Type::s_DOUBLE_TYPENAME;}


//------------------------------------------------------------------------------

Type::Type()
{
    Type::setType<void>();
}

//------------------------------------------------------------------------------

Type::Type(const std::string &type)
{
    *this = Type::create(type);
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

const std::type_info &Type::typeId() const
{
    return m_tool->m_typeinfo;
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
    TypeMapType::const_iterator iter = s_TYPEMAP.find(name);
    if (iter != s_TYPEMAP.end())
    {
        return iter->second;
    }
    return s_UNSPECIFIED_TYPE;
}

//-----------------------------------------------------------------------------

std::string Type::toString(const void * value) const
{
    return m_tool->toString(value);
}

//-----------------------------------------------------------------------------

Type::ToolBase::ToolBase() : m_typeinfo(typeid(void))
{}

//-----------------------------------------------------------------------------

Type::ToolBase::ToolBase(const std::type_info &typeinfo) : m_typeinfo(typeinfo)
{}

//-----------------------------------------------------------------------------

std::string Type::ToolBase::toString(::boost::any value) const
{
    SLM_ASSERT("unable to convert an unspecified type value", 0);
    return "";
}

//-----------------------------------------------------------------------------

std::string Type::ToolBase::toString(const void *value) const
{
    SLM_ASSERT("unable to convert an unspecified type value", 0);
    return "";
}

//-----------------------------------------------------------------------------

template <>
void Type::setType<char>()
{
    this->setType<signed char>();
}

//-----------------------------------------------------------------------------

template <>
void Type::setType<void>()
{
    m_name = s_UNSPECIFIED_TYPENAME;
    m_sizeof = 0;
    m_isSigned = false;
    m_isFixedPrecision = false;

    m_tool = SPTR(ToolBase)(new Type::ToolBase());

    m_min = 0;
    m_max = 0;
}

} // namespace fwTools

