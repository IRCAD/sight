/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Study.hpp"
#include "fwData/Patient.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Patient, ::fwData::Patient );
namespace fwData
{
const Object::FieldID Patient::ID_STUDIES = "ID_STUDIES";
const Object::FieldID Patient::ID_TOOLBOX = "ID_TOOLBOX";
const Object::FieldID Patient::ID_SCENARIOS = "ID_SCENARIOS";
//------------------------------------------------------------------------------

Patient::Patient () :
                    m_sName (""),
                    m_sFirstname (""),
                    m_sIDDicom (""),
                    m_sBirthdate ( ::boost::date_time::min_date_time ),
                    m_bIsMale (true),
                    m_i32DbID (-1)
{
    SLM_WARN("::fwData::Patient() : (ToDo) field default value");
    setField( Patient::ID_STUDIES );
}

//------------------------------------------------------------------------------

Patient::~Patient ()
{
    SLM_WARN("::fwData::~Patient() : (ToDo) Destruction of study vector");
}

//------------------------------------------------------------------------------

Patient &Patient::operator=(const Patient &_patient)
{
    this->m_sName       = _patient.m_sName;
    this->m_sFirstname  = _patient.m_sFirstname;
    this->m_sIDDicom    = _patient.m_sIDDicom;
    this->m_sBirthdate  = _patient.m_sBirthdate;
    this->m_bIsMale     = _patient.m_bIsMale;
    this->m_i32DbID     = _patient.m_i32DbID;

    this->getField( Patient::ID_STUDIES )->children() = _patient.getField( Patient::ID_STUDIES )->children();

    return (*this);
}

//------------------------------------------------------------------------------

boost::uint32_t  Patient::getStudySize() const
{
    return this->getField( Patient::ID_STUDIES )->children().size();
}

//------------------------------------------------------------------------------

void Patient::addStudy( ::fwData::Study::sptr _study )
{
    this->addFieldElement(Patient::ID_STUDIES, _study);
}

//------------------------------------------------------------------------------

std::pair< Patient::StudyIterator, Patient::StudyIterator > Patient::getStudies()
{
    StudyIterator begin(  getField( Patient::ID_STUDIES )->children().begin() );
    StudyIterator   end(  getField( Patient::ID_STUDIES )->children().end()   );
    return std::make_pair( begin, end );
}

//------------------------------------------------------------------------------

std::pair< Patient::StudyConstIterator, Patient::StudyConstIterator > Patient::getStudies() const
{
    StudyConstIterator begin(  getField( Patient::ID_STUDIES )->children().begin()   );
    StudyConstIterator   end(  getField( Patient::ID_STUDIES )->children().end()   );
    return std::make_pair( begin, end );
}

//------------------------------------------------------------------------------

void Patient::setToolBox( ::fwData::Composite::sptr _toolBox )
{
    this->setFieldSingleElement(Patient::ID_TOOLBOX, _toolBox);
}

//------------------------------------------------------------------------------

::fwData::Composite::sptr Patient::getToolBox()
{
    ::fwData::Composite::sptr _pToolBox;

    if ( this->getFieldSize( Patient::ID_TOOLBOX ) != 0)
    {
        const unsigned int NbChildren = this->getField( Patient::ID_TOOLBOX )->children().size();

        assert ( NbChildren <= 1 );

        // Test if the toolBox exists
        if( NbChildren == 1 )
        {
            _pToolBox = ::fwData::Composite::dynamicCast ( this->getField( Patient::ID_TOOLBOX )->children()[0] );
        }
        else
        {
            SLM_WARN("Patient::getToolBox : return a null toolBox pointer.");
        }
    }

    return _pToolBox;
}

//------------------------------------------------------------------------------

::fwData::Composite::csptr Patient::getToolBox() const
        {
    ::fwData::Composite::csptr _pToolBox;

    if ( this->getFieldSize( Patient::ID_TOOLBOX ) != 0)
    {
        const unsigned int NbChildren = this->getField( Patient::ID_TOOLBOX )->children().size();

        assert ( NbChildren <= 1 );

        // Test if the toolBox exists
        if( NbChildren == 1 )
        {
            _pToolBox = ::fwData::Composite::dynamicCast ( this->getField( Patient::ID_TOOLBOX )->children()[0] );
        }
        else
        {
            SLM_WARN("Patient::getToolBox : return a null toolBox pointer.");
        }
    }

    return _pToolBox;
        }

//------------------------------------------------------------------------------

void Patient::setScenarios( ::fwData::Composite::sptr _scenarios )
{
    this->setFieldSingleElement(Patient::ID_SCENARIOS, _scenarios);
}

//------------------------------------------------------------------------------

::fwData::Composite::sptr Patient::getScenarios()
{
    ::fwData::Composite::sptr _pScenarios;

    if ( this->getFieldSize( Patient::ID_SCENARIOS ) != 0)
    {
        const unsigned int NbChildren = this->getField( Patient::ID_SCENARIOS )->children().size();

        assert ( NbChildren <= 1 );

        // Test if the scenarios exists
        if( NbChildren == 1 )
        {
            _pScenarios = ::fwData::Composite::dynamicCast ( this->getField( Patient::ID_SCENARIOS )->children()[0] );
        }
        else
        {
            SLM_WARN("Patient::getScenarios : return a null scenarios pointer.");
        }
    }
    return _pScenarios;
}

//------------------------------------------------------------------------------

::fwData::Composite::csptr Patient::getScenarios() const
        {
    ::fwData::Composite::csptr _pScenarios;

    if ( this->getFieldSize( Patient::ID_SCENARIOS ) != 0)
    {
        const unsigned int NbChildren = this->getField( Patient::ID_SCENARIOS )->children().size();

        assert ( NbChildren <= 1 );

        // Test if the scenarios exists
        if( NbChildren == 1 )
        {
            _pScenarios = ::fwData::Composite::dynamicCast ( this->getField( Patient::ID_SCENARIOS )->children()[0] );
        }
        else
        {
            SLM_WARN("Patient::getScenarios : return a null scenarios pointer.");
        }
    }

    return _pScenarios;
}
//------------------------------------------------------------------------------

void Patient::addTool( std::string _name, ::fwData::Object::sptr _tool )
{
    if ( !getToolBox() )
    {
        setToolBox(::fwData::Composite::New());
    }

    getToolBox()->getRefMap()[_name] = _tool;
}
//------------------------------------------------------------------------------

::fwData::Object::sptr Patient::getTool( std::string _name )
{
    ::fwData::Object::sptr tool;

    if (getToolBox())
    {
        tool = getToolBox()->getRefMap()[_name];
    }

    return tool;
}
//------------------------------------------------------------------------------

void Patient::addScenario( std::string _name, ::fwData::Object::sptr _scenario )
{
    if ( !getScenarios() )
    {
        setScenarios(::fwData::Composite::New());
    }

    getScenarios()->getRefMap()[_name] = _scenario;
}
//------------------------------------------------------------------------------

::fwData::Object::sptr Patient::getScenario( std::string _name )
{
    ::fwData::Object::sptr scenario;

    if (getScenarios())
    {
        scenario = getScenarios()->getRefMap()[_name];
    }

    return scenario;
}

//------------------------------------------------------------------------------

} // end namespace fwData
