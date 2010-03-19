/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_VIEW_DEFAULT_VIEW_HPP_
#define _GUI_VIEW_DEFAULT_VIEW_HPP_

#include <fwTools/Failed.hpp>

#include <wx/aui/framemanager.h>

#include "gui/export.hpp"
#include "gui/view/IView.hpp"

namespace gui
{
namespace view
{

/**
 * @brief   Defines the default panel for standard application.
 * @class   DefaultView.
 * @author  IRCAD (Research and Development Team)

 * @date    2009.
 */
class GUI_CLASS_API DefaultView : public ::gui::view::IView
{
    class ViewInfo
    {
    public :

        ViewInfo() :
            m_minSize (std::make_pair(-1,-1)),
            m_panel (0),
            m_autostart(false)
            {}
        std::pair< int, int >   m_minSize;
        wxPanel *               m_panel;
        bool                    m_autostart;
    };

public :

    fwCoreServiceClassDefinitionsMacro ( (DefaultView)(::gui::view::IView::Baseclass) ) ;

    /// Constructor. Do nothing.
    GUI_API DefaultView() throw() ;

    /// Destructor. Do nothing.
    GUI_API virtual ~DefaultView() throw() ;

protected :

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief This method is used to configure the class parameters.
     */
    GUI_API virtual void configuring() throw( ::fwTools::Failed );

    /**
     * @brief Register a view with defined id.
     */
    GUI_API virtual void starting() throw(::fwTools::Failed);

    /**
     * @brief This method remove the view in the frame.
     */
    GUI_API virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief This method is used to update services on notification. Do nothing.
     */
    GUI_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed) ;

    /**
     * @brief This method is used to update services. Do nothing.
     */
    GUI_API virtual void updating() throw(::fwTools::Failed) ;

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    GUI_API virtual void info(std::ostream &_sstream ) ;
    ///@}

private:

    /// Layout manager of this view
    wxAuiManager * m_manager;

    /// New UID-wxContainer definition
    typedef std::map< std::string , ViewInfo > PanelContainer;

    /// Save layout configuration definition and created container
    PanelContainer m_panels;

};


} // namespace view
} // namespace gui


#endif /*_GUI_VIEW_DEFAULT_VIEW_HPP_*/
