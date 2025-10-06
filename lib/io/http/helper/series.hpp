/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <sight/io/http/config.hpp>

#include <data/series_set.hpp>

#include <QJsonObject>

namespace sight::io::http::helper
{

/**
 * @brief Series Helper
 */
class SIGHT_IO_HTTP_CLASS_API series
{
public:

    using dicom_series_container_t = data::series_set::container_t;
    using instance_uid_container_t = std::vector<std::string>;

    /**
     * @brief Convert HTTP series response to data::dicom_series
     * @param[in] _answer HTTP responses from the PACS that must be converted
     */
    SIGHT_IO_HTTP_API static dicom_series_container_t to_fw_med_data(const QJsonObject& _series_json);

    /**
     * @brief Convert std::vector< data::dicom_series > to series instance uid container
     * @param[in] _series Series vector used to extract the series instance uids
     */
    SIGHT_IO_HTTP_API static instance_uid_container_t to_series_instance_uid_container(
        dicom_series_container_t _series
    );
};

} // namespace sight::io::http::helper
