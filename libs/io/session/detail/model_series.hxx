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
#include "io/session/detail/series.hxx"
#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

namespace sight::io::session::detail::model_series
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
    const auto modelSeries = helper::safe_cast<data::model_series>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::model_series>(tree, 1);

    // Since model_series inherits from Series, we could use SeriesSerializer
    series::write(archive, tree, modelSeries, children, password);

    // Serialize other attributes
    children[s_DicomReference] = modelSeries->getDicomReference();

    std::size_t index = 0;
    for(const auto& reconstruction : modelSeries->getReconstructionDB())
    {
        children[data::reconstruction::classname() + std::to_string(index++)] = reconstruction;
    }
}

//------------------------------------------------------------------------------

inline static data::model_series::sptr read(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& children,
    data::object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto modelSeries = helper::cast_or_create<data::model_series>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::model_series>(tree, 0, 1);

    // Since model_series inherits from Series, we could use SeriesDeserializer
    series::read(archive, tree, children, modelSeries, password);

    // Deserialize other attributes
    modelSeries->setDicomReference(std::dynamic_pointer_cast<data::dicom_series>(children.at(s_DicomReference)));

    // Deserialize ReconstructionDB
    std::vector<data::reconstruction::sptr> reconstructionDB;

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(data::reconstruction::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        reconstructionDB.push_back(std::dynamic_pointer_cast<data::reconstruction>(it->second));
    }

    modelSeries->setReconstructionDB(reconstructionDB);

    return modelSeries;
}

SIGHT_REGISTER_SERIALIZER(data::model_series, write, read);

} // namespace sight::io::session::detail::model_series
