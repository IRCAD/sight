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
            m_uid(""),
            m_minSize (std::make_pair(-1,-1)),
            m_autostart(false),
            m_caption (std::make_pair(false,""))
        {}

        int                            m_proportion;
        int                            m_border;
        std::string                    m_uid;
        wxPanel *                      m_panel;
        std::pair< int, int >          m_minSize;
        bool                           m_autostart;
        std::pair< bool, std::string > m_caption;
    };

public :

    fwCoreServiceClassDefinitionsMacro ( (MultiSizerView)(::gui::view::IView) ) ;

    /// Constructor, does nothing.
    GUI_API MultiSizerView() throw() ;

    /// Destructor, does nothing.
    GUI_API virtual ~MultiSizerView() throw() ;

protected:

    /// Analyses xml configuration to configure this service, espacially umber of panel and the layout of this view.
    /**
     * @brief Configuring method allows to configure an application with several sizer views.
     * Here a sample of the multiSizerView service declaration with two views:
     * @verbatim
            <service uid="multiViewOrgans" type="::gui::view::IView" implementation="::gui::view::MultiSizerView" autoComChannel="no">
                <views orientation="vertical" >
                    <view uid="view1" proportion="1" />
                    <view uid="view2" minHeight="80" proportion="0" />
                </views>
            </service>
       @endverbatim
     *  for which:\n
     * <views> node is mandatory and can only contain <view> node.It has the following parameter\n
     *  - orientation can be {vertical|horizontal}. It indicates if the views are put in column or in row.\n
     * Each <view> node must contain uid attribute and the following attribute can be used but aren't mandatory.\n
     *  - proportion is an integer . -- Default value: 1.\n
     *    If the proportion tag is setted to 0, the view can't be resized, If the value is above zero, the view can be resized depending on the proportion factor
     *    of the others view inside of the service. the direction of resize is fixed by the tag orientation  {vertical|horizontal} in the views node.\n
     *  - border is an integer.It is the size of the border around the view -- Default value: 0.
     *  - caption -- No default value.
     *  - minWidth is the min width of the view. -- Default value: -1.
     *  - minHeight is the min height of the view -- Default value: -1.
     *  - autoStart can be {yes|no}   -- Default value: false.
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

private :

    int m_orient;

    /// Save layout configuration definition and created container
    std::list< ViewInfo>  m_views;

};


} // namespace view
} // namespace gui


#endif /*_GUI_VIEW_MULTI_SIZER_VIEW_HPP_*/
