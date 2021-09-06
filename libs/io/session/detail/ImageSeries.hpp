/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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
#include "io/session/detail/Helper.hpp"
#include "io/session/detail/Series.hpp"

#include <data/ImageSeries.hpp>

namespace sight::io::session
{

namespace detail::ImageSeries
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
    const core::crypto::secure_string& password = ""
)
{
    const auto imageSeries = Helper::safeCast<data::ImageSeries>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::ImageSeries>(tree, 1);

    // Since ImageSeries inherits from Series, we could use SeriesSerializer
    Series::serialize(archive, tree, imageSeries, children, password);

    // Serialize other attributes
    Helper::writeString(tree, s_ContrastAgent, imageSeries->getContrastAgent(), password);
    Helper::writeString(tree, s_ContrastRoute, imageSeries->getContrastRoute(), password);
    Helper::writeString(tree, s_ContrastVolume, imageSeries->getContrastVolume(), password);
    Helper::writeString(tree, s_ContrastStartTime, imageSeries->getContrastStartTime(), password);
    Helper::writeString(tree, s_ContrastStopTime, imageSeries->getContrastStopTime(), password);
    Helper::writeString(tree, s_ContrastTotalDose, imageSeries->getContrastTotalDose(), password);
    Helper::writeString(tree, s_ContrastFlowRate, imageSeries->getContrastFlowRate(), password);
    Helper::writeString(tree, s_ContrastFlowDuration, imageSeries->getContrastFlowDuration(), password);
    Helper::writeString(tree, s_ContrastIngredient, imageSeries->getContrastIngredient(), password);
    Helper::writeString(
        tree,
        s_ContrastIngredientConcentration,
        imageSeries->getContrastIngredientConcentration(),
        password
    );
    Helper::writeString(tree, s_AcquisitionDate, imageSeries->getAcquisitionDate(), password);
    Helper::writeString(tree, s_AcquisitionTime, imageSeries->getAcquisitionTime(), password);

    children[s_Image]          = imageSeries->getImage();
    children[s_DicomReference] = imageSeries->getDicomReference();
}

//------------------------------------------------------------------------------

inline static data::ImageSeries::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto imageSeries = Helper::safeCast<data::ImageSeries>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::ImageSeries>(tree, 0, 1);

    // Since ImageSeries inherits from Series, we could use SeriesDeserializer
    Series::deserialize(archive, tree, children, imageSeries, password);

    // Deserialize other attributes
    imageSeries->setContrastAgent(Helper::readString(tree, s_ContrastAgent, password));
    imageSeries->setContrastRoute(Helper::readString(tree, s_ContrastRoute, password));
    imageSeries->setContrastVolume(Helper::readString(tree, s_ContrastVolume, password));
    imageSeries->setContrastStartTime(Helper::readString(tree, s_ContrastStartTime, password));
    imageSeries->setContrastStopTime(Helper::readString(tree, s_ContrastStopTime, password));
    imageSeries->setContrastTotalDose(Helper::readString(tree, s_ContrastTotalDose, password));
    imageSeries->setContrastFlowRate(Helper::readString(tree, s_ContrastFlowRate, password));
    imageSeries->setContrastFlowDuration(Helper::readString(tree, s_ContrastFlowDuration, password));
    imageSeries->setContrastIngredient(Helper::readString(tree, s_ContrastIngredient, password));
    imageSeries->setContrastIngredientConcentration(
        Helper::readString(
            tree,
            s_ContrastIngredientConcentration,
            password
        )
    );
    imageSeries->setAcquisitionDate(Helper::readString(tree, s_AcquisitionDate, password));
    imageSeries->setAcquisitionTime(Helper::readString(tree, s_AcquisitionTime, password));

    imageSeries->setImage(std::dynamic_pointer_cast<data::Image>(children.at(s_Image)));
    imageSeries->setDicomReference(std::dynamic_pointer_cast<data::DicomSeries>(children.at(s_DicomReference)));

    return imageSeries;
}

} // namespace detail::ImageSeries

} // namespace sight::io