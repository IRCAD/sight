/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/shared_ptr.hpp>
#include <boost/utility/enable_if.hpp>



#include <fwCamp/Mapper/ValueMapper.hpp>
#include <fwData/camp/mapper.hpp>
#include <fwData/Array.hpp>
#include "fwDataCamp/Factory.hpp"
#include <fwMetaData/Sequence.hpp>
#include <fwMetaData/Map.hpp>
#include <fwMetaData/Blob.hpp>
#include <fwMetaData/Numeric.hpp>
#include <fwMetaData/String.hpp>
#include <fwMetaData/Boolean.hpp>
#include <fwMetaData/Base.hpp>

#include <fwTools/UUID.hpp>
#include <fwTools/BufferObject.hpp>

#include "fwMetaConversion/CampObjectPrinterVisitor.hpp"

#include "fwMetaConversion/camp/ValueMapper.hpp"



namespace fwMetaConversion
{


struct StringValueVisitor : public camp::ValueVisitor<std::string>
{
    std::string operator()(camp::NoType value)
    {
        SLM_FATAL("Case not managed");
        return std::string("");
    }

    std::string operator()(bool value)
    {
        std::string str;
        if( value )
        {
            str = "true";
        }
        else
        {
            str = "false";
        }

        return str;
    }

    std::string operator()(long value)
    {
        return ::boost::lexical_cast<std::string>(value);
    }

    std::string operator()(double value)
    {
        return ::boost::lexical_cast<std::string>(value);
    }

    std::string operator()(const std::string& value)
    {
        return value;
    }

    std::string operator()(const camp::EnumObject& value)
    {
        return value.name();
    }

    std::string operator()(const camp::UserObject& value)
    {
        SLM_FATAL("Case not managed");
        return std::string("");
    }
};

//-----------------------------------------------------------------------------

struct CampValuePrinterVisitor : public camp::ValueVisitor<void>
{
    std::string m_prefix;

    CampValuePrinterVisitor( std::string prefix = std::string() ) : m_prefix (prefix)
    {}

    void operator()(camp::NoType value)
    {
        SLM_FATAL("Case not managed");
    }

     void operator()(bool value)
     {
         OSLM_INFO( m_prefix << "-- "<<  value);
     }

     void operator()(long value)
     {
         OSLM_INFO( m_prefix << "-- "<<  value);
     }

     void operator()(double value)
     {
         OSLM_INFO( m_prefix << "-- "<<  value);
     }

     void operator()(const std::string& value)
     {
         OSLM_INFO( m_prefix << "-- "<<  value);
     }

     void operator()(const camp::EnumObject& value)
     {
         OSLM_INFO( m_prefix << "-- "<<  value.name());
     }

     void operator()(const camp::UserObject& value)
     {
         const camp::Class& metaclass = value.getClass();
         if ( value.pointer() )
         {
             std::string classname = value.call("classname").to<std::string>();

             if ( metaclass.name() != classname )
             {
                 const camp::Class& newMetaclass = ::camp::classByName(classname);
                 ::fwMetaConversion::CampObjectPrinterVisitor visitor( value, m_prefix);
                 newMetaclass.visit(visitor);
             }
             else
             {
                 ::fwMetaConversion::CampObjectPrinterVisitor visitor( value, m_prefix);
                 metaclass.visit(visitor);
             }
         }
         else
         {
             OSLM_INFO( "try visiting class= '" << metaclass.name() << " but we have a null pointer" );
         }
     }
 };

//-----------------------------------------------------------------------------

CampObjectPrinterVisitor::CampObjectPrinterVisitor( const camp::UserObject & campObj, std::string prefix )
    : m_campObj(campObj),
      m_prefix(prefix)
{}

//-----------------------------------------------------------------------------

CampObjectPrinterVisitor::~CampObjectPrinterVisitor()
{}

//-----------------------------------------------------------------------------

void CampObjectPrinterVisitor::visit(const camp::SimpleProperty& property)
{
    SLM_TRACE_FUNC();

    const std::string name ( property.name() );
    const std::string type ( this->getCampType( property.type() ) );

    // ::camp::Value elemValue = property.get( m_campObj );
//    CampValuePrinterVisitor visitor;
//    elemValue.visit( visitor );

    StringValueVisitor visitor;
    ::camp::Value elemValue = property.get( m_campObj );
    std::string val = elemValue.visit( visitor );

    OSLM_INFO( m_prefix << "-- "<<name<<" = "<<  val <<" , "<<type );

}

//-----------------------------------------------------------------------------

void CampObjectPrinterVisitor::visit(const camp::EnumProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name ( property.name() );
    const std::string type ( this->getCampType( property.type() ) );
    //OSLM_INFO( "name = "<<name<<" ( type = "<<type<<" )" );

    StringValueVisitor visitor;
    ::camp::Value elemValue = property.get( m_campObj );
    std::string val = elemValue.visit( visitor );
    OSLM_INFO( m_prefix << "-- "<<name<<" = "<<  val <<" , "<<type );

//    ::camp::Value elemValue = property.get( m_campObj );
//    CampValuePrinterVisitor visitor;
//    elemValue.visit( visitor );

}

//-----------------------------------------------------------------------------

void CampObjectPrinterVisitor::visit(const camp::MapProperty& property)
{
    const std::string name ( property.name() );
    const std::string type ( this->getCampType( property.type() ) );
    const std::string elementType ( this->getCampType( property.elementType() ) );
    const unsigned int size = property.getSize( m_campObj );
    OSLM_INFO( m_prefix << "-+ "<<name<<" , "<<elementType<<" map, size = "<<size );

    std::pair< ::camp::Value, ::camp::Value > value;
    ::camp::Value first;
    ::camp::Value second;

    for (int var = 0; var < property.getSize(m_campObj); ++var)
    {
        value = property.getElement(m_campObj, var);

        first = value.first;
        second = value.second;

        StringValueVisitor visitorKey;
        std::string val = first.visit( visitorKey );

        if ( second.type() == ::camp::userType )
        {
            camp::UserObject obj = second.to< camp::UserObject >();
            std::string classname = obj.call("classname").to<std::string>();
            OSLM_INFO( m_prefix << " |-+ "<<  val << " (" << classname <<" )" );
            CampValuePrinterVisitor visitor ( m_prefix + " | |" );
            second.visit( visitor );
        }
        else
        {
            OSLM_INFO( m_prefix << " |-+ "<<  val );
            StringValueVisitor visitor;
            std::string val = second.visit( visitor );

            OSLM_INFO( m_prefix << " | |-- "<<  val);
        }
    }
}


//-----------------------------------------------------------------------------

void CampObjectPrinterVisitor::visit(const camp::ArrayProperty& property)
{
    const std::string name ( property.name() );
    const std::string type ( this->getCampType( property.type() ) );
    const std::string elementType ( this->getCampType( property.elementType() ) );
    const unsigned int size = property.size( m_campObj );
    OSLM_INFO( m_prefix << "-+ "<<name<<" , "<<elementType<<" array, size = "<<size );

    ::camp::Value elemValue;
    for (int i = 0; i < size; i++)
    {
            elemValue = property.get( m_campObj, i );

        if ( elemValue.type() == ::camp::userType )
        {
            camp::UserObject obj = elemValue.to< camp::UserObject >();
            std::string classname = obj.call("classname").to<std::string>();
            OSLM_INFO( m_prefix << " |-+ "<<  i << " (" << classname <<" )" );
            CampValuePrinterVisitor visitor ( m_prefix + " | |" );
            elemValue.visit( visitor );
        }
        else
        {
            OSLM_INFO( m_prefix << " |-+ "<<  i );
            StringValueVisitor visitor;
            std::string val = elemValue.visit( visitor );

            OSLM_INFO( m_prefix << " | |-- "<<  val);
        }
    }
}

//-----------------------------------------------------------------------------

void CampObjectPrinterVisitor::visit(const camp::UserProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name ( property.name() );
    const std::string type ( this->getCampType( property.type() ) );
    //OSLM_INFO( "name = "<<name<<" ( type = "<<type<<" )" );



    ::camp::Value elemValue = property.get( m_campObj );

    SLM_ASSERT("hum", elemValue.type() == ::camp::userType );

    camp::UserObject obj = elemValue.to< camp::UserObject >();
    if(obj.pointer())
    {
        std::string classname = obj.call("classname").to<std::string>();
        OSLM_INFO( m_prefix << "-+ "<<name<<" ( "<<classname<<" )" );
    }
    else
    {
        OSLM_INFO( m_prefix << "-+ "<<name<<" ( "<<type<<" ) ");
    }


    CampValuePrinterVisitor visitor ( m_prefix + " |" );
    elemValue.visit( visitor );
}

//-----------------------------------------------------------------------------

void CampObjectPrinterVisitor::visit(const camp::Function& function)
{}

//-----------------------------------------------------------------------------

std::string CampObjectPrinterVisitor::getCampType( const ::camp::Type & type )
{
    std::string strType;
    switch( type )
    {
        case ::camp::enumType:
            strType = "enumType";
            break;
        case ::camp::stringType:
            strType = "stringType";
            break;
        case ::camp::intType :
            strType = "intType";
            break;
        case ::camp::realType :
            strType = "realType";
            break;
        case ::camp::boolType :
            strType = "boolType";
            break;
        case ::camp::userType:
            strType = "userType";
            break;
        case ::camp::noType :
            strType = "noType";
            break;
        default :
            strType = "unknow";
            break;
    }

    return strType;
}

} // fwMetaConversion
