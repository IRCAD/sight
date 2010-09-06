/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATAIO_WRITER_IOBJECTWRITER_HPP_
#define _FWDATAIO_WRITER_IOBJECTWRITER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include <fwData/location/ILocation.hpp>

#include "fwDataIO/config.hpp"

namespace fwDataIO
{

namespace writer
{

/**
 * @brief   Base class for all object writers.
 * @class   IObjectWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2009
 *
 * This class defines the API to use basic object writers. This writer is not
 * a service. Their equivalent exist as services see ::io::IWriter. To write an
 * object with this class, use setLocation and setOject method before execute
 * the method write.
 *
 * @todo Remove m_extension of this class. It must be deprecated.
 */
class FWDATAIO_CLASS_API IObjectWriter : public ::fwCore::BaseObject
{

public :

    fwCoreNonInstanciableClassDefinitionsMacro( (IObjectWriter) );

    /// Constructor. Do nothing.
    FWDATAIO_API IObjectWriter();

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~IObjectWriter();

    /**
     * @brief Defines an writer interface.
     *
     * This method write the object given in parameter of setObject method.
     */
    FWDATAIO_API virtual void write() = 0;

    /**
     * @brief m_location setter.
     * @param[in] _location set location where object will be saved
     */
    FWDATAIO_API virtual void setLocation( const ::fwData::location::ILocation::sptr location );

    /**
     * @brief m_location getter.
     *
     * @return m_location
     */
    FWDATAIO_API virtual ::fwData::location::ILocation::sptr getLocation();

    /**
     * @brief m_object setter.
     * @param[in] _pObject replace m_object of the instance writer
     * @note m_object is save in class with a weakptr
     * @note This object can be get with the method getObject()
     */
    FWDATAIO_API virtual void setObject( ::fwTools::Object::sptr _pObject );

    /**
     * @brief m_object getter.
     *
     * @return m_object
     */
    FWDATAIO_API virtual ::fwTools::Object::sptr  getObject();

    /**
     * @brief Return the default filename extension can be dynamic.
     * @return m_extension
     *
     * The extension can be empty (for a repository for example) or must return a string BEGINNING WITH A DOT
     * by default be empty
     */
    FWDATAIO_API virtual std::string  extension()=0;

protected :

    /**
     * @brief Object write on filesystem by the process.
     *
     * This object is given in parameter of setObject method but it is conserved with a weakptr.
     */
    ::fwTools::Object::wptr m_object;

    /// Object location ( file path, directory path, url, etc )
    ::fwData::location::ILocation::sptr m_location;

    /// Extension of file format
    std::string m_extension;

};

} // namespace writer

} // namespace fwDataIO


#endif // _FWDATAIO_WRITER_IOBJECTWRITER_HPP_
