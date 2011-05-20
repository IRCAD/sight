/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "itkIO/ImageIOFactoryRegistrar.hxx"

#include "inr2itk/itkInrImageIOFactory.h"

#include "itkCreateObjectFunction.h"
#include "inr2itk/itkInrImageIO.h"
#include "itkVersion.h"

#include <fwCore/base.hpp>


REGISTER_IMAGEIOFACTORY( itk::InrImageIOFactory );


namespace itk
{

void InrImageIOFactory::PrintSelf(std::ostream&, Indent) const
{}

//------------------------------------------------------------------------------

InrImageIOFactory::InrImageIOFactory()
{
    SLM_TRACE_FUNC();
    this->RegisterOverride("itkImageIOBase",
            "itkInrImageIO",
            "InrImage IO",
            1,
            CreateObjectFunction<InrImageIO>::New());
}

//------------------------------------------------------------------------------

InrImageIOFactory::~InrImageIOFactory()
{}

//------------------------------------------------------------------------------

const char* InrImageIOFactory::GetITKSourceVersion(void) const
{
    return ITK_SOURCE_VERSION;
}

//------------------------------------------------------------------------------

const char* InrImageIOFactory::GetDescription() const
{
    return "InrImage ImageIO Factory, allows the loading of InrImage into ITK";
}

} // end namespace itk
