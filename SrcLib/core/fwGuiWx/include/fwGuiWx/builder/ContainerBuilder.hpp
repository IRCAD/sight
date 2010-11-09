/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_BUILDER_CONTAINERBUILDER_HPP_
#define _FWGUIWX_BUILDER_CONTAINERBUILDER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwGui/builder/IContainerBuilder.hpp>
#include <fwGui/container/fwContainer.hpp>

#include "fwGuiWx/config.hpp"

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
class FWGUIWX_CLASS_API ContainerBuilder : public ::fwGui::builder::IContainerBuilder
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ContainerBuilder)(::fwGui::builder::IContainerBuilder) ,(()), new ContainerBuilder);

    /// Constructor. Do nothing.
    FWGUIWX_API ContainerBuilder();

    /// Destructor. Do nothing.
    FWGUIWX_API virtual ~ContainerBuilder();

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent container must be instanced.
     */
    FWGUIWX_API virtual void createContainer( ::fwGui::container::fwContainer::sptr parent );

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIWX_API virtual void destroyContainer();

    FWGUIWX_API virtual void setParent(::fwGui::container::fwContainer::sptr parent);

protected:
    ::fwGuiWx::container::WxContainer::sptr m_parent;
};

} // namespace builder
} // namespace fwGui

#endif /*_FWGUIWX_BUILDER_CONTAINERBUILDER_HPP_*/


