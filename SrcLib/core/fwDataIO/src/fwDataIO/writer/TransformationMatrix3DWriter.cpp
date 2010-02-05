/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <vector>

#include <boost/filesystem/convenience.hpp>



#include <fwTools/ClassRegistrar.hpp>


#include "fwDataIO/writer/TransformationMatrix3DWriter.hpp"



REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::fwDataIO::writer::TransformationMatrix3DWriter , ::fwDataIO::writer::TransformationMatrix3DWriter);


namespace fwDataIO
{

namespace writer
{

//------------------------------------------------------------------------------

TransformationMatrix3DWriter::TransformationMatrix3DWriter()
: ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{}

//------------------------------------------------------------------------------

TransformationMatrix3DWriter::~TransformationMatrix3DWriter()
{}

//------------------------------------------------------------------------------

void TransformationMatrix3DWriter::write()
{
        std::fstream file;
        file.open(getFile().native_file_string().c_str(), std::fstream::out);
        if (!file.is_open())
        {
                OSLM_ERROR( "Trf file writing error for " << getFile());
                return;
        }
        ::std::vector< double > mat = this->getConcreteObject()->getRefCoefficients();
        ::std::vector< double >::iterator iter;
        int i=0;
        for (iter = mat.begin() ; iter!= mat.end() ; ++iter)
        {
                OSLM_TRACE("MATRIX : " << (*iter));
                file << (*iter);
                i++;
                if (i%4 == 0 && i!=16)
                        file << std::endl;
                else
                        file << " ";
        }

        file.close();
}


std::string  TransformationMatrix3DWriter::extension()
{
        static std::string ext(".trf");
        return ext;
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace fwDataIO
