/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "gui/config.hpp"

#include <fwGui/view/IView.hpp>

#include <fwTools/Failed.hpp>

namespace gui
{
namespace view
{

/**
 * @brief Defines the generic layout for application.
 * @see ::fwGui::IGuiContainer for the service configuration
 */
class GUI_CLASS_API SDefaultView : public ::fwGui::view::IView
{
public:

    fwCoreServiceClassDefinitionsMacro( (SDefaultView)(::fwGui::view::IView) );

    /// Constructor. Do nothing.
    GUI_API SDefaultView() noexcept;

    /// Destructor. Do nothing.
    GUI_API virtual ~SDefaultView() noexcept;

protected:

    /**
     * @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief This method is used to configure the class parameters.
     * @see ::fwGui::IGuiContainerSrv::initialize()
     */
    GUI_API virtual void configuring() override;

    /**
     * @brief Register a view with defined id.
     * @see ::fwGui::IGuiContainerSrv::create()
     */
    GUI_API virtual void starting() override;

    /**
     * @brief This method remove the view in the frame.
     * @see ::fwGui::IGuiContainerSrv::destroy()
     */
    GUI_API virtual void stopping() override;

    /**
     * @brief This method is used to update services. Do nothing.
     */
    GUI_API virtual void updating() override;

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    GUI_API virtual void info(std::ostream& _sstream ) override;
    ///@}

};

} // namespace view
} // namespace gui
