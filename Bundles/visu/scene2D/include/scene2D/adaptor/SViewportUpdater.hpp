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
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="viewportUpdater" type="::scene2D::adaptor::SViewportUpdater" autoConnect="yes">
       <in key="viewport" uid="..." />
       <config xAxis="xAxis" yAxis="yAxis" zValue="7" />
   </service>
   @endcode
 *
 * @subsection In In
 * - \b viewport [::fwRenderQt::data::Viewport]: the viewport object this updater takes values from.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *
 * This adaptor is intended to be used with a ::scene2D::adaptor::SViewportRangeSelector adaptor.
 */
class SCENE2D_CLASS_API SViewportUpdater : public ::fwRenderQt::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SViewportUpdater)(::fwRenderQt::IAdaptor) );
    SCENE2D_API SViewportUpdater() noexcept;
    SCENE2D_API ~SViewportUpdater() noexcept;

protected:
    SCENE2D_API void configuring() override;
    SCENE2D_API void starting() override;
    SCENE2D_API void updating() override;
    SCENE2D_API void stopping() override;

    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event ) override;
    SCENE2D_API KeyConnectionsMap getAutoConnections() const override;
};

}   // namespace adaptor
}   // namespace scene2D

#endif  // __SCENE2D_ADAPTOR_SVIEWPORTUPDATER_HPP__

