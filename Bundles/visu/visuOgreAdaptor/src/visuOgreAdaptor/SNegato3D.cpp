/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SNegato3D.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>

#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTextureManager.h>

#include <algorithm>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SNegato3D, ::fwData::Image);

const ::fwCom::Slots::SlotKeyType SNegato3D::s_NEWIMAGE_SLOT          = "newImage";
const ::fwCom::Slots::SlotKeyType SNegato3D::s_SLICETYPE_SLOT         = "sliceType";
const ::fwCom::Slots::SlotKeyType SNegato3D::s_SLICEINDEX_SLOT        = "sliceIndex";
const ::fwCom::Slots::SlotKeyType SNegato3D::s_UPDATE_OPACITY_SLOT    = "updateOpacity";
const ::fwCom::Slots::SlotKeyType SNegato3D::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const ::fwCom::Slots::SlotKeyType SNegato3D::s_SET_VISIBILITY_SLOT    = "setVisibility";

static const std::string s_IMAGE_INOUT = "image";
static const std::string s_TF_INOUT    = "tf";

static const std::string s_ENABLE_APLHA_CONFIG = "tfalpha";

static const std::string TRANSPARENCY_FIELD = "TRANSPARENCY";
static const std::string VISIBILITY_FIELD   = "VISIBILITY";

//------------------------------------------------------------------------------

SNegato3D::SNegato3D() noexcept :
    m_helperTF(std::bind(&SNegato3D::updateTF, this)),
    m_autoResetCamera(true),
    m_activePlane(nullptr),
    m_negatoSceneNode(nullptr),
    m_filtering( ::fwRenderOgre::Plane::FilteringEnumType::NONE )
{
    newSlot(s_NEWIMAGE_SLOT, &SNegato3D::newImage, this);
    newSlot(s_SLICETYPE_SLOT, &SNegato3D::changeSliceType, this);
    newSlot(s_SLICEINDEX_SLOT, &SNegato3D::changeSliceIndex, this);
    newSlot(s_UPDATE_OPACITY_SLOT, &SNegato3D::setPlanesOpacity, this);
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SNegato3D::setPlanesOpacity, this);
    newSlot(s_SET_VISIBILITY_SLOT, &SNegato3D::setVisibility, this);
}

//------------------------------------------------------------------------------

SNegato3D::~SNegato3D() noexcept
{
}

//------------------------------------------------------------------------------

void SNegato3D::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    // Axis orientation mode by default
    m_orientation = OrientationMode::Z_AXIS;

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
    if(config.count("autoresetcamera"))
    {
        m_autoResetCamera = config.get<std::string>("autoresetcamera") == "yes";
    }
    if(config.count("transform"))
    {
        this->setTransformId(config.get<std::string>("transform"));
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

void SNegato3D::starting()
{
    this->initialize();

    ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
    if(tf != nullptr)
    {
        const ::fwData::mt::ObjectWriteLock tfLock(tf);
        m_helperTF.setTransferFunction(tf);
    }
    else
    {
        m_helperTF.setTransferFunction(tf);
    }

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    m_helperTF.createTransferFunction(image);

    // 3D source texture instantiation
    m_3DOgreTexture = ::Ogre::dynamic_pointer_cast< ::Ogre::Texture >(
        ::Ogre::TextureManager::getSingleton().createOrRetrieve(this->getID() + "_Texture",
                                                                ::Ogre::ResourceGroupManager::
                                                                DEFAULT_RESOURCE_GROUP_NAME, true).first );

    // TF texture initialization
    m_gpuTF = std::unique_ptr< ::fwRenderOgre::TransferFunction>(new ::fwRenderOgre::TransferFunction());
    m_gpuTF->createTexture(this->getID());

    // Scene node's instantiation
    m_negatoSceneNode = this->getSceneManager()->getRootSceneNode()->createChildSceneNode();

    // Instanciation of the planes
    m_planes[0] = new ::fwRenderOgre::Plane(this->getID(), m_negatoSceneNode,
                                            getSceneManager(),
                                            OrientationMode::X_AXIS, true, m_3DOgreTexture, m_filtering);
    m_planes[1] = new ::fwRenderOgre::Plane(this->getID(), m_negatoSceneNode,
                                            getSceneManager(),
                                            OrientationMode::Y_AXIS, true, m_3DOgreTexture, m_filtering);
    m_planes[2] = new ::fwRenderOgre::Plane(this->getID(), m_negatoSceneNode,
                                            getSceneManager(),
                                            OrientationMode::Z_AXIS, true, m_3DOgreTexture, m_filtering);

    this->setPlanesOpacity();

    // Default active plane = sagittal plane.
    m_activePlane = m_planes[0];

    if (m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }

    m_helperTF.installTFConnections();

    bool isValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image);
    if (isValid)
    {
        this->newImage();
    }
}

//------------------------------------------------------------------------------

void SNegato3D::stopping()
{
    m_helperTF.removeTFConnections();

    delete m_planes[0];
    delete m_planes[1];
    delete m_planes[2];
    this->getSceneManager()->destroySceneNode(m_negatoSceneNode);

    m_3DOgreTexture.reset();
    m_gpuTF.reset();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::updating()
{
    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::swapping(const KeyType& key)
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

void SNegato3D::createPlanes(const ::fwData::Image::SpacingType& _spacing, const ::fwData::Image::OriginType& _origin)
{
    ::Ogre::Vector3 origin(static_cast< ::Ogre::Real >(_origin[0]),
                           static_cast< ::Ogre::Real >(_origin[1]),
                           static_cast< ::Ogre::Real >(_origin[2]));

    // Fits the planes to the new texture
    for(int i(0); i < 3; ++i)
    {
        m_planes[i]->setDepthSpacing(_spacing);
        m_planes[i]->setOriginPosition(origin);
        m_planes[i]->initialize3DPlane();
        m_planes[i]->enableAlpha(m_enableAlpha);
    }
}

//------------------------------------------------------------------------------

void SNegato3D::newImage()
{
    this->getRenderService()->makeCurrent();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    ::fwRenderOgre::Utils::convertImageForNegato(m_3DOgreTexture.get(), image);

    createPlanes(image->getSpacing(), image->getOrigin());

    // Update Slice
    this->changeSliceIndex(m_axialIndex, m_frontalIndex, m_sagittalIndex);

    // Update tranfer function in Gpu programs
    this->updateTF();

    if (m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }

    this->setPlanesOpacity();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::changeSliceType(int /*_from*/, int _to)
{
    // We have to update the active plane
    m_activePlane = m_planes[_to];

    this->getRenderService()->makeCurrent();

    // Update TF
    const ::fwData::TransferFunction::csptr tf = m_helperTF.getTransferFunction();
    {
        const ::fwData::mt::ObjectReadLock tfLock(tf);
        this->updateTF();
    }

    // Update threshold if necessary
    this->updateTF();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex)
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    float sliceIndex[3] = {
        static_cast<float>(_sagittalIndex ) / (static_cast<float>(image->getSize()[0] - 1)),
        static_cast<float>(_frontalIndex  ) / (static_cast<float>(image->getSize()[1] - 1)),
        static_cast<float>(_axialIndex    ) / (static_cast<float>(image->getSize()[2] - 1))
    };

    for (int i = 0; i < 3; ++i)
    {
        m_planes[i]->changeSlice( sliceIndex[i] );
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SNegato3D::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_NEWIMAGE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_NEWIMAGE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SLICETYPE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_SLICEINDEX_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_VISIBILITY_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_TRANSPARENCY_MODIFIED_SIG, s_UPDATE_VISIBILITY_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------

void SNegato3D::updateTF()
{
    const ::fwData::TransferFunction::csptr tf = m_helperTF.getTransferFunction();
    {
        const ::fwData::mt::ObjectReadLock tfLock(tf);
        m_gpuTF->updateTexture(tf);

        for(int i(0); i < 3; ++i)
        {
            m_planes[i]->switchThresholding(tf->getIsClamped());

            // Sends the TF texture to the negato-related passes
            m_planes[i]->setTFData(*m_gpuTF.get());
        }
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SNegato3D::setPlanesOpacity()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    ::fwData::Integer::sptr transparency = image->setDefaultField(TRANSPARENCY_FIELD, ::fwData::Integer::New(0));
    ::fwData::Boolean::sptr isVisible    = image->setDefaultField(VISIBILITY_FIELD, ::fwData::Boolean::New(true));

    const bool visible  = isVisible->getValue();
    const float opacity = (100.f - static_cast<float>(transparency->getValue()))/100.f;

    if(m_planes[0] && m_planes[1] && m_planes[2])
    {
        m_planes[0]->setVisible(visible);
        m_planes[1]->setVisible(visible);
        m_planes[2]->setVisible(visible);

        m_planes[0]->setEntityOpacity(opacity);
        m_planes[1]->setEntityOpacity(opacity);
        m_planes[2]->setEntityOpacity(opacity);
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::setVisibility(bool visibility)
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    image->setField(VISIBILITY_FIELD, ::fwData::Boolean::New(visibility));

    this->setPlanesOpacity();

    using VisModSigType = ::fwData::Image::VisibilityModifiedSignalType;
    auto visUpdateSig = image->signal<VisModSigType>( ::fwData::Image::s_VISIBILITY_MODIFIED_SIG );

    {
        ::fwCom::Connection::Blocker(visUpdateSig->getConnection(this->slot(s_UPDATE_VISIBILITY_SLOT)));
        visUpdateSig->asyncEmit(visibility);
    }
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor
