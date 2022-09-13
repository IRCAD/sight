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
#include "io/session/detail/Image.hpp"
#include "io/session/detail/Series.hpp"
#include "io/session/Helper.hpp"

#include <data/ImageSeries.hpp>

namespace sight::io::session::detail::ImageSeries
{

constexpr static auto s_ContrastBolusAgent {"ContrastBolusAgent"};
constexpr static auto s_ContrastBolusRoute {"ContrastBolusRoute"};
constexpr static auto s_ContrastBolusVolume {"ContrastBolusVolume"};
constexpr static auto s_ContrastBolusStartTime {"ContrastBolusStartTime"};
constexpr static auto s_ContrastBolusStopTime {"ContrastBolusStopTime"};
constexpr static auto s_ContrastBolusTotalDose {"ContrastBolusTotalDose"};
constexpr static auto s_ContrastFlowRate {"ContrastFlowRate"};
constexpr static auto s_ContrastFlowDuration {"ContrastFlowDuration"};
constexpr static auto s_ContrastBolusIngredient {"ContrastBolusIngredient"};
constexpr static auto s_ContrastBolusIngredientConcentration {"ContrastBolusIngredientConcentration"};
constexpr static auto s_AcquisitionDate {"AcquisitionDate"};
constexpr static auto s_AcquisitionTime {"AcquisitionTime"};
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
    const auto imageSeries = Helper::safe_cast<data::ImageSeries>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::ImageSeries>(tree, 1);

    // Since ImageSeries inherits from Series and Image, we could use SeriesSerializer and ImageSerializer
    Series::serialize(archive, tree, imageSeries, children);
    Image::serialize(archive, tree, imageSeries, children);

    // Serialize other attributes
    Helper::writeString(tree, s_ContrastBolusAgent, imageSeries->getContrastBolusAgent());
    Helper::writeString(tree, s_ContrastBolusRoute, imageSeries->getContrastBolusRoute());

    if(const auto& contrastBolusVolume = imageSeries->getContrastBolusVolume(); contrastBolusVolume)
    {
        tree.put(s_ContrastBolusVolume, *contrastBolusVolume);
    }

    Helper::writeString(tree, s_ContrastBolusStartTime, imageSeries->getContrastBolusStartTime());
    Helper::writeString(tree, s_ContrastBolusStopTime, imageSeries->getContrastBolusStopTime());

    if(const auto& contrastBolusTotalDose = imageSeries->getContrastBolusTotalDose(); contrastBolusTotalDose)
    {
        tree.put(s_ContrastBolusTotalDose, *contrastBolusTotalDose);
    }

    Helper::writeString(tree, s_ContrastFlowRate, imageSeries->getContrastFlowRate());
    Helper::writeString(tree, s_ContrastFlowDuration, imageSeries->getContrastFlowDuration());
    Helper::writeString(tree, s_ContrastBolusIngredient, imageSeries->getContrastBolusIngredient());

    if(const auto& contrastBolusIngredientConcentration = imageSeries->getContrastBolusIngredientConcentration();
       contrastBolusIngredientConcentration)
    {
        tree.put(s_ContrastBolusIngredientConcentration, *contrastBolusIngredientConcentration);
    }

    Helper::writeString(tree, s_AcquisitionDate, imageSeries->getAcquisitionDate());
    Helper::writeString(tree, s_AcquisitionTime, imageSeries->getAcquisitionTime());

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
    auto imageSeries = Helper::cast_or_create<data::ImageSeries>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::ImageSeries>(tree, 0, 1);

    // Since ImageSeries inherits from Series and Image, we could use SeriesDeserializer and ImageDeserializer
    Series::deserialize(archive, tree, children, imageSeries);
    Image::deserialize(archive, tree, children, imageSeries);

    // Deserialize other attributes
    imageSeries->setContrastBolusAgent(Helper::readString(tree, s_ContrastBolusAgent));
    imageSeries->setContrastBolusRoute(Helper::readString(tree, s_ContrastBolusRoute));

    if(const auto& contrastBolusVolume = tree.get_optional<double>(s_ContrastBolusVolume); contrastBolusVolume)
    {
        imageSeries->setContrastBolusVolume(*contrastBolusVolume);
    }

    imageSeries->setContrastBolusStartTime(Helper::readString(tree, s_ContrastBolusStartTime));
    imageSeries->setContrastBolusStopTime(Helper::readString(tree, s_ContrastBolusStopTime));

    if(const auto& contrastBolusTotalDose = tree.get_optional<double>(s_ContrastBolusTotalDose); contrastBolusTotalDose)
    {
        imageSeries->setContrastBolusTotalDose(*contrastBolusTotalDose);
    }

    imageSeries->setContrastFlowRate(Helper::readString(tree, s_ContrastFlowRate));
    imageSeries->setContrastFlowDuration(Helper::readString(tree, s_ContrastFlowDuration));
    imageSeries->setContrastBolusIngredient(Helper::readString(tree, s_ContrastBolusIngredient));

    if(const auto& contrastBolusIngredientConcentration =
           tree.get_optional<double>(s_ContrastBolusIngredientConcentration); contrastBolusIngredientConcentration)
    {
        imageSeries->setContrastBolusIngredientConcentration(*contrastBolusIngredientConcentration);
    }

    imageSeries->setAcquisitionDate(Helper::readString(tree, s_AcquisitionDate));
    imageSeries->setAcquisitionTime(Helper::readString(tree, s_AcquisitionTime));
    imageSeries->setDicomReference(std::dynamic_pointer_cast<data::DicomSeries>(children.at(s_DicomReference)));

    return imageSeries;
}

} // namespace sight::io::session::detail::ImageSeries
