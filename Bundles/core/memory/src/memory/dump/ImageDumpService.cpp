/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include <boost/filesystem/convenience.hpp>
#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Failed.hpp>
#include <fwTools/fwID.hpp>
#include <fwTools/UUID.hpp>

#include <fwData/Image.hpp>
#include <fwData/StandardBuffer.hpp>
#include <fwData/IBufferDelegate.hpp>

#include <fwServices/Base.hpp>

// VAG todo use ImageFileFormatService
#include <itkIO/ImageWriter.hpp>
#include <itkIO/ImageReader.hpp>

#include <fwMemory/IMemoryService.hpp>
#include <fwMemory/MemoryMonitor.hpp>

#include "memory/dump/ImageDumpService.hpp"

namespace memory
{
namespace dump
{
//------------------------------------------------------------------------------

REGISTER_SERVICE( ::fwMemory::IDumpService, ::memory::dump::ImageDumpService , ::fwData::Image );

//------------------------------------------------------------------------------

ImageDumpService::ImageDumpService() :
m_currentSize( 0 ),
m_fsFilename("")
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

ImageDumpService::~ImageDumpService()
{
    SLM_TRACE_FUNC();
    if( m_bManagesBuff && m_buffer != NULL )
    {
        char *buff=static_cast<char *>(m_buffer);
        if( buff )
        {
            delete[] buff;
        }
        m_buffer = NULL ;
    }
}

//------------------------------------------------------------------------------

void ImageDumpService::info(std::ostream &_sstream )
{
    _sstream << "ImageDumpService (info)";
}

//------------------------------------------------------------------------------

void ImageDumpService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    m_image = this->getObject< ::fwData::Image >() ;

    OSLM_TRACE("ImageDumpService::start() for Image (" <<  m_image.lock() << ")");

    // Managment of current buffer
    ::boost::shared_ptr< ::fwData::IBufferDelegate > pCurrentDelegateBuffer = m_image.lock()->getBufferDelegate();
    void * currentBuffer = pCurrentDelegateBuffer->getBuffer(); // restore current buffer if is managed too
    bool currentManagesBuffer = pCurrentDelegateBuffer->getManagesBuff();
    if ( currentManagesBuffer )
    {
        pCurrentDelegateBuffer->setManagesBuff( false );
        this->setManagesBuff( true );
    }
    else
    {
        SLM_WARN("ImageDumpService::start : Dump service is activated on an image which does not manage is buffer.");
        // pCurrentDelegateBuffer->setManagesBuff( false ); // is allready the case
        this->setManagesBuff( false );
    }

    m_buffer = currentBuffer;
    m_status = IDumpService::RESTORED; // update status
//  image->setBufferDelegate( shared_from_this() ); // Change delegate buffer, the oldest (pCurrentDelegateBuffer) is removed at the end of this function.
    m_image.lock()->setBufferDelegate( ::boost::dynamic_pointer_cast< ::fwData::IBufferDelegate >( this->getSptr() ) ); // Change delegate buffer, the oldest (pCurrentDelegateBuffer) is removed at the end of this function.

    // Update current size
    m_currentSize = getVirtualSize();
    this->updateCurrentSize();
}

//------------------------------------------------------------------------------

void ImageDumpService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    OSLM_ASSERT( "Object status is NOTSET",  m_status != IDumpService::NOTSET  );
    OSLM_ASSERT( "Object status is DUMPING",  m_status != IDumpService::DUMPING  );
    OSLM_ASSERT( "Object status is RESTORING",  m_status != IDumpService::RESTORING  );

    bool associatedObjectIsDeleted = ( m_image.expired() );

    if ( ! associatedObjectIsDeleted )
    {
        if ( m_status == IDumpService::DUMPED )
        {
            this->restore();
        }

        assert( m_status == IDumpService::RESTORED );

        // Change delegate buffer
        ::fwData::StandardBuffer::NewSptr stdBuffer;
        stdBuffer->setManagesBuff( m_bManagesBuff );
        stdBuffer->setBuffer( m_buffer );
        m_image.lock()->setBufferDelegate( stdBuffer );
        m_bManagesBuff = false;
        // MemoryMonitor::getDefault() => normally last restore as already set good data => m_currentSize = virtualSize
    }
    else // Associated object is deleted ( we cannot replace IDelegateBuffer because the ptr is expired )
    {
        if ( m_status == IDumpService::RESTORED )
        {
            ::fwMemory::MemoryMonitor::getDefault()->clean(); // Erase memory info relative to this image ( cannot specify image because the ptr is expired )
        }
        else // m_status == IDumpService::DUMPED ( we cannot apply restore function because the ptr is expired )
        {
            SLM_ASSERT("m_bManagesBuff not instanced", m_bManagesBuff);       // Does not manage this case for the moment
            assert( m_buffer == NULL );     // Buffer is dumped
            assert( ::boost::filesystem::exists( m_fsFilename ) );

            ::boost::filesystem::remove( m_fsFilename ); // Erase tempory file from filesystem
            ::fwMemory::MemoryMonitor::getDefault()->clean(); // Erase memory info relative to this image ( cannot specify image because the ptr is expired )
        }
    }
}

//------------------------------------------------------------------------------

void ImageDumpService::dump()
{
    SLM_TRACE_FUNC();
    OSLM_ASSERT( "Object status must be RESTORED for dump", m_status == IDumpService::RESTORED );
    OSLM_ASSERT( "Object has a NULL buffer", m_buffer != NULL );
    OSLM_ASSERT( "Object must be unlocked for dump",  !getDumpIsLocked() );

    // Start of Dump
    m_status = IDumpService::DUMPING;

    std::stringstream ss ;
    ss << "image" << "_" << ::fwTools::UUID::get(m_image.lock()) << ".inr.gz"; // use UUID instead fwID
    m_fsFilename = getDumpFolder() / ss.str();

    // Save image
    SLM_DEBUG("Dump process : saving");

    assert(!m_image.expired());
    //::fwDataIO::ITKImageWriter::writerToFile( this->m_fsFilename.string(), m_image.lock() );
    ::itkIO::ImageWriter::NewSptr writer;
    writer->setObject(m_image.lock());
    writer->setFile( this->m_fsFilename  );
    writer->write();

    SLM_DEBUG("Dump process : saving done");

    // Delete buffer
    char * buff = static_cast< char * > ( m_buffer );
    if( buff )
    {
        delete[] buff;
    }
    OSLM_DEBUG("ImageDumpService::dump() setting buffer NULL for image" << m_image.lock().get() );
    m_buffer = NULL;

    // End of Dump
    m_status = IDumpService::DUMPED;

    // Update current size
    m_currentSize = 0;
    this->updateCurrentSize();
}

//------------------------------------------------------------------------------

bool ImageDumpService::requestMemory()
{
    SLM_TRACE_FUNC();
    SLM_ASSERT("Image expired", !m_image.expired());
    ::boost::uint64_t  virtualSize = ::fwMemory::MemoryMonitor::getDefault()->getStatus( m_image.lock() ).virtualSizeInBytes;
    return ::fwMemory::MemoryMonitor::getDefault()->reserveMemory( virtualSize - m_currentSize );
}

//------------------------------------------------------------------------------

void ImageDumpService::restore()
{
    SLM_TRACE_FUNC();
    OSLM_ASSERT( "Object status must be DUMPED for restore",  m_status == IDumpService::DUMPED );
    OSLM_ASSERT( "Object has not NULL buffer", m_buffer == NULL );

    if( requestMemory() == false ) // call policy to notify extra memory need
    {
        SLM_FATAL("Can not restore image");
        return ;
    }

    m_status = IDumpService::RESTORING;

    assert(!m_image.expired());

    try
    {
        // Load Image
        SLM_DEBUG("Restore process : loading");
        ::itkIO::ImageReader::NewSptr reader;
        reader->setObject(m_image.lock());
        reader->setFile( this->m_fsFilename );
        reader->read();

        SLM_DEBUG("Restore process : loading done");

        m_status = IDumpService::RESTORED;
    }
    catch(std::exception &e)
    {
        SLM_WARN("Not enough memory to restore, dumping other data.");

        // Get dump services
        std::vector< ::fwMemory::IDumpService::sptr > dumpServices;
        dumpServices = ::fwServices::OSR::getServices< ::fwMemory::IDumpService >();
        BOOST_FOREACH(::fwMemory::IDumpService::sptr dumpService, dumpServices)
        {
            if ( dumpService->getStatus() == IDumpService::RESTORED && !dumpService->getDumpIsLocked() && dumpService!= getSptr() )
            {
                dumpService->dump();
            }
        }

        // Load Image
        SLM_DEBUG("Restore process : loading");
        ::itkIO::ImageReader::NewSptr reader;
        reader->setObject(m_image.lock());
        reader->setFile( this->m_fsFilename );
        reader->read();

        SLM_DEBUG("Restore process : loading done");

        m_status = IDumpService::RESTORED;
    }

    ::boost::filesystem::remove( this->m_fsFilename );

    // Update current size
    m_currentSize = getVirtualSize();
    this->updateCurrentSize();
}

//------------------------------------------------------------------------------

void * ImageDumpService::getBuffer()
{
    SLM_TRACE_FUNC();
    OSLM_ASSERT( "Object status is RESTORING",  m_status != IDumpService::RESTORING );

    // If dumped => restored
    if ( m_status == IDumpService::RESTORED )
    {
        this->updateTimeStamp();
    }
    else if ( m_status == IDumpService::DUMPING )
    {
        // Do Noting
    }
    else if ( m_status == IDumpService::DUMPED )
    {
        this->restore();
        updateTimeStamp();
    }

    return m_buffer;
}

//------------------------------------------------------------------------------

void ImageDumpService::setBuffer( void * _buffer )
{
    SLM_TRACE_FUNC();
    OSLM_ASSERT( "Object status is DUMPING",  m_status != IDumpService::DUMPING );

    // If dumped => restored
    if ( m_status == IDumpService::RESTORED )
    {
        if(_buffer != m_buffer )
        {
            if( m_bManagesBuff && m_buffer != NULL)
            {
                // Delete buffer
                char *buff=static_cast<char *>(m_buffer);
                delete[] buff;
                m_buffer = NULL ;
            }
            m_buffer = _buffer;
            updateTimeStamp();
            updateVirtualSize();
            m_currentSize = getVirtualSize();
            this->updateCurrentSize();
        }
    }
    else if ( m_status == IDumpService::RESTORING )
    {
        m_buffer = _buffer;
        // Do Noting
    }
    else if ( m_status == IDumpService::DUMPED )
    {
        this->restore();
        if(_buffer != m_buffer )
        {
            if( m_bManagesBuff && m_buffer != NULL )
            {
                // Delete buffer
                char *buff=static_cast<char *>(m_buffer);
                delete[] buff;
                m_buffer = NULL ;
            }
            m_buffer = _buffer;
            updateTimeStamp();
            updateVirtualSize();
            m_currentSize = getVirtualSize();
            this->updateCurrentSize();
        }
    }
}

//------------------------------------------------------------------------------
::fwMemory::IMemoryService::sptr ImageDumpService::getMemoryService()
{
    ::fwMemory::IMemoryService::sptr memSrv;
    if (m_memoryService.expired())
    {
        if ( ::fwServices::OSR::has(m_image.lock(), "::fwMemory::IMemoryService"))
        {
            memSrv = ::fwServices::get< ::fwMemory::IMemoryService>(m_image.lock());
            m_memoryService = memSrv;
        }
    }
    else
    {
        memSrv = m_memoryService.lock();
    }
    return memSrv;
}

//------------------------------------------------------------------------------
void ImageDumpService::updateMemoryService()
{
    ::fwMemory::IMemoryService::sptr memSrv = getMemoryService();
    if (memSrv)
    {
        memSrv->update();
    }
}

//------------------------------------------------------------------------------
void ImageDumpService::updateTimeStamp()
{
    SLM_TRACE_FUNC();
    // update MemoryInfo for time stamp & virtual size
    if( !m_image.expired() )
    {
        this->updateMemoryService();
    }
}

//------------------------------------------------------------------------------

void ImageDumpService::updateVirtualSize()
{
    SLM_TRACE_FUNC();
    // update MemoryInfo for time stamp & virtual size
    if( !m_image.expired() )
    {
        this->updateMemoryService();
    }
}

//------------------------------------------------------------------------------

::boost::uint64_t ImageDumpService::getVirtualSize()
{
    SLM_TRACE_FUNC();
    ::boost::uint64_t size;
    assert(!m_image.expired());

    if ( ::fwServices::OSR::has(m_image.lock(), "::fwMemory::IMemoryService"))
    {
        size = ::fwServices::get< ::fwMemory::IMemoryService >(m_image.lock())->getVirtualSize();
        ::fwMemory::IMemoryService::sptr memSrv = getMemoryService();
        if (memSrv)
        {
            memSrv->update();
        }
    }
    else
    {
        SLM_WARN("ImageDumpService::getVirtualSize => No memory service to compute image size");
        size = 0;
    }

    return size;
}

//------------------------------------------------------------------------------

void ImageDumpService::updateCurrentSize()
{
    SLM_TRACE_FUNC();
    SLM_ASSERT("Image expired", !m_image.expired());

    ::fwMemory::MemoryMonitor::MemoryInfo memInfo = ::fwMemory::MemoryMonitor::getDefault()->getStatus( m_image.lock() );
    memInfo.currentSizeInBytes = m_currentSize;
    ::fwMemory::MemoryMonitor::getDefault()->statusChanged( m_image.lock(), memInfo );
}

//------------------------------------------------------------------------------

::boost::uint64_t ImageDumpService::getCurrentSize()
{
    SLM_TRACE_FUNC();
    return m_currentSize;
}

//------------------------------------------------------------------------------
} // end namespace dump
} // end namespace memory
