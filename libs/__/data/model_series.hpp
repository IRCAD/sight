/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "data/config.hpp"
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
class DATA_CLASS_API model_series final : public series,
                                          public has_fiducials
{
public:

    using ReconstructionVectorType = std::vector<reconstruction::sptr>;

    SIGHT_DECLARE_CLASS(model_series, series);

    /**
     * @brief Creates the models series.
     * @param _key private construction key.
     */
    DATA_API model_series();

    /// Destroys the models series.
    DATA_API ~model_series() noexcept override = default;

    /// Gets the reconstruction container use to store mesh, material and image mask.
    const ReconstructionVectorType& getReconstructionDB() const;

    /// Sets the reconstruction container use to store mesh, material and image mask.
    void setReconstructionDB(const ReconstructionVectorType& _val);

    /// Gets the DICOM reference use to generate valid DICOM Segmentation Surface.
    dicom_series::csptr getDicomReference() const;

    /// Sets the DICOM reference use to generate valid DICOM Segmentation Surface.
    void setDicomReference(const dicom_series::csptr& _reference);

    /**
     * @name Signals
     * @{
     */
    /// Defines the type of signal sent when a reconstruction is added.
    typedef core::com::signal<void (ReconstructionVectorType)> ReconstructionsAddedSignalType;
    DATA_API static const core::com::signals::key_t RECONSTRUCTIONS_ADDED_SIG;

    /// Defines the type of signal sent when a reconstruction is removed.
    typedef core::com::signal<void (ReconstructionVectorType)> ReconstructionsRemovedSignalType;
    DATA_API static const core::com::signals::key_t RECONSTRUCTIONS_REMOVED_SIG;
/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const model_series& other) const noexcept;
    DATA_API bool operator!=(const model_series& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& source,
        const std::unique_ptr<deep_copy_cache_t>& cache = std::make_unique<deep_copy_cache_t>()
    ) override;

    DATA_API fiducials_series::csptr getFiducials() const;
    DATA_API fiducials_series::sptr getFiducials();
    DATA_API bool hasFiducials() const;

protected:

    /// Stores models.
    ReconstructionVectorType m_reconstructionDB;

    /// Stores the DICOM reference used to generate a valid DICOM Segmentation Surface.
    dicom_series::sptr m_dicomReference;

private:

    /// Contains the associated Spatial Fiducials file
    fiducials_series::sptr m_fiducialsSeries {std::make_shared<data::fiducials_series>()};

    /// Stores the signal emitted when reconstructions are added.
    ReconstructionsAddedSignalType::sptr m_sigReconstructionsAdded;

    /// Stores the signal emitted when reconstructions are removed.
    ReconstructionsRemovedSignalType ::sptr m_sigReconstructionsRemoved;
};

//-----------------------------------------------------------------------------

inline const model_series::ReconstructionVectorType& model_series::getReconstructionDB() const
{
    return m_reconstructionDB;
}

//-----------------------------------------------------------------------------

inline void model_series::setReconstructionDB(const model_series::ReconstructionVectorType& _val)
{
    m_reconstructionDB = _val;
}

//-----------------------------------------------------------------------------

inline dicom_series::csptr model_series::getDicomReference() const
{
    return m_dicomReference;
}

//-----------------------------------------------------------------------------

inline void model_series::setDicomReference(const dicom_series::csptr& _reference)
{
    m_dicomReference = std::const_pointer_cast<dicom_series>(_reference);
}

//-----------------------------------------------------------------------------

} // namespace sight::data
