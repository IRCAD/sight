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
 * @brief Adaptor implementation that manages the camera on the view.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="interactor" type="::scene2D::adaptor::SViewportInteractor">
   </service>
   @endcode
 *
 */
class SCENE2D_CLASS_API SViewportInteractor : public ::fwRenderQt::IAdaptor
{

public:

    fwCoreServiceMacro(SViewportInteractor, ::fwRenderQt::IAdaptor);

    SCENE2D_API SViewportInteractor() noexcept;

    SCENE2D_API ~SViewportInteractor() noexcept;

protected:

    SCENE2D_API void configuring() override;

    SCENE2D_API void starting() override;

    SCENE2D_API void updating() override;

    SCENE2D_API void stopping() override;

    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event ) override;

    SCENE2D_API void zoom( bool zoomIn );

private:

    bool m_viewportIsTranslated;
    ::fwRenderQt::data::Coord m_lastCoordEvent;
};

}   // namespace adaptor
}   // namespace scene2D
