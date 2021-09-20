/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "modules/ui/viz/config.hpp"

#include <OGRE/OgreColourValue.h>

#include <QColor>

namespace sight::module::ui::viz
{

namespace helper
{

class MODULE_UI_VIZ_CLASS_API Utils
{
public:

    /**
     * @brief convertOgreColorToQColor
     * @param _ogreColor the Ogre color to convert.
     * @return the converted QColor.
     */
    MODULE_UI_VIZ_API static QColor convertOgreColorToQColor(const Ogre::ColourValue& _ogreColor);

    /**
     * @brief convertQColorToOgreColor
     * @param _qColor the Qt color to convert.
     * @return the converted Ogre color.
     */
    MODULE_UI_VIZ_API static Ogre::ColourValue convertQColorToOgreColor(const QColor& _qColor);
};

} // namespace helper

} // namespace sight::module::ui::viz
