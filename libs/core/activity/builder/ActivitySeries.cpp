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

#include "activity/builder/ActivitySeries.hpp"

#include "activity/builder/registry/macros.hpp"

#include <core/tools/dateAndTime.hpp>
#include <core/tools/UUID.hpp>

#include <data/Composite.hpp>
#include <data/Equipment.hpp>
#include <data/Patient.hpp>
#include <data/reflection/getObject.hpp>
#include <data/Study.hpp>
#include <data/Vector.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace sight::activity
{
namespace builder
{

fwActivitiesBuilderRegisterMacro(activity::builder::ActivitySeries, "::activity::builder::ActivitySeries");

//-----------------------------------------------------------------------------

ActivitySeries::ActivitySeries(activity::IBuilder::Key key)
{
}

//-----------------------------------------------------------------------------

ActivitySeries::~ActivitySeries()
{
}

//-----------------------------------------------------------------------------

data::Composite::sptr vectorToComposite(const data::Vector::csptr& vector,
                                        const activity::extension::ActivityRequirement& req)
{
    namespace ActReg = activity::extension;
    data::Composite::sptr composite = data::Composite::New();

    SIGHT_ASSERT("Each possible items in requirement need to have a matching key", req.keys.size() >= req.maxOccurs );

    ActReg::ActivityRequirement::KeyType::const_iterator iter = req.keys.begin();

    for(const data::Object::sptr& obj :  *vector)
    {
        const ActReg::ActivityRequirementKey& keyTag = (*iter++);
        if(keyTag.path.empty())
        {
            (*composite)[keyTag.key] = obj;
        }
        else
        {
            (*composite)[keyTag.key] = data::reflection::getObject( obj, keyTag.path );
        }
    }

    return composite;
}

//-----------------------------------------------------------------------------

data::ActivitySeries::sptr ActivitySeries::buildData(
    const activity::extension::ActivityInfo& activityInfo,
    const data::Vector::csptr& currentSelection ) const
{
    data::ActivitySeries::sptr actSeries = data::ActivitySeries::New();

    data::Series::sptr series;
    for(const data::Object::sptr& obj :  *currentSelection)
    {
        series = data::Series::dynamicCast(obj);
        if(series)
        {
            break;
        }
    }

    if(series)
    {
        actSeries->setPatient( data::Object::copy(series->getPatient()) );
        actSeries->setStudy( data::Object::copy(series->getStudy()) );
        actSeries->setEquipment( data::Object::copy(series->getEquipment()) );
    }

    actSeries->setModality("OT");
    actSeries->setInstanceUID("activity." + core::tools::UUID::generateUUID() );

    ::boost::posix_time::ptime now = ::boost::posix_time::second_clock::local_time();
    actSeries->setDate(core::tools::getDate(now));
    actSeries->setTime(core::tools::getTime(now));

    actSeries->setActivityConfigId(activityInfo.id);
    data::Composite::sptr data = actSeries->getData();

    namespace ActReg = activity::extension;

    ActReg::ActivityInfo::RequirementsType reqVect = activityInfo.requirements;
    for(const ActReg::ActivityRequirement& req :  reqVect)
    {
        data::Vector::sptr vectorType = this->getType(currentSelection, req.type);
        // param is optional (minOccurs==0) or required (minOccurs==1), but is single (maxOccurs == 1)
        if(req.maxOccurs == 1 && req.minOccurs == 1)
        {
            SIGHT_ASSERT("No param name "<<req.name<<" with type "<<req.type, !vectorType->empty());
            (*data)[req.name] = (*vectorType)[0];
        }
        else
        {
            SIGHT_ASSERT("Unknown specified container: '"+req.container+"'.",
                         req.container.empty() ||
                         req.container == "vector" ||
                         req.container == "composite");
            if(req.container == "vector")
            {
                (*data)[req.name] = vectorType;
            }
            else if(req.container == "composite" || req.container.empty())
            {
                (*data)[req.name] = vectorToComposite(vectorType, req);
            }
        }
    }

    return actSeries;
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace sight::activity
