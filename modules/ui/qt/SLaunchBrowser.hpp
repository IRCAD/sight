/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/ui/qt/config.hpp"

#include <core/tools/Failed.hpp>

#include <ui/base/IAction.hpp>

namespace sight::module::ui::qt
{

/**
 * @brief   This action launch a browser on the url given in configuration.
 */
class MODULE_UI_QT_CLASS_API SLaunchBrowser : public ::sight::ui::base::IAction
{
public:

    SIGHT_DECLARE_SERVICE(SLaunchBrowser, ::sight::ui::base::IAction);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API SLaunchBrowser() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API virtual ~SLaunchBrowser() noexcept;

protected:

    /**
     * @brief configure the action.
     * @code{.xml}
       <service type="ui::base::IAction" impl="::sight::module::ui::qt::SLaunchBrowser"
     * autoConnect="false">
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

    typedef core::runtime::ConfigurationElement::sptr Configuration;

private:

    std::string m_url;
};

} // sight::module::ui::qt
