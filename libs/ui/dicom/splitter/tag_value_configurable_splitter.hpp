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

#include <sight/ui/dicom/config.hpp>

#include <data/dicom_series.hpp>

#include <filter/dicom/splitter/tag_value_splitter.hpp>

namespace sight::ui::dicom::splitter
{

/**
 * @brief filter that uses a tag to split the instances.
 */
class SIGHT_UI_DICOM_CLASS_API tag_value_configurable_splitter : public sight::filter::dicom::splitter::
                                                                 tag_value_splitter
{
public:

    SIGHT_DECLARE_CLASS(
        tag_value_configurable_splitter,
        sight::filter::dicom::splitter::tag_value_splitter,
        sight::filter::dicom::factory::make<tag_value_configurable_splitter>
    );

    /// Destructor
    SIGHT_UI_DICOM_API ~tag_value_configurable_splitter() override = default;

    /// Return the name of the filter
    SIGHT_UI_DICOM_API std::string get_name() const override;

    /// Return the description of the filter
    SIGHT_UI_DICOM_API std::string get_description() const override;

    /// Return true if a configuration is required
    SIGHT_UI_DICOM_API bool is_configuration_required() const override;

    /// Return true if the filter is configurable using GUI
    SIGHT_UI_DICOM_API bool is_configurable_with_gui() const override;

    /// Configure the filter using GUI dialog
    SIGHT_UI_DICOM_API void configure_with_gui() override;

protected:

    /// filter name
    SIGHT_UI_DICOM_API static const std::string FILTER_NAME;

    /// filter description
    SIGHT_UI_DICOM_API static const std::string FILTER_DESCRIPTION;
};

} // namespace sight::ui::dicom::splitter
