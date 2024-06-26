/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <core/tools/failed.hpp>

#include <ui/__/action.hpp>

namespace sight::module::ui::qt
{

/**
 * @brief   This action launch a browser on the url given in configuration.
 */
class launch_browser : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(launch_browser, sight::ui::action);

    /// Constructor. Do nothing.
    launch_browser() noexcept;

    /// Destructor. Do nothing.
    ~launch_browser() noexcept override;

protected:

    /**
     * @brief configure the action.
     * @code{.xml}
       <service type="ui::action" impl="sight::module::ui::qt::launch_browser"
     * auto_connect="false">
        <url>http://www.ircad.fr</url>
       </service>
       @endcode
     */
    void configuring() override;

    /// Starts action
    void starting() override;

    /// Launch a browser on the url given in the configuration
    void updating() override;

    /// Stops action
    void stopping() override;

    void info(std::ostream& _sstream) override;

private:

    std::string m_url;
};

} // namespace sight::module::ui::qt
