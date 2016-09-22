/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_BUILDER_SLIDEVIEWBUILDER_HPP__
#define __FWGUIQT_BUILDER_SLIDEVIEWBUILDER_HPP__

#include "fwGuiQt/config.hpp"
#include "fwGuiQt/container/QtContainer.hpp"

#include <fwCore/base.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwGui/builder/ISlideViewBuilder.hpp>
#include <fwGui/container/fwContainer.hpp>


namespace fwGui
{
namespace builder
{

/**
 * @brief   Defines the qt implementation of the slide view builder.
 */
class FWGUIQT_CLASS_API SlideViewBuilder : public ::fwGui::builder::ISlideViewBuilder
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (SlideViewBuilder)(::fwGui::builder::ISlideViewBuilder),
                                            (()),
                                            ::fwGui::factory::New< SlideViewBuilder >);

    FWGUIQT_API SlideViewBuilder(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~SlideViewBuilder();

    /// Create the slide view container
    FWGUIQT_API virtual void createContainer( ::fwGui::container::fwContainer::sptr parent );

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIQT_API virtual void destroyContainer();

protected:
    ::fwGuiQt::container::QtContainer::sptr m_parent;
};

} // namespace builder
} // namespace fwGui

#endif /*__FWGUIQT_BUILDER_SLIDEVIEWBUILDER_HPP__*/
