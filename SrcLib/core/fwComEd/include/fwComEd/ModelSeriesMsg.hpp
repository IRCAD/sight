/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_MODELSERIESMSG_HPP__
#define __FWCOMED_MODELSERIESMSG_HPP__

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for model series : store modification information
 * @class   ModelSeriesMsg
 * @date    2013
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API ModelSeriesMsg : public ::fwServices::ObjectMsg
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((ModelSeriesMsg)(::fwServices::ObjectMsg), ( () ),
                                           ::fwServices::factory::message::New< ModelSeriesMsg > );

    /**
     * @name Event identifier
     */
    /// @{
    /// Event identifier used to inform for modification
    FWCOMED_API static std::string ADD_RECONSTRUCTION;
    FWCOMED_API static std::string REMOVED_RECONSTRUCTIONS;
    FWCOMED_API static std::string SHOW_RECONSTRUCTIONS;
    FWCOMED_API static std::string VISIBILITY;
    FWCOMED_API static std::string NEW_RECONSTRUCTION_SELECTED;
    /// @}

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWCOMED_API ModelSeriesMsg(::fwServices::ObjectMsg::Key key);


    /**
     * @brief   Destuctor : do nothing.
     */
    FWCOMED_API virtual ~ModelSeriesMsg() throw();

};

} // namespace fwComEd

#endif //__FWCOMED_MODELSERIESMSG_HPP__

