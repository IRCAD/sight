/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_IMAGEMSG_HPP_
#define _FWCOMED_IMAGEMSG_HPP_

#include <fwData/Object.hpp>
#include <fwData/Integer.hpp>

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for image : store modification information
 * @class   ImageMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API ImageMsg : public ::fwServices::ObjectMsg
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((ImageMsg)(::fwServices::ObjectMsg), ( () ), new ImageMsg );

    /**
     * @name Event identifier
     */
    /// @{
    /// Event identifier used to inform for modification
    FWCOMED_API static std::string NEW_IMAGE;
    FWCOMED_API static std::string BUFFER;
    FWCOMED_API static std::string MODIFIED;
    FWCOMED_API static std::string DIMENSION;
    FWCOMED_API static std::string SPACING;
    FWCOMED_API static std::string REGION;
    FWCOMED_API static std::string PIXELTYPE;
    FWCOMED_API static std::string TRANSFERTFUNCTION;
    FWCOMED_API static std::string WINDOWING;
    FWCOMED_API static std::string LANDMARK;
    FWCOMED_API static std::string NEW_DISTANCE;
    FWCOMED_API static std::string DISTANCE;
    FWCOMED_API static std::string DELETE_DISTANCE;
    FWCOMED_API static std::string SLICE_INDEX;
    FWCOMED_API static std::string ACTIVATE_SYNC_CROSS;
    FWCOMED_API static std::string INACTIVATE_SYNC_CROSS;
    FWCOMED_API static std::string CHANGE_SLICE_TYPE;
    /// Event identifier used to inform that the value is modified.
    FWCOMED_API static std::string VALUE_IS_MODIFIED;
    FWCOMED_API static std::string TRANSPARENCY;
    FWCOMED_API static std::string VISIBILITY;
    /// @}

    /**
     * @brief Constructor : does nothing.
     */
    FWCOMED_API ImageMsg() throw();

    /**
     * @brief   Destuctor : does nothing.
     */
    FWCOMED_API virtual ~ImageMsg() throw();

    /**
     * @brief Add a SLICE_INDEX even with the slice index position.
     * @param[in] a : axial position
     * @param[in] f : frontal position
     * @param[in] s : sagittal position
     * @param[in] _pDataInfo : message data info (optional)
     */
    FWCOMED_API void setSliceIndex(::fwData::Integer::sptr a, ::fwData::Integer::sptr f, ::fwData::Integer::sptr s, ::fwData::Object::sptr _pDataInfo = ::fwData::Object::sptr() );

    /**
     * @brief Add a WINDOWING even with the min and max.
     * @param[in] min : window min
     * @param[in] max : window max
     * @param[in] _pDataInfo : message data info (optional)
     */
    FWCOMED_API void setWindowMinMax(::fwData::Integer::sptr min, ::fwData::Integer::sptr max, ::fwData::Object::sptr _pDataInfo = ::fwData::Object::sptr() );

    /**
     * @brief Get the slice index position.
     * @pre   The message must have a SLICE_INDEX even
     * @param[out] a : axial position
     * @param[out] f : frontal position
     * @param[out] s : sagittal position
     */
    FWCOMED_API void getSliceIndex(::fwData::Integer::sptr a, ::fwData::Integer::sptr f, ::fwData::Integer::sptr s ) const;

    /**
     * @brief Get the window min and max.
     * @pre   The message must have a WINDOWING even
     * @param[out] min : window min
     * @param[out] max : window max
     */
    FWCOMED_API void getWindowMinMax(::fwData::Integer::sptr min, ::fwData::Integer::sptr max ) const;

protected:
    /// Axial slice index
    ::fwData::Integer::sptr m_axialIndex;
    /// Frontal slice index
    ::fwData::Integer::sptr m_frontalIndex;
    /// Sagittal slice index
    ::fwData::Integer::sptr m_sagittalIndex;

    /// Windowing min
    ::fwData::Integer::sptr m_windowMin;
    /// Windowing max
    ::fwData::Integer::sptr m_windowMax;


};

} // namespace fwComEd

#endif //_FWCOMED_IMAGEMSG_HPP_

