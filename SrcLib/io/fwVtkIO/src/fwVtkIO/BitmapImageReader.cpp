/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwVtkIO/BitmapImageReader.hpp"

#include "fwVtkIO/helper/vtkLambdaCommand.hpp"
#include "fwVtkIO/vtk.hpp"

#include <fwDataIO/reader/registry/macros.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <boost/tokenizer.hpp>

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Collection.h>
#include <vtkImageReader2Factory.h>
#include <vtkSmartPointer.h>

fwDataIOReaderRegisterMacro( ::fwVtkIO::BitmapImageReader );

namespace fwVtkIO
{
//------------------------------------------------------------------------------

BitmapImageReader::BitmapImageReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this),
    m_job(::fwJobs::Observer::New("Bitmap image reader"))
{
    /* Initialize the available extensions */
    std::vector<std::string> ext;
    BitmapImageReader::getAvailableExtensions(ext);

    if(ext.size() > 0)
    {
        m_availableExtensions = ext.at(0);
        for(int i = 1; i < ext.size(); i++)
        {
            m_availableExtensions = m_availableExtensions + " " + ext.at(i);
        }
    }
}

//------------------------------------------------------------------------------

BitmapImageReader::~BitmapImageReader()
{
}

//------------------------------------------------------------------------------

void BitmapImageReader::read()
{
    SLM_ASSERT("The current object has expired.", !m_object.expired() );
    SLM_ASSERT("Unable to lock object", m_object.lock() );

    ::fwData::Image::sptr pImage = getConcreteObject();

    // Use a vtkImageReader2Factory to automatically detect the type of the input file
    // And select the right reader for the file
    vtkSmartPointer<vtkImageReader2Factory> factory =
        vtkSmartPointer< vtkImageReader2Factory >::New();
    vtkSmartPointer<vtkImageReader2> reader =
        factory->CreateImageReader2( this->getFile().string().c_str() );

    FW_RAISE_IF("BitmapImageReader cannot read Bitmap image file :" << this->getFile().string(), !reader);

    reader->SetFileName(this->getFile().string().c_str());

    using namespace fwVtkIO::helper;
    vtkSmartPointer< vtkLambdaCommand > progressCallback;

    progressCallback = vtkSmartPointer< vtkLambdaCommand >::New();
    progressCallback->SetCallback(
        [&](vtkObject* caller, long unsigned int, void*)
        {
            auto filter = static_cast<vtkGenericDataObjectReader*>(caller);
            m_job->doneWork(filter->GetProgress() * 100);
        }
        );
    reader->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook([&] { reader->AbortExecuteOn(); });

    reader->Update();

    vtkDataObject* obj = reader->GetOutput();
    vtkImageData* img  = vtkImageData::SafeDownCast(obj);

    m_job->finish();

    FW_RAISE_IF("BitmapImageReader cannot read Bitmap image file :"<<this->getFile().string(), !img);
    try
    {
        ::fwVtkIO::fromVTKImage( img, pImage);
    }
    catch( std::exception& e)
    {
        FW_RAISE("BitmapImage to fwData::Image failed "<<e.what());
    }
}

//------------------------------------------------------------------------------

std::string BitmapImageReader::extension()
{
    return m_availableExtensions;
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr BitmapImageReader::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

void BitmapImageReader::getAvailableExtensions(std::vector<std::string>& ext)
{
    /* Get the collection of available bitmap image readers */
    vtkSmartPointer<vtkImageReader2Collection> ir2c = vtkSmartPointer<vtkImageReader2Collection>::New();
    vtkImageReader2Factory::GetRegisteredReaders(ir2c);

    const ::boost::char_separator<char> sep(" ");

    /* Iterate over the elements of the collection */
    ir2c->InitTraversal();
    for(int i = 0; i < ir2c->GetNumberOfItems(); i++)
    {
        vtkImageReader2* ir2 = ir2c->GetNextItem();

        /* Split the string returned by GetFileExtensions() (several extensions can be available for the same type) */
        const std::string s = ir2->GetFileExtensions();
        const ::boost::tokenizer< ::boost::char_separator<char> > tokens {s, sep};

        for(const auto& token : tokens)
        {
            ext.push_back(token);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace fwVtkIO
