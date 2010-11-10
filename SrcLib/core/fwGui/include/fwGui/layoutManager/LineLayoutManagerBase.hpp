/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file fwGui/LineLayoutManagerBase.hpp
 * @brief This file defines the interface of the class for managing a line layout geometry.
 *
 * @author IRCAD (Research and Development Team).
 * @date 2009-2010
 */

#ifndef _FWGUI_LAYOUTMANAGER_LINELAYOUTMANAGERBASE_HPP_
#define _FWGUI_LAYOUTMANAGER_LINELAYOUTMANAGERBASE_HPP_

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
 * @class   LineLayoutManagerBase.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API LineLayoutManagerBase : public IViewLayoutManager
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (LineLayoutManagerBase)(::fwGui::layoutManager::IViewLayoutManager) )

    /// Defines all possible orientation for a LineLayout
    typedef enum
    {
        VERTICAL,
        HORIZONTAL
     } Orientation;

     typedef std::string RegistryKeyType;

     class ViewInfo
     {
     public :

         ViewInfo() :
             m_proportion (1),
             m_border(0),
             m_minSize (std::make_pair(-1,-1)),
             m_caption (std::make_pair(false,"")),
             m_visible (true)
         {}

         int                            m_proportion;
         int                            m_border;
         std::pair< int, int >          m_minSize;
         std::pair< bool, std::string > m_caption;
         bool                           m_visible;
     };

    /// Constructor. Do nothing.
    FWGUI_API LineLayoutManagerBase();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~LineLayoutManagerBase();

    /**
     * @brief Initialize Line layout manager before the creation of layout.
     *
     * Example of configuration with cardinal layout.
     * @verbatim
       <service uid="subView2" type="::gui::view::IView" implementation="::gui::view::DefaultView" autoComChannel="no" >
           <gui>
              <layout type="::fwGui::LineLayoutManager" >
                   <orientation value="vertical" />
                   <view caption="CardinalView1" align="center" />
                   <view caption="CardinalView2" align="right" minWidth="400" />
                   <view caption="CardinalView3" align="bottom" minHeight="400" />
               </layout>
           </gui>
           <registry>
               <view sid="view1" start="yes" />
               <view sid="view2" start="yes" />
               <view sid="view3" start="yes" />
           </registry>
       </service>
      @endverbatim
    *  - <layout type="::fwGui::CardinalLayoutManager" > : define a cardinal layout.
    *  - <view caption="CardinalView1" align="center" /> : define a new view which can have the following attributes
    *   - \b caption : name of the view (display on the screen).
    *   - \b align  {center | bottom | top | right | left}: define the position of the view
    *   - \b minWidth : minimal width of the view
    *   - \b minHeight : minimal height of the view
    *   - \b resizable  {yes | no}: define if the view can be resized.
    *   - \b position : indicates the sequential position, starting with zero. It uses if more than one view as the same align value (available only with wxWidget see wxAuiManager in wxWidgets documenattion for more details).
    *   - \b layer : available only with wxWidget. See wxAuiManager in wxWidgets documenattion for more details
    *   - \b row : use to place several views next to each other (available only with wxWidget). See wxAuiManager in wxWidgets documenattion for more details
    *   - \b visible  {true | yes | false | no} : define if the view is visible or not.
    */

    FWGUI_API virtual void initialize( ConfigurationType configuration);

    FWGUI_API static const RegistryKeyType REGISTRY_KEY;

    FWGUI_API Orientation getOrientation(){ return m_orientation;};

protected:

    FWGUI_API std::list< ViewInfo> getViewsInfo(){ return m_views;};

private:

    /// Save layout configuration definition
    std::list< ViewInfo>  m_views;

    Orientation m_orientation;
};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUI_LAYOUTMANAGER_LINELAYOUTMANAGERBASE_HPP_*/


