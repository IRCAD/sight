/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file fwGui/IToolBarBuilder.hpp
 * @brief This file defines the interface class  for the toolbar builder.
 *
 * @author IRCAD (Research and Development Team).
 * @date 2009-2010
 */

#ifndef _FWGUI_BUILDER_ITOOLBARBUILDER_HPP_
#define _FWGUI_BUILDER_ITOOLBARBUILDER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/container/fwToolBar.hpp"
#include "fwGui/container/fwContainer.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
/**
 * @brief       The namespace fwGui::builder contains the base interface for GUI elements builder.
 * @namespace   fwGui::builder
 * @author      IRCAD (Research and Development Team).
 * @date        2009-2010.
 *
 */
namespace builder
{

/**
 * @brief   Defines the generic layout manager for toolBar.
 * @class   IToolBarBuilder.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IToolBarBuilder : public ::fwCore::BaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IToolBarBuilder)(::fwCore::BaseObject) )

    typedef std::string RegistryKeyType;

    FWGUI_API const static RegistryKeyType REGISTRY_KEY;

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
     * @verbatim
        <toolBar >
            <toolBitmapSize height= "50" width="50" />
        </toolBar>
       @endverbatim
     *  - <toolBitmapSize height= "50" width="50"/> : give the size of the icon.
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

};

} // namespace builder
} // namespace fwGui

#endif /*_FWGUI_BUILDER_ITOOLBARBUILDER_HPP_*/


