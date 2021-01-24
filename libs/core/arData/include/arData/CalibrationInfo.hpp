/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "arData/config.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>
#include <core/macros.hpp>

#include <data/Image.hpp>
#include <data/Object.hpp>
#include <data/PointList.hpp>

#include <list>

fwCampAutoDeclareDataMacro((arData)(CalibrationInfo));

namespace arData
{
/**
 * @brief class for handle calibration information , this class handle two lists the first one contains images
 * and the second one contains pointList
 *
 */
class ARDATA_CLASS_API CalibrationInfo : public data::Object
{
public:

    fwCoreClassMacro(CalibrationInfo, data::Object, data::factory::New< CalibrationInfo >)

    fwCampMakeFriendDataMacro((arData)(CalibrationInfo));

    ///typedefs
    typedef std::list< data::Image::sptr > ImageContainerType;
    typedef std::list< data::PointList::sptr > PointListContainerType;

    /**
     * @name Signals API
     * @{
     */
    ARDATA_API static const core::com::Signals::SignalKeyType s_ADDED_RECORD_SIG;
    typedef core::com::Signal<void ()> AddedRecordSignalType;

    ARDATA_API static const core::com::Signals::SignalKeyType s_REMOVED_RECORD_SIG;
    typedef core::com::Signal<void ()> RemovedRecordSignalType;

    ARDATA_API static const core::com::Signals::SignalKeyType s_RESET_RECORD_SIG;
    typedef core::com::Signal<void ()> ResetRecordSignalType;

    ARDATA_API static const core::com::Signals::SignalKeyType s_GET_RECORD_SIG;
    typedef core::com::Signal<void (size_t)> GetRecordSignalType;

    /**
     * @}
     */

    ///Constructor
    ARDATA_API CalibrationInfo(data::Object::Key key);
    ///Destructor
    ARDATA_API ~CalibrationInfo();

    /// Defines shallow copy
    ARDATA_API void shallowCopy(const data::Object::csptr& _source ) override;

    /// Defines deep copy
    ARDATA_API void cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**
     * @brief add an Image and a pointList into CalibrationInfo
     * @param[in] img the new image
     * @param[in] pl the corresponding point list
     */
    ARDATA_API void addRecord( const data::Image::sptr& img, const data::PointList::sptr& pl );

    /**
     * @brief remove the desired record (both image and pointList)
     * @param[in] idx index of the record to be removed
     */
    ARDATA_API void removeRecord(size_t idx);

    /// Reset all records
    ARDATA_API void resetRecords();

    ///Get all the images
    ARDATA_API ImageContainerType getImageContainer() const;

    ///Get all the pointLists
    ARDATA_API PointListContainerType getPointListContainer() const;

    /**
     * @brief Get the pointList corresponding to the specified image, if the specified image is not found
     * a null pointer is returned
     * @param[in] img image
     * @return pointList corresponding to the image
     */
    ARDATA_API data::PointList::sptr getPointList( const data::Image::sptr& img) const;

    /**
     * @brief Get the image corresponding to the specified pointList, if the specified pointList is not found
     *  a null pointer is returned
     * @param[in] pl the pointList
     * @return image corresponding to the pointList
     */
    ARDATA_API data::Image::sptr getImage( const data::PointList::sptr& pl) const;

    /**
     * @brief Get the image corresponding to the specified index, if the index is not found
     *  a null pointer is returned
     * @param[in] idx index of image
     * @return image corresponding to the index
     */
    ARDATA_API data::Image::sptr getImage(size_t idx) const;

protected:
    ///List of Image
    ImageContainerType m_imageContainer;
    ///List of pointList
    PointListContainerType m_pointListContainer;
    ///added record signal
    AddedRecordSignalType::sptr m_sigAddedRecord;
    /// removed record signal
    RemovedRecordSignalType::sptr m_sigRemovedRecord;
    ///reset records signal
    ResetRecordSignalType::sptr m_sigResetRecord;
    ///get record signal
    GetRecordSignalType::sptr m_sigGetRecord;

};

}// namespace arData
