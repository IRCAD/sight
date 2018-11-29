/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "fwAtomsPatch/infos/log.hpp"
#include "fwAtomsPatch/helper/functions.hpp"
#include "fwAtomsPatch/helper/Object.hpp"

namespace fwAtomsPatch
{
namespace helper
{

Object::Object(::fwAtoms::Object::sptr metaObject) : m_object(metaObject)
{
}

// ----------------------------------------------------------------------------

Object::~Object()
{
}

// ----------------------------------------------------------------------------

void Object::addAttribute(const std::string& name,
                          ::fwAtoms::Base::sptr value,
                          conditions::Abstract::sptr condition)
{
    if(m_object->getAttributes().find(name) == m_object->getAttributes().end())
    {
        if(condition->test(value))
        {
            m_object->setAttribute(name, value);
            fwAtomsPatchAddAttributeLogMacro("'"
                                             + ::fwAtomsPatch::helper::getClassname(m_object) + "|"
                                             + ::fwAtomsPatch::helper::getVersion(m_object) + "' : '" + name + "'");
        }
    }
    else
    {
        fwAtomsPatchErrorLogMacro("Attribute '" + name + "' already exists");
    }
}

// ----------------------------------------------------------------------------

void Object::addOrReplaceAttribute(const std::string& name,
                                   ::fwAtoms::Base::sptr value,
                                   conditions::Abstract::sptr condition)
{
    if(m_object->getAttributes().find(name) != m_object->getAttributes().end())
    {
        m_object->eraseAttribute(name);
        fwAtomsPatchEraseAttributeLogMacro("'"
                                           + ::fwAtomsPatch::helper::getClassname(m_object) + "|"
                                           + ::fwAtomsPatch::helper::getVersion(m_object) + "' : '" + name + "'");
    }

    if(condition->test(value))
    {
        m_object->setAttribute(name, value);
        fwAtomsPatchAddAttributeLogMacro("'"
                                         + ::fwAtomsPatch::helper::getClassname(m_object) + "|"
                                         + ::fwAtomsPatch::helper::getVersion(m_object) + "' : '" + name + "'");
    }
}

// ----------------------------------------------------------------------------

void Object::removeAttribute(const std::string& name,
                             conditions::Abstract::sptr condition)
{
    ::fwAtoms::Object::AttributesType::const_iterator it = m_object->getAttributes().find(name);

    if(it != m_object->getAttributes().end())
    {
        if(condition->test(it->second))
        {
            m_object->eraseAttribute(name);
            fwAtomsPatchEraseAttributeLogMacro("'"
                                               + ::fwAtomsPatch::helper::getClassname(m_object) + "|"
                                               + ::fwAtomsPatch::helper::getVersion(m_object) + "' : '" + name + "'");
        }
    }
    else
    {
        fwAtomsPatchErrorLogMacro("Missing attribute '" + name + "'");
    }
}

// ----------------------------------------------------------------------------

void Object::replaceAttribute(const std::string& name,
                              ::fwAtoms::Base::sptr newValue,
                              conditions::Abstract::sptr condition)
{
    ::fwAtoms::Object::AttributesType::const_iterator cIt = m_object->getAttributes().find(name);

    if(cIt != m_object->getAttributes().end())
    {
        fwAtomsPatchReplaceAttributeLogMacro("'"
                                             + ::fwAtomsPatch::helper::getClassname(m_object) + "|"
                                             + ::fwAtomsPatch::helper::getVersion(m_object) + "' : '" + name + "'");

        if(condition->test(cIt->second, newValue))
        {
            m_object->setAttribute(name, newValue);
        }
        else
        {
            m_object->setAttribute(name, condition->getDefaultValue());
            fwAtomsPatchOutOfRangeLogMacro("Value '" + name
                                           + "' is out of range, using default value : '"
                                           + condition->getDefaultValue()->getString() + "'");
        }
    }
    else
    {
        fwAtomsPatchErrorLogMacro("Missing attribute '" + name + "'");
    }
}

// ----------------------------------------------------------------------------

void Object::renameAttribute(const std::string& name, const std::string& newName,
                             conditions::Abstract::sptr condition)
{
    ::fwAtoms::Object::AttributesType::const_iterator it = m_object->getAttributes().find(name);

    if(it != m_object->getAttributes().end())
    {
        if(condition->test(it->second))
        {
            ::fwAtoms::Base::sptr base = m_object->getAttribute(name);
            fwAtomsPatchEraseAttributeLogMacro("'"
                                               + ::fwAtomsPatch::helper::getClassname(m_object) + "|"
                                               + ::fwAtomsPatch::helper::getVersion(m_object) + "' : '" + name + "'");
            fwAtomsPatchAddAttributeLogMacro("'"
                                             + ::fwAtomsPatch::helper::getClassname(m_object) + "|"
                                             + ::fwAtomsPatch::helper::getVersion(m_object) + "' : '" + newName + "'");
            m_object->setAttribute(newName, base);
            m_object->eraseAttribute(name);
        }
    }
    else
    {
        fwAtomsPatchErrorLogMacro("Missing attribute '" + name + "'");
    }
}

// ----------------------------------------------------------------------------

::fwAtoms::Object::sptr Object::getObject() const
{
    return m_object;
}

} //helper
} //fwAtomHelper
