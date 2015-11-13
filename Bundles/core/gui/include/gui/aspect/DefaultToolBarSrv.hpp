/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_ASPECT_DEFAULTTOOLBARSRV_HPP__
#define __GUI_ASPECT_DEFAULTTOOLBARSRV_HPP__

#include <fwCore/base.hpp>

#include <fwGui/IToolBarSrv.hpp>

#include "gui/config.hpp"

namespace gui
{
namespace aspect
{

/**
 * @brief   Defines the default toolbar for standard application
 * @class   DefaultToolBarSrv
 */
class GUI_CLASS_API DefaultToolBarSrv : public ::fwGui::IToolBarSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (DefaultToolBarSrv)(::fwGui::IToolBarSrv) );

    /// Constructor. Do nothing.
    GUI_API DefaultToolBarSrv() throw();

    /// Destructor. Do nothing.
    GUI_API virtual ~DefaultToolBarSrv() throw();

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief This method create a new ToolBar in frame.
     */

    /**
     * @brief Create the toolBar
     * @see ::fwGui::IToolBarSrv::create()
     */
    GUI_API virtual void starting() throw( ::fwTools::Failed );

    /**
     * @brief This method remove the tools situated in the ToolBar.
     * @see ::fwGui::IToolBarSrv::destroy()
     */
    GUI_API virtual void stopping() throw( ::fwTools::Failed );

    /**
     * @brief This method is used to update services. Do nothing.
     */
    GUI_API virtual void updating() throw(::fwTools::Failed);

    /**
     * @brief Configuring method allows to create a toolBar with several actions.
     *
     * Example of configuration
     * @verbatim
       <service uid="toolbar2" type="::fwGui::IToolBarSrv" impl="::gui::aspect::DefaultToolBarSrv" autoConnect="no" >
           <gui>
               <layout>
                   <menuItem name="My item 2" style="radio" icon="Bundles/TutoGui_0-1/icons/system.png"/>
                   <menuItem name="My item 3" style="radio" icon="Bundles/TutoGui_0-1/icons/system.png"/>
                   <separator />
                   <menuItem name="My item A" style="radio" icon="Bundles/TutoGui_0-1/icons/monkey.png"/>
                   <menuItem name="My item B" style="radio" icon="Bundles/TutoGui_0-1/icons/monkey.png"/>
                   <separator />
                   <menu name="My menu" />
                   <separator />
                   <editor />
               </layout>
           </gui>
           <registry>
               <menuItem sid="item2" />
               <menuItem sid="item3" />
               <menuItem sid="item4" />
               <menuItem sid="item5" />
               <menu sid="menu" />
               <editor sid="editor" />
           </registry>
       </service>
       @endverbatim
     *   - \<gui\> \</gui\> : (mandatory) describe the interface of the service.
     *   - \<registry\> \</registry\> : (mandatory) describe the service management.
     *     - \<menuItem\> represents IActionSrv
     *     - \<menu\> represents IMenuSrv
     *     - \<editor\> represents container service (IEditor, IView, ...)
     *
     *   @warning
     *   - The number of item in the gui section must be EQUAL to the registry section.
     *   - The order of the item in each section (gui and registry) must be the same.\n
     *  For example: the item named "My item 2" will be connected with the service which have the sid = "item2".
     *   - A toolbar can't have the same service connected on two different buttons.
     *
     *  @see ::fwGui::IToolBarSrv::initialize(), ::fwGui::layoutManager::IToolBarLayoutManager::initialize()
     */
    GUI_API virtual void configuring() throw( ::fwTools::Failed );

    /**
     * @brief This method is used to give information about the service. Do nothing.
     */
    GUI_API virtual void info(std::ostream &_sstream );
    ///@}

};

}

}

#endif // __GUI_ASPECT_DEFAULTTOOLBARSRV_HPP__
