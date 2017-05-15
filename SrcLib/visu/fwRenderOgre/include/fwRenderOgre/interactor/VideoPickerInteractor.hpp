/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_INTERACTOR_VIDEOPICKERINTERACTOR_HPP__
#define __FWRENDEROGRE_INTERACTOR_VIDEOPICKERINTERACTOR_HPP__

#include <fwRenderOgre/config.hpp>
#include <fwRenderOgre/interactor/IPickerInteractor.hpp>
#include <fwRenderOgre/picker/IPicker.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Point.hpp>

#include <OGRE/OgreSceneManager.h>

namespace fwRenderOgre
{

namespace interactor
{

/**
 * @brief Implementation for selection of points on Video
 */
class FWRENDEROGRE_CLASS_API VideoPickerInteractor : public ::fwRenderOgre::interactor::IPickerInteractor
{

public:

    /// Constructor.
    FWRENDEROGRE_API VideoPickerInteractor() throw();

    /// Destructor. Does nothing
    FWRENDEROGRE_API ~VideoPickerInteractor() throw();

protected:

    /**
     * @brief Mouse's left button pressed.
     *        The picker will do ray cast command.
     * @param x The mouse's X displacement
     * @param y The mouse's Y displacement
     * @param width the render window width
     * @param height the render window height
     */
    FWRENDEROGRE_API void mouseClickEvent(int x, int y, int width, int height);
};
} //namespace itneractor
} //namespace fwRenderOgre

#endif // __FWRENDEROGRE_INTERACTOR_VIDEOPICKERINTERACTOR_HPP__
