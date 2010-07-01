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
            m_visible (true),
            m_panel (0),
            m_position(0),
            m_layer(0),
            m_row(0),
            m_autostart(false),
            m_caption (std::make_pair(false,""))
        {}

        std::string                      m_align;
        std::pair< int, int >            m_minSize;
        bool                             m_visible;
        wxPanel *                        m_panel;
        bool                             m_isResizable;
        int                              m_position;
        int                              m_layer;
        int                              m_row;
        bool                             m_autostart;
        std::pair< bool, std::string >   m_caption;
    };

public :

    fwCoreServiceClassDefinitionsMacro ( (MultiView)(::gui::view::IView) ) ;

    /// Constructor, does nothing.
    GUI_API MultiView() throw() ;

    /// Destructor, does nothing.
    GUI_API virtual ~MultiView() throw() ;

    /// Analyses xml configuration to configure this service, especially number of panel and the layout of this view.
    /**
     * @brief Configuring method allows to configure an application with several views.
     * Here a sample of the multiview service declaration with two views:
     * @verbatim
            <service uid="multiView_1" type="::gui::view::IView" implementation="::gui::view::MultiView" autoComChannel="no">
                <views>
                    <view uid="multiViewIO" align="center" minWidth="250" autoStart="no" />
                    <view uid="multiViewGraphEditors" align="right" minWidth="250"  autoStart="yes" />
                </views>
            </service>
       @endverbatim
     * for which:\n
     * <views> node can only contain <view> node.\n
     * Each <view> node must contain uid attribute and the following attribute can be used but aren't mandatory.\n
     *  - align can be {center|right|left|bottom|top}. -- Default value: "center"
     *  - minWidth is the min width of the view. -- Default value: -1
     *  - minHeight is the min height of the view -- Default value: -1
     *  - resizable can be {yes|no}. -- Default value: true
     *  - position is a integer -- Default value: 0
     *  - layer -- Default value: 0
     *  - row is a integer -- Default value: 0
     *  - visible can be {yes|true|no|false} -- Default value: true
     *  - caption -- No default value.
     *  - autoStart can be {yes|no}   -- Default value: false
     */
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
    typedef std::map< std::string , ViewInfo > PanelContainer;

    /// Save layout configuration definition and created container
    PanelContainer m_panels;

};


} // namespace view
} // namespace gui


#endif /*_GUI_VIEW_MULTI_VIEW_HPP_*/
