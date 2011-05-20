/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIGENERICQT_ACTION_OPENURLACTIONSERVICE_HPP_
#define _UIGENERICQT_ACTION_OPENURLACTIONSERVICE_HPP_

#include <fwTools/Failed.hpp>

#include <fwGui/IActionSrv.hpp>

#include "uiGenericQt/config.hpp"

namespace uiGeneric
{

namespace action
{

/**
 * @brief   This action launch a browser on the url given in configuration.
 * @class   LaunchBrowserActionService.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIGENERICQT_CLASS_API LaunchBrowserActionService : public ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (LaunchBrowserActionService)(::fwGui::IActionSrv) ) ;

    /// Constructor. Do nothing.
    UIGENERICQT_API LaunchBrowserActionService() throw() ;

    /// Destructor. Do nothing.
    UIGENERICQT_API virtual ~LaunchBrowserActionService() throw() ;

protected:


    /**
     * @brief configure the action.
     * @verbatim
    <service type="::fwGui::IActionSrv" implementation="::uiGeneric::action::LaunchBrowserActionService" autoComChannel="no">
        <url>http://www.ircad.fr</url>
    </service>
       @endverbatim
     */
    void configuring() throw( ::fwTools::Failed );

    /// Starts action
    void starting() throw (::fwTools::Failed);

    /// Does nothing
    void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /// Launch a brower on the url given in the configuration
    void updating() throw(::fwTools::Failed);

    /// Stops action
    void stopping() throw (::fwTools::Failed);

    void info( std::ostream &_sstream ) ;

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

private:

    std::string m_url;

};

} // action

} // uiGeneric

#endif /*_UIGENERICQT_ACTION_OPENURLACTIONSERVICE_HPP_*/


