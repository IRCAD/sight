/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Initial authors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#include <vtkRenderWindowInteractor.h>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>

#include <visuVTKAdaptor/NegatoPickingCallBack.hpp>

#include "visuVTKAdaptor/ShowPickedPointInfo.hpp"

REGISTER_SERVICE( ::visuVTKAdaptor::INegatoInteractor , ::visuVTKAdaptor::ShowPickedPointInfo , ::visuVTKAdaptor::NegatoOneSlice ) ;

namespace visuVTKAdaptor
{

ShowPickedPointInfo::ShowPickedPointInfo() throw()
{}

ShowPickedPointInfo::~ShowPickedPointInfo() throw()
{}

void ShowPickedPointInfo::doStart() throw(fwTools::Failed)
{
    ///Manage picking
    m_adaptor.lock()->getInteractor()->AddObserver(vtkCommand::MouseMoveEvent, m_pickingCallBack, m_priority);

    ///Manage text rendering
    m_textAdaptor = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService , ::visuVTKAdaptor::Text >(m_adaptor.lock());
    m_textAdaptor.lock()->setRenderService(m_adaptor.lock()->getRenderService());
    m_textAdaptor.lock()->setRenderId( m_adaptor.lock()->getRenderId() );
    m_textAdaptor.lock()->start();
}
void ShowPickedPointInfo::doStop() throw(fwTools::Failed)
{
    ///Manage picking
    m_adaptor.lock()->getInteractor()->RemoveObservers(vtkCommand::MouseMoveEvent  , m_pickingCallBack);
    m_textAdaptor.lock()->stop();

    ///Manage text rendering
    ::fwServices::erase(m_textAdaptor.lock());
}

void ShowPickedPointInfo::doProcessPicking() throw(fwTools::Failed)
{
    std::vector< boost::int32_t > vect=this->getObject< ::visuVTKAdaptor::NegatoOneSlice >()->getObject< ::fwData::Image >()->getSize();

    std::string text;
    if( m_isPickingInside )
    {
        std::stringstream ss;
        ss << "[" << m_valuedIndex->getCoord()[0] << "," << m_valuedIndex->getCoord()[1] << "," << m_valuedIndex->getCoord()[2] << "] = " << m_valuedIndex->getValue();
        text = ss.str();
    }

    m_textAdaptor.lock()->setText(text);
    m_adaptor.lock()->getRenderService()->render();
}

} // namespace processor
