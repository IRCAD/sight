/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOITK_SIMAGESERIESWRITER_HPP__
#define __IOITK_SIMAGESERIESWRITER_HPP__

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IWriter.hpp>

#include "ioITK/export.hpp"

namespace ioITK
{

/**
* @brief Write an image to inr format
**/
class IOITK_CLASS_API SImageSeriesWriter : public ::io::IWriter
{

public :
    fwCoreServiceClassDefinitionsMacro ( (SImageSeriesWriter)( ::io::IWriter) ) ;

    IOITK_API SImageSeriesWriter() throw();

    IOITK_API virtual ~SImageSeriesWriter() throw();

protected:

    /// Override
    IOITK_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOITK_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOITK_API void updating() throw(::fwTools::Failed);

    /// Override
    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed) {} ;

    /// Override
    IOITK_API void info(std::ostream &_sstream ) ;

    /// Override
    IOITK_API virtual void configureWithIHM();

    /// Return managed file type, here FILE
    IOITK_API ::io::IOPathType getIOPathType() const;

};

} // namespace ioITK

#endif //__IOITK_SIMAGESERIESWRITER_HPP__

