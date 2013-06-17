/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    m_version = "1";
}

// ----------------------------------------------------------------------------

ModelSeries1::~ModelSeries1()
{}

// ----------------------------------------------------------------------------

ModelSeries1::ModelSeries1( const ModelSeries1 &cpy ) : Series1(cpy)
{}

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
