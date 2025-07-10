/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <sight/data/config.hpp>

#include "data/dicom_series.hpp"
#include "data/factory/new.hpp"
#include "data/reconstruction.hpp"
#include "data/series.hpp"
#include "data/types.hpp"

#include "fiducials_series.hpp"
#include "has_fiducials.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

#include <vector>

namespace sight::data
{

/**
 * @brief Holds models series.
 */
class SIGHT_DATA_CLASS_API model_series final : public series,
                                                public has_fiducials
{
public:

    using reconstruction_vector_t = std::vector<reconstruction::sptr>;

    SIGHT_DECLARE_CLASS(model_series, series);

    /**
     * @brief Creates the models series.
     */
    SIGHT_DATA_API model_series();

    /// Destroys the models series.
    SIGHT_DATA_API ~model_series() noexcept override = default;

    /// Gets the reconstruction container use to store mesh, material and image mask.
    const reconstruction_vector_t& get_reconstruction_db() const;

    /// Sets the reconstruction container use to store mesh, material and image mask.
    void set_reconstruction_db(const reconstruction_vector_t& _val);

    /// Gets the DICOM reference use to generate valid DICOM Segmentation Surface.
    dicom_series::csptr get_dicom_reference() const;

    /// Sets the DICOM reference use to generate valid DICOM Segmentation Surface.
    void set_dicom_reference(const dicom_series::csptr& _reference);

    /**
     * @name Signals
     * @{
     */
    /// Defines the type of signal sent when a reconstruction is added.
    using reconstructions_added_signal_t = core::com::signal<void (reconstruction_vector_t)>;
    SIGHT_DATA_API static const core::com::signals::key_t RECONSTRUCTIONS_ADDED_SIG;

    /// Defines the type of signal sent when a reconstruction is removed.
    using reconstructions_removed_signal_t = core::com::signal<void (reconstruction_vector_t)>;
    SIGHT_DATA_API static const core::com::signals::key_t RECONSTRUCTIONS_REMOVED_SIG;
/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const model_series& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const model_series& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    /// Stores models.
    reconstruction_vector_t m_reconstruction_db;

    /// Stores the DICOM reference used to generate a valid DICOM Segmentation Surface.
    dicom_series::sptr m_dicom_reference;

private:

    /// Stores the signal emitted when reconstructions are added.
    reconstructions_added_signal_t::sptr m_sig_reconstructions_added;

    /// Stores the signal emitted when reconstructions are removed.
    reconstructions_removed_signal_t ::sptr m_sig_reconstructions_removed;
};

//-----------------------------------------------------------------------------

inline const model_series::reconstruction_vector_t& model_series::get_reconstruction_db() const
{
    return m_reconstruction_db;
}

//-----------------------------------------------------------------------------

inline void model_series::set_reconstruction_db(const model_series::reconstruction_vector_t& _val)
{
    m_reconstruction_db = _val;
}

//-----------------------------------------------------------------------------

inline dicom_series::csptr model_series::get_dicom_reference() const
{
    return m_dicom_reference;
}

//-----------------------------------------------------------------------------

inline void model_series::set_dicom_reference(const dicom_series::csptr& _reference)
{
    m_dicom_reference = std::const_pointer_cast<dicom_series>(_reference);
}

//-----------------------------------------------------------------------------

} // namespace sight::data
