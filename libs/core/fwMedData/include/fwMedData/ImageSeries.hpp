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

#include <fwData/factory/new.hpp>

fwCampAutoDeclareDataMacro((fwMedData)(ImageSeries), FWMEDDATA_API)

namespace fwData
{
class Image;
}

namespace fwMedData
{

/**
 * @brief Holds a medical images data.
 */
class FWMEDDATA_CLASS_API ImageSeries : public ::fwMedData::Series
{

public:

    fwCoreClassMacro(ImageSeries, ::fwData::Object, ::fwData::factory::New< ImageSeries >)

    fwCampMakeFriendDataMacro((fwMedData)(ImageSeries))

    /**
     * @brief Creates the series.
     * @param _key private construction key.
     */
    FWMEDDATA_API ImageSeries(::fwData::Object::Key _key);

    /// Destroys the series.
    FWMEDDATA_API virtual ~ImageSeries() override;

    /**
     * @brief Defines shallow copy.
     * @param _source the source object to copy into this one.
     */
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object to copy into this one.
     * @param _cache contains all copied objects to avoid duplication.
     */
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& _cache) override;

    /// Gets the image container.
    SPTR(::fwData::Image) getImage() const;

    /// Sets the image data.
    void setImage(const SPTR(::fwData::Image)& _image);

    /// Gets the DICOM reference used to generate valid Dicom Segmentation.
    ::fwMedData::DicomSeries::csptr getDicomReference() const;

    /// Sets the DICOM reference used to generate valid Dicom Segmentation.
    void setDicomReference(const ::fwMedData::DicomSeries::csptr& _reference);

    /// Gets the contrast/bolus agent.
    const DicomValueType& getContrastAgent() const;

    /// Sets the contrast/bolus agent.
    void setContrastAgent(const DicomValueType& _val);

    /// Gets the administration route of contrast agent.
    const DicomValueType& getContrastRoute() const;

    /// Sets the administration route of contrast agent.
    void setContrastRoute(const DicomValueType& _val);

    /// Gets the volume injected in milliliters of diluted contrast agent.
    const DicomValueType& getContrastVolume() const;

    /// Sets the volume injected in milliliters of diluted contrast agent.
    void setContrastVolume(const DicomValueType& _val);

    // Gets injection start time.
    const DicomValueType& getContrastStartTime() const;

    /// Sets the injection start time.
    void setContrastStartTime(const DicomValueType& _val);

    /// Gets the injection end time.
    const DicomValueType& getContrastStopTime() const;

    /// Sets injection end time.
    void setContrastStopTime(const DicomValueType& _val);

    /// Gets the total amount in milliliters of the undiluted contrast agent.
    const DicomValueType& getContrastTotalDose() const;

    /// Sets the total amount in milliliters of the undiluted contrast agent.
    void setContrastTotalDose(const DicomValueType& _val);

    ///  Gets the rate of injection in milliliters/sec.
    const DicomValueType& getContrastFlowRate() const;

    /// Sets the rate of injection in milliliters/sec.
    void setContrastFlowRate(const DicomValueType& _val);

    /// Gets the duration of injection in seconds.
    const DicomValueType& getContrastFlowDuration() const;

    /// Sets the duration of injection in seconds.
    void setContrastFlowDuration(const DicomValueType& _val);

    /// Gets the active ingredient of agent (IODINE, GADOLINIUM, CARBON DIOXIDE, BARIUM).
    const DicomValueType& getContrastIngredient() const;

    /// Sets the active ingredient of agent (IODINE, GADOLINIUM, CARBON DIOXIDE, BARIUM).
    void setContrastIngredient(const DicomValueType& _val);

    /// Gets the milligrams of active ingredient per milliliter of (diluted) agent.
    const DicomValueType& getContrastIngredientConcentration() const;

    /// Sets the milligrams of active ingredient per milliliter of (diluted) agent.
    void setContrastIngredientConcentration(const DicomValueType& _val);

    /// Gets the acquisition date of data that resulted in this image started.
    const DicomValueType& getAcquisitionDate() const;

    /// Sets the acquisition date of data that resulted in this image started.
    void setAcquisitionDate(const DicomValueType& _val);

    /// Gets the acquisition time of data that resulted in this image started.
    const DicomValueType& getAcquisitionTime() const;

    /// Sets the acquisition time of data that resulted in this image started.
    void setAcquisitionTime(const DicomValueType& _val);

protected:

    /// Contains the image.
    SPTR(::fwData::Image) m_image;

    /// Contains the DICOM reference used to generate a valid DICOM Segmentation.
    ::fwMedData::DicomSeries::sptr m_dicomReference;

    /// Defines the contrast/bolus agent.
    DicomValueType m_contrastBolusAgent;

    /// Defines the administration route of contrast agent.
    DicomValueType m_contrastBolusRoute;

    /// Defines the volume injected in milliliters of diluted contrast agent.
    DicomValueType m_contrastBolusVolume;

    /// Defines the injection start time.
    DicomValueType m_contrastBolusStartTime;

    /// Defines the injection end time.
    DicomValueType m_contrastBolusStopTime;

    /// Defines the total amount in milliliters of the undiluted contrast agent.
    DicomValueType m_contrastBolusTotalDose;

    /// Defines the rate of injection in milliliters/sec;
    DicomValueType m_contrastBolusFlowRate;

    /// Defines the duration of injection in seconds;
    DicomValueType m_contrastBolusFlowDuration;

    /// Defines the active ingredient of agent (IODINE, GADOLINIUM, CARBON DIOXIDE, BARIUM).
    DicomValueType m_contrastBolusIngredient;

    /// Defines the milligrams of active ingredient per milliliter of (diluted) agent.
    DicomValueType m_contrastBolusIngredientConcentration;

    /// Defines the date acquisition of data that resulted in this image started.
    DicomValueType m_acquisitionDate;

    /// Defines the time acquisition of data that resulted in this image started.
    DicomValueType m_acquisitionTime;

};

//-----------------------------------------------------------------------------

inline SPTR(::fwData::Image) ImageSeries::getImage() const
{
    return m_image;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setImage(const SPTR(::fwData::Image)& _image)
{
    m_image = _image;
}

//-----------------------------------------------------------------------------

inline ::fwMedData::DicomSeries::csptr ImageSeries::getDicomReference() const
{
    return m_dicomReference;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setDicomReference(const ::fwMedData::DicomSeries::csptr& _reference)
{
    m_dicomReference = std::const_pointer_cast< ::fwMedData::DicomSeries >( _reference );
}

//-----------------------------------------------------------------------------

inline const DicomValueType& ImageSeries::getContrastAgent() const
{
    return m_contrastBolusAgent;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setContrastAgent(const DicomValueType& _val)
{
    m_contrastBolusAgent = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& ImageSeries::getContrastRoute() const
{
    return m_contrastBolusRoute;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setContrastRoute(const DicomValueType& _val)
{
    m_contrastBolusRoute = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& ImageSeries::getContrastVolume() const
{
    return m_contrastBolusVolume;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setContrastVolume(const DicomValueType& _val)
{
    m_contrastBolusVolume = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& ImageSeries::getContrastStartTime() const
{
    return m_contrastBolusStartTime;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setContrastStartTime(const DicomValueType& _val)
{
    m_contrastBolusStartTime = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& ImageSeries::getContrastStopTime() const
{
    return m_contrastBolusStopTime;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setContrastStopTime(const DicomValueType& _val)
{
    m_contrastBolusStopTime = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& ImageSeries::getContrastTotalDose() const
{
    return m_contrastBolusTotalDose;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setContrastTotalDose(const DicomValueType& _val)
{
    m_contrastBolusTotalDose = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& ImageSeries::getContrastFlowRate() const
{
    return m_contrastBolusFlowRate;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setContrastFlowRate(const DicomValueType& _val)
{
    m_contrastBolusFlowRate = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& ImageSeries::getContrastFlowDuration() const
{
    return m_contrastBolusFlowDuration;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setContrastFlowDuration(const DicomValueType& _val)
{
    m_contrastBolusFlowDuration = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& ImageSeries::getContrastIngredient() const
{
    return m_contrastBolusIngredient;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setContrastIngredient(const DicomValueType& _val)
{
    m_contrastBolusIngredient = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& ImageSeries::getContrastIngredientConcentration() const
{
    return m_contrastBolusIngredientConcentration;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setContrastIngredientConcentration(const DicomValueType& _val)
{
    m_contrastBolusIngredientConcentration = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& ImageSeries::getAcquisitionDate() const
{
    return m_acquisitionDate;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setAcquisitionDate(const DicomValueType& _val)
{
    m_acquisitionDate = _val;
}

//-----------------------------------------------------------------------------

inline const DicomValueType& ImageSeries::getAcquisitionTime() const
{
    return m_acquisitionTime;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setAcquisitionTime(const DicomValueType& _val)
{
    m_acquisitionTime = _val;
}

} //end namespace fwMedData.
