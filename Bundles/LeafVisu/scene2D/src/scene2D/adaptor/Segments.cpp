/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>
#include <fwData/Composite.hpp>

#include <QGraphicsItemGroup>

#include "scene2D/adaptor/Segments.hpp"
#include "scene2D/data/InitQtPen.hpp"


REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor , ::scene2D::adaptor::Segments  , ::fwData::Composite ) ;


namespace scene2D
{
namespace adaptor
{

Segments::Segments() throw()
{
}

Segments::~Segments() throw()
{
}

void Segments::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"config\" tag missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();

    SLM_TRACE("IAdaptor configuring ok");

    // If the corresponding attributes are present in the config, set the color of the line
    if (!m_configuration->getAttributeValue("color").empty())
    {
        ::scene2D::data::InitQtPen::setPenColor(m_pen, m_configuration->getAttributeValue("color"));
    }
}


void Segments::doStart() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Initialize the layer
    m_layer = new QGraphicsItemGroup();
}

void Segments::doUpdate() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void Segments::doUpdate( fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void Segments::doSwap() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void Segments::doStop() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Remove the layer (and therefore its related line item) from the scene
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

} // namespace adaptor
} // namespace scene2D



