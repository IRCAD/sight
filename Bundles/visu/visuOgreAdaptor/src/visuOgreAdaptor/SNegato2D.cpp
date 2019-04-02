/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "visuOgreAdaptor/SNegato2D.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>

#include <OgreCamera.h>
#include <OgreSceneNode.h>
#include <OgreTextureManager.h>

#include <algorithm>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SNegato2D, ::fwData::Image);

const ::fwCom::Slots::SlotKeyType s_NEWIMAGE_SLOT   = "newImage";
const ::fwCom::Slots::SlotKeyType s_SLICETYPE_SLOT  = "sliceType";
const ::fwCom::Slots::SlotKeyType s_SLICEINDEX_SLOT = "sliceIndex";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";
static const ::fwServices::IService::KeyType s_TF_INOUT    = "tf";

static const std::string s_ENABLE_APLHA_CONFIG = "tfalpha";

//------------------------------------------------------------------------------

SNegato2D::SNegato2D() noexcept :
    m_plane(nullptr),
    m_negatoSceneNode(nullptr),
    m_filtering( ::fwRenderOgre::Plane::FilteringEnumType::NONE ),
    m_helperTF(std::bind(&SNegato2D::updateTF, this))
{
    m_currentSliceIndex = {0.f, 0.f, 0.f};

    newSlot(s_NEWIMAGE_SLOT, &SNegato2D::newImage, this);
    newSlot(s_SLICETYPE_SLOT, &SNegato2D::changeSliceType, this);
    newSlot(s_SLICEINDEX_SLOT, &SNegato2D::changeSliceIndex, this);
}

//------------------------------------------------------------------------------

SNegato2D::~SNegato2D() noexcept
{
}

//------------------------------------------------------------------------------

void SNegato2D::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    if(config.count("sliceIndex"))
    {
        const std::string orientation = config.get<std::string>("sliceIndex");

        if(orientation == "axial")
        {
            m_orientation = OrientationMode::Z_AXIS;
        }
        else if(orientation == "frontal")
        {
            m_orientation = OrientationMode::Y_AXIS;
        }
        else if(orientation == "sagittal")
        {
            m_orientation = OrientationMode::X_AXIS;
        }
    }
    else
    {
        // Axis orientation mode by default
        m_orientation = OrientationMode::Z_AXIS;
    }

    if(config.count("filtering"))
    {
        const std::string filteringValue = config.get<std::string>("filtering");
        ::fwRenderOgre::Plane::FilteringEnumType filtering(::fwRenderOgre::Plane::FilteringEnumType::LINEAR);

        if(filteringValue == "none")
        {
            filtering = ::fwRenderOgre::Plane::FilteringEnumType::NONE;
        }
        else if(filteringValue == "anisotropic")
        {
            filtering = ::fwRenderOgre::Plane::FilteringEnumType::ANISOTROPIC;
        }

        this->setFiltering(filtering);
    }

    m_enableAlpha = config.get<bool>(s_ENABLE_APLHA_CONFIG, m_enableAlpha);
}

//------------------------------------------------------------------------------

void SNegato2D::starting()
{
    this->initialize();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing.", image);

    ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
    m_helperTF.setOrCreateTF(tf, image);

    // 3D source texture instantiation
    m_3DOgreTexture = ::Ogre::TextureManager::getSingleton().create(
        this->getID() + "_Texture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);

    // TF texture initialization
    m_gpuTF = std::unique_ptr< ::fwRenderOgre::TransferFunction>(new ::fwRenderOgre::TransferFunction());
    m_gpuTF->createTexture(this->getID());

    // Scene node's instanciation
    m_negatoSceneNode = this->getSceneManager()->getRootSceneNode()->createChildSceneNode();

    // Plane's instanciation
    m_plane = new ::fwRenderOgre::Plane(this->getID(), m_negatoSceneNode, getSceneManager(),
                                        m_orientation, false, m_3DOgreTexture, m_filtering);

    ::Ogre::Camera* cam = this->getLayer()->getDefaultCamera();
    m_cameraNode        = cam->getParentSceneNode();
    cam->setProjectionType( ::Ogre::ProjectionType::PT_ORTHOGRAPHIC );

    bool isValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image);
    if(isValid)
    {
        this->newImage();
    }
}

//------------------------------------------------------------------------------

void SNegato2D::stopping()
{
    m_helperTF.removeTFConnections();

    if (!m_connection.expired())
    {
        m_connection.disconnect();
    }

    delete m_plane;

    m_3DOgreTexture.reset();
    m_gpuTF.reset();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::updating()
{
    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::swapping(const KeyType& key)
{
    if (key == s_TF_INOUT)
    {
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("Missing image", image);

        ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
        m_helperTF.setOrCreateTF(tf, image);

        this->updateTF();
    }
}

//------------------------------------------------------------------------------

void SNegato2D::newImage()
{
    if(!m_3DOgreTexture)
    {
        // The adaptor hasn't start yet (the window is maybe not visible)
        return;
    }
    this->getRenderService()->makeCurrent();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    // Retrieves or creates the slice index fields
    ::fwRenderOgre::Utils::convertImageForNegato(m_3DOgreTexture.get(), image);

    this->createPlane( image->getSpacing() );
    this->updateCamera();

    // Update Slice
    int axialIndex =
        image->getField< ::fwData::Integer >(::fwDataTools::fieldHelper::Image::m_axialSliceIndexId)->getValue();
    int frontalIndex =
        image->getField< ::fwData::Integer >(::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId)->getValue();
    int sagittalIndex =
        image->getField< ::fwData::Integer >(::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId)->getValue();

    this->changeSliceIndex(axialIndex, frontalIndex, sagittalIndex);

    // Update tranfer function in Gpu programs
    this->updateTF();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::changeSliceType(int /*_from*/, int _to)
{
    OrientationMode newOrientationMode = OrientationMode::X_AXIS;

    switch (_to)
    {
        case 0:
            newOrientationMode = OrientationMode::X_AXIS;
            break;
        case 1:
            newOrientationMode = OrientationMode::Y_AXIS;
            break;
        case 2:
            newOrientationMode = OrientationMode::Z_AXIS;
            break;
    }

    // The orientation update setter will change the fragment shader
    m_plane->setOrientationMode(newOrientationMode);
    m_orientation = newOrientationMode;
    this->updateCamera();

    // Update threshold if necessary
    this->updateTF();

    this->updateShaderSliceIndexParameter();
}

//------------------------------------------------------------------------------

void SNegato2D::changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex)
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    m_currentSliceIndex = {
        static_cast<float>(_sagittalIndex ) / (static_cast<float>(image->getSize()[0] - 1)),
        static_cast<float>(_frontalIndex  ) / (static_cast<float>(image->getSize()[1] - 1)),
        static_cast<float>(_axialIndex    ) / (static_cast<float>(image->getSize()[2] - 1))
    };

    this->updateShaderSliceIndexParameter();
}

//------------------------------------------------------------------------------

void SNegato2D::updateShaderSliceIndexParameter()
{
    this->getRenderService()->makeCurrent();
    m_plane->changeSlice( m_currentSliceIndex[static_cast<size_t>(m_plane->getOrientationMode())] );

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::updateTF()
{
    const ::fwData::TransferFunction::csptr tf = m_helperTF.getTransferFunction();
    {
        const ::fwData::mt::ObjectReadLock tfLock(tf);
        m_gpuTF->updateTexture(tf);

        m_plane->switchThresholding(tf->getIsClamped());
    }

    // Sends the TF texture to the negato-related passes
    m_plane->setTFData(*m_gpuTF.get());

    this->requestRender();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SNegato2D::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_NEWIMAGE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_NEWIMAGE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SLICETYPE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_SLICEINDEX_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

void SNegato2D::createPlane(const fwData::Image::SpacingType& _spacing)
{
    this->getRenderService()->makeCurrent();
    // Fits the plane to the new texture
    m_plane->setDepthSpacing(_spacing);
    m_plane->initialize2DPlane();
    m_plane->enableAlpha(m_enableAlpha);
}

//------------------------------------------------------------------------------

void SNegato2D::updateCamera()
{

    ::Ogre::Real renderWindowWidth, renderWindowHeight, renderWindowRatio;
    ::Ogre::RenderSystem* renderSystem = getSceneManager()->getDestinationRenderSystem();

    renderWindowWidth  = static_cast< ::Ogre::Real >(renderSystem->_getViewport()->getActualWidth());
    renderWindowHeight = static_cast< ::Ogre::Real >(renderSystem->_getViewport()->getActualHeight());
    renderWindowRatio  = renderWindowWidth / renderWindowHeight;

    ::Ogre::Camera* cam = this->getLayer()->getDefaultCamera();

    if( static_cast<int>(renderWindowWidth) == static_cast<int>(renderWindowHeight) )
    {
        cam->setOrthoWindow(m_plane->getWidth(), m_plane->getHeight());
    }
    else if( renderWindowWidth > renderWindowHeight)
    {
        cam->setOrthoWindowHeight(m_plane->getHeight());
    }
    else
    {
        cam->setOrthoWindowWidth(m_plane->getWidth());
    }
    cam->setAspectRatio( renderWindowRatio );

    m_cameraNode->setPosition(::Ogre::Vector3(0, 0, 0));
    m_cameraNode->resetOrientation();
    switch(m_orientation)
    {
        case OrientationMode::X_AXIS:
            m_cameraNode->rotate(::Ogre::Vector3(0, 1, 0), ::Ogre::Degree(::Ogre::Real(90)));
            m_cameraNode->rotate(::Ogre::Vector3(0, 0, 1), ::Ogre::Degree(::Ogre::Real(90)));
            m_cameraNode->translate(::Ogre::Vector3(static_cast<float>(m_3DOgreTexture->getWidth()),
                                                    m_plane->getWidth()/2,
                                                    m_plane->getHeight()/2));
            break;
        case OrientationMode::Y_AXIS:
            m_cameraNode->rotate(::Ogre::Vector3(1, 0, 0), ::Ogre::Degree(::Ogre::Real(-90)));
            m_cameraNode->translate(::Ogre::Vector3(m_plane->getWidth()/2,
                                                    static_cast<float>(m_3DOgreTexture->getHeight()),
                                                    m_plane->getHeight()/2));
            break;
        case OrientationMode::Z_AXIS:
            m_cameraNode->translate(::Ogre::Vector3(m_plane->getWidth()/2, m_plane->getHeight()/2,
                                                    static_cast<float>(m_3DOgreTexture->getDepth())));
            break;
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor
