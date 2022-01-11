/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>

#include <service/macros.hpp>

namespace sight::module::data
{

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

MedicalImageSrv::MedicalImageSrv() noexcept
{
}

//-----------------------------------------------------------------------------

MedicalImageSrv::~MedicalImageSrv() noexcept
{
}

//-----------------------------------------------------------------------------

void MedicalImageSrv::convertImage()
{
    auto pImg = m_image.lock();

    if(sight::data::helper::MedicalImage::checkImageValidity(pImg.get_shared()))
    {
        bool isModified = false;
        isModified |= sight::data::helper::MedicalImage::checkLandmarks(pImg.get_shared());
        isModified |= sight::data::helper::MedicalImage::checkTransferFunctionPool(pImg.get_shared());
        isModified |= sight::data::helper::MedicalImage::checkImageSliceIndex(pImg.get_shared());

        if(isModified)
        {
            auto sig = pImg->signal<sight::data::Object::ModifiedSignalType>(sight::data::Object::s_MODIFIED_SIG);
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

void MedicalImageSrv::info(std::ostream&)
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

} // sight::module::data
