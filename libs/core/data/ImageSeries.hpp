/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include "data/factory/new.hpp"
#include "data/Image.hpp"
#include "data/Series.hpp"
#include "data/types.hpp"

namespace sight::data
{

class Image;

} // namespace sight::data

namespace sight::data
{

/**
 * @brief Holds a medical images data.
 */
class DATA_CLASS_API ImageSeries final : public Image,
                                         public Series
{
public:

    SIGHT_DECLARE_CLASS(ImageSeries, Image, factory::New<ImageSeries>);

    /**
     * @brief Creates the series.
     * @param _key private construction key.
     */
    DATA_API ImageSeries(Object::Key _key);

    /// Destroys the series.
    DATA_API ~ImageSeries() noexcept override = default;

    /// Gets the DICOM reference used to generate valid Dicom Segmentation.
    DicomSeries::csptr getDicomReference() const;

    /// Sets the DICOM reference used to generate valid Dicom Segmentation.
    void setDicomReference(const DicomSeries::csptr& _reference);

    /// Getter/Setter of DICOM General Image Module related attributes
    /// @note the definition is in Series.cpp
    /// @{
    DATA_API std::string getAcquisitionDate() const noexcept;
    DATA_API void setAcquisitionDate(const std::string& acquisitionDate);

    DATA_API std::string getAcquisitionTime() const noexcept;
    DATA_API void setAcquisitionTime(const std::string& acquisitionTime);
    /// @}

    /// Getter/Setter of DICOM Contrast/Bolus Module related attributes
    /// @note the definition is in Series.cpp
    /// @{
    DATA_API std::string getContrastBolusAgent() const noexcept;
    DATA_API void setContrastBolusAgent(const std::string& contrastBolusAgent);

    DATA_API std::string getContrastBolusRoute() const noexcept;
    DATA_API void setContrastBolusRoute(const std::string& contrastBolusRoute);

    DATA_API std::optional<double> getContrastBolusVolume() const noexcept;
    DATA_API void setContrastBolusVolume(const std::optional<double>& contrastBolusVolume = std::nullopt);

    DATA_API std::string getContrastBolusStartTime() const noexcept;
    DATA_API void setContrastBolusStartTime(const std::string& contrastBolusStartTime);

    DATA_API std::string getContrastBolusStopTime() const noexcept;
    DATA_API void setContrastBolusStopTime(const std::string& contrastBolusStopTime);

    DATA_API std::optional<double> getContrastBolusTotalDose() const noexcept;
    DATA_API void setContrastBolusTotalDose(const std::optional<double>& contrastBolusTotalDose = std::nullopt);

    DATA_API std::vector<double> getContrastFlowRates() const noexcept;
    DATA_API void setContrastFlowRates(const std::vector<double>& contrastFlowRates);

    DATA_API std::string getContrastFlowRate() const noexcept;
    DATA_API void setContrastFlowRate(const std::string& contrastFlowRates);

    DATA_API std::vector<double> getContrastFlowDurations() const noexcept;
    DATA_API void setContrastFlowDurations(const std::vector<double>& contrastFlowDurations);

    DATA_API std::string getContrastFlowDuration() const noexcept;
    DATA_API void setContrastFlowDuration(const std::string& contrastFlowDurations);

    DATA_API std::string getContrastBolusIngredient() const noexcept;
    DATA_API void setContrastBolusIngredient(const std::string& contrastBolusIngredient);

    DATA_API std::optional<double> getContrastBolusIngredientConcentration() const noexcept;
    DATA_API void setContrastBolusIngredientConcentration(
        const std::optional<double>& contrastBolusIngredientConcentration = std::nullopt
    );
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const ImageSeries& other) const noexcept;
    DATA_API bool operator!=(const ImageSeries& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallowCopy(const Object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deepCopy(
        const Object::csptr& source,
        const std::unique_ptr<DeepCopyCacheType>& cache = std::make_unique<DeepCopyCacheType>()
    ) override;

private:

    /// Contains the DICOM reference used to generate a valid DICOM Segmentation.
    DicomSeries::sptr m_dicomReference;
};

//-----------------------------------------------------------------------------

inline DicomSeries::csptr ImageSeries::getDicomReference() const
{
    return m_dicomReference;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setDicomReference(const DicomSeries::csptr& _reference)
{
    m_dicomReference = std::const_pointer_cast<DicomSeries>(_reference);
}

} //end namespace sight::data.
