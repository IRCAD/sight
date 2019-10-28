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

#include "ctrlSelection/MedicalImageSrv.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwServices/macros.hpp>

namespace ctrlSelection
{

static const std::string s_IMAGE_INOUT = "image";

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IController, ::ctrlSelection::MedicalImageSrv, ::fwData::Image )

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
    ::fwData::Image::sptr pImg = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(pImg))
    {
        ::fwDataTools::helper::Image helper( pImg );

        bool isModified = false;
        isModified |= helper.createLandmarks();
        isModified |= helper.createTransferFunctionPool();
        isModified |= helper.createImageSliceIndex();

        if (isModified)
        {
            auto sig = pImg->signal< ::fwData::Object::ModifiedSignalType >( ::fwData::Object::s_MODIFIED_SIG );
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            helper.notify();
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

::fwServices::IService::KeyConnectionsMap MedicalImageSrv::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push("image", ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push("image", ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

} // ctrlSelection
