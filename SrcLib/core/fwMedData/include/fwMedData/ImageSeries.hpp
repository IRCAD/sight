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
     * @param _cache
     */
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& _cache) override;

    /**
     * @brief Gets the image.
     * @return The image data container.
     */
    SPTR(::fwData::Image) getImage() const;

    /**
     * @brief Sets the image data.
     * @param _image the image data container.
     */
    void setImage(const SPTR(::fwData::Image)& _image);

    /**
     * @brief Gets the DICOM reference used to generate valid Dicom Segmentation.
     * @return The DICOM reference of this data.
     */
    ::fwMedData::DicomSeries::csptr getDicomReference() const;

    /**
     * @brief Sets the DICOM reference used to generate valid Dicom Segmentation.
     * @param _reference the DICOM reference of this data.
     */
    void setDicomReference(const ::fwMedData::DicomSeries::csptr& _reference);

    /**
     * @brief Gets the contrast/bolus agent.
     * @return The contrast/bolus agent.
     */
    const DicomValueType& getContrastAgent() const;

    /**
     * @brief Sets the contrast/bolus agent.
     * @param _val the contrast/bolus agent.
     */
    void setContrastAgent(const DicomValueType& _val);

    /**
     * @brief Gets the administration route of contrast agent.
     * @return The administration route of contrast agent.
     */
    const DicomValueType& getContrastRoute() const;

    /**
     * @brief Sets the administration route of contrast agent.
     * @param _val the administration route of contrast agent.
     */
    void setContrastRoute(const DicomValueType& _val);

    /**
     * @brief Gets the volume injected in milliliters of diluted contrast agent.
     * @return The volume.
     */
    const DicomValueType& getContrastVolume() const;

    /**
     * @brief Sets the volume injected in milliliters of diluted contrast agent.
     * @param _val the volume.
     */
    void setContrastVolume(const DicomValueType& _val);

    /**
     * @brief Gets injection start time.
     * @return The time.
     */
    const DicomValueType& getContrastStartTime() const;

    /**
     * @brief Sets the injection start time.
     * @param _val the time.
     */
    void setContrastStartTime(const DicomValueType& _val);

    /**
     * @brief Gets the injection end time.
     * @return The time.
     */
    const DicomValueType& getContrastStopTime() const;

    /**
     * @brief Sets injection end time.
     * @param _val the time.
     */
    void setContrastStopTime(const DicomValueType& _val);

    /**
     * @brief Gets the total amount in milliliters of the undiluted contrast agent.
     * @return The total amount in milliliters of the undiluted contrast agent.
     */
    const DicomValueType& getContrastTotalDose() const;

    /**
     * @brief Sets the total amount in milliliters of the undiluted contrast agent.
     * @param _val the total amount in milliliters of the undiluted contrast agent.
     */
    void setContrastTotalDose(const DicomValueType& _val);

    /**
     * @brief Gets the rate of injection in milliliters/sec.
     * @return The rate of injection.
     */
    const DicomValueType& getContrastFlowRate() const;

    /**
     * @brief Sets the rate of injection in milliliters/sec.
     * @param _val the rate of injection.
     */
    void setContrastFlowRate(const DicomValueType& _val);

    /**
     * @brief Gets the duration of injection in seconds.
     * @return The duration.
     */
    const DicomValueType& getContrastFlowDuration() const;

    /**
     * @brief Sets the duration of injection in seconds.
     * @param _val the duration.
     */
    void setContrastFlowDuration(const DicomValueType& _val);

    /**
     * @brief Gets the active ingredient of agent (IODINE, GADOLINIUM, CARBON DIOXIDE, BARIUM).
     * @return The active ingredient of agent.
     */
    const DicomValueType& getContrastIngredient() const;

    /**
     * @brief Sets the active ingredient of agent (IODINE, GADOLINIUM, CARBON DIOXIDE, BARIUM).
     * @param _val the active ingredient of agent.
     */
    void setContrastIngredient(const DicomValueType& _val);

    /**
     * @brief Gets the milligrams of active ingredient per milliliter of (diluted) agent.
     * @return The milligrams of active ingredient per milliliter of (diluted) agent.
     */
    const DicomValueType& getContrastIngredientConcentration() const;

    /**
     * @brief Sets the milligrams of active ingredient per milliliter of (diluted) agent.
     * @param _val the milligrams of active ingredient per milliliter of (diluted) agent.
     */
    void setContrastIngredientConcentration(const DicomValueType& _val);

    /**
     * @brief Gets the acquisition date of data that resulted in this image started.
     * @return The date.
     */
    const DicomValueType& getAcquisitionDate() const;

    /**
     * @brief Sets the acquisition date of data that resulted in this image started.
     * @param _val the date.
     */
    void setAcquisitionDate(const DicomValueType& _val);

    /**
     * @brief Gets the acquisition time of data that resulted in this image started.
     * @return The time.
     */
    const DicomValueType& getAcquisitionTime() const;

    /**
     * @brief Sets the acquisition time of data that resulted in this image started.
     * @param _val the time.
     */
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
