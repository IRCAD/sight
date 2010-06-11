/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef GUI_ASPECT_ITOOLBAR_HPP_
#define GUI_ASPECT_ITOOLBAR_HPP_

#include <fwServices/IService.hpp>

#include "gui/export.hpp"

namespace gui
{

namespace aspect
{

/**
 * @brief   Defines the default toolbar for standard application
 * @class   IToolBar.
 * @author  IRCAD (Research and Development Team)
 * @date    2009.
 */
class GUI_CLASS_API IToolBar : public ::fwServices::IService
{

public :

    fwCoreNonInstanciableClassDefinitionsMacro ( (IToolBar)( ::fwServices::IService) ) ;

protected :

    /**
     * @brief Constructor. Do nothing (Just initialize parameters).
     */
    GUI_API IToolBar() throw() ;

    /**
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~IToolBar() throw() ;

};

}
}

#endif /*GUI_ASPECT_ITOOLBAR_HPP_*/
