/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "core/runtime/Extension.hpp"

#include "core/runtime/detail/ExtensionPoint.hpp"
#include "core/runtime/detail/io/Validator.hpp"
#include "core/runtime/detail/Runtime.hpp"
#include "core/runtime/helper.hpp"
#include "core/runtime/Module.hpp"

#include <libxml/xmlstring.h>

namespace sight::core::runtime::detail
{

//------------------------------------------------------------------------------

Extension::Extension(
    std::shared_ptr<core::runtime::Module> module,
    const std::string& id,
    const std::string& point,
    xmlNodePtr xmlNode
) :
    sight::core::runtime::Extension(module, id, point),
    m_xmlDoc(xmlNewDoc(reinterpret_cast<const xmlChar*>("1.0"))),
    m_xmlNode(xmlCopyNode(xmlNode, 1))
{
    xmlDocSetRootElement(m_xmlDoc, m_xmlNode);
}

//------------------------------------------------------------------------------

Extension::~Extension()
{
    xmlFreeDoc(m_xmlDoc);
}

//------------------------------------------------------------------------------

xmlNodePtr Extension::getXmlNode() const
{
    return m_xmlNode;
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
    const auto& runtime = detail::Runtime::get();
    std::shared_ptr<detail::ExtensionPoint> point(runtime.findExtensionPoint(getPoint()));

    // Checks that the point exists.
    if(!point)
    {
        throw RuntimeException(getPoint() + " : invalid point reference.");
    }

    // Validates the extension.
    std::shared_ptr<detail::io::Validator> validator(point->getExtensionValidator());
    SIGHT_ASSERT("The validator creation failed for the point " << point->getIdentifier(), validator);

    // Check extension XML Node <extension id="xxx" implements="yyy" >...</extension>
    validator->clearErrorLog();
    if(validator->validate(m_xmlNode))
    {
        m_validity = Valid;
    }
    else
    {
        m_validity = Invalid;
        const auto id                = this->getIdentifier();
        const std::string identifier = id.empty() ? "anonymous" : id;
        SIGHT_ERROR(
            "In bundle " << getModule()->getIdentifier() << ". " << identifier
            << ": invalid extension XML element node does not respect schema. Verification error log is : "
            << std::endl << validator->getErrorLog()
        );
    }

    return m_validity;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail
