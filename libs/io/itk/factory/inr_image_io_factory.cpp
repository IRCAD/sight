/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "inr_image_io_factory.hpp"

#include "inr_image_io.hpp"

#include "io/itk/image_io_factory_registry.hxx"

#include <core/base.hpp>

#include <itkCreateObjectFunction.h>
#include <itkVersion.h>

REGISTER_IMAGEIOFACTORY(itk::inr_image_io_factory);

namespace itk
{

//------------------------------------------------------------------------------

void inr_image_io_factory::PrintSelf(std::ostream& /*os*/, Indent /*indent*/) const
{
}

//------------------------------------------------------------------------------

inr_image_io_factory::inr_image_io_factory()
{
    this->RegisterOverride(
        "itkImageIOBase",
        "InrImageIO",
        "InrImage IO",
        true,
        CreateObjectFunction<inr_image_io>::New()
    );
}

//------------------------------------------------------------------------------

inr_image_io_factory::~inr_image_io_factory()
= default;

//------------------------------------------------------------------------------

const char* inr_image_io_factory::GetITKSourceVersion() const
{
    return ITK_SOURCE_VERSION;
}

//------------------------------------------------------------------------------

const char* inr_image_io_factory::GetDescription() const
{
    return "InrImage ImageIO factory, allows the loading of InrImage into ITK";
}

} // end namespace itk
