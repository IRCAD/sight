/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "validator.hpp"

#include "core/runtime/runtime_exception.hpp"

#include <core/base.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xinclude.h>

#include <filesystem>
#include <utility>

namespace sight::core::runtime::detail::io
{

//------------------------------------------------------------------------------

validator::validator(std::string _schema) :
    m_xsd_content(std::move(_schema))
{
    initialize_context();
}

//------------------------------------------------------------------------------

validator::validator(const std::filesystem::path& _path)
{
    std::string str_path(_path.string());
    // Checks the path validity.
    if(!std::filesystem::exists(_path) || std::filesystem::is_directory(_path))
    {
        throw runtime_exception(str_path + ": is not a valid path to an xml schema file.");
    }

    m_xsd_content = str_path;

    initialize_context();
}

//------------------------------------------------------------------------------

void validator::clear_error_log()
{
    m_error_log.str(std::string());
}

//------------------------------------------------------------------------------

std::string validator::get_error_log() const
{
    return m_error_log.str();
}

//------------------------------------------------------------------------------

void validator::initialize_context()
{
    if(auto schema_parser_context = schema_parser_ctxt_sptr(
           xmlSchemaNewParserCtxt(m_xsd_content.c_str()),
           xmlSchemaFreeParserCtxt
    ); schema_parser_context != nullptr)
    {
        // Set the structured error callback
        xmlSchemaSetParserStructuredErrors(schema_parser_context.get(), validator::error_handler, this);

        // Load XML schema content
        if(!m_schema)
        {
            m_schema = schema_sptr(xmlSchemaParse(schema_parser_context.get()), xmlSchemaFree);
            if(!m_schema)
            {
                return;
            }
        }

        // Create XML schemas validation context
        if((m_schema_valid_context = schema_valid_ctxt_sptr(
                xmlSchemaNewValidCtxt(m_schema.get()),
                xmlSchemaFreeValidCtxt
        )))
        {
            // Set the structured error callback
            xmlSchemaSetValidStructuredErrors(m_schema_valid_context.get(), validator::error_handler, this);
        }
    }
    else
    {
        return;
    }
}

//------------------------------------------------------------------------------

bool validator::validate(const std::filesystem::path& _xml_file)
{
    int result = 0;

    xmlDocPtr xml_doc = xmlParseFile(_xml_file.string().c_str());
    if(xml_doc == nullptr)
    {
        throw std::ios_base::failure("Unable to parse the XML file " + _xml_file.string());
    }

    xmlNodePtr xml_root = xmlDocGetRootElement(xml_doc);
    if(xmlXIncludeProcessTreeFlags(xml_root, XML_PARSE_NOBASEFIX) == -1)
    {
        xmlFreeDoc(xml_doc);
        throw std::ios_base::failure(std::string("Unable to manage xinclude !"));
    }

    if(!m_schema_valid_context)
    {
        return false;
    }

    result = xmlSchemaValidateDoc(m_schema_valid_context.get(), xml_doc);

    xmlFreeDoc(xml_doc);

    if(result != 0)
    {
        SIGHT_WARN("Validator::validation NOK, xml = " << _xml_file.string());
        SIGHT_WARN("Validator::validation NOK, xsd = " << get_xsd_content());
        SIGHT_ERROR("Validator::validation NOK, error log = " << get_error_log());
    }

    return result == 0;
}

//------------------------------------------------------------------------------

bool validator::validate(xmlNodePtr _node)
{
    int result = 0;

    if(!m_schema_valid_context)
    {
        return false;
    }

    result = xmlSchemaValidateOneElement(m_schema_valid_context.get(), _node);

    if(result != 0)
    {
        xmlBufferPtr buffer = xmlBufferCreate();
        xmlNodeDump(buffer, _node->doc, _node, 1, 1);
        SIGHT_WARN("Validator::validation NOK, node :\n " << buffer->content);
        xmlBufferFree(buffer);
        SIGHT_WARN("Validator::validation NOK, xsd = " << get_xsd_content());
        SIGHT_ERROR("Validator::validation NOK, error log = " << get_error_log());
    }

    return result == 0;
}

//------------------------------------------------------------------------------

bool validator::validate(const config_t& _config)
{
    int result = 0;

    if(!m_schema_valid_context)
    {
        return false;
    }

    std::stringstream config_ss;
    boost::property_tree::write_xml(config_ss, _config);

    auto config_string = config_ss.str();
    xmlDocPtr document = xmlParseMemory(config_string.c_str(), static_cast<int>(config_string.size()));

    result = xmlSchemaValidateDoc(m_schema_valid_context.get(), document);

    xmlFreeDoc(document);

    if(result != 0)
    {
        SIGHT_WARN("Validator::validation NOK, xml = " << config_string);
        SIGHT_WARN("Validator::validation NOK, xsd = " << get_xsd_content());
        SIGHT_ERROR("Validator::validation NOK, error log = " << get_error_log());
    }

    return result == 0;
}

//------------------------------------------------------------------------------

void validator::error_handler(
    void* _user_data,
#if LIBXML_VERSION >= 21305
    const xmlError* _error
#else
    xmlErrorPtr _error
#endif
)
{
    auto* validator = reinterpret_cast<class validator*>(_user_data);

    validator->m_error_log << "At line " << _error->line << ": " << _error->message;
}

//------------------------------------------------------------------------------

std::string validator::get_xsd_content()
{
    return m_xsd_content;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail::io
