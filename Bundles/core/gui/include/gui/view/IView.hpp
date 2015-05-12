/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_VIEW_IVIEW_HPP__
#define __GUI_VIEW_IVIEW_HPP__

#include <fwServices/IService.hpp>

#include <fwGui/IGuiContainerSrv.hpp>

#include "gui/config.hpp"

namespace gui
{
namespace view
{

/**
 * @brief   Defines the service interface managing a view placed in main frame.
 * @class   IView
 *
 */
class GUI_CLASS_API IView : public ::fwGui::IGuiContainerSrv
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro ( (IView)(::fwGui::IGuiContainerSrv) );

protected:

    /**
     * @brief Constructor. Do nothing (Just initialize parameters).
     */
    GUI_API IView() throw();

    /**
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~IView() throw();

};



} // namespace view
} // namespace gui


#endif /*__GUI_VIEW_IVIEW_HPP__*/
