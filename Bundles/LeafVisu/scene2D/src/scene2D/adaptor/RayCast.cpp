/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>
#include <fwData/Composite.hpp>

#include <QGraphicsItemGroup>

#include "scene2D/adaptor/RayCast.hpp"
#include "scene2D/data/InitQtPen.hpp"


REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor , ::scene2D::adaptor::RayCast  , ::fwData::Composite ) ;


namespace scene2D
{
namespace adaptor
{

RayCast::RayCast() throw()
{
}

RayCast::~RayCast() throw()
{
}

void RayCast::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"config\" tag missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();

    SLM_TRACE("IAdaptor configuring ok");

}


void RayCast::doStart() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Initialize the layer
    m_layer = new QGraphicsItemGroup();
}

void RayCast::doUpdate() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void RayCast::doUpdate( fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void RayCast::doSwap() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void RayCast::doStop() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Remove the layer (and therefore its related line item) from the scene
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

} // namespace adaptor
} // namespace scene2D



