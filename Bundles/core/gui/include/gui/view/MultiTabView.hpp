/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_VIEW_MULTI_TAB_VIEW_HPP_
#define _GUI_VIEW_MULTI_TAB_VIEW_HPP_

#include <wx/wx.h>
#include <wx/notebook.h>

#include <fwTools/Failed.hpp>

#include "gui/export.hpp"
#include "gui/view/IView.hpp"


namespace gui
{
namespace view
{

/**
 * @brief   Defines the default tab panel for standard application.
 * @class   MultiTabView.
 * @author  IRCAD (Research and Development Team)

 * @date    2009-2010.
 */
class GUI_CLASS_API MultiTabView : public ::gui::view::IView
{
    class ViewInfo
    {
    public :

        ViewInfo() :
            m_panel (0),
            m_uid(""),
            m_name(""),
            m_autostart(false)
        {}

        std::string  m_uid;
        std::string  m_name;
        wxPanel *    m_panel;
        bool         m_autostart;
    };

public :

    fwCoreServiceClassDefinitionsMacro ( (MultiTabView)(::gui::view::IView::Baseclass) ) ;

    /// Constructor, does nothing.
    GUI_API MultiTabView() throw() ;

    /// Destructor, does nothing.
    GUI_API virtual ~MultiTabView() throw() ;

protected:

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

private :

    int m_orient;

    /// Layout manager of this view
    wxNotebook * m_notebook;

    /// Save layout configuration definition and created container
    std::list< ViewInfo>  m_views;

};


} // namespace view
} // namespace gui


#endif /*_GUI_VIEW_MULTI_TAB_VIEW_HPP_*/
