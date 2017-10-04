/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_EDITOR_IDIALOGEDITOR_HPP__
#define __GUI_EDITOR_IDIALOGEDITOR_HPP__

#include "gui/config.hpp"

#include <fwGui/IGuiContainerSrv.hpp>

#include <fwServices/IService.hpp>

#include <fwTools/Failed.hpp>

namespace gui
{
namespace editor
{

/**
 * @brief Defines the service interface managing the editor service which create their own container.
 */

class GUI_CLASS_API IDialogEditor : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro( (IDialogEditor)(::fwServices::IService) );

protected:

    GUI_API IDialogEditor() noexcept;

    GUI_API virtual ~IDialogEditor() noexcept;

    /** @name Service methods ( override from ::fwServices::IService )
     *  @{
     */

    /**
     * @brief This method is used to give information about the service. Do nothing.
     * @todo Must be not implemented and class child declaration should be imposed.
     */
    GUI_API virtual void info(std::ostream& _sstream ) override;

    /// @}
};

}
}

#endif /*__GUI_EDITOR_IDIALOGEDITOR_HPP__*/

