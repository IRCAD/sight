/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <boost/utility/enable_if.hpp>

#include <fwData/camp/mapper.hpp>
#include <fwData/Float.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/String.hpp>
#include <fwData/Integer.hpp>

#include "fwDataCamp/visitor/GetObject.hpp"

#include "fwDataCamp/exception/NullPointer.hpp"
#include "fwDataCamp/exception/ObjectNotFound.hpp"

namespace fwDataCamp
{

namespace visitor
{

struct GetCampValueVisitor : public camp::ValueVisitor< ::fwData::Object::sptr >
{
    std::string m_subObjPath;
    PathVisitor::sptr m_pathVisitor;

    GetCampValueVisitor( const std::string& subObjPath, PathVisitor::sptr pathVisitor) :
        m_subObjPath(subObjPath), m_pathVisitor(pathVisitor)
    {
    }

    ::fwData::Object::sptr operator()(camp::NoType value)
    {
        SLM_FATAL("Enter in void GetCampValueVisitor()(camp::NoType value) : case not managed");
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
        SLM_FATAL("Enter in void GetCampValueVisitor()(camp::EnumObject value) : case not managed");
        ::fwData::Object::sptr val;
        return val;
    }

    ::fwData::Object::sptr operator()(const camp::UserObject& value)
    {
        ::fwData::Object::sptr val;
        const camp::Class& metaclass = value.getClass();
        if ( value.pointer() )
        {
            if ( !m_subObjPath.empty() )
            {
                OSLM_DEBUG( "visit class= '" << metaclass.name() << "' ( classname = '"<< value.call(
                                "classname") <<"' )" );
                ::fwData::Object* ptr = value.get< ::fwData::Object* >();
                ::fwDataCamp::visitor::GetObject visitor( ptr->getSptr(), m_subObjPath );
                val = visitor.get();
                m_pathVisitor->merge(visitor.getPathVisitor());
            }
            else
            {
                ::fwData::Object* ptr = value.get< ::fwData::Object* >();
                val                   = ptr->getSptr();
            }
        }
        else
        {
            FW_RAISE_EXCEPTION( ::fwDataCamp::exception::NullPointer(
                                    "Object '" + metaclass.name() + "' not instanced.")
                                );
        }

        return val;
    }
};

//-----------------------------------------------------------------------------

GetObject::GetObject( ::fwData::Object::csptr object, const std::string& subObjPath ) :
    m_object(object), m_subObjPath(subObjPath),
    m_newSubObjPath(subObjPath),
    m_pathVisitor(std::make_shared<PathVisitor>(subObjPath))
{
    SLM_FATAL_IF("Cannot retrieve an object with an empty path.", subObjPath.empty());
    m_campObj      = camp::UserObject( *object );
    m_propertyName = this->getNextPropertyName();
}

//-----------------------------------------------------------------------------

GetObject::~GetObject()
{
}

//-----------------------------------------------------------------------------

void GetObject::visit(const camp::SimpleProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name ( property.name() );
    OSLM_DEBUG( "SimpleProperty name =" << name );
    if( name == m_propertyName )
    {
        m_pathVisitor->addObject(name);
        OSLM_DEBUG( "Ok SimpleProperty name =" << name );
        ::camp::Value elemValue = property.get( m_campObj );
        GetCampValueVisitor visitor(m_newSubObjPath, m_pathVisitor);
        m_subObject = elemValue.visit( visitor );
    }
}

//-----------------------------------------------------------------------------

void GetObject::visit(const camp::EnumProperty& property)
{
    SLM_TRACE_FUNC();
    OSLM_FATAL_IF( "EnumProperty is not still managed : name =" <<  property.name(),
                   property.name() == m_propertyName );
}

//-----------------------------------------------------------------------------

void GetObject::visit(const camp::MapProperty& property)
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
            value  = property.getElement(m_campObj, var);
            mapKey = value.first.to< std::string >();
            if ( key == mapKey )
            {
                m_pathVisitor->addObject(key);
                GetCampValueVisitor visitor( m_newSubObjPath, m_pathVisitor );
                m_subObject = value.second.visit( visitor );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void GetObject::visit(const camp::ArrayProperty& property)
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

        m_pathVisitor->addObject(key);

        // If the index is out of range, camp throws an exception
        // We need to catch it because this means we failed to reach the object
        ::camp::Value elemValue;
        try
        {
            elemValue = property.get( m_campObj, index );
        }
        catch(::camp::OutOfRange e)
        {
            FW_RAISE_EXCEPTION_MSG( ::fwDataCamp::exception::NullPointer,
                                    "Index '" << index << "' not found in array property '" << name << "'.");
        }

        GetCampValueVisitor visitor(m_newSubObjPath, m_pathVisitor);
        m_subObject = elemValue.visit( visitor );
    }
}

//-----------------------------------------------------------------------------

void GetObject::visit(const camp::UserProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name ( property.name() );
    OSLM_DEBUG( "UserProperty name =" << name );
    if( name == m_propertyName )
    {
        m_pathVisitor->addObject(name);
        OSLM_DEBUG( "Ok UserProperty name =" << name );
        ::camp::Value elemValue = property.get( m_campObj );
        GetCampValueVisitor visitor(m_newSubObjPath, m_pathVisitor);
        m_subObject = elemValue.visit( visitor );
    }
}

//-----------------------------------------------------------------------------

void GetObject::visit(const camp::Function& function)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

std::string GetObject::getNextPropertyName()
{
    SLM_FATAL_IF( "Path is empty.", m_newSubObjPath.empty() );
    size_t dotPos = m_newSubObjPath.find(".");
    std::string nextItem;
    if ( dotPos != std::string::npos )
    {
        nextItem        = m_newSubObjPath.substr( 0, dotPos );
        m_newSubObjPath = m_newSubObjPath.substr( dotPos+1 );
    }
    else
    {
        nextItem        = m_newSubObjPath;
        m_newSubObjPath = "";
    }
    OSLM_DEBUG( "nextItem = " << nextItem );
    OSLM_DEBUG( "m_newSubObjPath = " << m_newSubObjPath );
    return nextItem;
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr GetObject::get()
{
    const camp::Class& camClass = ::camp::classByName( m_object->getClassname() );
    camClass.visit( *this );
    return m_subObject;
}

//-----------------------------------------------------------------------------

bool GetObject::objectsFound() const
{
    return m_pathVisitor->allObjectsFound();
}

//-----------------------------------------------------------------------------

} // visitor

} // fwDataCamp

