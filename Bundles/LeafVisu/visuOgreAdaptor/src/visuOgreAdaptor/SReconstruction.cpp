/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SReconstruction.hpp"

#include <visuOgreAdaptor/defines.hpp>

#include <fwCom/Slots.hxx>

#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SReconstruction, ::fwData::Reconstruction);

const ::fwCom::Slots::SlotKeyType SReconstruction::s_CHANGE_MESH_SLOT = "changeMesh";
const ::fwCom::Slots::SlotKeyType SReconstruction::s_VISIBILITY_SLOT  = "modifyVisibility";

//------------------------------------------------------------------------------

SReconstruction::SReconstruction() throw() :
    m_autoResetCamera(true),
    m_materialTemplateName(SMaterial::DEFAULT_MATERIAL_TEMPLATE_NAME),
    m_isDynamic(false),
    m_isDynamicVertices(false)
{
    m_slots(s_CHANGE_MESH_SLOT, &SReconstruction::changeMesh, this);
    m_slots(s_VISIBILITY_SLOT, &SReconstruction::modifyVisibility, this);
}

//------------------------------------------------------------------------------

SReconstruction::~SReconstruction() throw()
{
}

//------------------------------------------------------------------------------

void SReconstruction::doConfigure() throw(::fwTools::Failed)
{
    // The transform attribute is mandatory in the XML configuration
    this->setTransformId(m_configuration->getAttributeValue("transform"));

    if (m_configuration->hasAttribute("autoresetcamera"))
    {
        std::string autoresetcamera = m_configuration->getAttributeValue("autoresetcamera");
        m_autoResetCamera = (autoresetcamera == "yes");
    }
}

//------------------------------------------------------------------------------

void SReconstruction::doStart() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    createMeshService();
}

//------------------------------------------------------------------------------

void SReconstruction::createMeshService()
{
    // Retrieves the associated Reconstruction object
    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction >();
    ::fwData::Mesh::sptr mesh                     = reconstruction->getMesh();

    SLM_TRACE_IF("Mesh is null", !mesh);
    if (mesh)
    {
        // Creates an Ogre adaptor and associates it with the f4s mesh object
        ::fwRenderOgre::IAdaptor::sptr meshService;
        meshService = ::fwServices::add< ::fwRenderOgre::IAdaptor >(
            mesh,
            "::visuOgreAdaptor::SMesh");

        // If the adaptor has been well instantiated, we can cast it into a mesh adaptor
        SLM_ASSERT("meshService not instantiated", meshService);
        ::visuOgreAdaptor::SMesh::sptr meshAdaptor = ::visuOgreAdaptor::SMesh::dynamicCast(meshService);

        meshService->setID(this->getID() + meshService->getID());
        meshService->setLayerID(m_layerID);
        meshService->setRenderService(this->getRenderService());

        meshAdaptor->setIsReconstructionManaged(true);
        meshAdaptor->setMaterial(reconstruction->getMaterial());
        meshAdaptor->setMaterialTemplateName(m_materialTemplateName);
        meshAdaptor->setAutoResetCamera(m_autoResetCamera);
        meshAdaptor->setTransformId(this->getTransformId());
        meshAdaptor->setParentTransformId(this->getParentTransformId());
        meshAdaptor->updateVisibility(reconstruction->getIsVisible());
        meshAdaptor->setDynamic(m_isDynamic);
        meshAdaptor->setDynamicVertices(m_isDynamicVertices);

        meshService->start();
        meshService->connect();

        m_meshAdaptor = meshService;
        this->registerService(meshService);
        OSLM_TRACE("Mesh is visible : " << reconstruction->getIsVisible());
        OSLM_TRACE("Mesh nb points : " << mesh->getNumberOfPoints());
    }
}

//------------------------------------------------------------------------------

void SReconstruction::doSwap() throw(::fwTools::Failed)
{
    this->updating();
}

//------------------------------------------------------------------------------

void SReconstruction::doUpdate() throw(::fwTools::Failed)
{
    if (!m_meshAdaptor.expired())
    {
        ::visuOgreAdaptor::SMesh::sptr meshAdaptor = this->getMeshAdaptor();

        // Retrieves the associated f4s reconstruction object
        ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction >();

        // Updates the mesh adaptor according to the reconstruction
        meshAdaptor->setMaterial(reconstruction->getMaterial());
        meshAdaptor->swap(reconstruction->getMesh());
        meshAdaptor->updateVisibility(reconstruction->getIsVisible());
    }
    else
    {
        // If m_meshService does not exists, we have to create it
        this->createMeshService();
    }
}

//------------------------------------------------------------------------------

void SReconstruction::doStop() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void SReconstruction::setForceHide(bool _hide)
{
    SLM_TRACE_FUNC();

    if (!m_meshAdaptor.expired())
    {
        ::visuOgreAdaptor::SMesh::sptr meshAdaptor = this->getMeshAdaptor();

        if (meshAdaptor)
        {
            ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction >();
            meshAdaptor->updateVisibility(_hide ? false : reconstruction->getIsVisible());
        }
    }
}

//------------------------------------------------------------------------------

void SReconstruction::changeMesh( SPTR( ::fwData::Mesh) )
{
    if (!m_meshAdaptor.expired())
    {
        ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction >();
        SLM_ASSERT("reconstruction not instantiated", reconstruction);
        this->updating();
    }
    else
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

void SReconstruction::modifyVisibility()
{
    if (!m_meshAdaptor.expired())
    {
        ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction >();
        SLM_ASSERT("reconstruction not instantiated", reconstruction);

        this->setForceHide(!reconstruction->getIsVisible());
    }
}

//------------------------------------------------------------------------------

::visuOgreAdaptor::SMesh::sptr SReconstruction::getMeshAdaptor()
{
    // Retrieves the associated mesh adaptor
    ::fwRenderOgre::IAdaptor::sptr adaptor     = m_meshAdaptor.lock();
    ::visuOgreAdaptor::SMesh::sptr meshAdaptor = ::visuOgreAdaptor::SMesh::dynamicCast(adaptor);

    return meshAdaptor;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType visuOgreAdaptor::SReconstruction::getObjSrvConnections() const
{

    ::fwServices::IService::KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Reconstruction::s_MESH_CHANGED_SIG, s_CHANGE_MESH_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Reconstruction::s_VISIBILITY_MODIFIED_SIG, s_VISIBILITY_SLOT ) );
    return connections;
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor
