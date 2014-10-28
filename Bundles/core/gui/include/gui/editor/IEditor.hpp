/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_EDITOR_IEDITOR_HPP_
#define _GUI_EDITOR_IEDITOR_HPP_

#include <fwTools/Failed.hpp>
#include <fwServices/IService.hpp>

#include <fwGui/IGuiContainerSrv.hpp>

#include "gui/export.hpp"

namespace gui
{
namespace editor
{

/**
 * @brief   Defines the service interface managing the editor service for object.
 * @class   IEditor
 */

class GUI_CLASS_API IEditor : public ::fwGui::IGuiContainerSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (IEditor)(::fwGui::IGuiContainerSrv) ) ;

protected :

    /**
     * @brief   Constructor. Do nothing (Just initialize parameters).
     */
    GUI_API IEditor() throw() ;

    /// Destructor. Do nothing.
    GUI_API virtual ~IEditor() throw() ;

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief This method is used to give information about the service. Do nothing.
     * @todo Must be not implemented and class child declaration should be imposed.
     */
    GUI_API virtual void info(std::ostream &_sstream ) ;

    ///@}

};

}
}

#endif /*_GUI_EDITOR_IEDITOR_HPP_*/


