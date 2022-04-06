/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "InrImageIOFactory.hpp"

#include "InrImageIO.hpp"

#include "io/itk/ImageIOFactoryRegistry.hxx"

#include <core/base.hpp>

#include <itkCreateObjectFunction.h>
#include <itkVersion.h>

REGISTER_IMAGEIOFACTORY(itk::InrImageIOFactory);

namespace itk
{

//------------------------------------------------------------------------------

void InrImageIOFactory::PrintSelf(std::ostream&, Indent) const
{
}

//------------------------------------------------------------------------------

InrImageIOFactory::InrImageIOFactory()
{
    this->RegisterOverride(
        "itkImageIOBase",
        "InrImageIO",
        "InrImage IO",
        1,
        CreateObjectFunction<InrImageIO>::New()
    );
}

//------------------------------------------------------------------------------

InrImageIOFactory::~InrImageIOFactory()
{
}

//------------------------------------------------------------------------------

const char* InrImageIOFactory::GetITKSourceVersion(void) const
{
    return ITK_SOURCE_VERSION;
}

//------------------------------------------------------------------------------

const char* InrImageIOFactory::GetDescription() const
{
    return "InrImage ImageIO Factory, allows the loading of InrImage into ITK";
}

} // end namespace itk
