/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/dicom/writer/iod/information_object_definition.hpp"

#include <core/spy_log.hpp>

namespace sight::io::dicom::writer::iod
{

//------------------------------------------------------------------------------

information_object_definition::information_object_definition(
    SPTR(io::dicom::container::dicom_instance)_instance,
    std::filesystem::path _destination_path,
    core::log::logger::sptr _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    m_instance(std::move(_instance)),
    m_destination_path(std::move(_destination_path)),
    m_logger(std::move(_logger)),
    m_progress_callback(std::move(_progress)),
    m_cancel_requested_callback(std::move(_cancel))
{
    SIGHT_ASSERT("Instance should not be null.", m_instance);
}

//------------------------------------------------------------------------------

information_object_definition::~information_object_definition()
= default;

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::iod
