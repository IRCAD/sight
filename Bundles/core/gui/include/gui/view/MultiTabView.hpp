///* ***** BEGIN LICENSE BLOCK *****
// * FW4SPL - Copyright (C) IRCAD, 2009-2010.
// * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
// * published by the Free Software Foundation.
// * ****** END LICENSE BLOCK ****** */
//
//#ifndef _GUI_VIEW_MULTI_TAB_VIEW_HPP_
//#define _GUI_VIEW_MULTI_TAB_VIEW_HPP_
//
//#include <wx/wx.h>
//#include <wx/notebook.h>
//
//#include <fwTools/Failed.hpp>
//
//#include "gui/export.hpp"
//#include "gui/view/IView.hpp"
//
//
//namespace gui
//{
//namespace view
//{
//
///**
// * @brief   Defines the default tab panel for standard application.
// * @class   MultiTabView.
// * @author  IRCAD (Research and Development Team)
//
// * @date    2009-2010.
// */
//class GUI_CLASS_API MultiTabView : public ::gui::view::IView
//{
//    class ViewInfo
//    {
//    public :
//
//        ViewInfo() :
//            m_panel (0),
//            m_uid(""),
//            m_name(""),
//            m_autostart(false),
//            m_isSelect(false)
//        {}
//
//        std::string  m_uid;
//        std::string  m_name;
//        wxPanel *    m_panel;
//        bool         m_autostart;
//        bool         m_isSelect;
//    };
//
//public :
//
//    fwCoreServiceClassDefinitionsMacro ( (MultiTabView)(::gui::view::IView) ) ;
//
//    /// Constructor, does nothing.
//    GUI_API MultiTabView() throw() ;
//
//    /// Destructor, does nothing.
//    GUI_API virtual ~MultiTabView() throw() ;
//
//protected:
//
//    /// Analyses xml configuration to configure this service, especially number of panel and the layout of this view.
//    /**
//     * @brief Configuring method allows to configure an application with several tab views.
//     * Here a sample of the multiTabView service declaration with two views:
//     * @verbatim
//            <service uid="multiTabView" type="::gui::view::IView" implementation="::gui::view::MultiTabView" autoComChannel="no">
//                <views>
//                    <view uid="multiViewTab1" name="Tab1" selected="yes" autoStart="yes" />
//                    <view uid="multiViewTab2" name="Tab2" autoStart="yes" />
//                </views>
//            </service>
//       @endverbatim
//     *  for which:\n
//     * <views> node is mandatory and can only contain <view> node.\n
//     * Each <view> node must contain uid attribute and the following attribute can be used but aren't mandatory.\n
//     *  - name is a string . -- Default value: "" \n
//     *    It is the name of the tab.\n
//     *  - selected can be {yes|no} -- Default value: false \n
//     *  It indicates which tab will be selected at start of the service. \n
//     *  - autoStart can be {yes|no}   -- Default value: false
//     */
//    GUI_API void configuring() throw( ::fwTools::Failed );
//
//    /// Reconfigures service, does nothing.
//    GUI_API void reconfiguring() throw( ::fwTools::Failed );
//
//    /// Builds defined layout and creates wxPanel.
//    GUI_API void starting() throw( ::fwTools::Failed );
//
//    /// Uninit layout and destroy panel.
//    GUI_API void stopping() throw( ::fwTools::Failed );
//
//    /// Swap associated object, does nothing.
//    GUI_API void swappping() throw( ::fwTools::Failed );
//
//    /// Updates service, does nothing.
//    GUI_API void updating() throw( ::fwTools::Failed );
//
//    /// Updates service on notification, does nothing.
//    GUI_API void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw( ::fwTools::Failed );
//
//    /// Prints service name
//    GUI_API void info(std::ostream &_sstream );
//
//private :
//
//    int m_orient;
//
//    /// Layout manager of this view
//    wxNotebook * m_notebook;
//
//    /// Save layout configuration definition and created container
//    std::list< ViewInfo>  m_views;
//
//};
//
//
//} // namespace view
//} // namespace gui
//
//
//#endif /*_GUI_VIEW_MULTI_TAB_VIEW_HPP_*/
