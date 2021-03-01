/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include "data/DicomSeries.hpp"
#include "data/Series.hpp"
#include "data/types.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>

#include <data/factory/new.hpp>

#include <vector>

SIGHT_DECLARE_DATA_REFLECTION((sight)(data)(ModelSeries))

namespace sight::data
{
class Reconstruction;
}

namespace sight::data
{

/**
 * @brief Holds models series.
 */
class DATA_CLASS_API ModelSeries : public data::Series
{

public:

    typedef std::vector< SPTR(data::Reconstruction) > ReconstructionVectorType;

    SIGHT_DECLARE_CLASS(ModelSeries, data::Object, data::factory::New< ModelSeries >)

    SIGHT_MAKE_FRIEND_REFLECTION((sight)(data)(ModelSeries))

    /**
     * @brief Creates the models series.
     * @param _key private construction key.
     */
    DATA_API ModelSeries(data::Object::Key _key);

    /// Destroys the models series.
    DATA_API virtual ~ModelSeries() override;

    /**
     * @brief Defines shallow copy.
     * @param _source the source object to copy into this one.
     */
    DATA_API void shallowCopy( const data::Object::csptr& _source ) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object to copy into this one.
     * @param _cache contains all copied objects to avoid duplication.
     */
    DATA_API void cachedDeepCopy( const data::Object::csptr& _source, DeepCopyCacheType& _cache) override;

    /// Gets the reconstruction container use to store mesh, material and image mask.
    const ReconstructionVectorType& getReconstructionDB() const;

    /// Sets the reconstruction container use to store mesh, material and image mask.
    void setReconstructionDB(const ReconstructionVectorType& _val);

    /// Gets the DICOM reference use to generate valid DICOM Segmentation Surface.
    data::DicomSeries::csptr getDicomReference() const;

    /// Sets the DICOM reference use to generate valid DICOM Segmentation Surface.
    void setDicomReference(const data::DicomSeries::csptr& _reference);

    /**
     * @name Signals
     * @{
     */
    /// Defines the type of signal sent when a reconstruction is added.
    typedef core::com::Signal< void (ReconstructionVectorType) > ReconstructionsAddedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_RECONSTRUCTIONS_ADDED_SIG;

    /// Defines the type of signal sent when a reconstruction is removed.
    typedef core::com::Signal< void (ReconstructionVectorType) > ReconstructionsRemovedSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_RECONSTRUCTIONS_REMOVED_SIG;
    /**
     * @}
     */

protected:

    /// Stores models.
    ReconstructionVectorType m_reconstructionDB;

    /// Stores the DICOM reference used to generate a valid DICOM Segmentation Surface.
    data::DicomSeries::sptr m_dicomReference;

private:

    /// Stores the signal emitted when reconstructions are added.
    ReconstructionsAddedSignalType::sptr m_sigReconstructionsAdded;

    /// Stores the signal emitted when reconstructions are removed.
    ReconstructionsRemovedSignalType ::sptr m_sigReconstructionsRemoved;

};

//-----------------------------------------------------------------------------

inline const ModelSeries::ReconstructionVectorType& ModelSeries::getReconstructionDB() const
{
    return m_reconstructionDB;
}

//-----------------------------------------------------------------------------

inline void ModelSeries::setReconstructionDB(const ModelSeries::ReconstructionVectorType& _val)
{
    m_reconstructionDB = _val;
}

//-----------------------------------------------------------------------------

inline data::DicomSeries::csptr ModelSeries::getDicomReference() const
{
    return m_dicomReference;
}

//-----------------------------------------------------------------------------

inline void ModelSeries::setDicomReference(const data::DicomSeries::csptr& _reference)
{
    m_dicomReference = std::const_pointer_cast< data::DicomSeries >( _reference );
}

//-----------------------------------------------------------------------------

} // Namespace fwMedData.
