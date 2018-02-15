/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwGui/config.hpp"
#include "fwGui/layoutManager/IViewLayoutManager.hpp"

#include <fwCore/base.hpp>

#include <list>

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines the base for line layout manager.
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
    public:

        ViewInfo() :
            m_proportion(1),
            m_border(0),
            m_minSize(std::make_pair(-1, -1)),
            m_caption(std::make_pair(false, "")),
            m_visible(true),
            m_isSpacer(false),
            m_useScrollBar(false)
        {
        }

        int m_proportion;
        int m_border;
        std::pair< int, int >          m_minSize;
        std::pair< bool, std::string > m_caption;
        bool m_visible;
        bool m_isSpacer;
        bool m_useScrollBar;
    };

    /// Constructor. Do nothing.
    FWGUI_API LineLayoutManagerBase();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~LineLayoutManagerBase();

    /**
     * @brief Initialize Line layout manager before the creation of layout.
     *
     * Example of configuration with cardinal layout.
     * @code{.xml}
       <service uid="subView2" type="::gui::view::SDefaultView" autoConnect="no" >
           <gui>
              <layout type="::fwGui::LineLayoutManager" >
                    <orientation value="horizontal" />
                    <view proportion="0" minWidth="30" />
                    <view proportion="0" minWidth="50" />
               </layout>
           </gui>
           <registry>
               <view sid="view1" start="yes" />
               <view sid="view2" start="yes" />
           </registry>
       </service>
       @endcode
     *  - \<layout type="::fwGui::LineLayoutManager" \> : define a line layout.
     *  - \<view proportion="0" minWidth="30" /\> : define a new view which can have the following attributes
     *   - \b orientation {horizontal | vertical} : define if the layout is vertical or horizontal.
     *   - \b proportion : define if the space allocate in proportion of the proportion factor (bigger it is, more space
     * are used).
     *   - \b border : define the size of the border in the layout
     *   - \b caption : define the title to display.
     *   - \b minWidth  : minimal width of the view.
     *   - \b minHeight : minimal height of the view.
     *   - \b visible  {true | yes | false | no} : define if the view is visible or not.
     */

    FWGUI_API virtual void initialize( ConfigurationType configuration) override;

    FWGUI_API static const RegistryKeyType REGISTRY_KEY;

    //------------------------------------------------------------------------------

    Orientation getOrientation()
    {
        return m_orientation;
    }

protected:

    //------------------------------------------------------------------------------

    std::list< ViewInfo> getViewsInfo()
    {
        return m_views;
    }

private:

    /// Save layout configuration definition
    std::list< ViewInfo>  m_views;

    Orientation m_orientation;
};

} // namespace layoutManager
} // namespace fwGui
