/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/interactor/VRWidgetsInteractor.hpp"

#include <fwRenderOgre/Layer.hpp>
#include <fwRenderOgre/registry/macros.hpp>

#include <fwCom/Signal.hxx>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreViewport.h>

fwRenderOgreRegisterInteractorMacro( ::fwRenderOgre::interactor::VRWidgetsInteractor );

namespace fwRenderOgre
{

namespace interactor
{

//------------------------------------------------------------------------------

VRWidgetsInteractor::VRWidgetsInteractor() noexcept :
    m_pickedObject(nullptr),
    m_widget(nullptr)
{
}

//------------------------------------------------------------------------------

VRWidgetsInteractor::~VRWidgetsInteractor() noexcept
{
}

//------------------------------------------------------------------------------

Ogre::MovableObject* VRWidgetsInteractor::pickObject(int x, int y)
{
    ::Ogre::Camera* camera = m_sceneManager->getCamera(::fwRenderOgre::Layer::DEFAULT_CAMERA_NAME);

    int height = camera->getViewport()->getActualHeight();
    int width  = camera->getViewport()->getActualWidth();

    bool pickSuccess = m_picker.executeRaySceneQuery( x, y, width, height, 0 );

    return pickSuccess ? m_picker.getSelectedObject() : nullptr;
}

//------------------------------------------------------------------------------

void VRWidgetsInteractor::mouseMoveEvent(MouseButton button, int x, int y, int dx, int dy)
{
    if(button == LEFT)
    {
        if(!m_widget->getVisibility() || m_pickedObject == nullptr)
        {
            TrackballInteractor::mouseMoveEvent(button, x, y, dx, dy);
        }
        else
        {
            m_widget->widgetPicked(m_pickedObject, x, y);
        }
    }
    else if(button == MIDDLE)
    {
        m_widget->moveClippingBox(x, y, -dx, -dy);
    }
    else if(button == RIGHT)
    {
        m_widget->scaleClippingBox(x, y, dy);
    }
}

//------------------------------------------------------------------------------

void VRWidgetsInteractor::buttonReleaseEvent(MouseButton /*button*/, int /*x*/, int /*y*/)
{
    m_widget->widgetReleased();
    m_pickedObject = nullptr;
}

//------------------------------------------------------------------------------

void VRWidgetsInteractor::buttonPressEvent(MouseButton button, int x, int y)
{
    if(button == LEFT)
    {
        m_pickedObject = pickObject(x, y);
        m_widget->widgetPicked(m_pickedObject, x, y);
    }
    else if(button == MIDDLE)
    {
        m_widget->moveClippingBox(x, y, 0, 0);
    }
    else if(button == RIGHT)
    {
        m_widget->scaleClippingBox(x, y, 0);
    }
}

//------------------------------------------------------------------------------

void VRWidgetsInteractor::attachWidget(ui::VRWidget::sptr widget)
{
    OSLM_ASSERT("Only one widget can be attached to a VR interactor", !m_widget);
    m_widget = widget;
}

//------------------------------------------------------------------------------

void VRWidgetsInteractor::detachWidget(ui::VRWidget::sptr widget)
{
    if(m_widget == widget)
    {
        m_widget = nullptr;
    }
}

//------------------------------------------------------------------------------

void VRWidgetsInteractor::initPicker()
{
    m_picker.setSceneManager(this->m_sceneManager);
}

} // namespace interactor
} // namespace fwRenderOgre
