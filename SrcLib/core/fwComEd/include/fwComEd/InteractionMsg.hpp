/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_INTERACTIONMSG_HPP_
#define _FWCOMED_INTERACTIONMSG_HPP_

#include <fwData/Object.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Point.hpp>

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for interaction : store modification information
 * @class   InteractionMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API InteractionMsg : public ::fwServices::ObjectMsg
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((InteractionMsg)(::fwServices::ObjectMsg), ( () ), new InteractionMsg );

    typedef ::fwData::Point::PointCoordType PointCoordType;

    //typedef enum
    //{
        //KEY_UP    ,
        //KEY_DOWN  ,
        //MOUSE_UP  ,
        //MOUSE_DOWN,
        //MOUSE_MOVE
    //} EventType;

    //typedef enum
    //{
        //LEFT           = (1 << 0),
        //MIDDLE         = (1 << 1),
        //RIGHT          = (1 << 2),
        //WHEEL_FORWARD  = (1 << 3),
        //WHEEL_BACKWARD = (1 << 4),
    //} MouseButtons;

    typedef enum
    {
        NONE      = 0,
        CTRL      = (1 << 0),
        SHIFT     = (1 << 1),
        //ALT     = (1 << 2),
        //SUPER     = (1 << 3),
        //APPLE_CMD = (1 << 4),
    } Modifiers;

    /**
     * @name Event identifier
     */
    /// @{
    /// Event identifier used to inform for modification
    FWCOMED_API static std::string MOUSE_LEFT_UP;
    FWCOMED_API static std::string MOUSE_RIGHT_UP;
    FWCOMED_API static std::string MOUSE_MIDDLE_UP;
    FWCOMED_API static std::string MOUSE_WHEELFORWARD_UP;
    FWCOMED_API static std::string MOUSE_WHEELBACKWARD_UP;

    FWCOMED_API static std::string MOUSE_LEFT_DOWN;
    FWCOMED_API static std::string MOUSE_RIGHT_DOWN;
    FWCOMED_API static std::string MOUSE_MIDDLE_DOWN;
    FWCOMED_API static std::string MOUSE_WHEELFORWARD_DOWN;
    FWCOMED_API static std::string MOUSE_WHEELBACKWARD_DOWN;

    FWCOMED_API static std::string MOUSE_MOVE;

    //FWCOMED_API static std::string KEY_UP;
    //FWCOMED_API static std::string KEY_DOWN;

    /// @}

    /**
     * @brief Constructor : does nothing.
     */
    FWCOMED_API InteractionMsg() throw();

    /**
     * @brief   Destuctor : does nothing.
     */
    FWCOMED_API virtual ~InteractionMsg() throw();

    /// Set the state of the modifier k.
    FWCOMED_API void setModifiersStatus(Modifiers k, bool state = true);
    /// Get the state of the modifier k.
    FWCOMED_API bool getModifiersStatus(Modifiers k) const;

    /// Get the control status.
    FWCOMED_API bool getControlStatus() const {return getModifiersStatus(CTRL);}
    /// Get the shift status.
    FWCOMED_API bool getShiftStatus() const {return getModifiersStatus(SHIFT);}

    /**
     * @brief Set the message event
     * @note  An InteractionMsg can only handle one event.
     */
    FWCOMED_API void setEvent(std::string event);

    /// Set the point coordinates of the event.
    FWCOMED_API void setEventPoint(::fwData::Point::csptr point);
    /// Set the point coordinates of the event.
    FWCOMED_API void setEventPoint(PointCoordType x, PointCoordType y, PointCoordType z);

    /// Return the point coordinates of the event.
    FWCOMED_API ::fwData::Point::csptr getEventPoint() const;

protected:

    /// Event point coordinates
    ::fwData::Point::sptr m_eventPoint;

    /// Modifiers status
    unsigned char m_modifiersStatus;


};

} // namespace fwComEd

#endif //_FWCOMED_INTERACTIONMSG_HPP_

