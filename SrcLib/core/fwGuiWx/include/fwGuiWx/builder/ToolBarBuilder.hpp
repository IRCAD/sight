/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_BUILDER_TOOLBARBUILDER_HPP_
#define _FWGUIWX_BUILDER_TOOLBARBUILDER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwGui/builder/IToolBarBuilder.hpp>
#include <fwGui/container/fwToolBar.hpp>
#include <fwGui/container/fwContainer.hpp>

#include "fwGuiWx/config.hpp"

namespace fwGui
{
namespace builder
{

/**
 * @brief   Defines the generic layout manager for IHM.
 * @class   ToolBarBuilder.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIWX_CLASS_API ToolBarBuilder : public ::fwGui::builder::IToolBarBuilder
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ToolBarBuilder)(::fwGui::builder::IToolBarBuilder) ,(()), new ToolBarBuilder);

    /// Constructor. Do nothing.
    FWGUIWX_API ToolBarBuilder();

    /// Destructor. Do nothing.
    FWGUIWX_API virtual ~ToolBarBuilder();

    /**
     * @brief Instantiate layout with parent toolBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent toolBar must be instanced.
     */
    FWGUIWX_API virtual void createToolBar( ::fwGui::container::fwContainer::sptr parent );

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIWX_API virtual void destroyToolBar();

protected:
    ::fwGuiWx::container::WxContainer::sptr m_parent;
};

} // namespace builder
} // namespace fwGui

#endif /*_FWGUIWX_BUILDER_TOOLBARBUILDER_HPP_*/


