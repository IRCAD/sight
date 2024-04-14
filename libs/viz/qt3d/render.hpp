/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include <sight/viz/qt3d/config.hpp>

#include "viz/qt3d/window_interactor.hpp"

#include <viz/__/render.hpp>

#include <QColor>
#include <QPointer>

namespace Qt3DExtras // NOLINT(readability-identifier-naming)
{

class Qt3DWindow;

} // namespace Qt3DExtras

namespace sight::viz::qt3d
{

namespace core
{

class generic_scene;

} // namespace core

/**
 * @brief Renders a generic scene with Qt3D API
 *
 * @section XML XML Configuration
 * @code{.xml}
     <service type="sight::viz::qt3d::render">
        <scene>
            <background color="...">
        </scene>
     </service>
   @endcode
 *
 * @subsection Configuration Configuration
 *  - \b scene (mandatory)
 *    - \b background (optional)
 *      - \b color (optional, string, default=#000000): background color. Must be defined in hexadecimal format or with
 * a string name accepted by QColor
 *    - \b adaptor (optional)
 *      - \b uid (mandatory): the identifier of the adaptor
 */
class SIGHT_VIZ_QT3D_CLASS_API render final : public viz::render
{
public:

    SIGHT_DECLARE_SERVICE(render, viz::render);

    /// Creates the service.
    SIGHT_VIZ_QT3D_API render();

    /// Destroys the service.
    SIGHT_VIZ_QT3D_API ~render() override;

    /// @returns the scene instantiated by this service.
    SIGHT_VIZ_QT3D_API sight::viz::qt3d::core::generic_scene* get_scene();

protected:

    /// Configures the render service.
    SIGHT_VIZ_QT3D_API void configuring() override;

    /// Creates a rendering context and instantiates a Qt3D generic scene.
    SIGHT_VIZ_QT3D_API void starting() override;

    /// Does nothing.
    SIGHT_VIZ_QT3D_API void updating() override;

    /// Destroys the service.
    SIGHT_VIZ_QT3D_API void stopping() override;

private:

    /// Contains the 3D view.
    QPointer<Qt3DExtras::Qt3DWindow> m_3d_view;

    /// Contains the generic scene associated to this service.
    QPointer<sight::viz::qt3d::core::generic_scene> m_scene;

    /// Specifies the background color.
    QColor m_background_color;
};

} // namespace sight::viz::qt3d.
