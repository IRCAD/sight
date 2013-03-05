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
#include <fwData/Float.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/String.hpp>
#include <fwData/Integer.hpp>

#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/String.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Base.hpp>

#include <fwTools/UUID.hpp>
#include <fwTools/BufferObject.hpp>

#include "fwAtomConversion/RetreiveObjectVisitor.hpp"

#include "fwAtomConversion/camp/ValueMapper.hpp"
#include "fwAtomConversion/exception/NullPointer.hpp"
#include "fwAtomConversion/exception/ObjectNotFound.hpp"

namespace fwAtomConversion
{

struct RetreiveCampValueVisitor : public camp::ValueVisitor< ::fwData::Object::sptr >
{
    std::string m_subObjPath;
    PathVisitor::sptr m_pathVisitor;

    RetreiveCampValueVisitor( const std::string & subObjPath, PathVisitor::sptr pathVisitor) :
        m_subObjPath(subObjPath), m_pathVisitor(pathVisitor)
    {}

    ::fwData::Object::sptr operator()(camp::NoType value)
    {
        SLM_FATAL("Enter in void RetreiveCampValueVisitor()(camp::NoType value) : case not managed");
        ::fwData::Object::sptr val;
        return val;
    }

    ::fwData::Object::sptr operator()(bool value)
    {
        return ::fwData::Boolean::New(value);
    }

    ::fwData::Object::sptr operator()(long value)
    {
        return ::fwData::Integer::New(value);
    }

    ::fwData::Object::sptr operator()(double value)
    {
        return ::fwData::Float::New(value);
    }

    ::fwData::Object::sptr operator()(const std::string& value)
    {
        return ::fwData::String::New(value);
    }

    ::fwData::Object::sptr operator()(const camp::EnumObject& value)
    {
        SLM_FATAL("Enter in void RetreiveCampValueVisitor()(camp::EnumObject value) : case not managed");
        ::fwData::Object::sptr val;
        return val;
    }

    ::fwData::Object::sptr operator()(const camp::UserObject& value)
    {
        ::fwData::Object::sptr val;
        const camp::Class& metaclass = value.getClass();
        if ( value.pointer() )
        {
            if ( ! m_subObjPath.empty() )
            {
                OSLM_DEBUG( "visit class= '" << metaclass.name() << "' ( classname = '"<< value.call("classname") <<"' )" );
                ::fwData::Object * ptr = value.get< ::fwData::Object * >();
                ::fwAtomConversion::RetreiveObjectVisitor visitor( ptr->getSptr(), m_subObjPath );
                val = visitor.retreive();
                m_pathVisitor->merge(visitor.getPathVisitor());
            }
            else
            {
                ::fwData::Object * ptr = value.get< ::fwData::Object * >();
                val = ptr->getSptr();
            }
        }
        else
        {
            FW_RAISE_EXCEPTION( ::fwAtomConversion::exception::NullPointer(
                    "Object '" + metaclass.name() + "' not instanced.")
            );
        }

        return val;
    }
 };

//-----------------------------------------------------------------------------

RetreiveObjectVisitor::RetreiveObjectVisitor( ::fwData::Object::sptr object, const std::string & subObjPath ) :
        m_object(object), m_subObjPath(subObjPath),
        m_newSubObjPath(subObjPath),
        m_pathVisitor(::boost::make_shared<PathVisitor>(subObjPath))
{
    SLM_FATAL_IF("Cannot retrieve an object with an empty path.", subObjPath.empty());
    m_campObj = camp::UserObject( object.get() );
    m_propertyName = this->getNextPropertyName();
}

//-----------------------------------------------------------------------------

RetreiveObjectVisitor::~RetreiveObjectVisitor()
{}

//-----------------------------------------------------------------------------

void RetreiveObjectVisitor::visit(const camp::SimpleProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name ( property.name() );
    OSLM_DEBUG( "SimpleProperty name =" << name );
    if( name == m_propertyName )
    {
        m_pathVisitor->addObject(name);
        OSLM_DEBUG( "Ok SimpleProperty name =" << name );
        ::camp::Value elemValue = property.get( m_campObj );
        RetreiveCampValueVisitor visitor(m_newSubObjPath, m_pathVisitor);
        m_subObject = elemValue.visit( visitor );
    }
}

//-----------------------------------------------------------------------------

void RetreiveObjectVisitor::visit(const camp::EnumProperty& property)
{
    SLM_TRACE_FUNC();
    OSLM_FATAL( "EnumProperty is not still managed : name =" <<  property.name() );
}

//-----------------------------------------------------------------------------

void RetreiveObjectVisitor::visit(const camp::MapProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name ( property.name() );
    OSLM_DEBUG( "MapProperty name =" << name);
    if( name == m_propertyName )
    {
        m_pathVisitor->addObject(name);
        OSLM_DEBUG( "Ok MapProperty name =" << name );
        std::string key = this->getNextPropertyName();

        std::pair< ::camp::Value, ::camp::Value > value;
        std::string mapKey;
        for (unsigned int var = 0; var < property.getSize(m_campObj); ++var)
        {
            value = property.getElement(m_campObj, var);
            mapKey = value.first.to< std::string >();
            if ( key == mapKey )
            {
                m_pathVisitor->addObject(key);
                RetreiveCampValueVisitor visitor( m_newSubObjPath, m_pathVisitor );
                m_subObject = value.second.visit( visitor );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void RetreiveObjectVisitor::visit(const camp::ArrayProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name ( property.name() );
    OSLM_DEBUG( "ArrayProperty name =" << name );
    if( name == m_propertyName )
    {
        m_pathVisitor->addObject(name);
        OSLM_DEBUG( "Ok ArrayProperty name =" << name );
        std::string key = this->getNextPropertyName();

        size_t index = ::boost::lexical_cast< size_t >( key );

        ::camp::Value elemValue = property.get( m_campObj, index );
        RetreiveCampValueVisitor visitor(m_newSubObjPath, m_pathVisitor);
        m_subObject = elemValue.visit( visitor );
        m_pathVisitor->addObject(key);
    }
}

//-----------------------------------------------------------------------------

void RetreiveObjectVisitor::visit(const camp::UserProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name ( property.name() );
    OSLM_DEBUG( "UserProperty name =" << name );
    if( name == m_propertyName )
    {
        m_pathVisitor->addObject(name);
        OSLM_DEBUG( "Ok UserProperty name =" << name );
        ::camp::Value elemValue = property.get( m_campObj );
        RetreiveCampValueVisitor visitor(m_newSubObjPath, m_pathVisitor);
        m_subObject = elemValue.visit( visitor );
    }
}

//-----------------------------------------------------------------------------

void RetreiveObjectVisitor::visit(const camp::Function& function)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

std::string RetreiveObjectVisitor::getNextPropertyName()
{
    SLM_FATAL_IF( "Path is empty.", m_newSubObjPath.empty() );
    size_t dotPos = m_newSubObjPath.find(".");
    std::string nextItem;
    if ( dotPos != std::string::npos )
    {
        nextItem = m_newSubObjPath.substr( 0, dotPos );
        m_newSubObjPath = m_newSubObjPath.substr( dotPos+1 );
    }
    else
    {
        nextItem = m_newSubObjPath;
        m_newSubObjPath = "";
    }
    OSLM_DEBUG( "nextItem = " << nextItem );
    OSLM_DEBUG( "m_newSubObjPath = " << m_newSubObjPath );
    return nextItem;
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr RetreiveObjectVisitor::getSubObject()
{
    return m_subObject;
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr RetreiveObjectVisitor::retreive()
{
    const camp::Class& camClass = ::camp::classByName( m_object->getClassname() );
    camClass.visit( *this );
    return m_subObject;
}

////-----------------------------------------------------------------------------

bool RetreiveObjectVisitor::objectsFound() const
{
    return m_pathVisitor->allObjectsFound();
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr getSubObject( ::fwData::Object::sptr object,
                                     const std::string & subObjPath,
                                     bool raiseException )
{
    SLM_FATAL_IF( "SubObjPath is empty.", subObjPath.empty() );
    SLM_FATAL_IF( "SubObjPath not start with '@'.", subObjPath[0] != '@' );
    const std::string path =  subObjPath.substr( 1 );
    ::fwAtomConversion::RetreiveObjectVisitor visitor( object, path );
    ::fwData::Object::sptr subObject;
    try
    {
        subObject = visitor.retreive();
    }
    catch(const ::fwAtomConversion::exception::NullPointer& np)
    {
        FW_FORWARD_EXCEPTION_IF(np, raiseException);
    }

    FW_RAISE_EXCEPTION_IF(
            ::fwAtomConversion::exception::ObjectNotFound("Object '" + subObjPath + "' not found."),
            !visitor.objectsFound() && raiseException);

    return subObject;
}

} // fwAtomConversion
