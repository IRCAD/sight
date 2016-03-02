/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIGENERICQT_ACTION_LAUNCHBROWSERACTIONSERVICE_HPP__
#define __UIGENERICQT_ACTION_LAUNCHBROWSERACTIONSERVICE_HPP__

#include <fwTools/Failed.hpp>

#include <fwGui/IActionSrv.hpp>

#include "uiGenericQt/config.hpp"

namespace uiGeneric
{

namespace action
{

/**
 * @brief   This action launch a browser on the url given in configuration.
 * @class   LaunchBrowserActionService
 */
class UIGENERICQT_CLASS_API LaunchBrowserActionService : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (LaunchBrowserActionService)(::fwGui::IActionSrv) );

    /// Constructor. Do nothing.
    UIGENERICQT_API LaunchBrowserActionService() throw();

    /// Destructor. Do nothing.
    UIGENERICQT_API virtual ~LaunchBrowserActionService() throw();

protected:


    /**
     * @brief configure the action.
     * @code{.xml}
       <service type="::fwGui::IActionSrv" impl="::uiGeneric::action::LaunchBrowserActionService" autoConnect="no">
        <url>http://www.ircad.fr</url>
       </service>
       @endcode
     */
    void configuring() throw( ::fwTools::Failed );

    /// Starts action
    void starting() throw (::fwTools::Failed);

    /// Launch a browser on the url given in the configuration
    void updating() throw(::fwTools::Failed);

    /// Stops action
    void stopping() throw (::fwTools::Failed);

    void info( std::ostream &_sstream );

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

private:

    std::string m_url;

};

} // action

} // uiGeneric

#endif /*__UIGENERICQT_ACTION_LAUNCHBROWSERACTIONSERVICE_HPP__*/


