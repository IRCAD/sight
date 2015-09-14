/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SNegato2D.hpp"

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>

#include <fwRenderOgre/Plane.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>

#include <algorithm>

#include <OgreHardwarePixelBuffer.h>
#include <OgreCamera.h>
#include <OgreSceneNode.h>
#include <OgreTextureManager.h>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SNegato2D, ::fwData::Image);

const ::fwCom::Slots::SlotKeyType s_NEWIMAGE_SLOT   = "newImage";
const ::fwCom::Slots::SlotKeyType s_SLICETYPE_SLOT  = "sliceType";
const ::fwCom::Slots::SlotKeyType s_SLICEINDEX_SLOT = "sliceIndex";

//------------------------------------------------------------------------------

SNegato2D::SNegato2D() throw() :
    ::fwComEd::helper::MedicalImageAdaptor(),
    m_plane(nullptr),
    m_negatoSceneNode(nullptr),
    m_interpolation(true)
{
    SLM_TRACE_FUNC();

    this->installTFSlots(this);

    m_currentSliceIndex = {0.f, 0.f, 0.f};

    newSlot(s_NEWIMAGE_SLOT, &SNegato2D::newImage, this);
    newSlot(s_SLICETYPE_SLOT, &SNegato2D::changeSliceType, this);
    newSlot(s_SLICEINDEX_SLOT, &SNegato2D::changeSliceIndex, this);
}

//------------------------------------------------------------------------------

SNegato2D::~SNegato2D() throw()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SNegato2D::doStart() throw(::fwTools::Failed)
{
    this->updateImageInfos(this->getObject< ::fwData::Image >());
    this->updateTransferFunction(this->getImage());

    // Texture instantiation
    m_3DOgreTexture = ::Ogre::TextureManager::getSingleton().create(
        this->getID() + "_Texture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);

    // Scene node's instanciation
    m_negatoSceneNode = this->getSceneManager()->getRootSceneNode()->createChildSceneNode();

    // Plane's instanciation
    m_plane = new ::fwRenderOgre::Plane(this->getID(), m_negatoSceneNode, getSceneManager(),
                                        OrientationMode::X_AXIS, false, m_3DOgreTexture);

    this->getSceneManager()->getCamera("PlayerCam")->setProjectionType( ::Ogre::ProjectionType::PT_ORTHOGRAPHIC );

    this->installTFConnections();

    bool isValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(this->getImage());
    if(isValid)
    {
        this->newImage();
    }
}

//------------------------------------------------------------------------------

void SNegato2D::doStop() throw(::fwTools::Failed)
{
    this->removeTFConnections();

    if (!m_connection.expired())
    {
        m_connection.disconnect();
    }

    m_plane->removeAndDestroyPlane();
    delete m_plane;

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::doConfigure() throw(::fwTools::Failed)
{
    SLM_ASSERT("No config tag", m_configuration->getName() == "config");

    if(m_configuration->hasAttribute("sliceIndex"))
    {
        std::string orientation = m_configuration->getAttributeValue("sliceIndex");

        if(orientation == "axial")
        {
            m_orientation = Z_AXIS;
        }
        else if(orientation == "frontal")
        {
            m_orientation = Y_AXIS;
        }
        else if(orientation == "sagittal")
        {
            m_orientation = X_AXIS;
        }
    }
    else
    {
        // Axis orientation mode by default
        m_orientation = OrientationMode::Z_AXIS;
    }

    if(m_configuration->hasAttribute("interpolation"))
    {
        // Whatever you specify, yes by default, needs explicit "no" to remove interpolation.
        this->setInterpolation(!(m_configuration->getAttributeValue("interpolation") == "no"));
    }

    this->parseTFConfig(m_configuration);
}

//------------------------------------------------------------------------------

void SNegato2D::doUpdate() throw(::fwTools::Failed)
{
    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::newImage()
{
    this->getRenderService()->makeCurrent();

    ::fwData::Image::sptr image = this->getImage();

    // Retrieves or creates the slice index fields
    this->updateImageInfos(image);

    ::fwRenderOgre::Utils::convertImageForNegato(m_3DOgreTexture.get(), image);

    this->createPlane( image->getSpacing() );
    this->updateCameraWindowBounds();

    // Update Slice
    this->changeSliceIndex(m_axialIndex->value(), m_frontalIndex->value(), m_sagittalIndex->value());

    // Update TF
    this->updatingTFWindowing(this->getTransferFunction()->getWindow(), this->getTransferFunction()->getLevel());

    // Update threshold if necessary
    this->updatingTFPoints();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::changeSliceType(int _from, int _to)
{
    OrientationMode newOrientationMode;

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

    // Update TF
    this->updatingTFWindowing(this->getTransferFunction()->getWindow(), this->getTransferFunction()->getLevel());

    // Update threshold if necessary
    this->updatingTFPoints();

    this->updateShaderSliceIndexParameter();
}

//------------------------------------------------------------------------------

void SNegato2D::changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex)
{
    ::fwData::Image::sptr image = this->getImage();

    m_axialIndex->value()    = _axialIndex;
    m_frontalIndex->value()  = _frontalIndex;
    m_sagittalIndex->value() = _sagittalIndex;

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

void SNegato2D::updatingTFPoints()
{
    m_plane->switchThresholding(this->getTransferFunction()->getIsClamped());

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::updatingTFWindowing(double _window, double _level)
{
    float minVal = static_cast<float>(_level) - static_cast<float>(_window) / 2.f;
    float maxVal = static_cast<float>(_level) + static_cast<float>(_window) / 2.f;
    m_plane->setWindowing(minVal, maxVal);

    this->requestRender();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SNegato2D::getObjSrvConnections() const
{
    ::fwServices::IService::KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_NEWIMAGE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_NEWIMAGE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SLICETYPE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_SLICEINDEX_SLOT ) );

    return connections;

}

//------------------------------------------------------------------------------

void SNegato2D::createPlane(const fwData::Image::SpacingType& _spacing)
{
    this->getRenderService()->makeCurrent();
    // Fits the plane to the new texture
    m_plane->setDepthSpacing(_spacing);
    m_plane->initialize2DPlane();
}

//------------------------------------------------------------------------------

void SNegato2D::doSwap() throw(fwTools::Failed)
{
    this->doStop();
    this->doStart();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void SNegato2D::updateCameraWindowBounds()
{

    ::Ogre::Real renderWindowWidth,renderWindowHeight, renderWindowRatio;
    ::Ogre::RenderSystem *renderSystem = getSceneManager()->getDestinationRenderSystem();

    renderWindowWidth  = static_cast< ::Ogre::Real >(renderSystem->_getViewport()->getActualWidth());
    renderWindowHeight = static_cast< ::Ogre::Real >(renderSystem->_getViewport()->getActualHeight());
    renderWindowRatio  = renderWindowWidth / renderWindowHeight;

    ::Ogre::Camera* cam = this->getSceneManager()->getCamera("PlayerCam");

    if( renderWindowWidth < renderWindowHeight)
    {
        cam->setOrthoWindowWidth(m_plane->getWidth());
    }
    else
    {
        cam->setOrthoWindowHeight(m_plane->getHeight());
    }
    cam->setAspectRatio( renderWindowRatio );

    this->requestRender();
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor
