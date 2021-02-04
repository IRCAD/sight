/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "modules/viz/ogre/config.hpp"

#include <viz/ogre/IParameter.hpp>

namespace sight::modules::viz::ogre::adaptor
{

class CompositorListener;

/**
 * @brief This adaptor binds a Sight data to a shader uniform from a specific compositor.
 *
 * @section Slots Slots
 * - \b updateVisibility(bool): sets whether the compositor is enabled or not.
 * - \b toggleVisibility(): toggle whether the compositor is enabled or not.
 * - \b show(): enables the compositor.
 * - \b hide(): enables the compositor.
 * - \b setBoolParameter(bool, std::string): set the uniform from an integer value.
 * - \b setColorParameter(std::array<std::uint8_t, 4>, std::string): set the uniform from a color value.
 * - \b setIntParameter(int, std::string): set the uniform from an integer value.
 * - \b setInt2Parameter(int, int, std::string): set the uniform from two integer values.
 * - \b setInt3Parameter(int, int, int, std::string): set the uniform from three integer values.
 * - \b setFloatParameter(float, std::string): set the uniform from an float value.
 * - \b setDoubleParameter(double, std::string): set the uniform from an double value.
 * - \b setDouble2Parameter(double, double, std::string): set the uniform from two double values.
 * - \b setDouble3Parameter(double, double, double, std::string): set the uniform from three double values.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::sight::modules::viz::ogre::adaptor::SCompositorParameter">
        <inout key="parameter" uid="collimation" />
        <config layer="..." compositorName="compositor" parameter="u_value" shaderType="fragment" />
    </service>
   @endcode
 *
 * @subsection InOut InOut:
 * - \b parameter [data::Object]: parameter containing the data to upload.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines the compositor's layer.
 * - \b visible (optional, bool, default=true): defines if the compositor is enabled or not.
 * - \b compositorName (mandatory, string): the name of the associated Ogre compositor.
 * - \b parameter (mandatory, string): name of the shader parameter to set.
 * - \b technique (optional, string, default=""): name of the technique, default to the first in the compositor.
 * - \b shaderType (optional, string, default=""): the type of the shader (vertex, geometry, fragment). Default to
 *      fragment.
 */
class MODULE_VIZ_OGRE_CLASS_API SCompositorParameter final : public ::sight::viz::ogre::IParameter
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SCompositorParameter, ::sight::viz::ogre::IParameter)

    /// Creates the adaptor.
    MODULE_VIZ_OGRE_API SCompositorParameter() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_OGRE_API ~SCompositorParameter() noexcept override;

protected:

    /// Configures the adaptor.
    MODULE_VIZ_OGRE_API void configuring() override;

    /// Creates the compositor listener.
    MODULE_VIZ_OGRE_API void starting() override;

    /// Sets the parameter as dirty.
    MODULE_VIZ_OGRE_API void updating() override;

    /// Removes the compositor listener.
    MODULE_VIZ_OGRE_API void stopping() override;

    /**
     * @brief Sets the compositor status.
     * @param _enable the status of the compositor.
     */
    MODULE_VIZ_OGRE_API void setVisible(bool _enable) override;

private:

    /// Updates parameter according to the attached data::Object.
    void updateValue(::Ogre::MaterialPtr& _mat);

    /// Defines the material name.
    std::string m_compositorName;

    /// Contains the Ogre compositor listener, we need to keep a pointer to unregister it.
    CompositorListener* m_listener;

    /// Contains the compositor we work on.
    ::Ogre::CompositorInstance* m_compositor;

    friend class CompositorListener;

};

} // namespace sight::modules::viz::ogre::adaptor.
