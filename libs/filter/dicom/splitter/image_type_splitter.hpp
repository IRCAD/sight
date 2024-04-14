/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <sight/filter/dicom/config.hpp>

#include "filter/dicom/splitter/tag_value_splitter.hpp"

namespace sight::filter::dicom::splitter
{

/**
 * @brief filter that uses the image_t tag to split the instances.
 */
class SIGHT_FILTER_DICOM_CLASS_API image_type_splitter : public sight::filter::dicom::splitter::tag_value_splitter
{
public:

    SIGHT_DECLARE_CLASS(image_type_splitter, base, sight::filter::dicom::factory::make<image_type_splitter>);

    /// Constructor
    SIGHT_FILTER_DICOM_API image_type_splitter();

    /// Destructor
    SIGHT_FILTER_DICOM_API ~image_type_splitter() override;

    /// Return the name of the filter
    SIGHT_FILTER_DICOM_API std::string get_name() const override;

    /// Return the description of the filter
    SIGHT_FILTER_DICOM_API std::string get_description() const override;

    /// Return true if a configuration is required
    SIGHT_FILTER_DICOM_API bool is_configuration_required() const override;

protected:

    /// filter name
    static const std::string FILTER_NAME;

    /// filter description
    static const std::string FILTER_DESCRIPTION;
};

} // namespace sight::filter::dicom::splitter
