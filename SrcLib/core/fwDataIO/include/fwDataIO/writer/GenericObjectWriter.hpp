/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATAIO_WRITER_GENERICOBJECTWRITER_HPP_
#define _FWDATAIO_WRITER_GENERICOBJECTWRITER_HPP_

#include <boost/shared_ptr.hpp>

#include "fwDataIO/config.hpp"
#include "fwDataIO/writer/IObjectWriter.hpp"

namespace fwDataIO
{

namespace writer
{

/**
 * @brief       generic class for all object writers.
 * @class       GenericObjectWriter.
 * @author      IRCAD (Research and Development Team).
 * @date        2009
 *
 * This class adds 2 methods to the API of IObjectWriter. This
 * generic reader test the object type given to the method setObject.
 * And add a new get method getConcreteObject() that returns an object
 * already cast.
 */
template<class DATATYPE>
class GenericObjectWriter  : public ::fwDataIO::writer::IObjectWriter
{

public :

        /// the object type related
        typedef DATATYPE DataType;

        fwCoreNonInstanciableClassDefinitionsMacro( (GenericObjectWriter<DATATYPE>)(::fwDataIO::writer::IObjectWriter::Baseclass) );

        /// Constructor. Do nothing.
        GenericObjectWriter() {};

        /// Destructor. Do nothing.
        virtual ~GenericObjectWriter() {};

        /**
         * @brief m_object setter.
         * @param[out] _pObject replace weakptr m_object of the instance reader
         *
         * This method overload setObject to ensure that given object correspond to DATATYPE
         */
        virtual void  setObject( ::fwTools::Object::sptr obj)
        {
                assert ( ::boost::dynamic_pointer_cast< DataType >( obj ) );
                m_object = obj;
        }

        /**
         * @brief m_object getter.
         * @return m_object
         *
         * This method automatic cast object in correct DataType.
         */
        virtual ::boost::shared_ptr< DataType >  getConcreteObject()
        {
                return ::boost::dynamic_pointer_cast< DataType >( getObject() );
        }

};

} // namespace writer

} // namespace fwDataIO


#endif // _FWDATAIO_WRITER_GENERICOBJECTWRITER_HPP_
