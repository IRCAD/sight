/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_BUILDER_ICONTAINERBUILDER_HPP_
#define _FWGUI_BUILDER_ICONTAINERBUILDER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/container/fwContainer.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
namespace builder
{

/**
 * @brief   Defines the interface class  for the container builder.
 * @class   IContainerBuilder.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IContainerBuilder : public ::fwCore::BaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IContainerBuilder)(::fwCore::BaseObject) )

    typedef std::string RegistryKeyType;

    FWGUI_API const static RegistryKeyType REGISTRY_KEY;

    /// Constructor. Do nothing.
    FWGUI_API IContainerBuilder();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~IContainerBuilder();

    /**
     * @brief Returns the builded menu bar.
     */
    FWGUI_API virtual ::fwGui::container::fwContainer::sptr getContainer();

    /**
     * @brief Configure the layout before creation. Does nothing.
     */
    FWGUI_API virtual void initialize( ::fwRuntime::ConfigurationElement::sptr configuration);

    /**
     * @brief Instantiate layout with parent container.
     * @pre LayoutManager must be initialized before.
     * @pre parent container must be instanced.
     */
    FWGUI_API virtual void createContainer( ::fwGui::container::fwContainer::sptr parent ) = 0;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUI_API virtual void destroyContainer() = 0;

    /// Replace parent container
    FWGUI_API virtual void setParent(::fwGui::container::fwContainer::sptr parent) = 0;

protected:

    /// Container.
    ::fwGui::container::fwContainer::sptr m_container;

};

} // namespace builder
} // namespace fwGui

#endif /*_FWGUI_BUILDER_ICONTAINERBUILDER_HPP_*/


