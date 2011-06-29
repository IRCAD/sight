/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_IFRAMESRV_HPP_
#define _FWGUI_IFRAMESRV_HPP_

#include <fwServices/IService.hpp>

#include "fwGui/config.hpp"
#include "fwGui/container/fwContainer.hpp"

#include "fwGui/registrar/ViewRegistrar.hpp"
#include "fwGui/layoutManager/IFrameLayoutManager.hpp"
#include "fwGui/builder/IMenuBarBuilder.hpp"
#include "fwGui/builder/IToolBarBuilder.hpp"

namespace fwGui
{

/**
 * @brief   Defines the service interface managing the menu bar.
 * @class   IFrameSrv.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IFrameSrv : public ::fwServices::IService
{

public :

    fwCoreServiceClassDefinitionsMacro ( (IFrameSrv)(::fwServices::IService) ) ;

    /// Get widget defined for porgress bar
    FWGUI_API static ::fwGui::container::fwContainer::sptr getProgressWidget();

protected :

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief   Constructor. Initialize default values.
     */
    FWGUI_API IFrameSrv() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUI_API virtual ~IFrameSrv() ;

    /// Initialize the frame and registry manager.
    FWGUI_API void initialize();

    FWGUI_API void create();

    FWGUI_API void destroy();

    FWGUI_API static const std::string CLOSE_POLICY_EXIT;
    FWGUI_API static const std::string CLOSE_POLICY_NOTIFY;

    /// Static reference on a widget defined for progress bar installation
    FWGUI_API static ::fwGui::container::fwContainer::wptr m_progressWidget;

private:

    void onCloseExit();
    void onCloseNotify();
    void initializeLayoutManager( ::fwRuntime::ConfigurationElement::sptr layoutConfig );
    void initializeMenuBarBuilder( ::fwRuntime::ConfigurationElement::sptr menuBarConfig );
    void initializeToolBarBuilder( ::fwRuntime::ConfigurationElement::sptr toolBarConfig );

    ::fwGui::layoutManager::IFrameLayoutManager::sptr m_frameLayoutManager;
    ::fwGui::registrar::ViewRegistrar::sptr           m_viewRegistrar;
    ::fwGui::builder::IMenuBarBuilder::sptr           m_menuBarBuilder;
    ::fwGui::builder::IToolBarBuilder::sptr           m_toolBarBuilder;

    ConfigurationType m_registrarConfig;
    ConfigurationType m_frameConfig;
    ConfigurationType m_menuBarConfig;
    ConfigurationType m_toolBarConfig;

    bool m_hasMenuBar;
    bool m_hasToolBar;

    std::string m_closePolicy;
};

} // namespace fwGui

#endif /*_FWGUI_IFRAMESRV_HPP_*/


