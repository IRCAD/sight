/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __BEGINNERTRAINING_TUTO01_SSTRINGREADER_HPP__
#define __BEGINNERTRAINING_TUTO01_SSTRINGREADER_HPP__

#include <io/IReader.hpp>

#include "beginnerTraining/config.hpp"

namespace beginnerTraining
{
namespace tuto01
{

class BEGINNERTRAINING_CLASS_API SStringReader : public ::io::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SStringReader)(::io::IReader) );

    BEGINNERTRAINING_API SStringReader();

    BEGINNERTRAINING_API virtual ~SStringReader() throw();

protected:


    /// Overrides
    BEGINNERTRAINING_API virtual void starting() throw ( ::fwTools::Failed );

    /// Overrides
    BEGINNERTRAINING_API virtual void stopping() throw ( ::fwTools::Failed );

    /// Overrides
    BEGINNERTRAINING_API virtual void updating() throw ( ::fwTools::Failed );

    /// Overrides
    BEGINNERTRAINING_API virtual void swapping() throw ( ::fwTools::Failed );

    /// Overrides io method
    BEGINNERTRAINING_API virtual void configureWithIHM();

    /// Overrides io method
    BEGINNERTRAINING_API virtual ::io::IOPathType getIOPathType() const;


};



} // namespace tuto01
} // namespace beginnerTraining

#endif /*__BEGINNERTRAINING_TUTO01_SSTRINGREADER_HPP__*/

