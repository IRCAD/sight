/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

// cspell:ignore FFTWFFT

#include "module/filter/image/plugin.hpp"

#if ITK_VERSION_MAJOR >= 5 && ITK_VERSION_MINOR >= 3 && defined(__unix__)
#include <itkFFTWFFTImageFilterInitFactory.h>
#endif

namespace sight::module::filter::image
{

//-----------------------------------------------------------------------------

SIGHT_REGISTER_PLUGIN("sight::module::filter::image::plugin");

//-----------------------------------------------------------------------------

plugin::~plugin() noexcept =
    default;

//-----------------------------------------------------------------------------

void plugin::start()
{
#if ITK_VERSION_MAJOR >= 5 && ITK_VERSION_MINOR >= 3 && defined(__unix__)
    auto factory = itk::FFTWFFTImageFilterInitFactory::New();
    factory->RegisterFactories();
#endif
}

//-----------------------------------------------------------------------------

void plugin::stop() noexcept
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::image
