/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwGui/config.hpp"
#include "fwGui/IGuiContainerSrv.hpp"

#include <fwServices/IService.hpp>

#include <fwTools/Failed.hpp>

namespace fwGui
{
namespace editor
{

/**
 * @brief Defines the service interface managing the editor service for object.
 */

class FWGUI_CLASS_API IEditor : public ::fwGui::IGuiContainerSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (IEditor)(::fwGui::IGuiContainerSrv) );

protected:

    /**
     * @brief   Constructor. Do nothing (Just initialize parameters).
     */
    IEditor() noexcept
    {
    }

    /// Destructor. Do nothing.
    virtual ~IEditor() noexcept
    {
    }

};

} // namespace editor
} // namespace fwGui
