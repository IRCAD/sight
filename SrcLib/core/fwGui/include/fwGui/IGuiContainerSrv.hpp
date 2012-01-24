/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_IGUICONTAINERSRV_HPP_
#define _FWGUI_IGUICONTAINERSRV_HPP_

#include <fwServices/IService.hpp>

#include "fwGui/config.hpp"
#include "fwGui/container/fwContainer.hpp"

#include "fwGui/registrar/ViewRegistrar.hpp"
#include "fwGui/layoutManager/IViewLayoutManager.hpp"
#include "fwGui/builder/IToolBarBuilder.hpp"
#include "fwGui/builder/IContainerBuilder.hpp"

namespace fwGui
{

/**
 * @brief   Defines the service interface managing the layout.
 * @class   IGuiContainerSrv.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IGuiContainerSrv : public ::fwServices::IService
{

public :

    fwCoreServiceClassDefinitionsMacro ( (IGuiContainerSrv)(::fwServices::IService) ) ;

    FWGUI_API ::fwGui::container::fwContainer::sptr getContainer();

    FWGUI_API void setParent(std::string wid);

protected :
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief   Constructor. Initialize default values.
     */
    FWGUI_API IGuiContainerSrv() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUI_API virtual ~IGuiContainerSrv() ;

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
                <menuBar />
            </gui>
            <registry>
                <parent wid="myView" />
                <toolBar sid="toolbar1" start="yes" />
                <menuBar sid="menubar1" start="yes" />
                <view sid="subView3" start="yes" />
                <view wid="subView4" />
                <view sid="subView5" />
            </registry>
        </service>
      @endverbatim
     *  - <layout type="::fwGui::LineLayoutManager" > : give the type of layout.
     *    - \b type {::fwGui::LineLayoutManager |::fwGui::CardinalLayoutManager |::fwGui::TabLayoutManager} :
     *     - \b ::fwGui::LineLayoutManager : all views will be on the same line or column (it depends of the orientation value attribute)
     *           @see ::fwGui::layoutManager::LineLayoutManagerBase
     *     - \b ::fwGui::CardinalLayoutManager : all views will be added around a central view define by the align attribute.
     *           @see ::fwGui::layoutManager::CardinalLayoutManagerBase
     *     - \b ::fwGui::TabLayoutManager : all views will be draw as tab.
     *           @see ::fwGui::layoutManager::TabLayoutManagerBase
     *  - The toolBar section isn't mandatory.
     *  - The menuBar section isn't mandatory.
     *
     * @note The layout and registry sections can be empty. In this case no subview will be created.
     *
     * @warning
     * - The order of the view in each section (gui and registry) must be the same.\n
     *   For example: the view caption "view3" will be connected with the service which have the sid = "subView3" and so one (it also could be a wid).
     *
     *  @see ::fwGui::registrar::ViewRegistrar::initialize(), ::fwGui::layoutManager::IViewLayoutManager::initialize(), ::fwGui::builder::IToolBarBuilder::initialize()
     */

    FWGUI_API void initialize();

    FWGUI_API void create();

    FWGUI_API void destroy();

private:

    void initializeLayoutManager( ::fwRuntime::ConfigurationElement::sptr layoutConfig );
    void initializeToolBarBuilder( ::fwRuntime::ConfigurationElement::sptr toolBarConfig );

    bool m_viewLayoutManagerIsCreated;
    ::fwGui::layoutManager::IViewLayoutManager::sptr m_viewLayoutManager;

    ::fwGui::registrar::ViewRegistrar::sptr    m_viewRegistrar;
    ::fwGui::builder::IToolBarBuilder::sptr    m_toolBarBuilder;
    ::fwGui::builder::IContainerBuilder::sptr  m_containerBuilder;

    ConfigurationType m_viewRegistrarConfig;
    ConfigurationType m_viewLayoutConfig;
    ConfigurationType m_toolBarConfig;

    bool m_hasToolBar;
};

} // namespace fwGui

#endif /*_FWGUI_IGUICONTAINERSRV_HPP_*/


