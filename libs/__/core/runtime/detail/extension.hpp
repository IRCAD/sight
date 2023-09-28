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

#pragma once

#include "core/config.hpp"
#include "core/runtime/extension.hpp"

#include <libxml/tree.h>

#include <string>

namespace sight::core::runtime::detail
{

class module;

/**
 * @brief   Defines the extension class.
 */
class extension final : public sight::core::runtime::extension
{
public:

    /**
     * @brief   Defines the validity states of an extension
     */
    typedef enum
    {
        unknown_validity, ///< The extension has not been validated.
        valid,            ///< The extension passed the validation.
        invalid           ///< The extension failed the validation.
    } validity;

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
    extension(
        std::shared_ptr<core::runtime::module> module,
        const std::string& id,
        const std::string& point,
        xmlNodePtr xml_node
    );
    /**
     * @brief   Destructor
     */
    ~extension() final;

    /// @copydoc core::runtime::extension::get_config
    const core::runtime::config_t& get_config() const final;

    /**
     * @brief   Retrieves the xml node that represents the extension
     *
     * @return  a pointer to an xml node
     */
    [[nodiscard]] xmlNodePtr get_xml_node() const;

    /**
     * @brief   Validates the extension.
     *
     * Calling this method more than one time will only cause
     * the current validity to be returned.
     *
     * @return  the extension validity
     */
    validity validate();

    /**
     * @brief       Sets the configuration of the extension.
     * @param[in]   element a shared pointer to the configuration element to add
     */
    void set_config(const core::runtime::config_t& config);

private:

    xmlDocPtr m_xml_doc;                    ///< A pointer to the xml document that contains the xml node representing
                                            ///< the extension
    xmlNodePtr m_xml_node;                  ///< A pointer to the xml node that represents the extension
    validity m_validity {unknown_validity}; ///< The validity state of the extension
    core::runtime::config_t m_config;       ///< Configuration of the extension
};

} // namespace sight::core::runtime::detail
