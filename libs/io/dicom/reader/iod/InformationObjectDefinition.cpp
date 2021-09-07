/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/dicom/reader/iod/InformationObjectDefinition.hpp"

namespace sight::io::dicom
{

namespace reader
{

namespace iod
{

//------------------------------------------------------------------------------

InformationObjectDefinition::InformationObjectDefinition(
    const data::DicomSeries::csptr& dicomSeries,
    const SPTR(io::dicom::container::DicomInstance)& instance,
    const core::log::Logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    m_instance(instance),
    m_dicomSeries(dicomSeries),
    m_logger(logger),
    m_progressCallback(progress),
    m_cancelRequestedCallback(cancel)
{
    SIGHT_ASSERT("DicomSeries should not be null.", dicomSeries);
    SIGHT_ASSERT("Instance should not be null.", instance);
    SIGHT_ASSERT("Logger should not be null.", logger);
}

//------------------------------------------------------------------------------

InformationObjectDefinition::~InformationObjectDefinition()
{
}

//------------------------------------------------------------------------------

} // namespace iod

} // namespace reader

} // namespace sight::io::dicom
