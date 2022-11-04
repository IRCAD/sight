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

#pragma once

#include "core/config.hpp"
#include "core/runtime/Extension.hpp"

#include <libxml/tree.h>

#include <string>

namespace sight::core::runtime::detail
{

class Module;

/**
 * @brief   Defines the extension class.
 */
class Extension final : public sight::core::runtime::Extension
{
public:

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
        std::shared_ptr<core::runtime::Module> module,
        const std::string& id,
        const std::string& point,
        xmlNodePtr xmlNode
    );
    /**
     * @brief   Destructor
     */
    ~Extension() final;

    /// @copydoc core::runtime::Extension::getConfig
    const core::runtime::config_t& getConfig() const final;

    /**
     * @brief   Retrieves the xml node that represents the extension
     *
     * @return  a pointer to an xml node
     */
    [[nodiscard]] xmlNodePtr getXmlNode() const;

    /**
     * @brief   Validates the extension.
     *
     * Calling this method more than one time will only cause
     * the current validity to be returned.
     *
     * @return  the extension validity
     */
    Validity validate();

    /**
     * @brief       Sets the configuration of the extension.
     * @param[in]   element a shared pointer to the configuration element to add
     */
    void setConfig(const core::runtime::config_t& config);

private:

    xmlDocPtr m_xmlDoc;                    ///< A pointer to the xml document that contains the xml node representing
                                           ///< the extension
    xmlNodePtr m_xmlNode;                  ///< A pointer to the xml node that represents the extension
    Validity m_validity {UnknownValidity}; ///< The validity state of the extension
    core::runtime::config_t m_config;      ///< Configuration of the extension
};

} // namespace sight::core::runtime::detail
