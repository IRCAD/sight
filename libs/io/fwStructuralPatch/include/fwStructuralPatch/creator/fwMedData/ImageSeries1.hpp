/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwStructuralPatch/config.hpp"
#include "fwStructuralPatch/creator/data/Series1.hpp"

namespace fwStructuralPatch
{
namespace creator
{
namespace sight::data
{

/// Structural creator for data::ImageSeries version '1'.
class FWSTRUCTURALPATCH_CLASS_API ImageSeries1 : public ::fwStructuralPatch::creatordata::Series1
{

public:

    fwCoreClassMacro(ImageSeries1, ::fwStructuralPatch::creatordata::Series1, new ImageSeries1)

    /// Constructor
    FWSTRUCTURALPATCH_API ImageSeries1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~ImageSeries1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API ImageSeries1( const ImageSeries1& cpy );

    /// Creates the specified object (sets 'image' attribute) and calls creator 'Series1'.
    FWSTRUCTURALPATCH_API virtual atoms::Object::sptr create() override;

};

} // namespace sight::data
} // namespace creator
} // namespace fwStructuralPatch
