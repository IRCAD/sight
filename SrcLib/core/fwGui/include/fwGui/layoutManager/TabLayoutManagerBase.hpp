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
             m_name(""),
             m_isSelect(false)
         {}

         std::string  m_name;
         bool         m_isSelect;
     };

    /// Constructor. Do nothing.
    FWGUI_API TabLayoutManagerBase();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~TabLayoutManagerBase();

    /**
     * @brief Configure the layout before creation.
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

#endif /*_FWGUI_LAYOUTMANAGER_CARDINALLAYOUTMANAGERBASE_HPP_*/


