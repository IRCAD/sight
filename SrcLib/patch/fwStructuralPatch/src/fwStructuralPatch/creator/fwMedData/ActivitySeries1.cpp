/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtoms/String.hpp>
#include <fwAtoms/Object.hpp>

#include <fwAtomsPatch/helper/Object.hpp>

#include "fwStructuralPatch/creator/fwMedData/ActivitySeries1.hpp"

namespace fwStructuralPatch
{
namespace creator
{
namespace fwMedData
{

ActivitySeries1::ActivitySeries1()
{
    m_classname = "::fwMedData::ActivitySeries";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

ActivitySeries1::~ActivitySeries1()
{
}

// ----------------------------------------------------------------------------

ActivitySeries1::ActivitySeries1( const ActivitySeries1 &cpy ) : Series1(cpy)
{
}

// ----------------------------------------------------------------------------

::fwAtoms::Object::sptr ActivitySeries1::create()
{
    ::fwAtoms::Object::sptr series = this->Series1::create();
    ::fwAtomsPatch::helper::Object helper(series);

    helper.addAttribute("activity_config_id", ::fwAtoms::String::New(""));
    helper.addAttribute("data", ::fwAtoms::Object::New());

    return series;
}

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch

