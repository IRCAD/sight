/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_REGISTRYMANAGER_VIEWREGISTRYMANAGER_HPP_
#define _FWGUI_REGISTRYMANAGER_VIEWREGISTRYMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/container/fwToolBar.hpp"
#include "fwGui/container/fwMenuBar.hpp"
#include "fwGui/container/fwContainer.hpp"
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

    FWGUI_API virtual ::fwGui::container::fwContainer::sptr getParent();

    /**
     * @brief Initialize managers.
     *
     * Example of configuration
     * @verbatim
        <service uid="subView1" type="::gui::view::IView" implementation="::gui::view::DefaultView" autoComChannel="no" >
            <gui>
                <layout type="::fwGui::LineLayoutManager" >
                    <orientation value="horizontal" />
                    <view caption="view3" />
                    <view caption="view4" />
                    <view caption="view5" />
                </layout>
                <toolBar />
            </gui>
            <registry>
                <parent wid="myView" />
                <toolBar sid="toolbar1" start="yes" />
                <view sid="subView3" start="yes" />
                <view wid="subView4" />
            </registry>
        </service>
      @endverbatim
     * - <parent wid="myView" /> : This section isn't mandatory. When the parent section is present, the wid attribute is used to find its parent container.
     * Otherwise, the service uid is used to find its parent container.
     * - <toolBar sid="toolbar1" start="yes" /> : This section isn't mandatory.
     *   - \b sid  (mandatory): the tool bar identifier.
     *   - \b start = {yes| no} (default value no): indicate if the tool bar service must be started by the service.
     * - <menuBar sid="menubar1" start="yes" /> : This section isn't mandatory.
     *   - \b sid  (mandatory): the menu bar identifier.
     *   - \b start = {yes| no} (default value no): indicate if the menu bar service must be started by the service.
     * - <view sid="subView3" start="yes" /> : define the service of the view to add in the container.
     *   - \b sid  (mandatory): the service identifier.
     *   - \b start = {yes| no} (default value no): indicate if the view service must be started by the service.
     * - <view wid="subView4" /> : reserve a sub container for the view in the parent container with the name "subView4". The service which want to use this sub container
     *   will have define a parent with <parent wid="subView4" />.
     *   - \b wid  (mandatory): the window identifier.
     *   @warning
     *   - The number of view item in the gui section must be equal or greater than in the registry section.
     *   - The order of the menu in each section (gui and registry) must be the same.\n
     *  For example: the view caption "view3" will be connected with the service which have the sid = "subView3" (It could also be a wid).
     *
     */
    FWGUI_API virtual void initialize( ::fwRuntime::ConfigurationElement::sptr configuration);

    /**
     * @brief Starting view manager.
     * All services managed in local subViews
     * and with start="yes" in configuration will be started.
     * @pre ViewRegistrar must be initialized before.
     * @pre sub containers must be instanced before.
     */
    FWGUI_API virtual void manage(std::vector< ::fwGui::container::fwContainer::sptr > subViews );

    /**
     * @brief Register menu bar.
     * If start="yes" in configuration the menu bar services will be started.
     */
    FWGUI_API virtual void manageMenuBar(::fwGui::container::fwMenuBar::sptr menuBar );

    /**
     * @brief Register tool bar.
     * If start="yes" in configuration the tool bar services will be started.
     */
    FWGUI_API virtual void manageToolBar(::fwGui::container::fwToolBar::sptr toolBar );

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
    ::fwGui::container::fwContainer::sptr m_parentContainer;

    /// Main service ID associate with this ViewRegistrar
    std::string m_sid;
};

} // namespace registrar
} // namespace fwGui

#endif /*_FWGUI_REGISTRYMANAGER_VIEWREGISTRYMANAGER_HPP_*/


