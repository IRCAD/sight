/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "visuVTKAdaptor/SResectionDB.hpp"

#include "visuVTKAdaptor/SResection.hpp"

#include <fwData/Resection.hpp>
#include <fwData/ResectionDB.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SResectionDB);

namespace visuVTKAdaptor
{

static const ::fwServices::IService::KeyType s_RESECTIONDB_INPUT = "resectionDB";

//------------------------------------------------------------------------------

SResectionDB::SResectionDB() noexcept
{
}

//------------------------------------------------------------------------------

SResectionDB::~SResectionDB() noexcept
{
}

//------------------------------------------------------------------------------

void SResectionDB::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    this->setClippingPlanes(config.get("clippingplanes", ""));

    const std::string autoresetcamera = config.get<std::string>("autoresetcamera", "yes");
    SLM_ASSERT("'autoresetcamera' value must be 'yes' or 'no', actual: " + autoresetcamera,
               autoresetcamera == "yes" || autoresetcamera == "no");
    m_autoResetCamera = (autoresetcamera == "yes");
}

//------------------------------------------------------------------------------

void SResectionDB::starting()
{
    this->initialize();
    this->updating();
}

//------------------------------------------------------------------------------

void SResectionDB::updating()
{
    this->stopping();

    ::fwData::ResectionDB::csptr resecDB = this->getInput< ::fwData::ResectionDB >(s_RESECTIONDB_INPUT);

    for( const ::fwData::Resection::sptr& resection :  resecDB->getResections())
    {
        // create the srv configuration for objects auto-connection
        auto resecSrv = this->registerService< ::visuVTKAdaptor::SResection>("::visuVTKAdaptor::SResection");
        resecSrv->registerInput(resection, SResection::s_RESECTION_INPUT, true);

        resecSrv->setTransformId( this->getTransformId() );
        resecSrv->setRendererId( this->getRendererId() );
        resecSrv->setPickerId( this->getPickerId() );
        resecSrv->setRenderService(this->getRenderService());

        resecSrv->setClippingPlanes( m_clippingPlanes );
        resecSrv->setAutoResetCamera(m_autoResetCamera);
        resecSrv->start();

    }

    // Safe part
    ::fwData::Resection::sptr resec = resecDB->getSafeResection();
    if(resec)
    {
        // create the srv configuration for objects auto-connection
        auto service = this->registerService< ::visuVTKAdaptor::SResection>("::visuVTKAdaptor::SResection");
        service->registerInput(resec, SResection::s_RESECTION_INPUT, true);

        service->setTransformId( this->getTransformId() );
        service->setRendererId( this->getRendererId() );
        service->setPickerId( this->getPickerId() );
        service->setRenderService(this->getRenderService());
        service->setAutoResetCamera(m_autoResetCamera);

        service->start();
    }
}

//------------------------------------------------------------------------------

void SResectionDB::stopping()
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SResectionDB::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_RESECTIONDB_INPUT, ::fwData::ResectionDB::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_RESECTIONDB_INPUT, ::fwData::ResectionDB::s_SAFE_PART_ADDED_SIG, s_UPDATE_SLOT);
    connections.push(s_RESECTIONDB_INPUT, ::fwData::ResectionDB::s_RESECTION_ADDED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
