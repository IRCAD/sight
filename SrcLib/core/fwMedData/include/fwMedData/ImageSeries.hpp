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
     * @param _source the source object where find data.
     */
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object where find data.
     * @param _cache
     */
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& _cache ) override;

    /**
     * @brief Gets the image.
     * @return The image data container.
     */
    SPTR(::fwData::Image) getImage() const;

    /**
     * @brief Sets the image data..
     * @param _source the source object where find data.
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

protected:

    /// Contains the image.
    SPTR(::fwData::Image) m_image;

    /// Contains the DICOM reference used to generate a valid DICOM Segmentation.
    ::fwMedData::DicomSeries::sptr m_dicomReference;

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

} //end namespace fwMedData
