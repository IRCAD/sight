/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_LAYOUTMANAGER_CARDINALLAYOUTMANAGERBASE_HPP_
#define _FWGUI_LAYOUTMANAGER_CARDINALLAYOUTMANAGERBASE_HPP_

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
 * @class   CardinalLayoutManagerBase.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API CardinalLayoutManagerBase : public IViewLayoutManager
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (CardinalLayoutManagerBase)(::fwGui::layoutManager::IViewLayoutManager) )

    /// Defines all possible position for a CardinalLayout
    typedef enum
    {
        CENTER,
        RIGHT,
        LEFT,
        BOTTOM,
        TOP
     } Align;

     typedef std::string RegistryKeyType;

     class ViewInfo
     {
     public :

         ViewInfo() :
             m_align (CENTER),
             m_minSize (std::make_pair(-1,-1)),
             m_isResizable(true),
             m_visible (true),
             m_position(0),
             m_layer(0),
             m_row(0),
             m_caption (std::make_pair(false,""))
         {}

         Align                            m_align;
         std::pair< int, int >            m_minSize;
         bool                             m_visible;
         bool                             m_isResizable;
         int                              m_position;
         int                              m_layer;
         int                              m_row;
         std::pair< bool, std::string >   m_caption;
     };

    /// Constructor. Do nothing.
    FWGUI_API CardinalLayoutManagerBase();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~CardinalLayoutManagerBase();

    /**
     * @brief Configure the layout before creation.
     */
    FWGUI_API virtual void initialize( ConfigurationType configuration);

    FWGUI_API static const RegistryKeyType REGISTRY_KEY;

    FWGUI_API std::list< ViewInfo> getViewsInfo(){ return m_views;};

protected:

    FWGUI_API static const std::map<std::string, Align> STRING_TO_ALIGN;

private:

    /// Save layout configuration definition
    std::list< ViewInfo>  m_views;

};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUI_LAYOUTMANAGER_CARDINALLAYOUTMANAGERBASE_HPP_*/


