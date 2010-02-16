/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef GUI_DEFAULT_TOOLBAR_HPP_
#define GUI_DEFAULT_TOOLBAR_HPP_

#include <wx/toolbar.h>

#include "gui/aspect/IToolBar.hpp"
#include "gui/export.hpp"

namespace gui
{

namespace aspect
{

/**
 * @brief   Defines the default toolbar for standard application
 * @class   DefaultToolBar.
 * @author  IRCAD (Research and Development Team)

 * @date    2009.
 */
class GUI_CLASS_API DefaultToolBar : public IToolBar
{

public :

    /// Constructor. Do nothing.
    GUI_API DefaultToolBar() throw();

    /// Destructor. Do nothing.
    GUI_API virtual ~DefaultToolBar() throw();

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief This method create a new ToolBar in frame.
     */
    GUI_API virtual void starting() throw( ::fwTools::Failed ) ;

    /**
     * @brief This method remove the tools situated in the ToolBar and delete ToolBar.
     */
    GUI_API virtual void stopping() throw( ::fwTools::Failed );

    /**
     * @brief This method is used to update services on notification. Do nothing.
     */
    GUI_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    /**
     * @brief This method is used to update services. Do nothing.
     */
    GUI_API virtual void updating() throw(::fwTools::Failed);

    /**
     * @brief This method is used to configure the class parameters. Do nothing.
    */
    GUI_API virtual void configuring() throw( ::fwTools::Failed );

    /**
     * @brief This method is used to give information about the service. Do nothing.
     */
    GUI_API virtual void info(std::ostream &_sstream );
    ///@}

private:

    wxToolBar*  m_toolBar;
};

}

}

#endif // GUI_DEFAULT_TOOLBAR_HPP_
