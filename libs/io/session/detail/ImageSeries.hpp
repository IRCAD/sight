/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

#include "io/session/config.hpp"
#include "io/session/detail/Series.hpp"
#include "io/session/Helper.hpp"

#include <data/ImageSeries.hpp>

namespace sight::io::session::detail::ImageSeries
{

constexpr static auto s_ContrastAgent {"ContrastAgent"};
constexpr static auto s_ContrastRoute {"ContrastRoute"};
constexpr static auto s_ContrastVolume {"ContrastVolume"};
constexpr static auto s_ContrastStartTime {"ContrastStartTime"};
constexpr static auto s_ContrastStopTime {"ContrastStopTime"};
constexpr static auto s_ContrastTotalDose {"ContrastTotalDose"};
constexpr static auto s_ContrastFlowRate {"ContrastFlowRate"};
constexpr static auto s_ContrastFlowDuration {"ContrastFlowDuration"};
constexpr static auto s_ContrastIngredient {"ContrastIngredient"};
constexpr static auto s_ContrastIngredientConcentration {"ContrastIngredientConcentration"};
constexpr static auto s_AcquisitionDate {"AcquisitionDate"};
constexpr static auto s_AcquisitionTime {"AcquisitionTime"};
constexpr static auto s_Image {"Image"};
constexpr static auto s_DicomReference {"DicomReference"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto imageSeries = Helper::safeCast<data::ImageSeries>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::ImageSeries>(tree, 1);

    // Since ImageSeries inherits from Series, we could use SeriesSerializer
    Series::serialize(archive, tree, imageSeries, children);

    // Serialize other attributes
    Helper::writeString(tree, s_ContrastAgent, imageSeries->getContrastAgent());
    Helper::writeString(tree, s_ContrastRoute, imageSeries->getContrastRoute());
    Helper::writeString(tree, s_ContrastVolume, imageSeries->getContrastVolume());
    Helper::writeString(tree, s_ContrastStartTime, imageSeries->getContrastStartTime());
    Helper::writeString(tree, s_ContrastStopTime, imageSeries->getContrastStopTime());
    Helper::writeString(tree, s_ContrastTotalDose, imageSeries->getContrastTotalDose());
    Helper::writeString(tree, s_ContrastFlowRate, imageSeries->getContrastFlowRate());
    Helper::writeString(tree, s_ContrastFlowDuration, imageSeries->getContrastFlowDuration());
    Helper::writeString(tree, s_ContrastIngredient, imageSeries->getContrastIngredient());
    Helper::writeString(tree, s_ContrastIngredientConcentration, imageSeries->getContrastIngredientConcentration());
    Helper::writeString(tree, s_AcquisitionDate, imageSeries->getAcquisitionDate());
    Helper::writeString(tree, s_AcquisitionTime, imageSeries->getAcquisitionTime());

    children[s_Image]          = imageSeries->getImage();
    children[s_DicomReference] = imageSeries->getDicomReference();
}

//------------------------------------------------------------------------------

inline static data::ImageSeries::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto imageSeries = Helper::safeCast<data::ImageSeries>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::ImageSeries>(tree, 0, 1);

    // Since ImageSeries inherits from Series, we could use SeriesDeserializer
    Series::deserialize(archive, tree, children, imageSeries);

    // Deserialize other attributes
    imageSeries->setContrastAgent(Helper::readString(tree, s_ContrastAgent));
    imageSeries->setContrastRoute(Helper::readString(tree, s_ContrastRoute));
    imageSeries->setContrastVolume(Helper::readString(tree, s_ContrastVolume));
    imageSeries->setContrastStartTime(Helper::readString(tree, s_ContrastStartTime));
    imageSeries->setContrastStopTime(Helper::readString(tree, s_ContrastStopTime));
    imageSeries->setContrastTotalDose(Helper::readString(tree, s_ContrastTotalDose));
    imageSeries->setContrastFlowRate(Helper::readString(tree, s_ContrastFlowRate));
    imageSeries->setContrastFlowDuration(Helper::readString(tree, s_ContrastFlowDuration));
    imageSeries->setContrastIngredient(Helper::readString(tree, s_ContrastIngredient));
    imageSeries->setContrastIngredientConcentration(Helper::readString(tree, s_ContrastIngredientConcentration));
    imageSeries->setAcquisitionDate(Helper::readString(tree, s_AcquisitionDate));
    imageSeries->setAcquisitionTime(Helper::readString(tree, s_AcquisitionTime));

    imageSeries->setImage(std::dynamic_pointer_cast<data::Image>(children.at(s_Image)));
    imageSeries->setDicomReference(std::dynamic_pointer_cast<data::DicomSeries>(children.at(s_DicomReference)));

    return imageSeries;
}

} // namespace sight::io::session::detail::ImageSeries
