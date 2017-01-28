/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_REGISTRAR_VIEWREGISTRAR_HPP__
#define __FWGUI_REGISTRAR_VIEWREGISTRAR_HPP__

#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/GuiBaseObject.hpp"
#include "fwGui/container/fwToolBar.hpp"
#include "fwGui/container/fwMenuBar.hpp"
#include "fwGui/container/fwContainer.hpp"
#include "fwGui/config.hpp"


namespace fwGui
{
namespace container
{
class fwContainer;
class fwMenuBar;
class fwToolBar;
}

namespace registrar
{

/**
 * @brief   Defines the generic view manager for IHM.
 * @class   ViewRegistrar
 *
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API ViewRegistrar : public ::fwGui::GuiBaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ViewRegistrar)(::fwGui::GuiBaseObject), (( (const std::string&) )),
                                            new ViewRegistrar );

    /// Constructor.
    FWGUI_API ViewRegistrar( const std::string &sid);

    /// Destructor. Do nothing
    FWGUI_API virtual ~ViewRegistrar();

    FWGUI_API virtual SPTR(::fwGui::container::fwContainer) getParent();

    FWGUI_API virtual void setParent(std::string wid);

    /**
     * @brief Initialize managers.
     *
     * Example of configuration
     * @code{.xml}
        <service uid="subView1" type="::gui::view::IView" impl="::gui::view::SDefaultView" autoConnect="no" >
            <gui>
                <layout type="::fwGui::LineLayoutManager" >
                    <orientation value="horizontal" />
                    <view caption="view3" />
                    <view caption="view4" />
                    <view caption="view5" />
                </layout>
                <toolBar />
                <menuBar />
            </gui>
            <registry>
                <parent wid="myView" />
                <toolBar sid="toolbar" start="yes" />
                <menuBar sid="menubar" start="yes" />
                <view sid="subView3" start="yes" />
                <view wid="subView4" />
                <view wid="subView5" />
            </registry>
        </service>
       @endcode
     * - \<parent wid="myView" /\> : This section isn't mandatory. When the parent section is present, the wid attribute is used to find its parent container.
     * Otherwise, the service uid is used to find its parent container.
     * - \<toolBar sid="toolbar1" start="yes" /\> : This section isn't mandatory.
     *   - \b sid  (mandatory): the tool bar identifier.
     *   - \b start = {yes| no} (default value no): indicate if the tool bar service must be started by the service.
     * - \<menuBar sid="menubar1" start="yes" /\> : This section isn't mandatory.
     *   - \b sid  (mandatory): the menu bar identifier.
     *   - \b start = {yes| no} (default value no): indicate if the menu bar service must be started by the service.
     * - \<view sid="subView3" start="yes" /\> : define the service of the view to add in the container.
     *   - \b sid  (mandatory): the service identifier.
     *   - \b start = {yes| no} (default value no): indicate if the view service must be started by the service.
     * - \<view wid="subView4" /\> : reserve a sub container for the view in the parent container with the name "subView4". The service which want to use this sub container
     *   will have define a parent with \<parent wid="subView4" /\>.
     *   - \b wid  (mandatory): the window identifier.
     *
     * @warning
     * - The number of view item in the gui section must be equal or greater than in the registry section.
     * - The order of the view in each section (gui and registry) must be the same.\n
     *   For example: the view caption "view3" will be connected with the service which have the sid = "subView3" (It could also be a wid).
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
    FWGUI_API virtual void manage(std::vector< SPTR(::fwGui::container::fwContainer) > subViews );

    /**
     * @brief Register menu bar.
     * If start="yes" in configuration the menu bar services will be started.
     */
    FWGUI_API virtual void manageMenuBar(SPTR(::fwGui::container::fwMenuBar) menuBar );

    /**
     * @brief Register tool bar.
     * If start="yes" in configuration the tool bar services will be started.
     */
    FWGUI_API virtual void manageToolBar(SPTR(::fwGui::container::fwToolBar) toolBar );

    /**
     * @brief Stopping view manager.
     * All services managed in local subViews will be stopped.
     */
    FWGUI_API virtual void unmanage();

    /**
     * @brief Stopping view manager.
     * ToolBar service will be stopped.
     */
    FWGUI_API virtual void unmanageToolBar();

    /**
     * @brief Stopping view manager.
     * MenuBar service will be stopped.
     */
    FWGUI_API virtual void unmanageMenuBar();



protected:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    typedef std::map< std::string, std::pair<unsigned int, bool> > SIDContainerMapType;
    typedef std::map< std::string, unsigned int > WIDContainerMapType;
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
    SPTR(::fwGui::container::fwContainer) m_parentContainer;

    /// Main service ID associate with this ViewRegistrar
    std::string m_sid;
};

} // namespace registrar
} // namespace fwGui

#endif /*__FWGUI_REGISTRAR_VIEWREGISTRAR_HPP__*/


