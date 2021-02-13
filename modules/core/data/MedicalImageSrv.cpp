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

#include "MedicalImageSrv.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Image.hpp>
#include <data/tools/fieldHelper/MedicalImageHelpers.hpp>

#include <service/macros.hpp>

namespace sight::modules::data
{

static const std::string s_IMAGE_INOUT = "image";

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::sight::service::IController, ::sight::modules::data::MedicalImageSrv, ::sight::data::Image )

//-----------------------------------------------------------------------------

MedicalImageSrv::MedicalImageSrv() noexcept
{
    this->registerObject(s_IMAGE_INOUT, AccessType::INOUT, true);
}

//-----------------------------------------------------------------------------

MedicalImageSrv::~MedicalImageSrv() noexcept
{
}

//-----------------------------------------------------------------------------

void MedicalImageSrv::convertImage()
{
    auto pImg = this->getInOut< sight::data::Image >(s_IMAGE_INOUT);

    if(sight::data::tools::fieldHelper::MedicalImageHelpers::checkImageValidity(pImg))
    {
        bool isModified = false;
        isModified |= sight::data::tools::fieldHelper::MedicalImageHelpers::checkLandmarks(pImg);
        isModified |= sight::data::tools::fieldHelper::MedicalImageHelpers::checkTransferFunctionPool(pImg);
        isModified |= sight::data::tools::fieldHelper::MedicalImageHelpers::checkImageSliceIndex(pImg);

        if (isModified)
        {
            auto sig = pImg->signal< sight::data::Object::ModifiedSignalType >( sight::data::Object::s_MODIFIED_SIG );
            core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        }
    }
}

//-----------------------------------------------------------------------------

void MedicalImageSrv::starting()
{
    this->convertImage();
}

//-----------------------------------------------------------------------------

void MedicalImageSrv::stopping()
{
}

//-----------------------------------------------------------------------------

void MedicalImageSrv::swapping()
{
    this->convertImage();
}

//-----------------------------------------------------------------------------

void MedicalImageSrv::configuring()
{
}

//-----------------------------------------------------------------------------

void MedicalImageSrv::reconfiguring()
{
}

//-----------------------------------------------------------------------------

void MedicalImageSrv::updating()
{
    this->convertImage();
}

//-----------------------------------------------------------------------------

void MedicalImageSrv::info( std::ostream& )
{
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap MedicalImageSrv::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push("image", sight::data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push("image", sight::data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

} // sight::modules::data
