/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_VIEW_SDEFAULTVIEW_HPP__
#define __GUI_VIEW_SDEFAULTVIEW_HPP__

#include "gui/view/IView.hpp"
#include "gui/config.hpp"

#include <fwTools/Failed.hpp>

namespace gui
{
namespace view
{

/**
 * @brief Defines the generic layout for application.
 * @see ::fwGui::IGuiContainer for the service configuration
 */
class GUI_CLASS_API SDefaultView : public ::gui::view::IView
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SDefaultView)(::gui::view::IView) );

    /// Constructor. Do nothing.
    GUI_API SDefaultView() throw();

    /// Destructor. Do nothing.
    GUI_API virtual ~SDefaultView() throw();

protected:

    /**
     * @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief This method is used to configure the class parameters.
     * @see ::fwGui::IGuiContainerSrv::initialize()
     */
    GUI_API virtual void configuring() throw( ::fwTools::Failed );

    /**
     * @brief Register a view with defined id.
     * @see ::fwGui::IGuiContainerSrv::create()
     */
    GUI_API virtual void starting() throw(::fwTools::Failed);

    /**
     * @brief This method remove the view in the frame.
     * @see ::fwGui::IGuiContainerSrv::destroy()
     */
    GUI_API virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief This method is used to update services. Do nothing.
     */
    GUI_API virtual void updating() throw(::fwTools::Failed);

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    GUI_API virtual void info(std::ostream& _sstream );
    ///@}

};


} // namespace view
} // namespace gui


#endif /*__GUI_VIEW_SDEFAULTVIEW_HPP__*/
