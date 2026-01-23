/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "insert_fiducials.hpp"

#include <core/com/signal.hxx>
#include <core/tools/uuid.hpp>

#include <data/fiducials_series.hpp>
#include <data/image_series.hpp>

namespace sight::module::data
{

//-----------------------------------------------------------------------------

void insert_fiducials::configuring()
{
}

//-----------------------------------------------------------------------------

void insert_fiducials::starting()
{
}

//-----------------------------------------------------------------------------

void insert_fiducials::updating()
{
    const auto fiducials = m_fiducials.lock();
    const auto image     = m_image.lock();

    if(fiducials.get_shared() && image.get_shared())
    {
        // Get all fiducial sets from the input fiducials_series
        const auto fiducial_sets = fiducials->get_fiducial_sets();

        // Get the fiducials from the image series
        auto image_fiducials = image->get_fiducials();

        std::string frame_of_reference_uid = image->get_string_value(
            sight::data::dicom::attribute::Keyword::FrameOfReferenceUID
        );
        if(frame_of_reference_uid.empty())
        {
            // Generate a frame of reference UID if the image doesn't have one. It is supposed to be mandatory
            // according to the DICOM standard anyway.
            frame_of_reference_uid = sight::core::tools::uuid::generate();
            image->set_string_value(
                sight::data::dicom::attribute::Keyword::FrameOfReferenceUID,
                frame_of_reference_uid
            );
        }

        // Insert each fiducial set into the image series
        for(const auto& fiducial_set : fiducial_sets)
        {
            auto new_fiducial_set = fiducial_set;
            new_fiducial_set.frame_of_reference_uid = frame_of_reference_uid;
            image_fiducials->append_fiducial_set(new_fiducial_set);
        }

        // Signal that the image has been modified
        auto sig = image->signal<sight::data::object::modified_signal_t>(sight::data::object::MODIFIED_SIG);
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

void insert_fiducials::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data.
