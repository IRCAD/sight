/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file fwGui/TabLayoutManagerBase.hpp
 * @brief This file defines the interface of the class for managing a tab layout geometry.
 *
 * @author IRCAD (Research and Development Team).
 * @date 2009-2010
 */

#ifndef _FWGUI_LAYOUTMANAGER_TABLAYOUTMANAGERBASE_HPP_
#define _FWGUI_LAYOUTMANAGER_TABLAYOUTMANAGERBASE_HPP_

#include <list>
#include <map>

#include <fwCore/base.hpp>

#include "fwGui/layoutManager/IViewLayoutManager.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines the base for cardinal layout manager.
 * @class   TabLayoutManagerBase.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API TabLayoutManagerBase : public IViewLayoutManager
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (TabLayoutManagerBase)(::fwGui::layoutManager::IViewLayoutManager) )

     typedef std::string RegistryKeyType;

     class ViewInfo
     {
     public :

         ViewInfo() :
             m_caption(""),
             m_isSelect(false)
         {}

         std::string  m_caption;
         bool         m_isSelect;
     };

    /// Constructor. Do nothing.
    FWGUI_API TabLayoutManagerBase();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~TabLayoutManagerBase();

    /**
     * @brief Initialize tab layout manager before the creation of layout.
     *
     * Example of configuration with Tab layout.
     * @verbatim
       <service uid="subView3" type="::gui::view::IView" implementation="::gui::view::DefaultView" autoComChannel="no" >
           <gui>
               <layout type="::fwGui::TabLayoutManager" >
                   <view caption="TabView1" />
                   <view caption="TabView2" selected="yes" />
                   <view caption="TabView3"  />
               </layout>
           </gui>
           <registry>
               <view sid="view1" start="yes" />
               <view sid="view2" start="yes" />
               <view sid="view3" start="yes" />
           </registry>
       </service>
      @endverbatim
    *  - <layout type="::fwGui::TabLayoutManager" > : define a tab layout.
    *  - <view caption="TabView1" selected="center" /> : define a new view which can have the following attributes
    *   - \b caption : name of the view (display on the screen).
    *   - \b selected  {yes | no}: define if the tab is the current one.
    * \note
    *   - The registry section can be empty. In this case none service use this zone
    */

    FWGUI_API virtual void initialize( ConfigurationType configuration);

    FWGUI_API static const RegistryKeyType REGISTRY_KEY;

    FWGUI_API std::list< ViewInfo> getViewsInfo(){ return m_views;};

private:

    /// Save layout configuration definition
    std::list< ViewInfo>  m_views;

};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUI_LAYOUTMANAGER_TABLAYOUTMANAGERBASE_HPP_*/


