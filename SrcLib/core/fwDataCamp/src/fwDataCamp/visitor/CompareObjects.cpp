/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataCamp/visitor/CompareObjects.hpp"

#include <fwCore/Exception.hpp>
#include <fwData/camp/mapper.hpp>
#include <fwMemory/BufferObject.hpp>

#include <boost/functional/hash.hpp>
#include <string>

namespace fwDataCamp
{

namespace visitor
{

const std::string CompareObjects::s_MISSING_PROPERTY("CompareObjects::s_MISSING_PROPERTY");

typedef CompareObjects::PropsMapType::value_type PropType;

struct PropertyVisitor : public camp::ValueVisitor< PropType >
{
    std::string m_prefix;
    SPTR(CompareObjects::PropsMapType) m_props;

    PropertyVisitor(std::string prefix) : m_prefix(prefix)
    {
    }

    PropertyVisitor(std::string prefix, SPTR(CompareObjects::PropsMapType)props)
        : m_prefix(prefix), m_props(props)
    {
    }

    PropType operator()(camp::NoType value)
    {
        return std::make_pair("", "");
    }

    PropType operator()(bool value)
    {
        return std::make_pair(m_prefix, value ? "true" : "false");
    }

    PropType operator()(long value)
    {
        return std::make_pair(m_prefix, ::boost::lexical_cast<std::string>(value));
    }

    PropType operator()(double value)
    {
        return std::make_pair(m_prefix, ::boost::lexical_cast<std::string>(value));
    }

    PropType operator()(const std::string& value)
    {
        return std::make_pair(m_prefix, value);
    }

    PropType operator()(const camp::EnumObject& value)
    {
        return std::make_pair(m_prefix, value.name());
    }

    PropType operator()(const camp::UserObject& value)
    {
        const camp::Class& metaclass = value.getClass();
        PropType prop;

        if (value.pointer())
        {
            std::string classname = value.call("classname").to<std::string>();

            if (metaclass.name() != classname)
            {
                const camp::Class& newMetaclass = ::camp::classByName(classname);
                CompareObjects visitor(value, m_prefix, m_props);
                newMetaclass.visit(visitor);
            }
            else if(classname == "::fwMemory::BufferObject")
            {
                ::fwMemory::BufferObject* bo = value.get< ::fwMemory::BufferObject* >();
                if(bo)
                {
                    ::fwMemory::BufferObject::Lock lock = bo->lock();
                    if(lock.getBuffer())
                    {
                        char* buffer               = static_cast< char* >(lock.getBuffer());
                        std::size_t seed           = 0;
                        const std::size_t buffsize = bo->getSize();
                        for(size_t i = 0; i < buffsize; ++i)
                        {
                            ::boost::hash_combine(seed, buffer[i]);
                        }
                        return std::make_pair(m_prefix, ::boost::lexical_cast< std::string >(seed));
                    }
                }
            }
            else
            {
                CompareObjects visitor(value, m_prefix, m_props);
                metaclass.visit(visitor);
            }
        }
        else
        {
            OSLM_INFO("try visiting class= '" << metaclass.name() << " but a null pointer was found");
        }
        return prop;
    }
};

//-----------------------------------------------------------------------------

CompareObjects::CompareObjects()
{
    m_props = SPTR(PropsMapType)(new PropsMapType);
}

//-----------------------------------------------------------------------------

CompareObjects::CompareObjects(
    const ::camp::UserObject& obj, const std::string& prefix, SPTR(PropsMapType)props)
    : m_campObj(obj), m_prefix(prefix), m_props(props)
{
}

//-----------------------------------------------------------------------------

CompareObjects::~CompareObjects()
{
}

//-----------------------------------------------------------------------------

void CompareObjects::visit(const camp::SimpleProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name ( property.name() );
    OSLM_DEBUG("SimpleProperty name = " << name);
    ::camp::Value elemValue = property.get(m_campObj);
    PropertyVisitor visitor(getPath(name), m_props);
    PropType pt = elemValue.visit(visitor);
    if(!pt.first.empty())
    {
        m_props->insert(m_props->end(), pt);
    }
}

//-----------------------------------------------------------------------------

void CompareObjects::visit(const camp::EnumProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name ( property.name() );
    ::camp::Value elemValue = property.get(m_campObj);

    PropertyVisitor visitor(getPath(name), m_props);
    PropType pt = elemValue.visit(visitor);
    if(!pt.first.empty())
    {
        m_props->insert(m_props->end(), pt);
    }
}

//-----------------------------------------------------------------------------

void CompareObjects::visit(const camp::MapProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name(property.name());
    OSLM_DEBUG("MapProperty name = " << name);

    std::pair< ::camp::Value, ::camp::Value > value;
    std::string mapKey;
    for (unsigned int i = 0; i < property.getSize(m_campObj); ++i)
    {
        value  = property.getElement(m_campObj, i);
        mapKey = value.first.to< std::string >();
        PropertyVisitor visitor(getPath(name + "." + mapKey), m_props);
        PropType pt = value.second.visit(visitor);
        if(!pt.first.empty())
        {
            m_props->insert(m_props->end(), pt);
        }
    }
}

//-----------------------------------------------------------------------------

void CompareObjects::visit(const camp::ArrayProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name(property.name());
    OSLM_DEBUG( "ArrayProperty name =" << name );

    for(unsigned int i = 0; i < property.size(m_campObj); ++i)
    {
        ::camp::Value elemValue = property.get(m_campObj, i);
        std::stringstream ss;
        ss << name << "." << i;
        PropertyVisitor visitor(getPath(ss.str()), m_props);
        PropType pt = elemValue.visit(visitor);
        if(!pt.first.empty())
        {
            m_props->insert(m_props->end(), pt);
        }
    }
}

//-----------------------------------------------------------------------------

void CompareObjects::visit(const camp::UserProperty& property)
{
    SLM_TRACE_FUNC();
    const std::string name ( property.name() );
    OSLM_DEBUG( "UserProperty name =" << name );
    ::camp::Value elemValue = property.get( m_campObj );

    if(m_campObj.call("is_a", ::camp::Args("::fwData::Object")).to<bool>())
    {
        PropertyVisitor visitor(getPath(name), m_props);
        PropType pt = elemValue.visit(visitor);
        if(!pt.first.empty())
        {
            m_props->insert(m_props->end(), pt);
        }
    }
}

//-----------------------------------------------------------------------------

void CompareObjects::visit(const camp::Function& function)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

std::string CompareObjects::getPath(const std::string& property) const
{
    return m_prefix + (!m_prefix.empty() ? "." : "") + property;
}

//-----------------------------------------------------------------------------

void CompareObjects::compare(SPTR(::fwData::Object)objRef, SPTR(::fwData::Object)objComp)
{
    if(objRef->getClassname() != objComp->getClassname())
    {
        std::stringstream ss;
        ss << "Classnames mismatch : '" << objRef->getClassname() << "' (reference object) vs. '"
           << objComp->getClassname() << "' (compared object)";
        throw ::fwCore::Exception(ss.str());
    }

    m_objRef  = objRef;
    m_objComp = objComp;

    SLM_ASSERT("Reference object not defined", m_objRef);
    m_campObj = ::camp::UserObject(m_objRef.get());
    const ::camp::Class & classRef = ::camp::classByName(m_objRef->getClassname());
    classRef.visit(*this);
    m_propsRef = ::boost::move(*m_props);
    m_props->clear();

    SLM_ASSERT("Reference object not defined", m_objComp);
    m_campObj = ::camp::UserObject(m_objComp.get());
    const ::camp::Class & classComp = ::camp::classByName(m_objComp->getClassname());
    classComp.visit(*this);
    m_propsComp = ::boost::move(*m_props);
    m_props->clear();

    for(PropsMapType::value_type prop :  m_propsComp)
    {
        if(m_propsRef.find(prop.first) != m_propsRef.end())
        {
            if(m_propsRef[prop.first] != prop.second)
            {
                (*m_props)[prop.first] = prop.second;
            }
        }
        else
        {
            (*m_props)[prop.first] = s_MISSING_PROPERTY;
        }
    }

    for(PropsMapType::value_type prop :  m_propsRef)
    {
        if(m_propsComp.find(prop.first) == m_propsComp.end() && m_props->find(prop.first) == m_props->end())
        {
            (*m_props)[prop.first] = s_MISSING_PROPERTY;
        }
    }
}

//-----------------------------------------------------------------------------

} // visitor

} // fwDataCamp

