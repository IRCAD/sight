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

#include "io/dicom/writer/iod/InformationObjectDefinition.hpp"

#include <core/spy_log.hpp>

namespace sight::io::dicom::writer::iod
{

//------------------------------------------------------------------------------

InformationObjectDefinition::InformationObjectDefinition(
    SPTR(io::dicom::container::DicomInstance)instance,
    std::filesystem::path destinationPath,
    core::log::logger::sptr logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    m_instance(std::move(instance)),
    m_destinationPath(std::move(destinationPath)),
    m_logger(std::move(logger)),
    m_progressCallback(std::move(progress)),
    m_cancelRequestedCallback(std::move(cancel))
{
    SIGHT_ASSERT("Instance should not be null.", m_instance);
}

//------------------------------------------------------------------------------

InformationObjectDefinition::~InformationObjectDefinition()
= default;

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::iod
