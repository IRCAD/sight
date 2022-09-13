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

#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>

namespace sight::io::session::detail::ModelSeries
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
    const auto modelSeries = Helper::safe_cast<data::ModelSeries>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::ModelSeries>(tree, 1);

    // Since ModelSeries inherits from Series, we could use SeriesSerializer
    Series::serialize(archive, tree, modelSeries, children, password);

    // Serialize other attributes
    children[s_DicomReference] = modelSeries->getDicomReference();

    std::size_t index = 0;
    for(const auto& reconstruction : modelSeries->getReconstructionDB())
    {
        children[data::Reconstruction::classname() + std::to_string(index++)] = reconstruction;
    }
}

//------------------------------------------------------------------------------

inline static data::ModelSeries::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto modelSeries = Helper::cast_or_create<data::ModelSeries>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::ModelSeries>(tree, 0, 1);

    // Since ModelSeries inherits from Series, we could use SeriesDeserializer
    Series::deserialize(archive, tree, children, modelSeries, password);

    // Deserialize other attributes
    modelSeries->setDicomReference(std::dynamic_pointer_cast<data::DicomSeries>(children.at(s_DicomReference)));

    // Deserialize ReconstructionDB
    std::vector<data::Reconstruction::sptr> reconstructionDB;

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(data::Reconstruction::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        reconstructionDB.push_back(std::dynamic_pointer_cast<data::Reconstruction>(it->second));
    }

    modelSeries->setReconstructionDB(reconstructionDB);

    return modelSeries;
}

} // namespace sight::io::session::detail::ModelSeries
