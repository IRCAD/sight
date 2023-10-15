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
#include "io/session/macros.hpp"
#include "io/session/helper.hpp"

#include <data/series.hpp>

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4702) // warning C4702: unreachable code
#endif

#include <gdcmDataSet.h>
#include <gdcmExplicitDataElement.h>
#include <gdcmSwapper.h>

#ifdef WIN32
#pragma warning( pop )
#endif

namespace sight::io::session::detail::series
{

constexpr static auto s_uuid {"uuid"};
constexpr static auto s_instance_dataset {"instance_dataset.dcm"};
constexpr static auto s_num_instances {"num_instances"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>&,
    const core::crypto::secure_string& password = ""
)
{
    const auto series = helper::safe_cast<data::series>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::series>(tree, 1);

    // Store the instance count to be able to know how many instances to read
    tree.put(s_num_instances, series->numInstances());

    // Write the "instance" datasets. In case the original data come from a DICOM series with several instances (files)
    for(std::size_t instance = 0, end = series->numInstances() ; instance < end ; ++instance)
    {
        const auto& ostream = archive.openFile(
            std::filesystem::path(series->get_uuid() + "/" + std::to_string(instance) + "_" + s_instance_dataset),
            password
        );

        series->getDataSet(instance).Write<gdcm::ExplicitDataElement, gdcm::SwapperNoOp>(*ostream);
    }
}

//------------------------------------------------------------------------------

inline static data::series::sptr read(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>&,
    data::object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto series = helper::cast_or_create<data::series>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::series>(tree, 0, 1);

    // Get the input stream
    const auto& uuid = tree.get<std::string>(s_uuid);

    // Read the instance count to be able to know how many instances to read
    for(std::size_t instance = 0, end = tree.get<std::size_t>(s_num_instances) ; instance < end ; ++instance)
    {
        const auto& istream = archive.openFile(
            std::filesystem::path(uuid + "/" + std::to_string(instance) + "_" + s_instance_dataset),
            password
        );

        gdcm::DataSet dataset;
        dataset.Read<gdcm::ExplicitDataElement, gdcm::SwapperNoOp>(*istream);
        series->setDataSet(dataset, instance);
    }

    return series;
}

SIGHT_REGISTER_SERIALIZER(data::series, write, read);

} // namespace sight::io::session::detail::series
