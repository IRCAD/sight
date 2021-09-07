/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#if defined(__GNUC__)
#pragma GCC visibility push(default)
#endif
#include <iostream>
#include <sstream>
#include <libxml/xmlstring.h>
#if defined(__GNUC__)
#pragma GCC visibility pop
#endif

#include <core/base.hpp>

#include "core/runtime/Module.hpp"
#include "core/runtime/detail/ExtensionPoint.hpp"
#include "core/runtime/detail/Runtime.hpp"
#include "core/runtime/detail/io/Validator.hpp"
#include "core/runtime/Extension.hpp"
#include "core/runtime/helper.hpp"

namespace sight::core::runtime
{

//------------------------------------------------------------------------------

Extension::Extension(
    std::shared_ptr<Module> module,
    const std::string& id,
    const std::string& point,
    xmlNodePtr xmlNode
) :
    ModuleElement(module),
    m_id(filterID(id)),
    m_point(filterID(point)),
    m_xmlDoc(xmlNewDoc(BAD_CAST "1.0")),
    m_xmlNode(xmlCopyNode(xmlNode, 1)),
    m_validity(UnknownValidity)
{
    xmlDocSetRootElement(m_xmlDoc, m_xmlNode);
}

//------------------------------------------------------------------------------

Extension::~Extension()
{
    xmlFreeDoc(m_xmlDoc);
}

//------------------------------------------------------------------------------

const std::string& Extension::getIdentifier() const
{
    return m_id;
}

//------------------------------------------------------------------------------

const std::string& Extension::getPoint() const
{
    return m_point;
}

//------------------------------------------------------------------------------

xmlNodePtr Extension::getXmlNode() const
{
    return m_xmlNode;
}

//------------------------------------------------------------------------------

Extension::Validity Extension::getValidity() const
{
    return m_validity;
}

//------------------------------------------------------------------------------

Extension::Validity Extension::validate()
{
    // Skips the validation if already done.
    if(m_validity != UnknownValidity)
    {
        return m_validity;
    }

    // Retrieves the extension point.
    detail::Runtime* rntm(detail::Runtime::getDefault());
    std::shared_ptr<detail::ExtensionPoint> point(rntm->findExtensionPoint(m_point));

    // Checks that the point exists.
    if(!point)
    {
        throw RuntimeException(m_point + " : invalid point reference.");
    }

    // Validates the extension.
    std::shared_ptr<detail::io::Validator> validator(point->getExtensionValidator());
    SIGHT_ASSERT("The validator creation failed for the point " << point->getIdentifier(), validator);

    // Check extension XML Node <extension id="xxx" implements="yyy" >...</extension>
    validator->clearErrorLog();
    if(validator->validate(m_xmlNode) == true)
    {
        m_validity = Valid;
    }
    else
    {
        m_validity = Invalid;
        const std::string identifier = m_id.empty() ? "anonymous" : m_id;
        SIGHT_ERROR(
            "In bundle " << getModule()->getIdentifier() << ". " << identifier
            << ": invalid extension XML element node does not respect schema. Verification error log is : "
            << std::endl << validator->getErrorLog()
        );
    }

    return m_validity;
}

//------------------------------------------------------------------------------

void Extension::operator=(const Extension&) noexcept
{
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime
