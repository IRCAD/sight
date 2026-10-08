/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <data/image.hpp>

#include <io/__/reader/generic_object_reader.hpp>

namespace sight::io::itk
{

class SIGHT_IO_ITK_CLASS_API nifti_image_reader : public reader::generic_object_reader<data::image>,
                                                  public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(nifti_image_reader, io::reader::generic_object_reader<data::image>);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    SIGHT_IO_ITK_API ~nifti_image_reader() override = default;

    SIGHT_IO_ITK_API void read(sight::sptr<sight::core::notification::observer> _progress) override;
};

} // namespace sight::io::itk
