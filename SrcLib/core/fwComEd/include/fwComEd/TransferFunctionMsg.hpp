/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_TRANSFERFUNCTIONMSG_HPP_
#define _FWCOMED_TRANSFERFUNCTIONMSG_HPP_

#include <fwData/Object.hpp>
#include <fwData/Integer.hpp>

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for transfer function : store modification information
 * @class   TransferFunctionMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API TransferFunctionMsg : public ::fwServices::ObjectMsg
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((TransferFunctionMsg)(::fwServices::ObjectMsg), ( () ), new TransferFunctionMsg );

    /**
     * @name Event identifier
     */
    /// @{
    /// Event identifier used to inform for modification
    FWCOMED_API static std::string MODIFIED_POINTS;
    FWCOMED_API static std::string WINDOWING;
    /// @}

    /**
     * @brief Constructor : does nothing.
     */
    FWCOMED_API TransferFunctionMsg() throw();

    /**
     * @brief   Destuctor : does nothing.
     */
    FWCOMED_API virtual ~TransferFunctionMsg() throw();

    /**
     * @brief Add a WINDOWING even with window and level.
     * @param[in] level : level of the image tf
     * @param[in] window : window of the image tf
     */
    FWCOMED_API void setWindowLevel( double window, double level );

    /**
     * @brief Get the window and level .
     * @pre   The message must have a WINDOWING even
     * @param[out] : the window of the image tf
     */
    FWCOMED_API double getWindow() const;
    /**
     * @brief Get the level .
     * @pre   The message must have a WINDOWING even
     * @param[out] : the level of the image tf
     */
    FWCOMED_API double getLevel() const;

protected:

    /// Windowing min
    double m_level;
    /// Windowing max
    double m_window;

};

} // namespace fwComEd

#endif //_FWCOMED_TRANSFERFUNCTIONMSG_HPP_

