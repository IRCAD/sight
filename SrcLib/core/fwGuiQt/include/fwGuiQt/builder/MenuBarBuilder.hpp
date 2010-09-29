/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_BUILDER_MENUBARBUILDER_HPP_
#define _FWGUIQT_BUILDER_MENUBARBUILDER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwGui/builder/IMenuBarBuilder.hpp>
#include <fwGui/fwMenuBar.hpp>
#include <fwGui/fwContainer.hpp>

#include "fwGuiQt/config.hpp"

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
class FWGUIQT_CLASS_API MenuBarBuilder : public ::fwGui::builder::IMenuBarBuilder
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MenuBarBuilder)(::fwGui::builder::IMenuBarBuilder) ,(()), new MenuBarBuilder);

    /// Constructor. Do nothing.
    FWGUIQT_API MenuBarBuilder();

    /// Destructor. Do nothing.
    FWGUIQT_API virtual ~MenuBarBuilder();

    /**
     * @brief Instantiate layout with parent menuBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    FWGUIQT_API virtual void createMenuBar( ::fwGui::fwContainer::sptr parent );

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIQT_API virtual void destroyMenuBar();

protected:
    ::fwGuiQt::container::QtContainer::sptr m_parent;
};

} // namespace builder
} // namespace fwGui

#endif /*_FWGUIQT_BUILDER_MENUBARBUILDER_HPP_*/


