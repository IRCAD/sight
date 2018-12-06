/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __UIGENERICQT_ACTION_LAUNCHBROWSERACTIONSERVICE_HPP__
#define __UIGENERICQT_ACTION_LAUNCHBROWSERACTIONSERVICE_HPP__

#include "uiGenericQt/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <fwTools/Failed.hpp>

namespace uiGenericQt
{

namespace action
{

/**
 * @brief   This action launch a browser on the url given in configuration.
 */
class UIGENERICQT_CLASS_API LaunchBrowserActionService : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (LaunchBrowserActionService)(::fwGui::IActionSrv) );

    /// Constructor. Do nothing.
    UIGENERICQT_API LaunchBrowserActionService() noexcept;

    /// Destructor. Do nothing.
    UIGENERICQT_API virtual ~LaunchBrowserActionService() noexcept;

protected:

    /**
     * @brief configure the action.
     * @code{.xml}
       <service type="::fwGui::IActionSrv" impl="::uiGenericQt::action::LaunchBrowserActionService" autoConnect="no">
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

    void info( std::ostream& _sstream ) override;

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

private:

    std::string m_url;

};

} // action

} // uiGenericQt

#endif /*__UIGENERICQT_ACTION_LAUNCHBROWSERACTIONSERVICE_HPP__*/

