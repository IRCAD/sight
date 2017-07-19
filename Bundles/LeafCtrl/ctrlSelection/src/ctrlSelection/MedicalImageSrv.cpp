/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSelection/MedicalImageSrv.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwServices/macros.hpp>

namespace ctrlSelection
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IController, ::ctrlSelection::MedicalImageSrv, ::fwData::Image );

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
    ::fwData::Image::sptr pImg;

    if (this->isVersion2())
    {
        pImg = this->getInOut< ::fwData::Image >("image");
    }
    else
    {
        pImg = this->getObject< ::fwData::Image >();
    }
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

void MedicalImageSrv::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType MedicalImageSrv::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//-----------------------------------------------------------------------------

} // ctrlSelection
