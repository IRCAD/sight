/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

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
    FWRENDEROGRE_API VRWidgetsInteractor() noexcept;

    /// Destructor.
    FWRENDEROGRE_API virtual ~VRWidgetsInteractor() noexcept;

    /// Behaviour on mouseMoveEvent
    FWRENDEROGRE_API virtual void mouseMoveEvent(MouseButton, int, int, int, int) override;

    /// Behaviour on a wheelEvent
    FWRENDEROGRE_API virtual void wheelEvent(int, int, int) override;

    /// Behaviour on button release.
    FWRENDEROGRE_API virtual void buttonReleaseEvent(MouseButton, int, int) override;

    /// Behaviour on button press.
    FWRENDEROGRE_API virtual void buttonPressEvent(MouseButton, int, int) override;

    /// Called when the window is resized
    FWRENDEROGRE_API virtual void resizeEvent(int, int) override;

    /// Called when a key is press
    FWRENDEROGRE_API virtual void keyPressEvent(int) override;

    /// Called when a key is release
    FWRENDEROGRE_API virtual void keyReleaseEvent(int) override;

    /// Called when the focus is win
    FWRENDEROGRE_API virtual void focusInEvent() override;

    /// Called when the focus is lost
    FWRENDEROGRE_API virtual void focusOutEvent() override;

    /// Attaches a widget
    FWRENDEROGRE_API void attachWidget(ui::VRWidget::sptr widget);

    /// Detaches a widget.
    FWRENDEROGRE_API void detachWidget(ui::VRWidget::sptr widget);

    /// Initializes the picker.
    FWRENDEROGRE_API void initPicker();

private:

    /// Currently selected widget.
    ::Ogre::MovableObject* m_pickedObject;

    /// All widgets with whom we interact.
    ui::VRWidget::sptr m_widget;

    /// The picker used by this interactor.
    fwRenderOgre::picker::IPicker m_picker;

    /// Picks the object at screen coordinates (x, y)
    Ogre::MovableObject* pickObject(int x, int y);

};
} // namespace interactor
} // namespace fwRenderOgre
