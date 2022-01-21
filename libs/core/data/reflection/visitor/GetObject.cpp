/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#define CAMP_COMPILATION

#include "data/reflection/visitor/GetObject.hpp"

#include "data/Boolean.hpp"
#include "data/Float.hpp"
#include "data/Integer.hpp"
#include "data/reflection/exception/NullPointer.hpp"
#include "data/reflection/exception/ObjectNotFound.hpp"
#include "data/reflection/mapper.hpp"
#include "data/String.hpp"

#include <boost/utility/enable_if.hpp>

namespace sight::data
{

namespace reflection
{

namespace visitor
{

struct GetCampValueVisitor : public camp::ValueVisitor<data::Object::sptr>
{
    std::string m_subObjPath;
    PathVisitor::sptr m_pathVisitor;

    GetCampValueVisitor(const std::string& subObjPath, PathVisitor::sptr pathVisitor) :
        m_subObjPath(subObjPath),
        m_pathVisitor(pathVisitor)
    {
    }

    //------------------------------------------------------------------------------

    data::Object::sptr operator()(camp::NoType)
    {
        SIGHT_FATAL("Enter in void GetCampValueVisitor()(camp::NoType value) : case not managed");
        data::Object::sptr val;
        return val;
    }

    //------------------------------------------------------------------------------

    data::Object::sptr operator()(bool value)
    {
        return data::Boolean::New(value);
    }

    //------------------------------------------------------------------------------

    data::Object::sptr operator()(long value)
    {
        return data::Integer::New(value);
    }

    //------------------------------------------------------------------------------

    data::Object::sptr operator()(double value)
    {
        return data::Float::New(static_cast<float>(value));
    }

    //------------------------------------------------------------------------------

    data::Object::sptr operator()(const std::string& value)
    {
        return data::String::New(value);
    }

    //------------------------------------------------------------------------------

    data::Object::sptr operator()(const camp::EnumObject&)
    {
        SIGHT_FATAL("Enter in void GetCampValueVisitor()(camp::EnumObject value) : case not managed");
        data::Object::sptr val;
        return val;
    }

    //------------------------------------------------------------------------------

    data::Object::sptr operator()(const camp::UserObject& value)
    {
        data::Object::sptr val;
        const camp::Class& metaclass = value.getClass();
        if(value.pointer())
        {
            if(!m_subObjPath.empty())
            {
                SIGHT_DEBUG(
                    "visit class= '" << metaclass.name() << "' ( classname = '" << value.call(
                        "classname"
                    ) << "' )"
                );
                data::Object* ptr = value.get<data::Object*>();
                data::reflection::visitor::GetObject visitor(ptr->getSptr(), m_subObjPath);
                val = visitor.get();
                m_pathVisitor->merge(visitor.getPathVisitor());
            }
            else
            {
                data::Object* ptr = value.get<data::Object*>();
                val = ptr->getSptr();
            }
        }
        else
        {
            SIGHT_THROW_EXCEPTION(
                data::reflection::exception::NullPointer(
                    "Object '" + metaclass.name() + "' not instanced."
                )
            );
        }

        return val;
    }
};

//-----------------------------------------------------------------------------

GetObject::GetObject(data::Object::csptr object, const std::string& subObjPath) :
    m_object(object),
    m_subObjPath(subObjPath),
    m_newSubObjPath(subObjPath),
    m_pathVisitor(std::make_shared<PathVisitor>(subObjPath))
{
    SIGHT_FATAL_IF("Cannot retrieve an object with an empty path.", subObjPath.empty());
    m_campObj      = camp::UserObject(*object);
    m_propertyName = this->getNextPropertyName();
}

//-----------------------------------------------------------------------------

GetObject::~GetObject()
{
}

//-----------------------------------------------------------------------------

void GetObject::visit(const camp::SimpleProperty& property)
{
    const std::string name(property.name());
    SIGHT_DEBUG("SimpleProperty name =" << name);
    if(name == m_propertyName)
    {
        m_pathVisitor->addObject(name);
        SIGHT_DEBUG("Ok SimpleProperty name =" << name);
        ::camp::Value elemValue = property.get(m_campObj);
        GetCampValueVisitor visitor(m_newSubObjPath, m_pathVisitor);
        m_subObject = elemValue.visit(visitor);
    }
}

//-----------------------------------------------------------------------------

void GetObject::visit(const camp::EnumProperty& property)
{
    SIGHT_FATAL_IF(
        "EnumProperty is not still managed : name =" << property.name(),
        property.name() == m_propertyName
    );
}

//-----------------------------------------------------------------------------

void GetObject::visit(const camp::MapProperty& property)
{
    const std::string name(property.name());
    SIGHT_DEBUG("MapProperty name =" << name);
    if(name == m_propertyName)
    {
        m_pathVisitor->addObject(name);
        SIGHT_DEBUG("Ok MapProperty name =" << name);
        std::string key = this->getNextPropertyName();

        std::pair< ::camp::Value, ::camp::Value> value;
        std::string mapKey;
        for(unsigned int var = 0 ; var < property.getSize(m_campObj) ; ++var)
        {
            value  = property.getElement(m_campObj, var);
            mapKey = value.first.to<std::string>();
            if(key == mapKey)
            {
                m_pathVisitor->addObject(key);
                GetCampValueVisitor visitor(m_newSubObjPath, m_pathVisitor);
                m_subObject = value.second.visit(visitor);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void GetObject::visit(const camp::ArrayProperty& property)
{
    const std::string name(property.name());
    SIGHT_DEBUG("ArrayProperty name =" << name);
    if(name == m_propertyName)
    {
        m_pathVisitor->addObject(name);
        SIGHT_DEBUG("Ok ArrayProperty name =" << name);
        std::string key = this->getNextPropertyName();

        std::size_t index = boost::lexical_cast<std::size_t>(key);

        m_pathVisitor->addObject(key);

        // If the index is out of range, camp throws an exception
        // We need to catch it because this means we failed to reach the object
        ::camp::Value elemValue;
        try
        {
            elemValue = property.get(m_campObj, index);
        }
        catch(const ::camp::OutOfRange&)
        {
            SIGHT_THROW_EXCEPTION_MSG(
                data::reflection::exception::NullPointer,
                "Index '" << index << "' not found in array property '" << name << "'."
            );
        }

        GetCampValueVisitor visitor(m_newSubObjPath, m_pathVisitor);
        m_subObject = elemValue.visit(visitor);
    }
}

//-----------------------------------------------------------------------------

void GetObject::visit(const camp::UserProperty& property)
{
    const std::string name(property.name());
    SIGHT_DEBUG("UserProperty name =" << name);
    if(name == m_propertyName)
    {
        m_pathVisitor->addObject(name);
        SIGHT_DEBUG("Ok UserProperty name =" << name);
        ::camp::Value elemValue = property.get(m_campObj);
        GetCampValueVisitor visitor(m_newSubObjPath, m_pathVisitor);
        m_subObject = elemValue.visit(visitor);
    }
}

//-----------------------------------------------------------------------------

void GetObject::visit(const camp::Function&)
{
}

//-----------------------------------------------------------------------------

std::string GetObject::getNextPropertyName()
{
    SIGHT_FATAL_IF("Path is empty.", m_newSubObjPath.empty());
    std::size_t dotPos = m_newSubObjPath.find(".");
    std::string nextItem;
    if(dotPos != std::string::npos)
    {
        nextItem        = m_newSubObjPath.substr(0, dotPos);
        m_newSubObjPath = m_newSubObjPath.substr(dotPos + 1);
    }
    else
    {
        nextItem        = m_newSubObjPath;
        m_newSubObjPath = "";
    }

    SIGHT_DEBUG("nextItem = " << nextItem);
    SIGHT_DEBUG("m_newSubObjPath = " << m_newSubObjPath);
    return nextItem;
}

//-----------------------------------------------------------------------------

data::Object::sptr GetObject::get()
{
    const camp::Class& camClass = ::camp::classByName(m_object->getClassname());
    camClass.visit(*this);
    return m_subObject;
}

//-----------------------------------------------------------------------------

bool GetObject::objectsFound() const
{
    return m_pathVisitor->allObjectsFound();
}

//-----------------------------------------------------------------------------

} // namespace visitor

} // namespace reflection

} // namespace sight::data
