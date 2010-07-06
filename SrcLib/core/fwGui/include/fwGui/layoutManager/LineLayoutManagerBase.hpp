/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
             m_caption (std::make_pair(false,""))
         {}

         int                            m_proportion;
         int                            m_border;
         std::pair< int, int >          m_minSize;
         std::pair< bool, std::string > m_caption;
     };

    /// Constructor. Do nothing.
    FWGUI_API LineLayoutManagerBase();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~LineLayoutManagerBase();

    /**
     * @brief Configure the layout before creation.
     */
    FWGUI_API virtual void initialize( ConfigurationType configuration);

    FWGUI_API static const RegistryKeyType REGISTRY_KEY;

    FWGUI_API Orientation getOrientation(){ return m_orientation;};

    FWGUI_API std::list< ViewInfo> getViewsInfo(){ return m_views;};

private:

    /// Save layout configuration definition
    std::list< ViewInfo>  m_views;

    Orientation m_orientation;
};

} // namespace layoutManager
} // namespace fwGui

#endif /*_FWGUI_LAYOUTMANAGER_LINELAYOUTMANAGERBASE_HPP_*/


