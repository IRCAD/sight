/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_BUILDER_TOOLBARBUILDER_HPP__
#define __FWGUIQT_BUILDER_TOOLBARBUILDER_HPP__

#include "fwGuiQt/config.hpp"
#include "fwGuiQt/container/QtContainer.hpp"

#include <fwCore/base.hpp>

#include <fwGui/builder/IToolBarBuilder.hpp>
#include <fwGui/container/fwContainer.hpp>
#include <fwGui/container/fwToolBar.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

namespace fwGui
{
namespace builder
{

/**
 * @brief   Defines the generic layout manager for IHM.
 *
 */
class FWGUIQT_CLASS_API ToolBarBuilder : public ::fwGui::builder::IToolBarBuilder
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ToolBarBuilder)(::fwGui::builder::IToolBarBuilder),
                                            (()),
                                            ::fwGui::factory::New< ToolBarBuilder >);

    FWGUIQT_API ToolBarBuilder(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~ToolBarBuilder();

    /**
     * @brief Instantiate layout with parent toolBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent toolBar must be instanced.
     */
    FWGUIQT_API virtual void createToolBar( ::fwGui::container::fwContainer::sptr parent ) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIQT_API virtual void destroyToolBar() override;

protected:
    ::fwGuiQt::container::QtContainer::sptr m_parent;
};

} // namespace builder
} // namespace fwGui

#endif /*__FWGUIQT_BUILDER_TOOLBARBUILDER_HPP__*/

