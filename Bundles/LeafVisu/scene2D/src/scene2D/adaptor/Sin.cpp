/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>
#include <fwData/Composite.hpp>

#include "scene2D/adaptor/Sin.hpp"
#include "scene2D/data/InitQtPen.hpp"

REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor , ::scene2D::adaptor::Sin  , ::fwData::Composite ) ;


namespace scene2D
{
namespace adaptor
{

Sin::Sin() throw()
{
}

Sin::~Sin() throw()
{
}

void Sin::configuring() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");

    std::string xMinStr(m_configuration->getAttributeValue("xMinBound"));
    std::string xMaxStr(m_configuration->getAttributeValue("xMaxBound"));
    std::istringstream xMinISS(xMinStr);
    std::istringstream xMaxISS(xMaxStr);
    float xMin;
    float xMax;
    xMinISS >> xMin;
    xMaxISS >> xMax;
    m_xMin = xMin;
    m_xMax = xMax;
}

void Sin::doStart()
{
    /*QGraphicsItemGroup* sinLayer = new QGraphicsItemGroup();
    QEasingCurve* sinus = new QEasingCurve(QEasingCurve::InSine);
    this->getScene2DRender->getScene()->addItem(sinLayer);*/
}

void Sin::doUpdate()
{

}

void Sin::doUpdate( fwServices::ObjectMsg::csptr _msg)
{

}

void Sin::doSwap()
{

}

void Sin::doStop()
{

}

} // namespace adaptor
} // namespace scene2D



