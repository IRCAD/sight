/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWGUI_BUILDER_ITOOLBARBUILDER_HPP__
#define __FWGUI_BUILDER_ITOOLBARBUILDER_HPP__

#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/GuiBaseObject.hpp"
#include "fwGui/container/fwToolBar.hpp"
#include "fwGui/container/fwContainer.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{

namespace builder
{

/**
 * @brief   Defines the interface class  for the toolbar builder.
 * @class   IToolBarBuilder
 *
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IToolBarBuilder : public ::fwGui::GuiBaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IToolBarBuilder)(::fwGui::GuiBaseObject) )

    typedef std::string RegistryKeyType;

    FWGUI_API const static RegistryKeyType REGISTRY_KEY;

    typedef enum
    {
        TOP,
        BOTTOM,
        RIGHT,
        LEFT
    } Aligment;


    /// Constructor. Do nothing.
    FWGUI_API IToolBarBuilder();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~IToolBarBuilder();

    /**
     * @brief Returns the builded tool bar.
     */
    FWGUI_API virtual ::fwGui::container::fwToolBar::sptr getToolBar();

    /**
     * @brief Initialize the tool bar.
     *
     * Example of configuration
     * @code{.xml}
        <toolBar align="top">
            <toolBitmapSize height= "50" width="50" />
        </toolBar>
       @endcode
        - \b align : toolbar alignment (top, bottom, left, right)
     *  - \<toolBitmapSize height= "50" width="50"/\> : give the size of the icon.
     */

    FWGUI_API virtual void initialize( ::fwRuntime::ConfigurationElement::sptr configuration);

    /**
     * @brief Instantiate layout with parent toolBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent toolBar must be instanced.
     */
    FWGUI_API virtual void createToolBar( ::fwGui::container::fwContainer::sptr parent ) = 0;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUI_API virtual void destroyToolBar() = 0;

protected:

    /// ToolBar.
    ::fwGui::container::fwToolBar::sptr m_toolBar;

    std::pair< int, int > m_toolBitmapSize;

    Aligment m_aligment;

};

} // namespace builder
} // namespace fwGui

#endif /*__FWGUI_BUILDER_ITOOLBARBUILDER_HPP__*/


