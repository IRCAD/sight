/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <QVBoxLayout>

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMedData/SeriesDB.hpp>

#include "uiMedDataQt/editor/SSelector.hpp"
#include "uiMedDataQt/widget/Selector.hpp"


namespace uiMedData
{

namespace editor
{
//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::gui::editor::IEditor , ::uiMedData::editor::SSelector , ::fwMedData::SeriesDB ) ;

//------------------------------------------------------------------------------

SSelector::SSelector()
{
}

//------------------------------------------------------------------------------

SSelector::~SSelector() throw()
{
}

//------------------------------------------------------------------------------

void SSelector::info(std::ostream &_sstream )
{
    // Update message
    _sstream << std::string("SSelector");
}

//------------------------------------------------------------------------------

void SSelector::starting() throw(::fwTools::Failed)
{
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    m_selectorWidget = new ::uiMedData::widget::Selector();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_selectorWidget);
    container->setLayout(layout);
}

//------------------------------------------------------------------------------

void SSelector::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    if (_msg->hasEvent(::fwServices::ObjectMsg::UPDATED_OBJECT))
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

void SSelector::stopping() throw(::fwTools::Failed)
{
    this->getContainer()->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
}

//------------------------------------------------------------------------------

void SSelector::updating() throw(::fwTools::Failed)
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getObject< ::fwMedData::SeriesDB >();
    BOOST_FOREACH(::fwMedData::Series::sptr series, seriesDB->getContainer())
    {
        m_selectorWidget->addSeries(series);
    }
}

//------------------------------------------------------------------------------

void SSelector::configuring() throw(::fwTools::Failed)
{
    this->::fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

} // namespace editor
} // namespace uiMedData
