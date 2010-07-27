/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_REGISTRYMANAGER_VIEWREGISTRYMANAGER_HPP_
#define _FWGUI_REGISTRYMANAGER_VIEWREGISTRYMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/fwToolBar.hpp"
#include "fwGui/fwMenuBar.hpp"
#include "fwGui/fwContainer.hpp"
#include "fwGui/config.hpp"


namespace fwGui
{

namespace registrar
{

/**
 * @brief   Defines the generic view manager for IHM.
 * @class   ViewRegistrar.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API ViewRegistrar : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ViewRegistrar)(::fwCore::BaseObject), (( (const std::string) )), new ViewRegistrar );

    /// Constructor.
    FWGUI_API ViewRegistrar( const std::string sid);

    /// Destructor. Do nothing
    FWGUI_API virtual ~ViewRegistrar();

    FWGUI_API virtual ::fwGui::fwContainer::sptr getParent();

    /**
     * @brief Configure views managed.
     */
    FWGUI_API virtual void initialize( ::fwRuntime::ConfigurationElement::sptr configuration);

    /**
     * @brief Starting view manager.
     * All services managed in local subViews
     * and with start="yes" in configuration will be started.
     * @pre ViewRegistrar must be initialized before.
     * @pre sub containers must be instanced before.
     */
    FWGUI_API virtual void manage(std::vector< ::fwGui::fwContainer::sptr > subViews );

    /**
     * @brief Register menu bar.
     * If start="yes" in configuration the menu bar services will be started.
     */
    FWGUI_API virtual void manageMenuBar(::fwGui::fwMenuBar::sptr menuBar );

    /**
     * @brief Register tool bar.
     * If start="yes" in configuration the tool bar services will be started.
     */
    FWGUI_API virtual void manageToolBar(::fwGui::fwToolBar::sptr toolBar );

    /**
     * @brief Stopping view manager.
     * All services managed in local subViews will be stopped.
     */
    FWGUI_API virtual void unmanage();

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::map< std::string, std::pair<int, bool> > SIDContainerMapType;
    typedef std::map< std::string, int > WIDContainerMapType;
    typedef std::pair< std::string, bool > SIDMenuBarPairType;
    typedef std::pair< std::string, bool > SIDToolBarPairType;

    /**
     * @brief All services ID managed and associated with pair containing:
     * subViews index vector and boolean describing if is started by the manager.
     */
    SIDContainerMapType m_sids;

    /// All windows containers ID managed (and associated with subViews index vector).
    WIDContainerMapType m_wids;

    /// Menu bar service ID associate with this ViewRegistrar
    SIDMenuBarPairType m_menuBarSid;

    /// Tool bar service ID associate with this ViewRegistrar
    SIDToolBarPairType m_toolBarSid;

    std::string m_parentWid;
    ::fwGui::fwContainer::sptr m_parentContainer;

    /// Main service ID associate with this ViewRegistrar
    std::string m_sid;
};

} // namespace registrar
} // namespace fwGui

#endif /*_FWGUI_REGISTRYMANAGER_VIEWREGISTRYMANAGER_HPP_*/


