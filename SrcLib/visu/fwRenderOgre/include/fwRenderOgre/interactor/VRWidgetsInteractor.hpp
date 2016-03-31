/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VRWIDGETSINTERACTOR_HPP__
#define __VRWIDGETSINTERACTOR_HPP__

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include "fwRenderOgre/interactor/TrackballInteractor.hpp"
#include "fwRenderOgre/picker/IPicker.hpp"

#include <OGRE/OgreMovableObject.h>

namespace fwRenderOgre
{

namespace interactor
{

//------------------------------------------------------------------------------

class FWRENDEROGRE_CLASS_API VRWidgetsInteractor : public ::fwRenderOgre::interactor::TrackballInteractor
{
public:

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal< void ( ::Ogre::MovableObject * ,int ,int ) > WidgetDragSigType;
    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_DRAG_WIDGET_SIG;

    typedef ::fwCom::Signal< void () > WidgetDropSigType;
    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_DROP_WIDGET_SIG;
    /** @} */

    /// Constructor.
    FWRENDEROGRE_API VRWidgetsInteractor() throw();

    /// Destructor.
    FWRENDEROGRE_API virtual ~VRWidgetsInteractor() throw();

    /// Rotate the camera (see TrackballInteractor) or displace widgets if in drag mode.
    FWRENDEROGRE_API virtual void mouseMoveEvent(int x, int y, int dx, int dy, bool click);

    /// Used to signal the end of drag mode.
    FWRENDEROGRE_API virtual void buttonReleaseEvent();

    /// Attaches signal used to emit dragging tasks.
    FWRENDEROGRE_API inline void attachSignal(WidgetDragSigType::sptr _widgetDraggedSignal);

    /// Attaches signal used to end dragging tasks.
    FWRENDEROGRE_API inline void attachSignal(WidgetDropSigType::sptr _mouseReleaseSignal);

    /// Initializes the picker.
    FWRENDEROGRE_API inline void initPicker();

private:

    /// Currently selected widget.
    ::Ogre::MovableObject *m_selectedWidget;

    /// Signal emitted when a widget is being dragged.
    WidgetDragSigType::sptr m_widgetDragSignal;

    /// Signal emitted when drag ends.
    WidgetDropSigType::sptr m_widgetDropSignal;

    /// The picker used by this interactor.
    fwRenderOgre::picker::IPicker m_picker;

    /// Picks the object at screen coordinates (x, y)
    Ogre::MovableObject * pickObject(int x, int y);

};

//------------------------------------------------------------------------------

void VRWidgetsInteractor::attachSignal(WidgetDragSigType::sptr _widgetDraggedSignal)
{
    m_widgetDragSignal = _widgetDraggedSignal;
}

//------------------------------------------------------------------------------

void VRWidgetsInteractor::attachSignal(WidgetDropSigType::sptr _mouseReleaseSignal)
{
    m_widgetDropSignal = _mouseReleaseSignal;
}

//------------------------------------------------------------------------------

void VRWidgetsInteractor::initPicker()
{
    m_picker.setSceneManager(this->m_sceneManager);
}

//------------------------------------------------------------------------------

} // namespace interactor
} // namespace fwRenderOgre

#endif // __VRWIDGETSINTERACTOR_HPP__
