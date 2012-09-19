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

#include <fwMetaData/Sequence.hpp>
#include <fwMetaData/Map.hpp>
#include <fwMetaData/Blob.hpp>
#include <fwMetaData/Numeric.hpp>
#include <fwMetaData/String.hpp>
#include <fwMetaData/Boolean.hpp>
#include <fwMetaData/Base.hpp>

#include <fwTools/UUID.hpp>
#include <fwTools/BufferObject.hpp>

#include "fwMetaConversion/CampObjectVisitor.hpp"

#include "fwMetaConversion/camp/ValueMapper.hpp"

namespace fwMetaConversion
{

struct CampValueVisitor : public camp::ValueVisitor<void>
{

    void operator()(camp::NoType value)
    {
        SLM_FATAL("Enter in void CampValueVisitor()(camp::NoType value) : case not managed");
    }

     void operator()(bool value)
     {
         OSLM_INFO( "val = " << value );
     }

     void operator()(long value)
     {
         OSLM_INFO( "val = " << value );
     }

     void operator()(double value)
     {
         OSLM_INFO( "val = " << value );
     }

     void operator()(const std::string& value)
     {
         OSLM_INFO( "val = '" << value << "'" );
     }

     void operator()(const camp::EnumObject& value)
     {
         OSLM_INFO( "name = '" << value.name() << "' val = " << value.value() );
     }

     void operator()(const camp::UserObject& value)
     {
         const camp::Class& metaclass = value.getClass();
         if ( value.pointer() )
         {
             OSLM_INFO( "visit class= '" << metaclass.name() << "' ( classname = '"<< value.call("classname") <<"' )" );
             std::string classname = value.call("classname").to<std::string>();

             if ( metaclass.name() != classname )
             {
                 const camp::Class& newMetaclass = ::camp::classByName(classname);
                 ::fwMetaConversion::CampObjectVisitor visitor( value );
                 newMetaclass.visit(visitor);
             }
             else
             {
                 ::fwMetaConversion::CampObjectVisitor visitor( value );
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

CampObjectVisitor::CampObjectVisitor( const camp::UserObject & campObj ) : m_campObj(campObj)
{}

//-----------------------------------------------------------------------------

CampObjectVisitor::~CampObjectVisitor()
{}

//-----------------------------------------------------------------------------

void CampObjectVisitor::visit(const camp::SimpleProperty& property)
{
    SLM_TRACE_FUNC();

    const std::string name ( property.name() );
    const std::string type ( this->getCampType( property.type() ) );
    OSLM_INFO( "name = "<<name<<" ( type = "<<type<<" )" );

    ::camp::Value elemValue = property.get( m_campObj );
    CampValueVisitor visitor;
    elemValue.visit( visitor );
}

//-----------------------------------------------------------------------------

void CampObjectVisitor::visit(const camp::EnumProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name ( property.name() );
    const std::string type ( this->getCampType( property.type() ) );
    OSLM_INFO( "name = "<<name<<" ( type = "<<type<<" )" );

    ::camp::Value elemValue = property.get( m_campObj );
    CampValueVisitor visitor;
    elemValue.visit( visitor );
}

//-----------------------------------------------------------------------------

void CampObjectVisitor::visit(const camp::MapProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name ( property.name() );
    const std::string type ( this->getCampType( property.type() ) );
    const std::string elementType ( this->getCampType( property.elementType() ) );
    unsigned int size = property.getSize( m_campObj );
    OSLM_INFO( "name = "<<name<<" ( type = "<<type<<" elementType = "<<elementType<<" size = "<<size<<" )" );

    std::pair< ::camp::Value, ::camp::Value > value;
    ::camp::Value first;
    ::camp::Value second;

    for (int var = 0; var < property.getSize(m_campObj); ++var)
    {
        value = property.getElement(m_campObj, var);

        first = value.first;
        CampValueVisitor visitor;
        first.visit( visitor );
        second = value.second;
        second.visit( visitor );
    }
}


//-----------------------------------------------------------------------------

void CampObjectVisitor::visit(const camp::ArrayProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name ( property.name() );
    const std::string type ( this->getCampType( property.type() ) );
    const std::string elementType ( this->getCampType( property.elementType() ) );
    unsigned int size = property.size( m_campObj );
    OSLM_INFO( "name = "<<name<<" ( type = "<<type<<" elementType = "<<elementType<<" size = "<<size<<" )" );

    ::camp::Value elemValue;
    for (int i = 0; i < size; i++)
    {
        elemValue = property.get( m_campObj, i );
        const std::string elemType ( this->getCampType( elemValue.type() ) );
        OSLM_INFO( "elemType = " << elemType );

        CampValueVisitor visitor;
        elemValue.visit( visitor );
    }
}

//-----------------------------------------------------------------------------

void CampObjectVisitor::visit(const camp::UserProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name ( property.name() );
    const std::string type ( this->getCampType( property.type() ) );
    OSLM_INFO( "name = "<<name<<" ( type = "<<type<<" )" );

    ::camp::Value elemValue = property.get( m_campObj );
    CampValueVisitor visitor;
    elemValue.visit( visitor );
}

//-----------------------------------------------------------------------------

void CampObjectVisitor::visit(const camp::Function& function)
{
    OSLM_INFO("function visitation not implemented");
}

//-----------------------------------------------------------------------------

std::string CampObjectVisitor::getCampType( const ::camp::Type & type )
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
            strType = "unknown";
            SLM_WARN("Camp type not defined.");
            break;
    }

    return strType;
}

} // fwMetaConversion
