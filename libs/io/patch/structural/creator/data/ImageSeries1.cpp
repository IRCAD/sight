/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/patch/structural/creator/data/ImageSeries1.hpp"

#include <atoms/Blob.hpp>
#include <atoms/Object.hpp>

#include <io/atoms/patch/helper/Object.hpp>
#include <io/atoms/patch/StructuralCreatorDB.hpp>

namespace sight::io::patch::structural
{

namespace creator
{

namespace data
{

ImageSeries1::ImageSeries1()
{
    m_classname = "sight::data::ImageSeries";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

ImageSeries1::~ImageSeries1()
{
}

// ----------------------------------------------------------------------------

ImageSeries1::ImageSeries1(const ImageSeries1& cpy) :
    Series1(cpy)
{
}

// ----------------------------------------------------------------------------

sight::atoms::Object::sptr ImageSeries1::create()
{
    sight::atoms::Object::sptr imageSeries = this->Series1::create();
    io::atoms::patch::helper::Object helper(imageSeries);

    helper.addAttribute("image", sight::atoms::Blob::New());

    return imageSeries;
}

} // namespace data

} // namespace creator

} // namespace sight::io::patch::structural
