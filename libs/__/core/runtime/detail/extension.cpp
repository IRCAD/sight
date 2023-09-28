/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/runtime/extension.hpp"

#include "core/runtime/detail/extension_point.hpp"
#include "core/runtime/detail/io/validator.hpp"
#include "core/runtime/detail/runtime.hpp"
#include "core/runtime/module.hpp"

#include <libxml/xmlstring.h>

namespace sight::core::runtime::detail
{

//------------------------------------------------------------------------------

extension::extension(
    std::shared_ptr<core::runtime::module> module,
    const std::string& id,
    const std::string& point,
    xmlNodePtr xml_node
) :
    sight::core::runtime::extension(module, id, point),
    m_xml_doc(xmlNewDoc(reinterpret_cast<const xmlChar*>("1.0"))),
    m_xml_node(xmlCopyNode(xml_node, 1))
{
    xmlDocSetRootElement(m_xml_doc, m_xml_node);
}

//------------------------------------------------------------------------------

extension::~extension()
{
    xmlFreeDoc(m_xml_doc);
}

//------------------------------------------------------------------------------

xmlNodePtr extension::get_xml_node() const
{
    return m_xml_node;
}

//------------------------------------------------------------------------------

extension::validity extension::validate()
{
    // Skips the validation if already done.
    if(m_validity != unknown_validity)
    {
        return m_validity;
    }

    // Retrieves the extension point.
    const auto& runtime = detail::runtime::get();
    std::shared_ptr<detail::extension_point> ext_point(runtime.find_extension_point(point()));

    // Checks that the point exists.
    if(!ext_point)
    {
        throw runtime_exception(point() + " : invalid point reference.");
    }

    // Validates the extension.
    std::shared_ptr<detail::io::validator> validator(ext_point->get_extension_validator());
    SIGHT_ASSERT("The validator creation failed for the extension point " << ext_point->identifier(), validator);

    // Check extension XML Node <extension id="xxx" implements="yyy" >...</extension>
    validator->clear_error_log();
    if(validator->validate(m_xml_node))
    {
        m_validity = valid;
    }
    else
    {
        m_validity = invalid;
        const auto id                = this->identifier();
        const std::string identifier = id.empty() ? "anonymous" : id;
        SIGHT_ERROR(
            "In bundle " << get_module()->identifier() << ". " << identifier
            << ": invalid extension XML element node does not respect schema. Verification error log is : "
            << std::endl << validator->get_error_log()
        );
    }

    return m_validity;
}

//------------------------------------------------------------------------------

void extension::set_config(const core::runtime::config_t& config)
{
    m_config = config;
}

//------------------------------------------------------------------------------

const core::runtime::config_t& extension::get_config() const
{
    return m_config;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail
