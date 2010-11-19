/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_EDITOR_IDIALOGEDITOR_HPP_
#define _GUI_EDITOR_IDIALOGEDITOR_HPP_

#include <fwTools/Failed.hpp>
#include <fwServices/IService.hpp>

#include <fwGui/IGuiContainerSrv.hpp>

#include "gui/export.hpp"

namespace gui
{
namespace editor
{

/**
 * @brief   Defines the service interface managing the editor service which create their own container.
 * @class   IDialogEditor.
 * @author  IRCAD (Research and Development Team).

 * @date    2010.
 */

class GUI_CLASS_API IDialogEditor : public ::fwServices::IService
{

public :

    fwCoreServiceClassDefinitionsMacro ( (IDialogEditor)(::fwServices::IService) ) ;

protected :

    /**
     * @brief   Constructor. Do nothing.
     */
    GUI_API IDialogEditor() throw() ;

    /// Destructor. Do nothing.
    GUI_API virtual ~IDialogEditor() throw() ;

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /// This method is used to configure the service.
    GUI_API virtual void configuring() throw( ::fwTools::Failed )  = 0 ;

    /**
     * Starts the service.
     */
    GUI_API virtual void starting() throw(::fwTools::Failed) = 0 ;

    /// Stops the service.
    GUI_API virtual void stopping() throw(::fwTools::Failed) = 0 ;

    /**
     * @brief This method is used to give information about the service. Do nothing.
     * @todo Must be not implemented and class child declaration should be imposed.
     */
    GUI_API virtual void info(std::ostream &_sstream ) ;

    /**
     * @brief Update/refresh/execute the service on an observation/notification. Do nothing.
     */
    GUI_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)  = 0 ;

    /**
     * @brief Update/refresh/execute the service. Do nothing.
     */
    GUI_API virtual void updating() throw(::fwTools::Failed)  = 0 ;

    ///@}

};

}
}

#endif /*_GUI_EDITOR_IDIALOGEDITOR_HPP_*/


