/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_VIEW_MULTI_SIZER_VIEW_HPP_
#define _GUI_VIEW_MULTI_SIZER_VIEW_HPP_

#include <wx/wx.h>
#include <wx/sizer.h>

#include <fwTools/Failed.hpp>

#include "gui/export.hpp"
#include "gui/view/IView.hpp"


namespace gui
{
namespace view
{

/**
 * @brief   Defines the default panel for standard application.
 * @class   MultiSizerView.
 * @author  IRCAD (Research and Development Team)

 * @date    2009.
 */
class GUI_CLASS_API MultiSizerView : public ::gui::view::IView
{
    class ViewInfo
    {
    public :

        ViewInfo() :
            m_proportion (1),
            m_border(0),
            m_panel (0),
            m_guid(-1)
        {}

        int         m_proportion;
        int         m_border;
        int         m_guid;
        wxPanel *   m_panel;
    };

public :

    /// Constructor, does nothing.
    GUI_API MultiSizerView() throw() ;
    
    /// Destructor, does nothing.
    GUI_API virtual ~MultiSizerView() throw() ;

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
    wxBoxSizer * m_sizer;

    /// Save layout configuration definition and created container
    std::list< ViewInfo>  m_views;

};


} // namespace view
} // namespace gui


#endif /*_GUI_VIEW_MULTI_SIZER_VIEW_HPP_*/
