/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_BUILDER_CONTAINERBUILDER_HPP_
#define _FWGUIQT_BUILDER_CONTAINERBUILDER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwGui/builder/IContainerBuilder.hpp>
#include <fwGui/container/fwContainer.hpp>

#include "fwGuiQt/config.hpp"

namespace fwGui
{
namespace builder
{

/**
 * @brief   Defines the generic layout manager for IHM.
 * @class   ContainerBuilder.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API ContainerBuilder : public ::fwGui::builder::IContainerBuilder
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ContainerBuilder)(::fwGui::builder::IContainerBuilder) ,(()), new ContainerBuilder);

    /// Constructor. Do nothing.
    FWGUIQT_API ContainerBuilder();

    /// Destructor. Do nothing.
    FWGUIQT_API virtual ~ContainerBuilder();

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent container must be instanced.
     */
    FWGUIQT_API virtual void createContainer( ::fwGui::container::fwContainer::sptr parent );

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIQT_API virtual void destroyContainer();

    FWGUIQT_API virtual void setParent(::fwGui::container::fwContainer::sptr parent);

protected:
    ::fwGuiQt::container::QtContainer::sptr m_parent;
};

} // namespace builder
} // namespace fwGui

#endif /*_FWGUIQT_BUILDER_CONTAINERBUILDER_HPP_*/


