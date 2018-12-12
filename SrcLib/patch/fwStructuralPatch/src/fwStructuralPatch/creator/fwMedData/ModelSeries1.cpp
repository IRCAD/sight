/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
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

#include <fwAtoms/String.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>

#include <fwAtomsPatch/StructuralCreatorDB.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include "fwStructuralPatch/creator/fwMedData/ModelSeries1.hpp"

namespace fwStructuralPatch
{
namespace creator
{
namespace fwMedData
{

ModelSeries1::ModelSeries1()
{
    m_classname = "::fwMedData::ModelSeries";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

ModelSeries1::~ModelSeries1()
{
}

// ----------------------------------------------------------------------------

ModelSeries1::ModelSeries1( const ModelSeries1 &cpy ) : Series1(cpy)
{
}

// ----------------------------------------------------------------------------

::fwAtoms::Object::sptr ModelSeries1::create()
{
    ::fwAtoms::Object::sptr modelSeries = this->Series1::create();
    ::fwAtomsPatch::helper::Object helper(modelSeries);

    helper.addAttribute("reconstruction_db", ::fwAtoms::Sequence::New());

    return modelSeries;
}

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
