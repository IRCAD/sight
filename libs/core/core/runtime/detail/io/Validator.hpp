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

#pragma once

#include "core/config.hpp"

#include <libxml/tree.h>
#include <libxml/xmlschemastypes.h>

#include <filesystem>
#include <sstream>
#include <string>

namespace sight::core::runtime
{

namespace detail
{

namespace io
{

/**
 * @brief   Implements an XML validator.
 *
 * @warning Potential Bug : class Validator can generate bug (infinite loop) when used with other Validator class
 * instance.
 * It seems that this class use a static ivar to save schemaValidationContext. If your call a instance of an visitor
 * this ivar should not be
 * directly well positionned. So the recommandation is to construct Validator-Use it-destry it and never store him
 */
class Validator
{
public:

    /**
     * @brief   Copy Constructor
     *
     * Copy a validator.
     *
     * @param   validator to be copied
     */
    Validator(const Validator& validator);

    /**
     * @brief   Constructor
     *
     * Builds a validator with a string containing a XML shema.
     *
     * @param   schema  a string containing the XML schema
     */
    Validator(const std::string& schema);

    /**
     * @brief   Constructor
     *
     * Builds a validator with a path to a file containing an XML schema.
     *
     * @param   path    a path to the xml schema file to load
     */
    Validator(const std::filesystem::path& path);

    /**
     * @brief   Destructor
     */
    ~Validator();

    /**
     * @brief   Clears the error log.
     */
    void clearErrorLog();

    /**
     * @brief   Retrieves the error log content.
     *
     * @return  a string containing the error log
     */
    const std::string getErrorLog() const;

    /**
     * @brief   Validates the given file.
     *
     * @param   xmlFile a path to an xml file to validate
     *
     * @return  true when the validation succeeds, false otherwise
     */
    bool validate(const std::filesystem::path& xmlFile);

    /**
     * @brief   Validates the given xml node.
     *
     * @param   node    a pointer to an xml node
     *
     * @return  true when the validation succeeds, false otherwise
     */
    bool validate(xmlNodePtr node);

    /**
     * @brief   Returns the xsd content in string format
     *
     * @return  xsd of the validator
     */
    std::string getXsdContent();

private:

    void initializeContext();

    std::string m_xsd_content;
    std::ostringstream m_errorLog;

    typedef std::shared_ptr<xmlSchemaParserCtxt> SchemaParserCtxtSptr;
    typedef std::shared_ptr<xmlSchema> SchemaSptr;
    typedef std::shared_ptr<xmlSchemaValidCtxt> SchemaValidCtxtSptr;

    SchemaParserCtxtSptr m_schemaParserContext;
    SchemaSptr m_schema;
    SchemaValidCtxtSptr m_schemaValidContext;

    static void ErrorHandler(void* userData, xmlErrorPtr error);
};

} // namespace io

} // namespace detail

} // namespace sight::core::runtime
