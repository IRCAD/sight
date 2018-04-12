/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwMedData/config.hpp"
#include "fwMedData/DicomSeries.hpp"
#include "fwMedData/Series.hpp"
#include "fwMedData/types.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwData/factory/new.hpp>

#include <vector>

fwCampAutoDeclareDataMacro((fwMedData)(ModelSeries), FWMEDDATA_API);

namespace fwData
{
class Reconstruction;
}

namespace fwMedData
{

/**
 * @brief Holds models data
 */
class FWMEDDATA_CLASS_API ModelSeries : public ::fwMedData::Series
{

public:
    typedef std::vector< SPTR(::fwData::Reconstruction) > ReconstructionVectorType;

    fwCoreClassDefinitionsWithFactoryMacro( (ModelSeries)(::fwData::Object), (()),
                                            ::fwData::factory::New< ModelSeries >);

    fwCampMakeFriendDataMacro((fwMedData)(ModelSeries));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API ModelSeries(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~ModelSeries() override;

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache ) override;

    /**
     * @name Getters / Setters
     * @{ */

    /**
     * @brief  Reconstruction container use to store mesh, material and image mask
     * @{ */
    const ReconstructionVectorType& getReconstructionDB() const;
    void setReconstructionDB(const ReconstructionVectorType& val);
    /**  @} */

    /**
     * @brief Dicom reference use to generate valid Dicom Segmentation Surface
     * @{ */
    ::fwMedData::DicomSeries::csptr getDicomReference() const;
    void setDicomReference(const ::fwMedData::DicomSeries::csptr& reference);
    /**  @} */

    /**  @} */

    /***
     * @name Signals
     * @{
     */
    /// Type of signal when reconstructions are added
    typedef ::fwCom::Signal< void (ReconstructionVectorType) > ReconstructionsAddedSignalType;

    /// Key in m_signals map of signal m_sigReconstructionsAdded
    FWMEDDATA_API static const ::fwCom::Signals::SignalKeyType s_RECONSTRUCTIONS_ADDED_SIG;

    /// Type of signal when reconstructions are removed
    typedef ::fwCom::Signal< void (ReconstructionVectorType) > ReconstructionsRemovedSignalType;

    /// Key in m_signals map of signal m_sigReconstructionsRemoved
    FWMEDDATA_API static const ::fwCom::Signals::SignalKeyType s_RECONSTRUCTIONS_REMOVED_SIG;

    /**
     * @}
     */

protected:

    /// Model container
    ReconstructionVectorType m_reconstructionDB;

    /// Dicom reference used to generate a valid Dicom Segmentation Surface
    ::fwMedData::DicomSeries::sptr m_dicomReference;

private:

    /***
     * @name Signals attributes
     * @{
     */
    /// Signal emitted when reconstructions are added
    ReconstructionsAddedSignalType::sptr m_sigReconstructionsAdded;

    /// Signal emitted when reconstructions are removed
    ReconstructionsRemovedSignalType ::sptr m_sigReconstructionsRemoved;
    /**
     * @}
     */

};

//-----------------------------------------------------------------------------

inline const ModelSeries::ReconstructionVectorType& ModelSeries::getReconstructionDB() const
{
    return m_reconstructionDB;
}

//-----------------------------------------------------------------------------

inline void ModelSeries::setReconstructionDB(const ModelSeries::ReconstructionVectorType& val)
{
    m_reconstructionDB = val;
}

//-----------------------------------------------------------------------------

inline ::fwMedData::DicomSeries::csptr ModelSeries::getDicomReference() const
{
    return m_dicomReference;
}

//-----------------------------------------------------------------------------

inline void ModelSeries::setDicomReference(const ::fwMedData::DicomSeries::csptr& reference)
{
    m_dicomReference = std::const_pointer_cast< ::fwMedData::DicomSeries >( reference );
}

//-----------------------------------------------------------------------------

}   //end namespace fwMedData
