/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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
#include "data/image.hpp"
#include "data/object.hpp"
#include "data/point_list.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>
#include <core/macros.hpp>

#include <list>

namespace sight::data
{

/**
 * @brief class for handle calibration information , this class handle two lists the first one contains images
 * and the second one contains pointList
 *
 */
class DATA_CLASS_API calibration_info final : public object
{
public:

    SIGHT_DECLARE_CLASS(calibration_info, object);

    ///typedefs
    using image_container_t      = std::list<image::sptr>;
    using point_list_container_t = std::list<point_list::sptr>;

    /**
     * @name Signals API
     * @{
     */
    DATA_API static const core::com::signals::key_t ADDED_RECORD_SIG;
    using added_record_signal_t = core::com::signal<void ()>;

    DATA_API static const core::com::signals::key_t REMOVED_RECORD_SIG;
    using removed_record_signal_t = core::com::signal<void ()>;

    DATA_API static const core::com::signals::key_t RESET_RECORD_SIG;
    using reset_record_signal_t = core::com::signal<void ()>;

    DATA_API static const core::com::signals::key_t GET_RECORD_SIG;
    using get_record_signal_t = core::com::signal<void (std::size_t)>;

    /**
     * @}
     */

    ///Constructor
    DATA_API calibration_info();

    ///Destructor
    DATA_API ~calibration_info() noexcept override = default;

    /**
     * @brief add an Image and a pointList into calibration_info
     * @param[in] img the new image
     * @param[in] pl the corresponding point list
     */
    DATA_API void add_record(const image::sptr& _img, const point_list::sptr& _pl);

    /**
     * @brief remove the desired record (both image and pointList)
     * @param[in] idx index of the record to be removed
     */
    DATA_API void remove_record(std::size_t _idx);

    /// Reset all records
    DATA_API void reset_records();

    ///Get all the images
    ///@{
    DATA_API std::list<image::sptr> get_image_container();
    DATA_API std::list<image::csptr> get_image_container() const;
    ///@}

    ///Get all the pointLists
    ///@{
    DATA_API std::list<point_list::sptr> get_point_list_container();
    DATA_API std::list<point_list::csptr> get_point_list_container() const;
    ///@}

    /**
     * @brief Get the pointList corresponding to the specified image, if the specified image is not found
     * a null pointer is returned
     * @param[in] img image
     * @return pointList corresponding to the image
     *
     */
    DATA_API point_list::csptr get_point_list(const image::csptr& _img) const;

    /**
     * @brief Get the image corresponding to the specified pointList, if the specified pointList is not found
     *  a null pointer is returned
     * @param[in] pl the pointList
     * @return image corresponding to the pointList
     */
    DATA_API image::csptr get_image(const point_list::csptr& _pl) const;

    /**
     * @brief Get the image corresponding to the specified index, if the index is not found
     *  a null pointer is returned
     * @param[in] idx index of image
     * @return image corresponding to the index
     * @{
     */
    DATA_API image::sptr get_image(std::size_t _idx);
    DATA_API image::csptr get_image(std::size_t _idx) const;
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const calibration_info& _other) const noexcept;
    DATA_API bool operator!=(const calibration_info& _other) const noexcept;

    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    ///List of Image
    std::list<image::sptr> m_image_container;
    ///List of pointList
    std::list<point_list::sptr> m_point_list_container_t;
    ///added record signal
    added_record_signal_t::sptr m_sig_added_record;
    /// removed record signal
    removed_record_signal_t::sptr m_sig_removed_record;
    ///reset records signal
    reset_record_signal_t::sptr m_sig_reset_record;
    ///get record signal
    get_record_signal_t::sptr m_sig_get_record;
};

} // namespace sight::data