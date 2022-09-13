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
#include "io/session/Helper.hpp"

#include <data/Series.hpp>

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

namespace sight::io::session::detail::Series
{

constexpr static auto s_uuid {"uuid"};
constexpr static auto s_dicom {"dataset.dcm"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>&,
    const core::crypto::secure_string& password = ""
)
{
    const auto series = Helper::safe_cast<data::Series>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Series>(tree, 1);

    // Get an output stream
    const auto& ostream = archive.openFile(
        std::filesystem::path(series->getUUID() + "/" + s_dicom),
        password
    );

    // Write to disk as raw data. This is not a "real" DICOM file as our "Series" are "generic" DICOM series with no
    // SOPClassUID and SOPInstanceUID.
    series->getDataSet().Write<gdcm::ExplicitDataElement, gdcm::SwapperNoOp>(*ostream);
}

//------------------------------------------------------------------------------

inline static data::Series::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>&,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto series = Helper::cast_or_create<data::Series>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Series>(tree, 0, 1);

    // Get the input stream
    const auto& uuid    = tree.get<std::string>(s_uuid);
    const auto& istream = archive.openFile(
        std::filesystem::path(uuid + "/" + s_dicom),
        password
    );

    gdcm::DataSet dataset;
    dataset.Read<gdcm::ExplicitDataElement, gdcm::SwapperNoOp>(*istream);
    series->setDataSet(dataset);

    return series;
}

} // namespace sight::io::session::detail::Series
