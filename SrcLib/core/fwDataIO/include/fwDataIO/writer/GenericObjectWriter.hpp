/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATAIO_WRITER_GENERICOBJECTWRITER_HPP__
#define __FWDATAIO_WRITER_GENERICOBJECTWRITER_HPP__

#include "fwDataIO/config.hpp"
#include "fwDataIO/writer/IObjectWriter.hpp"

namespace fwDataIO
{

namespace writer
{

/**
 * @brief   generic class for all object writers.
 *
 * This class adds 2 methods to the API of IObjectWriter. This
 * generic reader test the object type given to the method setObject.
 * And add a new get method getConcreteObject() that returns an object
 * already cast.
 */
template<class DATATYPE>
class GenericObjectWriter : public ::fwDataIO::writer::IObjectWriter
{

public:

    /// the object type related
    typedef DATATYPE DataType;

    fwCoreNonInstanciableClassDefinitionsMacro( (GenericObjectWriter<DATATYPE>)(::fwDataIO::writer::IObjectWriter) );

    /// Constructor. Do nothing.
    GenericObjectWriter()
    {
    }

    /// Destructor. Do nothing.
    virtual ~GenericObjectWriter()
    {
    }

    /**
     * @brief m_object setter.
     * @param[out] obj replace weakptr m_object of the instance reader
     *
     * This method overload setObject to ensure that given object correspond to DATATYPE
     */
    virtual void setObject( ::fwTools::Object::sptr obj) override
    {
        assert( std::dynamic_pointer_cast< DataType >( obj ) );
        m_object = obj;
    }

    /**
     * @brief m_object getter.
     * @return m_object
     *
     * This method automatic cast object in correct DataType.
     */
    virtual std::shared_ptr< DataType >  getConcreteObject()
    {
        return std::dynamic_pointer_cast< DataType >( getObject() );
    }

};

} // namespace writer

} // namespace fwDataIO

#endif // __FWDATAIO_WRITER_GENERICOBJECTWRITER_HPP__
