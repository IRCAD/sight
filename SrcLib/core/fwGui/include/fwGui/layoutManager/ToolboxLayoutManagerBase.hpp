/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file fwGui/ToolboxLayoutManagerBase.hpp
 * @brief This file defines the interface of the class for managing a line layout geometry.
 *
 * @author IRCAD (Research and Development Team).
 * @date 2009-2010
 */

#ifndef _FWGUI_LAYOUTMANAGER_TOOLBOXLAYOUTMANAGERBASE_HPP_
#define _FWGUI_LAYOUTMANAGER_TOOLBOXLAYOUTMANAGERBASE_HPP_

#include <list>

#include <fwCore/base.hpp>

#include "fwGui/layoutManager/IViewLayoutManager.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines the base for line layout manager.
 * @class   ToolboxLayoutManagerBase.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API ToolboxLayoutManagerBase : public IViewLayoutManager
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (ToolboxLayoutManagerBase)(::fwGui::layoutManager::IViewLayoutManager) )

     typedef std::string RegistryKeyType;

     class ViewInfo
     {
     public :

         ViewInfo() :
             m_border(0),
             m_minSize (std::make_pair(-1,-1)),
             m_caption (""),
             m_visible (true),
             m_expanded(false),
             m_useScrollBar(false)
         {}

         int                    m_border;
         std::pair< int, int >  m_minSize;
         std::string            m_caption;
         bool                   m_visible;
         bool                   m_expanded;
         bool                   m_useScrollBar;
     };

    /// Constructor. Do nothing.
    FWGUI_API ToolboxLayoutManagerBase();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~ToolboxLayoutManagerBase();

    /**
     * @brief Initialize Line layout manager before the creation of layout.
     *
     * Example of configuration with cardinal layout.
     * @verbatim
       <service uid="subView2" type="::gui::view::IView" implementation="::gui::view::DefaultView" autoComChannel="no" >
           <gui>
               <layout type="::fwGui::ToolboxLayoutManager">
                   <view caption="My view 1" expanded="true" />
                   <view caption="My view 2" expanded="true" />
                   <view caption="My view 3" expanded="false" />
                   <spacer />
               </layout>
           </gui>
           <registry>
               <view sid="view1" start="yes" />
               <view sid="view2" start="no" />
               <view wid="view3WID" />
           </registry>
       </service>
      @endverbatim
    *  - <layout type="::fwGui::ToolboxLayoutManager" > : define a toolbox layout.
    *  - <view caption="My view 1" expanded="true" minWidth="30" /> : define a new view which can have the following attributes
    *   - \b border : define the size of the border in the layout
    *   - \b caption : define the title to display.
    *   - \b minWidth  : minimal width of the view.
    *   - \b minHeight : minimal height of the view.
    *   - \b visible  {true | yes | false | no} : define if the view is visible or not.
    *   - \b expanded  {true | yes | false | no} : define if the view is expanded or not.
    *   - \b useScrollBar  {true | yes | false | no} : define if the view use scrollbar or not.
    */
    FWGUI_API virtual void initialize( ConfigurationType configuration);

    FWGUI_API static const RegistryKeyType REGISTRY_KEY;

protected:

    FWGUI_API std::list< ViewInfo> getViewsInfo(){ return m_views;};

private:

    /// Save layout configuration definition
    std::list< ViewInfo>  m_views;
};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUI_LAYOUTMANAGER_TOOLBOXLAYOUTMANAGERBASE_HPP_*/


