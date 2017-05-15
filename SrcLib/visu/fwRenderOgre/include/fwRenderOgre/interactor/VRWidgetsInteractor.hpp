/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_INTERACTOR_VRWIDGETSINTERACTOR_HPP__
#define __FWRENDEROGRE_INTERACTOR_VRWIDGETSINTERACTOR_HPP__

#include "fwRenderOgre/interactor/TrackballInteractor.hpp"
#include "fwRenderOgre/picker/IPicker.hpp"
#include "fwRenderOgre/ui/VRWidget.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <OGRE/OgreMovableObject.h>

namespace fwRenderOgre
{

namespace interactor
{

/**
 * @brief Picks VR widgets and updates clipping cube.
 */
class FWRENDEROGRE_CLASS_API VRWidgetsInteractor : public ::fwRenderOgre::interactor::TrackballInteractor
{
public:

    /// Constructor.
    FWRENDEROGRE_API VRWidgetsInteractor() throw();

    /// Destructor.
    FWRENDEROGRE_API virtual ~VRWidgetsInteractor() throw();

    /// Rotate the camera (see TrackballInteractor) or displace widgets if in drag mode.
    FWRENDEROGRE_API virtual void mouseMoveEvent(MouseButton button, int x, int y, int dx, int dy);

    /// Used to signal the end of drag mode.
    FWRENDEROGRE_API virtual void buttonReleaseEvent(MouseButton button, int x, int y);

    /// Starts object picking.
    FWRENDEROGRE_API virtual void buttonPressEvent(MouseButton button, int x, int y);

    /// Attaches a widget.
    FWRENDEROGRE_API inline void attachWidget(ui::VRWidget::sptr widget);

    /// Detaches a widget.
    FWRENDEROGRE_API inline void detachWidget(ui::VRWidget::sptr widget);

    /// Initializes the picker.
    FWRENDEROGRE_API inline void initPicker();

private:

    /// Currently selected widget.
    ::Ogre::MovableObject *m_pickedObject;

    /// All widgets with whom we interact.
    ui::VRWidget::sptr m_widget;

    /// The picker used by this interactor.
    fwRenderOgre::picker::IPicker m_picker;

    /// Picks the object at screen coordinates (x, y)
    Ogre::MovableObject* pickObject(int x, int y);

};

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

//------------------------------------------------------------------------------

} // namespace interactor
} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_INTERACTOR_VRWIDGETSINTERACTOR_HPP__
