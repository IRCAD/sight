/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_IFRAMESRV_HPP_
#define _FWGUI_IFRAMESRV_HPP_

#include <fwServices/IService.hpp>

#include "fwGui/config.hpp"
#include "fwGui/fwContainer.hpp"

#include "fwGui/registrar/ViewRegistrar.hpp"
#include "fwGui/layoutManager/IFrameLayoutManager.hpp"


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

private:

    void initializeLayoutManager( ::fwRuntime::ConfigurationElement::sptr layoutConfig );

    ::fwGui::layoutManager::IFrameLayoutManager::sptr m_frameLayoutManager;
    ::fwGui::registrar::ViewRegistrar::sptr           m_viewRegistrar;

    ConfigurationType m_registrarConfig;
    ConfigurationType m_frameConfig;
};

} // namespace fwGui

#endif /*_FWGUI_IFRAMESRV_HPP_*/


