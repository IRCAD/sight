/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SInteractorStyle.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/interactor/IMovementInteractor.hpp>
#include <fwRenderOgre/interactor/IPickerInteractor.hpp>
#include <fwRenderOgre/interactor/VRWidgetsInteractor.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SInteractorStyle,
                        ::fwData::Object);

namespace visuOgreAdaptor
{

const ::fwCom::Signals::SignalKeyType visuOgreAdaptor::SInteractorStyle::s_POINT_CLICKED_SIG = "pointClickedSignal";
const ::fwCom::Slots::SlotKeyType visuOgreAdaptor::SInteractorStyle::s_POINT_CLICKED_SLOT    = "pointClickedSlot";

//------------------------------------------------------------------------------

SInteractorStyle::SInteractorStyle() throw()
{
    newSlot( s_POINT_CLICKED_SLOT, &::visuOgreAdaptor::SInteractorStyle::clickedPoint, this );

    m_sigPointClicked = newSignal< PointClickedSignalType >( s_POINT_CLICKED_SIG );

    m_interactorStyles["Trackball"] = "::fwRenderOgre::interactor::TrackballInteractor";
    m_interactorStyles["Fixed"]     = "::fwRenderOgre::interactor::FixedStyleInteractor";
    m_interactorStyles["Mesh"]      = "::fwRenderOgre::interactor::MeshPickerInteractor";
    m_interactorStyles["Video"]     = "::fwRenderOgre::interactor::VideoPickerInteractor";
    m_interactorStyles["Negato2D"]  = "::fwRenderOgre::interactor::Negato2DInteractor";
    m_interactorStyles["VR"]        = "::fwRenderOgre::interactor::VRWidgetsInteractor";
}

//------------------------------------------------------------------------------

SInteractorStyle::~SInteractorStyle() throw()
{
    m_connections.disconnect();
}

//------------------------------------------------------------------------------

void SInteractorStyle::doConfigure() throw(::fwTools::Failed)
{
    SLM_ASSERT("Interactor style not found, in xml, config style must be 'Default', 'Fixed', 'Negato2D', 'Mesh', "
               "or 'Video'",
               m_configuration->hasAttribute("style"));
    m_configuredStyle = m_configuration->getAttributeValue("style");
}

//------------------------------------------------------------------------------

void SInteractorStyle::doStart() throw(::fwTools::Failed)
{
    this->setInteractorStyle();

    if(!std::strcmp("Mesh", m_configuredStyle.c_str()) || !std::strcmp("Video", m_configuredStyle.c_str()))
    {
        ::fwRenderOgre::interactor::IPickerInteractor::sptr pickerInteractor =
            this->getRenderService()->getLayer(m_layerID)->getSelectInteractor();

        OSLM_ASSERT("There is no interactor found for this layer", pickerInteractor);

        //Connect all the modification signals of the frameTL to our updating function
        m_connections.connect(pickerInteractor,
                              ::fwRenderOgre::interactor::IInteractor::s_POINT_CLICKED_SIG,
                              this->getSptr(),
                              ::visuOgreAdaptor::SInteractorStyle::s_POINT_CLICKED_SLOT);
    }
}

//------------------------------------------------------------------------------

void SInteractorStyle::doUpdate() throw(::fwTools::Failed)
{
    this->setInteractorStyle();
}

//------------------------------------------------------------------------------

void SInteractorStyle::doSwap() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SInteractorStyle::doStop() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SInteractorStyle::setInteractorStyle()
{
    std::string style = m_interactorStyles[m_configuredStyle];

    ::fwRenderOgre::interactor::IInteractor::sptr interactor = ::fwRenderOgre::interactorFactory::New(style);
    interactor->setSceneID(this->getSceneManager()->getName());

    OSLM_ASSERT("Unknown interactor style : " << style, interactor);

    if(!std::strcmp("Trackball", m_configuredStyle.c_str())
       || !std::strcmp("Fixed", m_configuredStyle.c_str())
       || !std::strcmp("Negato2D", m_configuredStyle.c_str())
       || !std::strcmp("VR", m_configuredStyle.c_str()))
    {
        this->getRenderService()->getLayer(m_layerID)->setMoveInteractor(
            ::fwRenderOgre::interactor::IMovementInteractor::dynamicCast(interactor));
    }
    if(!std::strcmp("Mesh", m_configuredStyle.c_str()) || !std::strcmp("Video", m_configuredStyle.c_str()))
    {
        this->getRenderService()->getLayer(m_layerID)->setSelectInteractor(
            ::fwRenderOgre::interactor::IPickerInteractor::dynamicCast(interactor));
    }
}

//------------------------------------------------------------------------------

void SInteractorStyle::clickedPoint( ::fwData::Object::sptr obj )
{
    m_sigPointClicked->asyncEmit( obj );
}

} //namespace visuOgreAdaptor
