/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"

#include "fwData/Study.hpp"
#include "fwData/Patient.hpp"

fwDataRegisterMacro( ::fwData::Patient );
namespace fwData
{

Patient::Patient () :
                    m_sName (""),
                    m_sFirstname (""),
                    m_sIDDicom (""),
                    m_sBirthdate ( ::boost::date_time::min_date_time ),
                    m_bIsMale (true),
                    m_i32DbID (-1)
{
}

//------------------------------------------------------------------------------

Patient::~Patient ()
{}

//------------------------------------------------------------------------------

void Patient::shallowCopy( Patient::csptr _source )
{
    this->fieldShallowCopy( _source );
    m_sName         = _source->m_sName;
    m_sFirstname    = _source->m_sFirstname;
    m_sIDDicom      = _source->m_sIDDicom;
    m_sBirthdate    = _source->m_sBirthdate;
    m_bIsMale       = _source->m_bIsMale;
    m_i32DbID       = _source->m_i32DbID;
    m_attrStudies   = _source->m_attrStudies;
    m_attrToolBox   = _source->m_attrToolBox;
    m_attrScenarios = _source->m_attrScenarios;
}

//------------------------------------------------------------------------------

void Patient::deepCopy( Patient::csptr _source )
{
    this->fieldDeepCopy( _source );
    m_sName         = _source->m_sName;
    m_sFirstname    = _source->m_sFirstname;
    m_sIDDicom      = _source->m_sIDDicom;
    m_sBirthdate    = _source->m_sBirthdate;
    m_bIsMale       = _source->m_bIsMale;
    m_i32DbID       = _source->m_i32DbID;

    m_attrStudies.clear();
    std::transform(
            _source->m_attrStudies.begin(), _source->m_attrStudies.end(),
            std::back_inserter(m_attrStudies),
            & ::fwData::Object::copy< StudyContainerType::value_type::element_type >
            );

    m_attrToolBox   = ::fwData::Object::copy(_source->m_attrToolBox);
    m_attrScenarios = ::fwData::Object::copy(_source->m_attrScenarios);
}

//------------------------------------------------------------------------------

Patient::StudyContainerType::size_type  Patient::getNumberOfStudies() const
{
    return this->getStudies().size();
}

//------------------------------------------------------------------------------

void Patient::addStudy( ::fwData::Study::sptr _study )
{
    m_attrStudies.push_back(_study);
}

//------------------------------------------------------------------------------

void Patient::addTool( std::string _name, ::fwData::Object::sptr _tool )
{
    if ( !this->getToolBox() )
    {
        this->setToolBox(::fwData::Composite::New());
    }

    getToolBox()->getContainer()[_name] = _tool;
}

//------------------------------------------------------------------------------

::fwData::Object::sptr Patient::getTool( std::string _name )
{
    ::fwData::Object::sptr tool;
    ::fwData::Composite::sptr tools = this->getToolBox();
    if (tools)
    {
        ::fwData::Composite::IteratorType iter = tools->find(_name);
        if(iter != tools->end())
        {
            tool = iter->second;
        }
    }
    return tool;
}

//------------------------------------------------------------------------------

void Patient::addScenario( std::string _name, ::fwData::Object::sptr _scenario )
{
    if ( !this->getScenarios() )
    {
        this->setScenarios(::fwData::Composite::New());
    }
    this->getScenarios()->getContainer()[_name] = _scenario;
}

//------------------------------------------------------------------------------

::fwData::Object::sptr Patient::getScenario( std::string _name )
{
    ::fwData::Object::sptr scenario;
    ::fwData::Composite::sptr scenarios = this->getScenarios();
    if (scenarios)
    {
        ::fwData::Composite::IteratorType iter = scenarios->find(_name);
        if(iter != scenarios->end())
        {
            scenario = iter->second;
        }
    }
    return scenario;
}

//------------------------------------------------------------------------------

} // end namespace fwData
