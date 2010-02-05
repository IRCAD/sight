/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ASPECT_IASPECT_HPP_
#define _GUI_ASPECT_IASPECT_HPP_

#include <boost/shared_ptr.hpp>
#include <fwServices/IService.hpp>
#include <wx/icon.h>

#include "gui/export.hpp"
#include "gui/aspect/IAspect.hpp"
#include "gui/aspect/IMenu.hpp"
#include "gui/view/IView.hpp"



namespace gui
{

/**
 * @namespace ::gui::aspect
 * @brief This namespace contains the classes which define graphical user interfaces (menu, toolbar...)
 */
namespace aspect
{

/**
 * @brief   Defines the service interface managing GUI aspect, including menu actions and views
 * @class   IAspect.
 * @author  IRCAD (Research and Development Team).

 * @date    2009.
 *
 * @todo    ACH: Generic actions should not be defined in the config aspect.
 * @todo    ACH: Using shared pointer in order to reference action and menu services is dubious and should be dangerous.
 */
class GUI_CLASS_API IAspect : public ::fwServices::IService
{

public :
    fwCoreNonInstanciableClassDefinitionsMacro ( (IAspect)( ::fwServices::IService::Baseclass) ) ;

    /**
     * @brief   Constructor. Do nothing (Just initializes parameters).
     *
     * By default, the IAspect::m_name is set to "NO NAME". IAspect::m_minSizeHeight and IAspect::m_minSizeWidth are set to -1.
     */
    GUI_API IAspect() throw() ;

    /// Destructor. Do nothing.
    GUI_API virtual ~IAspect() throw() ;

    /**
     * @brief This method returns the application title, IAspect::m_name.
     *
     *@return Application title.
     */
    GUI_API std::string getName() ;

    /**
     * @brief This method returns the application icon, IAspect::m_icon.
     *
     *@return Application icon.
     */
    GUI_API ::boost::shared_ptr< wxIcon > getIcon() ;

    /**
     * @brief This method returns the application minimum height and width size, IAspect::m_minSizeHeight and IAspect::m_minSizeWidth.
     *
     * @return wxSize Application minimum size.
     */
    GUI_API wxSize getMinSize() ;

protected :

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */
    /**
     * @brief   This method initializes class member parameters from configuration elements.
     *
     * @todo ACH: It should be useful to verify the config is well formed.
     */
    GUI_API virtual void configuring() throw( ::fwTools::Failed ) ;

    /// This method first registers the aspect gui manager and it starts the menus contained in the IAspect::m_menus.
    GUI_API virtual void starting() throw( ::fwTools::Failed );

    /// This method first unregisters the aspect gui manager and it stops the menus contained in the IAspect::m_menus.
    GUI_API virtual void stopping() throw( ::fwTools::Failed );

    /**
     * @brief   Normally, perform some computations according to object (this service is attached to) attribute values and its internal state. Do nothing here.
     *
     * @todo ACH: This method should not be implemented, but imposed to its children classes.
     */
    GUI_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed ) = 0;

    /**
     * @brief   Normally, perform some computations according to object (this service is attached to) attribute values and its internal state. Do nothing here.
     *
     * @todo ACH: This method should not be implemented, but imposed to its children classes.
     */
    GUI_API virtual void updating() throw( ::fwTools::Failed ) = 0 ;

    /// Gives informations about the class. Do nothing.
    GUI_API virtual void info(std::ostream &_sstream ) ;

    ///@}

    /// Service vector of ::gui::view::IView type.  List of the view declared in the aspect config.
    std::vector< ::gui::view::IView::sptr >         m_views ;

    /// Service vector of ::gui::aspect::IMenu UUID.  List of the menus declared in the aspect config.
    std::vector< std::string >                      m_menusUUID ;

    /// Application name.
    std::string                                     m_name ;

    /// Application icon.
    ::boost::shared_ptr< wxIcon >                   m_icon;

    /// Application minimum height.
    int                                             m_minSizeHeight;

    /// Application minimum width.
    int                                             m_minSizeWidth;

    /**
    * @brief This method creates and returns the application icon, using an image path.
    *
    *@param[in] _imagePath Icon path.
    *@return Application icon.
    */
    wxIcon* createIcon( std::string _imagePath) const throw( ::fwTools::Failed );

};



} // namespace aspect
} // namespace gui


#endif /*_GUI_ASPECT_IASPECT_HPP_*/
