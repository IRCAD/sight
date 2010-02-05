/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <exception>
#include <iostream>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>


#include "fwData/ProcessObject.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::ProcessObject,  ::fwData::ProcessObject );
namespace fwData
{
const std::string ProcessObject::InputKey  = "Inputs";
const std::string ProcessObject::OutputKey = "Outputs";

//------------------------------------------------------------------------------

ProcessObject::ProcessObject()
{
        ::fwTools::Field::sptr input( new ::fwTools::Field(InputKey) );
        m_children.push_back( input );
        ::fwTools::Field::sptr output( new ::fwTools::Field(OutputKey) );
        m_children.push_back( output );
}

//------------------------------------------------------------------------------

ProcessObject::~ProcessObject()
{
        SLM_DEBUG("ProcessObject::~ProcessObject()");
}

//------------------------------------------------------------------------------

::fwTools::Object::sptr
ProcessObject::getInputs()
{
        // FIXME set a more efficient way
        assert( ::fwTools::Field::dynamicCast( m_children.at(0) ));
//      return ::fwData::Object::dynamicCast( ::fwTools::Field::dynamicCast( m_children.at(0) ) );
        return m_children.at(0) ;
}

//------------------------------------------------------------------------------

::fwTools::Object::sptr
ProcessObject::getOutputs(  )
{
         // FIXME set a more efficient way
         assert( ::fwTools::Field::dynamicCast(m_children.at(1) ));
//      return ::fwData::Object::dynamicCast( ::fwTools::Field::dynamicCast( m_children.at(1) ) );
                return m_children.at(1) ;
}

//------------------------------------------------------------------------------

::fwTools::Object::csptr
ProcessObject::getInputs() const
{
        // FIXME set a more efficient way
        assert( ::fwTools::Field::dynamicCast( m_children.at(0) ));
//      return ::fwData::Object::dynamicCast( ::fwTools::Field::dynamicCast( m_children.at(0) ) );
        return m_children.at(0) ;
}

//------------------------------------------------------------------------------

::fwTools::Object::csptr
ProcessObject::getOutputs(  ) const
{
         // FIXME set a more efficient way
         assert( ::fwTools::Field::dynamicCast(m_children.at(1) ));
//      return ::fwData::Object::dynamicCast( ::fwTools::Field::dynamicCast( m_children.at(1) ) );
                return m_children.at(1) ;
}
//------------------------------------------------------------------------------

::fwData::Object::sptr  ProcessObject::getInput( ProcessObject::MapKeyType _id  )
{
        return ::boost::dynamic_pointer_cast< ::fwData::Object >( this->getInputs()->getField( _id )->children().at(0) ) ;
}

//------------------------------------------------------------------------------

::fwData::Object::sptr ProcessObject::getOutput( ProcessObject::MapKeyType _id  )
{
        return ::boost::dynamic_pointer_cast< ::fwData::Object >( this->getOutputs()->getField( _id )->children().at(0) );
}

//------------------------------------------------------------------------------

void ProcessObject::setInputValue(ProcessObject::MapKeyType _id, ::fwData::Object::sptr _object)
{
        getInputs()->setFieldSingleElement( _id, _object);
}

//------------------------------------------------------------------------------

void ProcessObject::setOutputValue(ProcessObject::MapKeyType _id, ::fwData::Object::sptr _object)
{
        getOutputs()->setFieldSingleElement( _id, _object);
}

//------------------------------------------------------------------------------

Object::ChildContainer ProcessObject::getIO()
{
        Object::ChildContainer inputContainer = this->getInputs()->children();
        Object::ChildContainer outputContainer = this->getOutputs()->children();
        Object::ChildContainer container;
        for( Object::ChildContainer::iterator iter = this->getInputs()->children().begin() ; iter != this->getInputs()->children().end() ; ++iter )
        {
                container.push_back( *iter ) ;
        }
        for( Object::ChildContainer::iterator iter = this->getOutputs()->children().begin() ; iter != this->getOutputs()->children().end() ; ++iter )
        {
                container.push_back( *iter ) ;
        }
        return container ;
}

//------------------------------------------------------------------------------

ProcessObject::MapType ProcessObject::getInputMap()
{
        ::fwTools::Object::ChildContainer::iterator iter ;
        ProcessObject::MapType myMap ;
        for(iter = this->getInputs()->children().begin() ; iter != this->getInputs()->children().end() ; ++iter )
        {
                ProcessObject::MapKeyType id = ::fwTools::Field::dynamicCast(*iter)->label() ;
                assert( myMap.find(id) == myMap.end() ) ;
                myMap[ id ] = this->getInput( id ) ;
        }
        return myMap ;
}

//------------------------------------------------------------------------------

ProcessObject::MapType ProcessObject::getOutputMap()
{
        ::fwTools::Object::ChildContainer::iterator iter ;
        ProcessObject::MapType myMap ;
        for(iter = this->getOutputs()->children().begin() ; iter != this->getOutputs()->children().end() ; ++iter )
        {
                ProcessObject::MapKeyType id = ::fwTools::Field::dynamicCast(*iter)->label() ;
                assert( myMap.find(id) == myMap.end() ) ;
                myMap[ id ] = this->getOutput( id ) ;
        }
        return myMap ;
}

//------------------------------------------------------------------------------

} // namespace fwData

