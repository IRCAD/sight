/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <io/IReader.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/SplineMsg.hpp>

#include <fwData/Spline.hpp>
#include <fstream>
#include <fwCore/base.hpp>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <fwServices/macros.hpp>

#include "ioData/SplineReaderService.hpp"

REGISTER_SERVICE( ::io::IReader , ::ioData::SplineReaderService , ::fwData::Spline ) ;

namespace ioData
{

SplineReaderService::SplineReaderService()
{
    isTransfo = false;
}

//-----------------------------------------------------------------------------

void SplineReaderService::info(std::ostream &_sstream )
{
    this->SuperClass::info( _sstream ) ;
    _sstream  << " spline reader" << std::endl;
}

//-----------------------------------------------------------------------------

std::vector< std::string > SplineReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back(".spline");
    return extensions ;
}

//-----------------------------------------------------------------------------

SplineReaderService::~SplineReaderService() throw()
{
}

//-----------------------------------------------------------------------------

void SplineReaderService::configuring( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for (iter = m_configuration->begin() ; iter != m_configuration->end() ; ++iter) {
        SLM_INFO((*iter)->getName());
        if ((*iter)->getName() == "spline")
        {
            if((*iter)->hasAttribute("id"))
            {
                m_idSpline = boost::lexical_cast<int >((*iter)->getExistingAttributeValue("id").c_str());
                SLM_INFO((*iter)->getExistingAttributeValue("id"));
            }
            if((*iter)->hasAttribute("nbSides"))
            {
                m_nbSides = boost::lexical_cast<int >((*iter)->getExistingAttributeValue("nbSides").c_str());
                SLM_INFO((*iter)->getExistingAttributeValue("nbSides"));
            }
            if((*iter)->hasAttribute("radius"))
            {
                m_radius = boost::lexical_cast<double >((*iter)->getExistingAttributeValue("radius").c_str());
                SLM_INFO((*iter)->getExistingAttributeValue("radius"));
            }
            if((*iter)->hasAttribute("matrix"))
            {
                objectMatrix = loadObjectTransformationMatrix3D((*iter)->getExistingAttributeValue("matrix"));
                isTransfo = true;
                OSLM_INFO("spline matrix: " << (*iter)->getExistingAttributeValue("matrix"));
            }

            ::fwRuntime::ConfigurationElementContainer::Iterator iter2;
            ::fwRuntime::ConfigurationElement::sptr m_configuration2 = m_configuration->findConfigurationElement("spline");
            for (iter2 = m_configuration2->begin() ; iter2 != m_configuration2->end() ; ++iter2)
            {
                if ((*iter2)->getName() == "point"
                    && (*iter2)->hasAttribute("id"))
                {
                    ::fwData::Spline::point pt;
                    pt.id = boost::lexical_cast<int >((*iter2)->getExistingAttributeValue("id").c_str());
                    SLM_INFO((*iter2)->getExistingAttributeValue("id"));
                    pt.p[0] = pt.id * 100.0;
                    pt.p[1] = 0.0;
                    pt.p[2] = 0.0;
                    pt.isVisible = false;
                    m_points.push_back( pt ) ;
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SplineReaderService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    // Retrieve object
    ::fwData::Spline::wptr spline = this->getObject< ::fwData::Spline >( );

    spline.lock()->points() = m_points ;
    spline.lock()->setRadius(m_radius);
    spline.lock()->setNbSides(m_nbSides);
    spline.lock()->setIdSpline(m_idSpline);

    // Notify reading
    ::fwComEd::SplineMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::SplineMsg::NEW_SPLINE ) ;

    if(isTransfo)
    {
        spline.lock()->setFieldSingleElement( ::fwComEd::Dictionary::position, objectMatrix ) ;
        msg->addEvent( ::fwComEd::Dictionary::position ) ;
        ::fwServices::IEditionService::notify(this->getSptr(), spline.lock(), msg);
    }
    else
    {
        ::fwServices::IEditionService::notify(this->getSptr(), spline.lock(), msg);
    }
}

//-----------------------------------------------------------------------------

::fwData::TransformationMatrix3D::sptr SplineReaderService::loadObjectTransformationMatrix3D(std::string m_file)
{
    ::fwData::TransformationMatrix3D::NewSptr matrix;
    ::boost::filesystem::path location(m_file) ;

    std::fstream file;
    file.open(location.string().c_str(), std::fstream::in);
    if (!file.is_open())
    {
        OSLM_WARN( "Object Matrix file loading error for " + location.string());
        return matrix;
    }
    double val;
    for( int l=0 ; l < 4 ; l++ )
    {
        for( int c=0 ; c < 4 ; c++ )
        {
            file >> val;
            matrix->setCoefficient(l, c, val);
        }
    }
    return matrix;
}

}
