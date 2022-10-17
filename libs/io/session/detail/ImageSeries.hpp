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
    const auto imageSeries = Helper::safe_cast<data::ImageSeries>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::ImageSeries>(tree, 1);

    // Since ImageSeries inherits from Series, we could use SeriesSerializer
    Series::serialize(archive, tree, imageSeries, children, password);
    Image::serialize(archive, tree, imageSeries, children, password);

    // Serialize other attributes
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
    auto imageSeries = Helper::cast_or_create<data::ImageSeries>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::ImageSeries>(tree, 0, 1);

    // Since ImageSeries inherits from Series we could use SeriesDeserializer
    Series::deserialize(archive, tree, children, imageSeries, password);
    Image::deserialize(archive, tree, children, imageSeries, password);

    // Deserialize other attributes
    imageSeries->setDicomReference(std::dynamic_pointer_cast<data::DicomSeries>(children.at(s_DicomReference)));

    return imageSeries;
}

} // namespace sight::io::session::detail::ImageSeries
