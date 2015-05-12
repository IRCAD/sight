/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_DATA_EVENT_HPP__
#define __SCENE2D_DATA_EVENT_HPP__

#include <Qt>

#include "scene2D/config.hpp"
#include "scene2D/data/Coord.hpp"
#include "scene2D/data/Size.hpp"

namespace scene2D
{
namespace data
{

//-----------------------------------------------------------------------------

/**
 * @brief This class manage events on the scene 2D (mouse event, keyboard event ,
 * ...).
 */
class SCENE2D_CLASS_API Event : public ::fwData::Object
{

public:

    typedef enum
    { NoType,
      Resize,                   // change within the size of the widget/view
      Viewport,                 // change within the size of the sene (contained into the widget/view)
      MouseButtonDoubleClick,
      MouseButtonPress,
      MouseButtonRelease,
      MouseMove,
      MouseWheelUp,
      MouseWheelDown,
      KeyPress,
      KeyRelease} Type;

    typedef enum
    {   NoButton,
        LeftButton,
        RightButton,
        MidButton} Button;

    typedef enum
    {  NoModifier,
       ShiftModifier,
       ControlModifier,
       AltModifier} Modifier;


    fwCoreClassDefinitionsWithFactoryMacro( (Event)(::fwData::Object), (()), ::fwData::factory::New< Event >);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    SCENE2D_API Event( ::fwData::Object::Key key );
    SCENE2D_API virtual ~Event();

    /// Defines shallow copy
    SCENE2D_API void shallowCopy( const ::fwData::Object::csptr& _source );

    /// Defines deep copy
    SCENE2D_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType &cache );

    fwGettersSettersDocMacro(OldSize, oldSize, Size, no comment);
    fwGettersSettersDocMacro(Size, size, Size, no comment);
    fwGettersSettersDocMacro(Type, type, Type, no comment);
    fwGettersSettersDocMacro(Coord, coord, Coord, no comment);
    fwGettersSettersDocMacro(Button, button, Button, no comment);
    fwGettersSettersDocMacro(Accepted, accepted, bool, no comment);
    fwGettersSettersDocMacro(Modifier, modifier, Modifier, no comment);
    fwGettersSettersDocMacro(Key, key, int, no comment);

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


} // namespace data
} // namespace scene2D


#endif // __SCENE2D_DATA_EVENT_HPP__

