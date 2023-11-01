/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/config.hpp"

#include <viz/scene3d/parameter_adaptor.hpp>

namespace sight::module::viz::scene3d::adaptor
{

class CompositorListener;

/**
 * @brief This adaptor binds a Sight data to a shader uniform from a specific compositor.
 *
 * @section Slots Slots
 * - \b update_visibility(bool): sets whether the compositor is enabled or not.
 * - \b toggle_visibility(): toggle whether the compositor is enabled or not.
 * - \b show(): enables the compositor.
 * - \b hide(): enables the compositor.
 * - \b set_bool_parameter(bool, std::string): set the uniform from an integer value.
 * - \b set_color_parameter(std::array<std::uint8_t, 4>, std::string): set the uniform from a color value.
 * - \b set_int_parameter(int, std::string): set the uniform from an integer value.
 * - \b set_int2_parameter(int, int, std::string): set the uniform from two integer values.
 * - \b set_int3_parameter(int, int, int, std::string): set the uniform from three integer values.
 * - \b setFloatParameter(float, std::string): set the uniform from an float value.
 * - \b set_double_parameter(double, std::string): set the uniform from an double value.
 * - \b set_double2_parameter(double, double, std::string): set the uniform from two double values.
 * - \b set_double3_parameter(double, double, double, std::string): set the uniform from three double values.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::compositor_parameter">
        <inout key="parameter" uid="collimation" />
        <config compositorName="compositor" parameter="u_value" shaderType="fragment" />
    </service>
   @endcode
 *
 * @subsection InOut InOut:
 * - \b parameter [sight::data::object]: parameter containing the data to upload.
 *
 * @subsection Configuration Configuration:
 * - \b visible (optional, bool, default=true): defines if the compositor is enabled or not.
 * - \b compositorName (mandatory, string): the name of the associated Ogre compositor.
 * - \b parameter (mandatory, string): name of the shader parameter to set.
 * - \b technique (optional, string, default=""): name of the technique, default to the first in the compositor.
 * - \b shaderType (optional, string, default=""): the type of the shader (vertex, geometry, fragment). Default to
 *      fragment.
 */
class MODULE_VIZ_SCENE3D_CLASS_API compositor_parameter final : public sight::viz::scene3d::parameter_adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(compositor_parameter, sight::viz::scene3d::parameter_adaptor);

    /// Creates the adaptor.
    MODULE_VIZ_SCENE3D_API compositor_parameter() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~compositor_parameter() noexcept override = default;

protected:

    /// Configures the adaptor.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Creates the compositor listener.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /// Sets the parameter as dirty.
    MODULE_VIZ_SCENE3D_API void updating() override;

    /// Removes the compositor listener.
    MODULE_VIZ_SCENE3D_API void stopping() override;

    /**
     * @brief Sets the compositor status.
     * @param _enable the status of the compositor.
     */
    MODULE_VIZ_SCENE3D_API void set_visible(bool _enable) override;

private:

    /// Updates parameter according to the attached data::object.
    void update_value(Ogre::MaterialPtr& _mat);

    /// Adds the listener
    void add_listener();

    /// Defines the material name.
    std::string m_compositor_name;

    /// Contains the Ogre compositor listener, we need to keep a pointer to unregister it.
    CompositorListener* m_listener {nullptr};

    /// Handles connection with the layer.
    core::com::helper::sig_slot_connection m_resize_connection;

    friend class CompositorListener;
};

} // namespace sight::module::viz::scene3d::adaptor.
