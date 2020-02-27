/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwMedData/config.hpp"
#include "fwMedData/DicomSeries.hpp"
#include "fwMedData/Series.hpp"
#include "fwMedData/types.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwData/factory/new.hpp>

#include <vector>

fwCampAutoDeclareDataMacro((fwMedData)(ModelSeries), FWMEDDATA_API)

namespace fwData
{
class Reconstruction;
}

namespace fwMedData
{

/**
 * @brief Holds models series.
 */
class FWMEDDATA_CLASS_API ModelSeries : public ::fwMedData::Series
{

public:

    typedef std::vector< SPTR(::fwData::Reconstruction) > ReconstructionVectorType;

    fwCoreClassMacro(ModelSeries, ::fwData::Object, ::fwData::factory::New< ModelSeries >)

    fwCampMakeFriendDataMacro((fwMedData)(ModelSeries))

    /**
     * @brief Creates the models series.
     * @param _key private construction key.
     */
    FWMEDDATA_API ModelSeries(::fwData::Object::Key _key);

    /// Destroys the models series.
    FWMEDDATA_API virtual ~ModelSeries() override;

    /**
     * @brief Defines shallow copy.
     * @param _source the source object where find data.
     */
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object where find data.
     * @param _cache
     */
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache ) override;

    /**
     * @brief Gets the reconstruction container use to store mesh, material and image mask.
     * @return The reconstruction container.
     */
    const ReconstructionVectorType& getReconstructionDB() const;

    /**
     * @brief Sets the reconstruction container use to store mesh, material and image mask.
     * @param _val the reconstruction container.
     */
    void setReconstructionDB(const ReconstructionVectorType& _val);

    /**
     * @brief Gets the DICOM reference use to generate valid DICOM Segmentation Surface
     * @return The DICOM reference
     */
    ::fwMedData::DicomSeries::csptr getDicomReference() const;

    /**
     * @brief Sets the DICOM reference use to generate valid DICOM Segmentation Surface
     * @param _reference the DICOM reference
     */
    void setDicomReference(const ::fwMedData::DicomSeries::csptr& _reference);

    /**
     * @name Signals
     * @{
     */
    /// Defines the type of signal sent when a reconstructions are added.
    typedef ::fwCom::Signal< void (ReconstructionVectorType) > ReconstructionsAddedSignalType;
    FWMEDDATA_API static const ::fwCom::Signals::SignalKeyType s_RECONSTRUCTIONS_ADDED_SIG;

    /// Defines the type of signal sent when a reconstructions are removed.
    typedef ::fwCom::Signal< void (ReconstructionVectorType) > ReconstructionsRemovedSignalType;
    FWMEDDATA_API static const ::fwCom::Signals::SignalKeyType s_RECONSTRUCTIONS_REMOVED_SIG;
    /**
     * @}
     */

protected:

    /// Stores models.
    ReconstructionVectorType m_reconstructionDB;

    /// Stores the DICOM reference used to generate a valid DICOM Segmentation Surface.
    ::fwMedData::DicomSeries::sptr m_dicomReference;

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

inline ::fwMedData::DicomSeries::csptr ModelSeries::getDicomReference() const
{
    return m_dicomReference;
}

//-----------------------------------------------------------------------------

inline void ModelSeries::setDicomReference(const ::fwMedData::DicomSeries::csptr& _reference)
{
    m_dicomReference = std::const_pointer_cast< ::fwMedData::DicomSeries >( _reference );
}

//-----------------------------------------------------------------------------

} // Namespace fwMedData.
