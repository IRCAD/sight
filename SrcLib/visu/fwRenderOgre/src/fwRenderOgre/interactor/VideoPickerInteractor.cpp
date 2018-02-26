/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/interactor/VideoPickerInteractor.hpp>
#include <fwRenderOgre/registry/macros.hpp>

#include <fwCom/Signal.hxx>

fwRenderOgreRegisterInteractorMacro( ::fwRenderOgre::interactor::VideoPickerInteractor );

namespace fwRenderOgre
{

namespace interactor
{

VideoPickerInteractor::VideoPickerInteractor() noexcept
{
}

//------------------------------------------------------------------------------

VideoPickerInteractor::~VideoPickerInteractor() noexcept
{
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::resizeEvent(int x, int y)
{
    m_width  = x;
    m_height = y;
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::buttonPressEvent(MouseButton button, int x, int y)
{
    if(m_picker->hasSceneManager())
    {
        if( m_picker->executeRaySceneQuery(x, y, m_width, m_height, 0) )
        {
            ::Ogre::Vector3 click = m_picker->getIntersectionInWorldSpace();

            ::fwData::Point::sptr point                = fwData::Point::New();
            ::fwData::Point::PointCoordArrayType cords =
            {{static_cast<double>(click.x), static_cast<double>(click.y), static_cast<double>(click.z)}};
            point->setCoord(cords);

            m_sigAddPoint->asyncEmit(::fwData::Object::dynamicCast(point));
        }
    }
    else
    {
        SLM_WARN("The picker scene hasn't been initialized, you are not using this interactor correctly");
    }
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::mouseMoveEvent(MouseButton, int, int, int, int)
{
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::wheelEvent(int, int, int)
{
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::buttonReleaseEvent(MouseButton, int, int)
{
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::keyPressEvent(int)
{
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::keyReleaseEvent(int)
{
}

//------------------------------------------------------------------------------

} //namespace interactor
} //namespace fwRenderOgre
