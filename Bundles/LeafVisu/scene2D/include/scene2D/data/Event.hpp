/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */




#ifndef _SCENE2D_DATA_EVENT_HPP_
#define _SCENE2D_DATA_EVENT_HPP_

#include <fwData/Object.hpp>

#include "scene2D/config.hpp"
#include "scene2D/data/Coord.hpp"
#include "scene2D/data/Size.hpp"

namespace scene2D
{
namespace data
{

//-----------------------------------------------------------------------------

class SCENE2D_CLASS_API Event : public ::fwTools::Object
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
      MouseWheelDown
    } Type;

    typedef enum
    {   NoButton,
        LeftButton,
        RightButton,
        MidButton
    } Button;

    typedef enum
    {  NoModifier,
       Shift,
       Control,
       AltModifier
    } Modifier;


    fwCoreClassDefinitionsWithFactoryMacro( (Event)(::fwTools::Object), (()), ::fwTools::Factory::New< Event >) ;

    SCENE2D_API Event() throw();
    SCENE2D_API virtual ~Event() throw();

    fwGettersSettersDocMacro(OldSize, oldSize, Size, no comment);
    fwGettersSettersDocMacro(Size, size, Size, no comment);
    fwGettersSettersDocMacro(Type, type, Type, no comment);
    fwGettersSettersDocMacro(Coord, coord, Coord, no comment);
    fwGettersSettersDocMacro(Button, button, Button, no comment);
    fwGettersSettersDocMacro(Accepted, accepted, bool, no comment);
    fwGettersSettersDocMacro(Modifier, modifier, Modifier, no comment);

private:

    Coord m_coord;
    Type m_type;
    Button m_button;
    Modifier m_modifier;
    Size m_size;
    Size m_oldSize;
    bool m_accepted;
};


} // namespace data
} // namespace scene2D


#endif // _SCENE2D_DATA_SCENEEVENT_HPP_

