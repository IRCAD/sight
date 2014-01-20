/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
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
 * @class   IDialogEditor
 */

class GUI_CLASS_API IDialogEditor : public ::fwServices::IService
{

public :

    fwCoreServiceClassDefinitionsMacro ( (IDialogEditor)(::fwServices::IService) ) ;

protected :

    GUI_API IDialogEditor() throw() ;

    GUI_API virtual ~IDialogEditor() throw() ;

    /** @name Service methods ( override from ::fwServices::IService )
     *  @{
     */

    /**
     * @brief This method is used to give information about the service. Do nothing.
     * @todo Must be not implemented and class child declaration should be imposed.
     */
    GUI_API virtual void info(std::ostream &_sstream ) ;

    /// @}
};

}
}

#endif /*_GUI_EDITOR_IDIALOGEDITOR_HPP_*/


