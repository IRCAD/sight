/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUOGRE_HELPER_UTILS_HPP__
#define __UIVISUOGRE_HELPER_UTILS_HPP__

#include "uiVisuOgre/config.hpp"

#include <OGRE/OgreColourValue.h>

#include <QColor>

namespace uiVisuOgre
{

namespace helper
{

class Utils
{
public:

    /**
     * @brief converOgreColorToQColor
     * @param _ogreColor the Ogre color to convert.
     * @return the converted QColor.
     */
    UIVISUOGRE_API static const QColor converOgreColorToQColor(const ::Ogre::ColourValue& _ogreColor);

    /**
     * @brief convertQColorToOgreColor
     * @param _qColor the Qt color to convert.
     * @return the converted Ogre color.
     */
    UIVISUOGRE_API static ::Ogre::ColourValue convertQColorToOgreColor(const QColor& _qColor);
};

} // namespace helper

} // namespace uiVisuOgre

#endif // __UIVISUOGRE_HELPER_UTILS_HPP__
