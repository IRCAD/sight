/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

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
    fwCoreServiceMacro(SViewportUpdater, ::fwRenderQt::IAdaptor);
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
