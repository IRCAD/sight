/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>

#include <fwComEd/helper/SeriesDB.hpp>

#include <fwMedData/SeriesDB.hpp>

#include "uiMedDataQt/action/SExportSeries.hpp"


namespace uiMedData
{

namespace action
{
//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv , ::uiMedData::action::SExportSeries , ::fwMedData::SeriesDB ) ;

//------------------------------------------------------------------------------

SExportSeries::SExportSeries()
{
}

//------------------------------------------------------------------------------

SExportSeries::~SExportSeries() throw()
{
}

//------------------------------------------------------------------------------

void SExportSeries::info(std::ostream &_sstream )
{
    // Update message
    _sstream << std::string("SExportSeries");
}

//------------------------------------------------------------------------------

void SExportSeries::starting() throw(::fwTools::Failed)
{
    this->actionServiceStarting();
    ::fwMedData::SeriesDB::sptr seriesDB = this->getObject< ::fwMedData::SeriesDB >();
    BOOST_FOREACH( ::fwMedData::Series::sptr series, seriesDB->getContainer() )
    {
        if(series == this->getSeries())
        {
            this->setIsExecutable(false);
        }
    }
}

//------------------------------------------------------------------------------

void SExportSeries::receiving( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    ::fwComEd::SeriesDBMsg::csptr seriesDBMsg = ::fwComEd::SeriesDBMsg::dynamicConstCast(msg);

    if ( seriesDBMsg && seriesDBMsg->hasEvent( ::fwComEd::SeriesDBMsg::ADDED_OBJECTS ) )
    {
        ::fwData::Vector::sptr addedObject = seriesDBMsg->getAddedSeries();
        BOOST_FOREACH( ::fwData::Object::sptr obj, addedObject->getContainer() )
        {
            ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
            if(series == this->getSeries())
            {
                this->setIsExecutable(false);
            }
        }
    }
    if ( seriesDBMsg && seriesDBMsg->hasEvent( ::fwComEd::SeriesDBMsg::REMOVED_OBJECTS ) )
    {
        ::fwData::Vector::sptr removedObject = seriesDBMsg->getRemovedSeries();
        BOOST_FOREACH( ::fwData::Object::sptr obj, removedObject->getContainer() )
        {
            ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
            if(series == this->getSeries())
            {
                this->setIsExecutable(true);
            }
        }
    }
}

//------------------------------------------------------------------------------

void SExportSeries::stopping() throw(::fwTools::Failed)
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SExportSeries::updating() throw(::fwTools::Failed)
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getObject< ::fwMedData::SeriesDB >();
    ::fwMedData::Series::sptr series = this->getSeries();

    ::fwComEd::helper::SeriesDB seriesDBHelper(seriesDB);
    seriesDBHelper.add(series);
    seriesDBHelper.notify(this->getSptr());
    this->setIsExecutable(false);
}

//------------------------------------------------------------------------------

void SExportSeries::configuring() throw(::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();

    std::vector < ::fwRuntime::ConfigurationElement::sptr > seriesCfg = m_configuration->find("seriesId");
    SLM_ASSERT("Missing tag 'seriesId'", !seriesCfg.empty());

    m_seriesId = seriesCfg.front()->getValue();
    SLM_ASSERT("selectionId must not be empty", !m_seriesId.empty());
}

//------------------------------------------------------------------------------

::fwMedData::Series::sptr SExportSeries::getSeries()
{
    SLM_ASSERT("Object " << m_seriesId << " doesn't exist", ::fwTools::fwID::exist(m_seriesId));

    ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(m_seriesId);
    ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
    SLM_ASSERT("Object " << m_seriesId << " is not a '::fwMedData::Series'", series);

    return series;
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiMedData
