/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_BUILDER_MENUBARBUILDER_HPP__
#define __FWGUIQT_BUILDER_MENUBARBUILDER_HPP__

#include "fwGuiQt/config.hpp"
#include "fwGuiQt/container/QtContainer.hpp"

#include <fwCore/base.hpp>

#include <fwGui/builder/IMenuBarBuilder.hpp>
#include <fwGui/container/fwContainer.hpp>
#include <fwGui/container/fwMenuBar.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
namespace fwGui
{
namespace builder
{

/**
 * @brief   Defines the generic layout manager for IHM.
 *
 */
class FWGUIQT_CLASS_API MenuBarBuilder : public ::fwGui::builder::IMenuBarBuilder
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MenuBarBuilder)(::fwGui::builder::IMenuBarBuilder),
                                            (()),
                                            ::fwGui::factory::New< MenuBarBuilder > );

    FWGUIQT_API MenuBarBuilder(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~MenuBarBuilder();

    /**
     * @brief Instantiate layout with parent menuBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    FWGUIQT_API virtual void createMenuBar( ::fwGui::container::fwContainer::sptr parent ) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIQT_API virtual void destroyMenuBar() override;

protected:
    ::fwGuiQt::container::QtContainer::sptr m_parent;
};

} // namespace builder
} // namespace fwGui

#endif /*__FWGUIQT_BUILDER_MENUBARBUILDER_HPP__*/

