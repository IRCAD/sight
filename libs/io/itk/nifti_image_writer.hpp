/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/io/itk/config.hpp>

#include <core/location/single_file.hpp>
#include <core/tools/progress_adviser.hpp>

#include <data/image.hpp>

#include <io/__/writer/generic_object_writer.hpp>

namespace sight::io::itk
{

class SIGHT_IO_ITK_CLASS_API nifti_image_writer : public writer::generic_object_writer<data::image>,
                                                  public core::location::single_file,
                                                  public core::tools::progress_adviser
{
public:

    SIGHT_DECLARE_CLASS(
        nifti_image_writer,
        io::writer::generic_object_writer<data::image>,
        io::writer::factory::make<nifti_image_writer>
    );
    SIGHT_ALLOW_SHARED_FROM_THIS();

    SIGHT_IO_ITK_API ~nifti_image_writer() override = default;

    SIGHT_IO_ITK_API void write() override;

    SIGHT_IO_ITK_API std::string extension() const override;
};

} // namespace sight::io::itk
