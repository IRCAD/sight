/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERQT_DATA_EVENT_HPP__
#define __FWRENDERQT_DATA_EVENT_HPP__

#include "fwRenderQt/data/Coord.hpp"
#include "fwRenderQt/data/Size.hpp"

#include <Qt>

namespace fwRenderQt
{
namespace data
{

//-----------------------------------------------------------------------------

/**
 * @brief This class manage events on the scene 2D (mouse event, keyboard event ,
 * ...).
 */
class Event
{

public:

    typedef enum
    {
        NoType,
        Resize,                   // change within the size of the widget/view
        Viewport,                 // change within the size of the sene (contained into the widget/view)
        MouseButtonDoubleClick,
        MouseButtonPress,
        MouseButtonRelease,
        MouseMove,
        MouseWheelUp,
        MouseWheelDown,
        KeyPress,
        KeyRelease
    } Type;

    typedef enum
    {
        NoButton,
        LeftButton,
        RightButton,
        MidButton
    } Button;

    typedef enum
    {
        NoModifier,
        ShiftModifier,
        ControlModifier,
        AltModifier
    } Modifier;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Event() :
        m_type(NoType),
        m_button(NoButton),
        m_modifier(NoModifier),
        m_accepted(false),
        m_key(Qt::Key_unknown)
    {
    }

    bool isAccepted() const;
    void setAccepted(bool accepted);

    Button getButton() const;
    void setButton(Button button);

    const Coord& getCoord() const;
    void setCoord(const Coord& coord);

    int getKey() const;
    void setKey(int key);

    Modifier getModifier() const;
    void setModifier(Modifier modifier);

    const Size& getOldSize() const;
    void setOldSize(const Size& oldSize);

    const Size& getSize() const;
    void setSize(const Size& size);

    Type getType() const;
    void setType(Type type);

private:

    Coord m_coord;
    Type m_type;
    Button m_button;
    Modifier m_modifier;
    Size m_size;
    Size m_oldSize;
    bool m_accepted;
    int m_key;
};

//------------------------------------------------------------------------------

inline bool Event::isAccepted() const
{
    return m_accepted;
}

//------------------------------------------------------------------------------

inline void Event::setAccepted(bool accepted)
{
    m_accepted = accepted;
}

//------------------------------------------------------------------------------

inline Event::Button Event::getButton() const
{
    return m_button;
}

//------------------------------------------------------------------------------

inline void Event::setButton(Button button)
{
    m_button = button;
}

//------------------------------------------------------------------------------

inline const Coord& Event::getCoord() const
{
    return m_coord;
}

//------------------------------------------------------------------------------

inline void Event::setCoord(const Coord& coord)
{
    m_coord = coord;
}

//------------------------------------------------------------------------------

inline int Event::getKey() const
{
    return m_key;
}

//------------------------------------------------------------------------------

inline void Event::setKey(int key)
{
    m_key = key;
}

//------------------------------------------------------------------------------

inline Event::Modifier Event::getModifier() const
{
    return m_modifier;
}

//------------------------------------------------------------------------------

inline void Event::setModifier(Modifier modifier)
{
    m_modifier = modifier;
}

//------------------------------------------------------------------------------

inline const Size& Event::getOldSize() const
{
    return m_oldSize;
}

//------------------------------------------------------------------------------

inline void Event::setOldSize(const Size& oldSize)
{
    m_oldSize = oldSize;
}

//------------------------------------------------------------------------------

inline const Size& Event::getSize() const
{
    return m_size;
}

//------------------------------------------------------------------------------

inline void Event::setSize(const Size& size)
{
    m_size = size;
}

//------------------------------------------------------------------------------

inline Event::Type Event::getType() const
{
    return m_type;
}

//------------------------------------------------------------------------------

inline void Event::setType(Type type)
{
    m_type = type;
}

} // namespace data
} // namespace fwRenderQt

#endif // __FWRENDERQT_DATA_EVENT_HPP__

