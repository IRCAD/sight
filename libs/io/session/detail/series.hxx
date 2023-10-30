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

constexpr static auto UUID {"uuid"};
constexpr static auto INSTANCE_DATASET {"instance_dataset.dcm"};
constexpr static auto NUM_INSTANCES {"num_instances"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& _archive,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>&,
    const core::crypto::secure_string& _password = ""
)
{
    const auto series = helper::safe_cast<data::series>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::series>(_tree, 1);

    // Store the instance count to be able to know how many instances to read
    _tree.put(NUM_INSTANCES, series->num_instances());

    // Write the "instance" datasets. In case the original data come from a DICOM series with several instances (files)
    for(std::size_t instance = 0, end = series->num_instances() ; instance < end ; ++instance)
    {
        const auto& ostream = _archive.open_file(
            std::filesystem::path(series->get_uuid() + "/" + std::to_string(instance) + "_" + INSTANCE_DATASET),
            _password
        );

        series->get_data_set(instance).Write<gdcm::ExplicitDataElement, gdcm::SwapperNoOp>(*ostream);
    }
}

//------------------------------------------------------------------------------

inline static data::series::sptr read(
    zip::archive_reader& _archive,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>&,
    data::object::sptr _object,
    const core::crypto::secure_string& _password = ""
)
{
    // Create or reuse the object
    auto series = helper::cast_or_create<data::series>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::series>(_tree, 0, 1);

    // Get the input stream
    const auto& uuid = _tree.get<std::string>(UUID);

    // Read the instance count to be able to know how many instances to read
    for(std::size_t instance = 0, end = _tree.get<std::size_t>(NUM_INSTANCES) ; instance < end ; ++instance)
    {
        const auto& istream = _archive.open_file(
            std::filesystem::path(uuid + "/" + std::to_string(instance) + "_" + INSTANCE_DATASET),
            _password
        );

        gdcm::DataSet dataset;
        dataset.Read<gdcm::ExplicitDataElement, gdcm::SwapperNoOp>(*istream);
        series->set_data_set(dataset, instance);
    }

    return series;
}

SIGHT_REGISTER_SERIALIZER(data::series, write, read);

} // namespace sight::io::session::detail::series
