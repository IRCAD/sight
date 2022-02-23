/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "data/config.hpp"
#include "data/Image.hpp"
#include "data/Object.hpp"
#include "data/PointList.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>
#include <core/macros.hpp>

#include <list>

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (CalibrationInfo));

namespace sight::data
{

/**
 * @brief class for handle calibration information , this class handle two lists the first one contains images
 * and the second one contains pointList
 *
 */
class DATA_CLASS_API CalibrationInfo : public Object
{
public:

    SIGHT_DECLARE_CLASS(CalibrationInfo, Object, factory::New<CalibrationInfo>);

    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (CalibrationInfo));

    /**
     * @name Signals API
     * @{
     */
    DATA_API static const core::com::Signals::SignalKeyType s_ADDED_RECORD_SIG;
    typedef core::com::Signal<void ()> AddedRecordSignalType;

    DATA_API static const core::com::Signals::SignalKeyType s_REMOVED_RECORD_SIG;
    typedef core::com::Signal<void ()> RemovedRecordSignalType;

    DATA_API static const core::com::Signals::SignalKeyType s_RESET_RECORD_SIG;
    typedef core::com::Signal<void ()> ResetRecordSignalType;

    DATA_API static const core::com::Signals::SignalKeyType s_GET_RECORD_SIG;
    typedef core::com::Signal<void (std::size_t)> GetRecordSignalType;

    /**
     * @}
     */

    ///Constructor
    DATA_API CalibrationInfo(Object::Key key);
    ///Destructor
    DATA_API ~CalibrationInfo();

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**
     * @brief add an Image and a pointList into CalibrationInfo
     * @param[in] img the new image
     * @param[in] pl the corresponding point list
     */
    DATA_API void addRecord(const Image::sptr& img, const PointList::sptr& pl);

    /**
     * @brief remove the desired record (both image and pointList)
     * @param[in] idx index of the record to be removed
     */
    DATA_API void removeRecord(std::size_t idx);

    /// Reset all records
    DATA_API void resetRecords();

    ///Get all the images
    ///@{
    DATA_API std::list<Image::sptr> getImageContainer();
    DATA_API std::list<Image::csptr> getImageContainer() const;
    ///@}

    ///Get all the pointLists
    ///@{
    DATA_API std::list<PointList::sptr> getPointListContainer();
    DATA_API std::list<PointList::csptr> getPointListContainer() const;
    ///@}

    /**
     * @brief Get the pointList corresponding to the specified image, if the specified image is not found
     * a null pointer is returned
     * @param[in] img image
     * @return pointList corresponding to the image
     *
     */
    DATA_API PointList::csptr getPointList(const Image::csptr& img) const;

    /**
     * @brief Get the image corresponding to the specified pointList, if the specified pointList is not found
     *  a null pointer is returned
     * @param[in] pl the pointList
     * @return image corresponding to the pointList
     */
    DATA_API Image::csptr getImage(const PointList::csptr& pl) const;

    /**
     * @brief Get the image corresponding to the specified index, if the index is not found
     *  a null pointer is returned
     * @param[in] idx index of image
     * @return image corresponding to the index
     * @{
     */
    DATA_API Image::sptr getImage(std::size_t idx);
    DATA_API Image::csptr getImage(std::size_t idx) const;
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const CalibrationInfo& other) const noexcept;
    DATA_API bool operator!=(const CalibrationInfo& other) const noexcept;

    /// @}

protected:

    ///List of Image
    std::list<Image::sptr> m_imageContainer;
    ///List of pointList
    std::list<PointList::sptr> m_pointListContainer;
    ///added record signal
    AddedRecordSignalType::sptr m_sigAddedRecord;
    /// removed record signal
    RemovedRecordSignalType::sptr m_sigRemovedRecord;
    ///reset records signal
    ResetRecordSignalType::sptr m_sigResetRecord;
    ///get record signal
    GetRecordSignalType::sptr m_sigGetRecord;
};

} // namespace sight::data
