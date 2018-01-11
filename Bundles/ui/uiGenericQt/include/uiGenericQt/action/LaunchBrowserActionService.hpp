/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

