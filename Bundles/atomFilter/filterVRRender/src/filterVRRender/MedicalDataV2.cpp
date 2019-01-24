/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "filterVRRender/MedicalDataV2.hpp"

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/Sequence.hpp>

#include <fwAtomsFilter/functions.hpp>
#include <fwAtomsFilter/registry/macros.hpp>

#include <fwAtomsPatch/helper/functions.hpp>

#include <fwCore/spyLog.hpp>

namespace filterVRRender
{

fwAtomsFilterRegisterMacro( ::filterVRRender::MedicalDataV2, "VRRenderMedicalDataV2" );

MedicalDataV2::MedicalDataV2(::fwAtomsFilter::IFilter::Key key)
{
    FW_DEPRECATED("filterVRRender", "filterUnknownSeries", "19.0");
    FW_DEPRECATED("MedicalDataV2", "FilterUnknownActivities", "19.0");
    FW_DEPRECATED("VRRenderMedicalDataV2", "FilterUnknownActivities", "19.0");
}

MedicalDataV2::~MedicalDataV2()
{
}

//------------------------------------------------------------------------------

void MedicalDataV2::apply(const SPTR(::fwAtoms::Object)& atom)
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("Unable to filter atom : invalid object", atom);

    const std::string expName    = "::fwMedData::SeriesDB"; // expected classname
    const std::string& classname = ::fwAtomsPatch::helper::getClassname(atom);
    FW_RAISE_IF("Unable to filter atom of class '" << classname << "'. Expected class is '" + expName + "'",
                classname != expName);

    ::fwAtoms::Sequence::sptr series = atom->getAttribute< ::fwAtoms::Sequence >("values");
    SLM_ASSERT("Failed to retrieve 'values' attribute as ::fwAtoms::Sequence", series);

    ::fwAtoms::Sequence::sptr knownSeries = ::fwAtoms::Sequence::New();
    for(::fwAtoms::Base::sptr serie :  series->getValue())
    {
        ::fwAtoms::Object::sptr obj = ::fwAtoms::Object::dynamicCast(serie);
        SLM_ASSERT("Failed to cast sequence element as ::fwAtoms::Object", obj);

        if(::fwAtomsFilter::isSeriesKnown(obj))
        {
            knownSeries->push_back(serie);
        }
    }

    series->clear();
    for(::fwAtoms::Base::sptr serie :  knownSeries->getValue())
    {
        series->push_back(serie);
    }
}

} // namespace filterVRRender
