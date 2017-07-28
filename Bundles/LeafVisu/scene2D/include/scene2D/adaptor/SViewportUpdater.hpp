/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_SVIEWPORTUPDATER_HPP__
#define __SCENE2D_ADAPTOR_SVIEWPORTUPDATER_HPP__

#include "scene2D/config.hpp"

#include <fwRenderQt/IAdaptor.hpp>

namespace scene2D
{
namespace adaptor
{

/**
 * @brief Adaptor implementation that manages the camera on the view (by updating the viewport object).
 *
 * The SViewportUpdater updates the viewport of the scene it is attached to, by picking values from the
 * managed ::fwRenderQt::data::Viewport object.
 *
 * Configuration example:
   @code{.xml}
   <service uid="viewportRangeUpdater" type="::scene2D::adaptor::SViewportUpdater">
       <in key="viewport" uid="..." />
       <config xAxis="xAxis" yAxis="yAxis" zValue="7" />
   </service>
   @endcode
 *
 * \b objectID  : the viewport object this updater takes values from.
 *
 * \b xAxis     : see ::fwRenderQt::IAdaptor
 *
 * \b yAxis     : see ::fwRenderQt::IAdaptor
 *
 * \b zValue    : see ::fwRenderQt::IAdaptor
 *
 * This adaptor is intended to be used with a ::scene2D::adaptor::SViewportRangeSelector adaptor.
 */
class SViewportUpdater : public ::fwRenderQt::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SViewportUpdater)(::fwRenderQt::IAdaptor) );
    SViewportUpdater() noexcept;
    ~SViewportUpdater() noexcept;

protected:
    SCENE2D_API void configuring();
    SCENE2D_API void doStart();
    SCENE2D_API void doUpdate();
    SCENE2D_API void doSwap();
    SCENE2D_API void doStop();

    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event );
    SCENE2D_API KeyConnectionsMap getAutoConnections() const;
};

}   // namespace adaptor
}   // namespace scene2D

#endif  // __SCENE2D_ADAPTOR_SVIEWPORTUPDATER_HPP__

