/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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
#include "io/session/detail/image.hxx"
#include "io/session/detail/series.hxx"
#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/image_series.hpp>

namespace sight::io::session::detail::image_series
{

constexpr static auto s_DicomReference {"DicomReference"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& children,
    const core::crypto::secure_string& password = ""
)
{
    const auto imageSeries = helper::safe_cast<data::image_series>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::image_series>(tree, 1);

    // Since image_series inherits from Series, we could use SeriesSerializer
    series::write(archive, tree, imageSeries, children, password);
    image::write(archive, tree, imageSeries, children, password);

    // Serialize other attributes
    children[s_DicomReference] = imageSeries->getDicomReference();
}

//------------------------------------------------------------------------------

inline static data::image_series::sptr read(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& children,
    data::object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto imageSeries = helper::cast_or_create<data::image_series>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::image_series>(tree, 0, 1);

    image::read(archive, tree, children, imageSeries, password);
    // Deserialize series last since it overwrites some attributes of image.
    series::read(archive, tree, children, imageSeries, password);

    // Deserialize other attributes
    imageSeries->setDicomReference(std::dynamic_pointer_cast<data::dicom_series>(children.at(s_DicomReference)));

    return imageSeries;
}

SIGHT_REGISTER_SERIALIZER(data::image_series, write, read);

} // namespace sight::io::session::detail::image_series
