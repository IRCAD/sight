/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "filterVRRender/MedicalDataV2.hpp"

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/Sequence.hpp>
#include <fwAtomsFilter/functions.hpp>

#include <fwAtomsFilter/registry/macros.hpp>

#include <fwAtomsPatch/helper/functions.hpp>

namespace filterVRRender
{

fwAtomsFilterRegisterMacro( ::filterVRRender::MedicalDataV2, "VRRenderMedicalDataV2" );

MedicalDataV2::MedicalDataV2(::fwAtomsFilter::IFilter::Key key)
{
}

MedicalDataV2::~MedicalDataV2()
{
}

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

