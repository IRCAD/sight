/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_BUILDER_MENUBARBUILDER_HPP_
#define _FWGUIWX_BUILDER_MENUBARBUILDER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwGui/builder/IMenuBarBuilder.hpp>
#include <fwGui/container/fwMenuBar.hpp>
#include <fwGui/container/fwContainer.hpp>

#include "fwGuiWx/config.hpp"

namespace fwGui
{
namespace builder
{

/**
 * @brief   Defines the generic layout manager for IHM.
 * @class   MenuBarBuilder.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIWX_CLASS_API MenuBarBuilder : public ::fwGui::builder::IMenuBarBuilder
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MenuBarBuilder)(::fwGui::builder::IMenuBarBuilder) ,(()), new MenuBarBuilder);

    /// Constructor. Do nothing.
    FWGUIWX_API MenuBarBuilder();

    /// Destructor. Do nothing.
    FWGUIWX_API virtual ~MenuBarBuilder();

    /**
     * @brief Instantiate layout with parent menuBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    FWGUIWX_API virtual void createMenuBar( ::fwGui::container::fwContainer::sptr parent );

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIWX_API virtual void destroyMenuBar();

protected:
    ::fwGuiWx::container::WxContainer::sptr m_parent;
};

} // namespace builder
} // namespace fwGui

#endif /*_FWGUIWX_BUILDER_MENUBARBUILDER_HPP_*/


