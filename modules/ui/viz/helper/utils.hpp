/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2017 IHU Strasbourg
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

#include <OGRE/OgreColourValue.h>

#include <QColor>

namespace sight::module::ui::viz::helper
{

class utils
{
public:

    /**
     * @brief convertOgreColorToQColor
     * @param _ogre_color the Ogre color to convert.
     * @return the converted QColor.
     */
    static constexpr QColor convert_ogre_color_to_q_color(const Ogre::ColourValue& _ogre_color)
    {
        const int r = static_cast<int>(_ogre_color.r * 255);
        const int g = static_cast<int>(_ogre_color.g * 255);
        const int b = static_cast<int>(_ogre_color.b * 255);
        const int a = static_cast<int>(_ogre_color.a * 255);

        return {r, g, b, a};
    }

    /**
     * @brief convertQColorToOgreColor
     * @param _q_color the Qt color to convert.
     * @return the converted Ogre color.
     */
    static inline Ogre::ColourValue convert_q_color_to_ogre_color(const QColor& _q_color)
    {
        const float r = static_cast<float>(_q_color.red()) / 255.F;
        const float g = static_cast<float>(_q_color.green()) / 255.F;
        const float b = static_cast<float>(_q_color.blue()) / 255.F;
        const float a = static_cast<float>(_q_color.alpha()) / 255.F;

        return Ogre::ColourValue(r, g, b, a);
    }
};

} // namespace sight::module::ui::viz::helper
