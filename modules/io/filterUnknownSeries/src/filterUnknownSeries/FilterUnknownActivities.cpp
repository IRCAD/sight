/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "filterUnknownSeries/FilterUnknownActivities.hpp"

#include <atoms/Base.hpp>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/Sequence.hpp>

#include <fwAtomsFilter/functions.hpp>
#include <fwAtomsFilter/registry/macros.hpp>

#include <fwAtomsPatch/helper/functions.hpp>

namespace filterUnknownSeries
{

fwAtomsFilterRegisterMacro( ::filterUnknownSeries::FilterUnknownActivities, "FilterUnknownActivities" );

FilterUnknownActivities::FilterUnknownActivities(::fwAtomsFilter::IFilter::Key key)
{
}

FilterUnknownActivities::~FilterUnknownActivities()
{
}

//------------------------------------------------------------------------------

void FilterUnknownActivities::apply(const SPTR(atoms::Object)& atom)
{

    SLM_ASSERT("Unable to filter atom : invalid object", atom);

    const std::string expName    = "::sight::data::SeriesDB"; // expected classname
    const std::string& classname = ::fwAtomsPatch::helper::getClassname(atom);
    FW_RAISE_IF("Unable to filter atom of class '" << classname << "'. Expected class is '" + expName + "'",
                classname != expName);

    atoms::Sequence::sptr series = atom->getAttribute< atoms::Sequence >("values");
    SLM_ASSERT("Failed to retrieve 'values' attribute as atoms::Sequence", series);

    atoms::Sequence::sptr knownSeries = atoms::Sequence::New();
    for(atoms::Base::sptr serie :  series->getValue())
    {
        atoms::Object::sptr obj = atoms::Object::dynamicCast(serie);
        SLM_ASSERT("Failed to cast sequence element as atoms::Object", obj);

        if(::fwAtomsFilter::isSeriesKnown(obj))
        {
            knownSeries->push_back(serie);
        }
    }

    series->clear();
    for(atoms::Base::sptr serie :  knownSeries->getValue())
    {
        series->push_back(serie);
    }
}

} // namespace filterUnknownSeries
