/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include <gdcmJPEG2000Codec.h>

namespace sight::io::dicom::codec
{

class nv_jpeg2_k : public gdcm::JPEG2000Codec
{
public:

    ~nv_jpeg2_k() override = default;

    bool Code(gdcm::DataElement const& _in, gdcm::DataElement& _out) override;

    ImageCodec* Clone() const override;
};

}
