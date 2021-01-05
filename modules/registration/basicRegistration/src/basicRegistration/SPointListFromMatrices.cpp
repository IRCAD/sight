/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "basicRegistration//SPointListFromMatrices.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::fwServices::IOperator, ::basicRegistration::SPointListFromMatrices)

namespace basicRegistration
{

const ::fwServices::IService::KeyType s_MATRICES_INPUT  = "matrices";
const ::fwServices::IService::KeyType s_POINTLIST_INOUT = "pointList";

//-----------------------------------------------------------------------------

SPointListFromMatrices::SPointListFromMatrices() :
    m_append(false)
{

}

//-----------------------------------------------------------------------------

SPointListFromMatrices::~SPointListFromMatrices()
{

}

//-----------------------------------------------------------------------------

void SPointListFromMatrices::configuring()
{
    const ConfigType configTree = this->getConfigTree();
    const ConfigType config     = configTree.get_child("config.<xmlattr>");
    if (!config.empty())
    {
        m_append = config.get<bool>("append", m_append);
    }
}

//-----------------------------------------------------------------------------

void SPointListFromMatrices::starting()
{

}

//-----------------------------------------------------------------------------

void SPointListFromMatrices::stopping()
{

}

//-----------------------------------------------------------------------------

void SPointListFromMatrices::updating()
{

    const size_t numMatrices = this->getKeyGroupSize(s_MATRICES_INPUT);
    SLM_ASSERT("no matrices found", numMatrices != 0);

    auto pointList = this->getLockedInOut< ::fwData::PointList >(s_POINTLIST_INOUT);
    if(!m_append)
    {
        pointList->getPoints().clear();
    }

    for(size_t j = 0; j < numMatrices; ++j)
    {
        const auto mat = this->getLockedInput< ::fwData::TransformationMatrix3D >(
            s_MATRICES_INPUT, j);
        ::fwData::TransformationMatrix3D::TMCoefArray coefs = mat->getCoefficients();

        //extract translation
        ::fwData::Point::sptr p = ::fwData::Point::New(coefs[3], coefs[7], coefs[11]);

        std::string label;
        if(m_append)
        {
            label = std::to_string(pointList->getPoints().size());
        }
        else
        {
            label = std::to_string(j);
        }

        ::fwData::String::sptr fwLabel = ::fwData::String::New(label);
        p->setField( ::fwDataTools::fieldHelper::Image::m_labelId, fwLabel );
        pointList->pushBack(p);

    }

    auto sig = pointList->signal< ::fwData::PointList::ModifiedSignalType >(::fwData::PointList::s_MODIFIED_SIG);
    sig->asyncEmit();

    m_sigComputed->asyncEmit();
}

//-----------------------------------------------------------------------------

}
