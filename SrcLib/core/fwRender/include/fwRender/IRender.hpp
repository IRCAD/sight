/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRENDER_IRENDER_HPP_
#define _FWRENDER_IRENDER_HPP_

#include <fwTools/Failed.hpp>
#include <fwServices/IService.hpp>

#include <fwGui/IGuiContainerSrv.hpp>

#include "fwRender/config.hpp"

class wxWindow;

/**
 * @brief   The namespace fwRender contains the base interface for rendering services.
 * @namespace   fwRender
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 */
namespace fwRender
{

/**
 * @brief   Defines the service interface managing the rendering service for object.
 * @class   IRender.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 * Use the base service methods :
 * @li The service is configured with methods setConfiguration(cfg) and configure()
 * @li The method start() initialize the service
 * @li Use update() method to update data
 * @li To react on modifications use update(msg)
 * @li Finally we must call stop() before deleting the service
 *
 */
class FWRENDER_CLASS_API IRender : public ::fwGui::IGuiContainerSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (IRender)(::fwGui::IGuiContainerSrv) ) ;

protected :

    /**
     * @brief   Constructor. Initialize default values.
     */
    FWRENDER_API IRender() throw() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWRENDER_API virtual ~IRender() throw() ;

};

} // namespace fwRenderVTK

#endif /*_FWRENDER_IRENDER_HPP_*/


