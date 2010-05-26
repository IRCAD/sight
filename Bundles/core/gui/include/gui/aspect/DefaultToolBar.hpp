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

    fwCoreServiceClassDefinitionsMacro ( (DefaultToolBar)(::gui::aspect::IToolBar) ) ;

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
    /**
     * @brief Configuring method allows to create a toolBar with several actions. The created toolbar is a horizontal one without border.
     * Each icon has a size of 32x32 by default.
     * Here a sample of the DefaultToolBar service declaration with two actions:
     * @verbatim
        <service uid="toolBar" type="::gui::aspect::IToolBar" implementation="::gui::aspect::DefaultToolBar" autoComChannel="no">
            <toolBitmapSize height="40" width="40" />
            <action uid="action_new" name="New file" icon="Bundles/MyApplication/icons/newFile.png"/>
            <separator/>
            <action uid="action_3Dview" style="radio" name="3D view" icon="Bundles/MyApplication/icons//icon-3D.png" />
            <action uid="action_2Dview" style="radio" name="2D view" icon="Bundles/MyApplication/icons/icon-2D.png" />
            <separator/>
            <action uid="action_hideInformation"  style="check" name="Show information window" icon="Bundles/MyApplication/icons/View-INFO.png" />
        </service>
       @endverbatim
      * - <toolBitmapSize height="40" width="40" /> : has to change the size of the icon used in the toolbar.
      * - <separator /> : allows to put a separation in the tool bar.
      * - Action Attributes:
      *       - icon is the icon file to use.
      *       - name is the text that will be put inside the tool tip.
      *       - state can be {check}. This attribute is available only with the style radio and check.
      *       - style is the style of the button. The available choice is {radio|check}. If style isn't specified normal state(without 'state')is used.
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
