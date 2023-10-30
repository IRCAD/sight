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

constexpr static auto DICOM_REFERENCE {"DicomReference"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& _archive,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& _children,
    const core::crypto::secure_string& _password = ""
)
{
    const auto model_series = helper::safe_cast<data::model_series>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::model_series>(_tree, 1);

    // Since model_series inherits from Series, we could use SeriesSerializer
    series::write(_archive, _tree, model_series, _children, _password);

    // Serialize other attributes
    _children[DICOM_REFERENCE] = model_series->get_dicom_reference();

    std::size_t index = 0;
    for(const auto& reconstruction : model_series->get_reconstruction_db())
    {
        _children[data::reconstruction::classname() + std::to_string(index++)] = reconstruction;
    }
}

//------------------------------------------------------------------------------

inline static data::model_series::sptr read(
    zip::archive_reader& _archive,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& _children,
    data::object::sptr _object,
    const core::crypto::secure_string& _password = ""
)
{
    // Create or reuse the object
    auto model_series = helper::cast_or_create<data::model_series>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::model_series>(_tree, 0, 1);

    // Since model_series inherits from Series, we could use SeriesDeserializer
    series::read(_archive, _tree, _children, model_series, _password);

    // Deserialize other attributes
    model_series->set_dicom_reference(std::dynamic_pointer_cast<data::dicom_series>(_children.at(DICOM_REFERENCE)));

    // Deserialize ReconstructionDB
    std::vector<data::reconstruction::sptr> reconstruction_db;

    for(std::size_t index = 0, end = _children.size() ; index < end ; ++index)
    {
        const auto& it = _children.find(data::reconstruction::classname() + std::to_string(index));

        if(it == _children.cend())
        {
            break;
        }

        reconstruction_db.push_back(std::dynamic_pointer_cast<data::reconstruction>(it->second));
    }

    model_series->set_reconstruction_db(reconstruction_db);

    return model_series;
}

SIGHT_REGISTER_SERIALIZER(data::model_series, write, read);

} // namespace sight::io::session::detail::model_series
