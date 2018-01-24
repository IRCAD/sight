/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwGui/config.hpp"
#include "fwGui/IGuiContainerSrv.hpp"

#include <fwServices/IService.hpp>

namespace fwGui
{
namespace view
{

/**
 * @brief   Defines the service interface managing a view placed in main frame.
 *
 */
class FWGUI_CLASS_API IView : public ::fwGui::IGuiContainerSrv
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (IView)(::fwGui::IGuiContainerSrv) );

protected:

    /**
     * @brief Constructor. Do nothing (Just initialize parameters).
     */
    IView() noexcept
    {
    }

    /**
     * @brief Destructor. Do nothing.
     */
    virtual ~IView() noexcept
    {
    }

};

} // namespace view
} // namespace fwGui
