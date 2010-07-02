/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_VIEWMANAGER_HPP_
#define _FWGUI_VIEWMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/fwContainer.hpp"
#include "fwGui/config.hpp"


namespace fwGui
{

/**
 * @brief   Defines the generic view manager for IHM.
 * @class   ViewManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API ViewManager : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ViewManager)(::fwCore::BaseObject), (( (const std::string) )), new ViewManager );

    /// Constructor.
    FWGUI_API ViewManager( const std::string sid);

    /// Destructor. Do nothing
    FWGUI_API virtual ~ViewManager();

    FWGUI_API virtual ::fwGui::fwContainer::sptr getParent();

    /**
     * @brief Configure views managed.
     */
    FWGUI_API virtual void initialize( ::fwRuntime::ConfigurationElement::sptr configuration);

    /**
     * @brief Starting view manager.
     * All services managed in local subViews
     * and with start="yes" in configuration will be started.
     * @pre ViewManager must be initialized before.
     * @pre sub containers must be instanced before.
     */
    FWGUI_API virtual void manage(std::vector< ::fwGui::fwContainer::sptr > subViews );

    /**
     * @brief Stopping view manager.
     * All services managed in local subViews will be stopped.
     */
    FWGUI_API virtual void unmanage();

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief All services ID managed and associated with pair containing:
     * subViews index vector and boolean describing if is started by the manager.
     */
    std::map< std::string, std::pair<int, bool> > m_sids;

    /// All windows containers ID managed (and associated with subViews index vector).
    std::map< std::string, int > m_wids;

    ::fwGui::fwContainer::sptr m_parentContainer;

    /// Main service ID associate with this ViewManager
    std::string m_sid;
};

} // namespace fwGui

#endif /*_FWGUI_VIEWMANAGER_HPP_*/


