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

#include "FilterUnknownActivities.hpp"

#include <atoms/Base.hpp>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/Sequence.hpp>

#include <io/atoms/filter/functions.hpp>
#include <io/atoms/filter/registry/macros.hpp>
#include <io/atoms/patch/helper/functions.hpp>

fwAtomsFilterRegisterMacro(::sight::module::io::atoms::FilterUnknownActivities, "FilterUnknownActivities");

namespace sight::module::io::atoms
{

FilterUnknownActivities::FilterUnknownActivities(sight::io::atoms::filter::IFilter::Key key)
{
}

FilterUnknownActivities::~FilterUnknownActivities()
{
}

//------------------------------------------------------------------------------

void FilterUnknownActivities::apply(const SPTR(sight::atoms::Object)& atom)
{
    SIGHT_ASSERT("Unable to filter atom : invalid object", atom);

    const std::string expName    = "::sight::data::SeriesDB"; // expected classname
    const std::string& classname = sight::io::atoms::patch::helper::getClassname(atom);
    SIGHT_THROW_IF(
        "Unable to filter atom of class '" << classname << "'. Expected class is '" + expName + "'",
        classname != expName
    );

    auto series = atom->getAttribute<sight::atoms::Sequence>("values");
    SIGHT_ASSERT("Failed to retrieve 'values' attribute as atoms::Sequence", series);

    auto knownSeries = sight::atoms::Sequence::New();
    for(auto serie : series->getValue())
    {
        auto obj = sight::atoms::Object::dynamicCast(serie);
        SIGHT_ASSERT("Failed to cast sequence element as atoms::Object", obj);

        if(sight::io::atoms::filter::isSeriesKnown(obj))
        {
            knownSeries->push_back(serie);
        }
    }

    series->clear();
    for(auto serie : knownSeries->getValue())
    {
        series->push_back(serie);
    }
}

} // namespace sight::module::io::atoms
