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
#include "core/runtime/ConfigurationElementContainer.hpp"
#include "core/runtime/ModuleElement.hpp"

#include <libxml/tree.h>

#include <string>

namespace sight::core::runtime
{

class Module;

namespace io
{

class ModuleDescriptorReader;

} // namespace io

/**
 * @brief   Defines the extension class.
 */
struct CORE_CLASS_API Extension : public ModuleElement,
                                  public ConfigurationElementContainer
{
    /**
     * @brief   Defines the validity states of an extension
     */
    typedef enum
    {
        UnknownValidity, ///< The extension has not been validated.
        Valid,           ///< The extension passed the validation.
        Invalid          ///< The extension failed the validation.
    } Validity;

    /**
     * @brief   Destructor
     */
    ~Extension();

    /**
     * @brief   Retrieves the extension identifier.
     *
     * @return  a string containing the extension identifier (may be empty)
     */
    CORE_API const std::string& getIdentifier() const;

    /**
     * @brief   Retrieves the extension point identifier.
     *
     * @return  a string containing the extension point identifier
     */
    CORE_API const std::string& getPoint() const;

    /**
     * @brief   Retrieves the validity of the extension.
     *
     * The validity is computed using the xml node representing the extension and
     * the xml schema of the extension's related extension point. When the extension
     * point does not provide a schema, the extension is assumed to be valid.
     *
     * @return  the validity
     */
    CORE_API Validity getValidity() const;

    /**
     * @brief   Retrieves the xml node that represents the extension
     *
     * @return  a pointer to an xml node
     */
    CORE_API xmlNodePtr getXmlNode() const;

    /**
     * @brief   Validates the extension.
     *
     * Calling this method more than one time will only cause
     * the current validity to be returned.
     *
     * @return  the extension validity
     */
    CORE_API Validity validate();

    //TODO: Fix visibility
    //protected:

    /**
     * @brief       Constructor.
     *
     * @param[in]   module  a pointer to the module the extension is attached to
     * @param[in]   id      a string containing the extension identifier
     * @param[in]   point   a string containing the extension point identifier
     * @param[in]   xmlNode a pointer to the xml node that represents the extension
     *
     * @todo        test parameters validity
     */
    Extension(
        std::shared_ptr<Module> module,
        const std::string& id,
        const std::string& point,
        const xmlNodePtr xmlNode
    );

    private:

        const std::string m_id;    ///< A string containing the extension identifier.
        const std::string m_point; ///< A string containing the extension point identifier the extension will be
        // connected to.
        xmlDocPtr m_xmlDoc; ///< A pointer to the xml document that contains the xml node representing the
        // extension
        xmlNodePtr m_xmlNode; ///< A pointer to the xml node that represents the extension
        Validity m_validity;  ///< The validity state of the extension

        /**
         * @brief   Assignment operator.
         *
         * @remark  Assignment is forbidden.
         */
        void operator=(const Extension&) noexcept;
};

} // namespace sight::core::runtime
