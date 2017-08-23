/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SModelSeries.hpp"

#include "visuOgreAdaptor/SMesh.hpp"
#include "visuOgreAdaptor/SReconstruction.hpp"
#include <visuOgreAdaptor/defines.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SModelSeries, ::fwMedData::ModelSeries);

namespace visuOgreAdaptor
{
//-----------------------------------------------------------------------------

static const ::fwCom::Slots::SlotKeyType s_ADD_RECONSTRUCTION_SLOT    = "addReconstruction";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_RECONSTRUCTION_SLOT = "removeReconstruction";

//------------------------------------------------------------------------------

SModelSeries::SModelSeries() noexcept :
    m_autoResetCamera(true),
    m_materialTemplateName(SMaterial::DEFAULT_MATERIAL_TEMPLATE_NAME),
    m_isDynamic(false),
    m_isDynamicVertices(false)
{
    newSlot(s_ADD_RECONSTRUCTION_SLOT, &SModelSeries::addReconstruction, this);
    newSlot(s_REMOVE_RECONSTRUCTION_SLOT, &SModelSeries::removeReconstruction, this);
}

//------------------------------------------------------------------------------

SModelSeries::~SModelSeries() noexcept
{
}

//------------------------------------------------------------------------------

void SModelSeries::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    if (config.count("transform"))
    {
        this->setTransformId(config.get<std::string>("transform"));
    }

    if (config.count("autoresetcamera"))
    {
        std::string autoresetcamera = config.get<std::string>("autoresetcamera");
        m_autoResetCamera = (autoresetcamera == "yes");
    }

    if (config.count("material"))
    {
        m_materialTemplateName = config.get<std::string>("material");
    }

    if(config.count("dynamic"))
    {
        m_isDynamic = config.get<bool>("dynamic");
    }

    if(config.count("dynamicVertices"))
    {
        m_isDynamicVertices = config.get<bool>("dynamicVertices");
    }
}

//------------------------------------------------------------------------------

void SModelSeries::starting()
{
    this->initialize();

    this->updating();
}

//------------------------------------------------------------------------------

void SModelSeries::updating()
{
    // Retrieves the associated f4s ModelSeries object
    ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();

    this->stopping();

    // showRec indicates if we have to show the associated reconstructions or not
    const bool showRec = modelSeries->getField("ShowReconstructions", ::fwData::Boolean::New(true))->value();

    for(auto reconstruction : modelSeries->getReconstructionDB())
    {
        ::fwRenderOgre::IAdaptor::sptr service =
            ::fwServices::add< ::fwRenderOgre::IAdaptor >(reconstruction, "::visuOgreAdaptor::SReconstruction");
        SLM_ASSERT("service not instantiated", service);

        // We use the default service ID to get a unique number because a ModelSeries contains several Reconstructions
        service->setID(this->getID() + "_" + service->getID());

        service->setRenderService(this->getRenderService());
        service->setLayerID(m_layerID);
        auto reconstructionAdaptor = ::visuOgreAdaptor::SReconstruction::dynamicCast(service);

        reconstructionAdaptor->setTransformId(reconstructionAdaptor->getID() + "_TF");
        reconstructionAdaptor->setMaterialTemplateName(m_materialTemplateName);
        reconstructionAdaptor->setParentTransformId(this->getTransformId());
        reconstructionAdaptor->setAutoResetCamera(m_autoResetCamera);

        service->start();
        service->connect();
        reconstructionAdaptor->setForceHide(!showRec);

        this->registerService(service);

        ::visuOgreAdaptor::SMesh::sptr meshAdaptor = reconstructionAdaptor->getMeshAdaptor();
        meshAdaptor->setDynamic(m_isDynamic);
        meshAdaptor->setDynamicVertices(m_isDynamicVertices);
    }
}

//------------------------------------------------------------------------------

void SModelSeries::swapping()
{
    this->updating();
}

//------------------------------------------------------------------------------

void SModelSeries::stopping()
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void SModelSeries::addReconstruction()
{
    this->updating();
}

//------------------------------------------------------------------------------

void SModelSeries::removeReconstruction()
{
    this->stopping();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SModelSeries::getObjSrvConnections() const
{
    ::fwServices::IService::KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwMedData::ModelSeries::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG,
                                           s_ADD_RECONSTRUCTION_SLOT ) );
    connections.push_back( std::make_pair( ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG,
                                           s_REMOVE_RECONSTRUCTION_SLOT ) );
    return connections;
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor
