/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "SGetImage.hpp"

#include "data/Exception.hpp"
namespace sight::module::data
{

//------------------------------------------------------------------------------

void SGetImage::configuring()
{
}

//------------------------------------------------------------------------------

void SGetImage::starting()
{
}

//-----------------------------------------------------------------------------

void SGetImage::updating()
{
    auto imageSeries = m_imageSeries.lock();
    if(imageSeries == nullptr)
    {
        SIGHT_THROW_EXCEPTION(sight::data::Exception("Missing image series"));
    }

    auto outputImage = imageSeries->getImage();
    if(outputImage == nullptr)
    {
        SIGHT_THROW_EXCEPTION(sight::data::Exception("Output Image is empty"));
    }

    this->setOutput(s_IMAGE_OUT, outputImage);
}

//-----------------------------------------------------------------------------

void SGetImage::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
