/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#pragma once

#include <sight/core/config.hpp>

#include <libxml/tree.h>
#include <libxml/xmlschemastypes.h>

#include <filesystem>
#include <sstream>
#include <string>

namespace sight::core::runtime::detail::io
{

/**
 * @brief   Implements an XML validator.
 *
 * @warning Potential Bug : class Validator can generate bug (infinite loop) when used with other Validator class
 * instance.
 * It seems that this class use a static ivar to save schemaValidationContext. If your call a instance of an visitor
 * this ivar should not be
 * directly well positionned. So the recommendation is to construct Validator-Use it-destry it and never store him
 */
class validator
{
public:

    /**
     * @brief   Copy Constructor
     *
     * Copy a validator.
     *
     * @param   _validator to be copied
     */
    validator(const validator& _validator);

    /**
     * @brief   Constructor
     *
     * Builds a validator with a string containing a XML schema.
     *
     * @param   _schema  a string containing the XML schema
     */
    validator(std::string _schema);

    /**
     * @brief   Constructor
     *
     * Builds a validator with a path to a file containing an XML schema.
     *
     * @param   _path    a path to the xml schema file to load
     */
    validator(const std::filesystem::path& _path);

    /**
     * @brief   Destructor
     */
    ~validator();

    /**
     * @brief   Clears the error log.
     */
    void clear_error_log();

    /**
     * @brief   Retrieves the error log content.
     *
     * @return  a string containing the error log
     */
    std::string get_error_log() const;

    /**
     * @brief   Validates the given file.
     *
     * @param   _xml_file a path to an xml file to validate
     *
     * @return  true when the validation succeeds, false otherwise
     */
    bool validate(const std::filesystem::path& _xml_file);

    /**
     * @brief   Validates the given xml node.
     *
     * @param   _node    a pointer to an xml node
     *
     * @return  true when the validation succeeds, false otherwise
     */
    bool validate(xmlNodePtr _node);

    /**
     * @brief   Returns the xsd content in string format
     *
     * @return  xsd of the validator
     */
    std::string get_xsd_content();

private:

    void initialize_context();

    std::string m_xsd_content;
    std::ostringstream m_error_log;

    using schema_parser_ctxt_sptr = std::shared_ptr<xmlSchemaParserCtxt>;
    using schema_sptr             = std::shared_ptr<xmlSchema>;
    using schema_valid_ctxt_sptr  = std::shared_ptr<xmlSchemaValidCtxt>;

    schema_parser_ctxt_sptr m_schema_parser_context;
    schema_sptr m_schema;
    schema_valid_ctxt_sptr m_schema_valid_context;

    static void error_handler(void* _user_data, xmlErrorPtr _error);
};

} // namespace sight::core::runtime::detail::io
