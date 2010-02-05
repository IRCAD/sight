/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_VIEW_MULTI_VIEW_HPP_
#define _GUI_VIEW_MULTI_VIEW_HPP_

#include <wx/wx.h>
#include <wx/aui/aui.h>

#include <fwTools/Failed.hpp>

#include "gui/export.hpp"
#include "gui/view/IView.hpp"


namespace gui
{
namespace view
{

/**
 * @brief   Defines the default panel for standard application.
 * @class   MultiView.
 * @author  IRCAD (Research and Development Team)

 * @date    2009.
 */
class GUI_CLASS_API MultiView : public ::gui::view::IView
{
    class ViewInfo
    {
    public :

        ViewInfo() :
            m_align ("center"),
            m_minSize (std::make_pair(-1,-1)),
            m_isResizable(true),
            m_visible (false),
            m_panel (0)
        {}

        std::string             m_align;
        std::pair< int, int >   m_minSize;
        bool                    m_visible;
        wxPanel *               m_panel;
        bool                    m_isResizable;
    };

public :

    /// Constructor, does nothing.
    GUI_API MultiView() throw() ;
    
    /// Destructor, does nothing.
    GUI_API virtual ~MultiView() throw() ;

    /// Analyses xml configuration to configure this service, espacially umber of panel and the layout of this view.
    GUI_API void configuring() throw( ::fwTools::Failed );

    /// Reconfigures service, does nothing.
    GUI_API void reconfiguring() throw( ::fwTools::Failed );

    /// Builds defined layout and creates wxPanel.
    GUI_API void starting() throw( ::fwTools::Failed );

    /// Uninit layout and destroy panel.
    GUI_API void stopping() throw( ::fwTools::Failed );

    /// Swap associated object, does nothing.
    GUI_API void swappping() throw( ::fwTools::Failed );

    /// Updates service, does nothing.
    GUI_API void updating() throw( ::fwTools::Failed );

    /// Updates service on notification, does nothing.
    GUI_API void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw( ::fwTools::Failed );

    /// Prints service name
    GUI_API void info(std::ostream &_sstream );

protected :

    /// Layout manager of this view
    wxAuiManager * m_manager;

    /// New Container definition
    typedef std::map< int , ViewInfo > PanelContainer;

    /// Save layout configuration definition and created container
    PanelContainer m_panels;

};


} // namespace view
} // namespace gui


#endif /*_GUI_VIEW_MULTI_VIEW_HPP_*/
