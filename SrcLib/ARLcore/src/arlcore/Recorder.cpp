/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Recorder.h>

arlCore::Recorder::Recorder( void ):
m_recorderFlag(false),
m_recorderAfterRender(false),
m_nbSnapshots(-1),
m_recorderDirectory(""),
m_fullRecorderDirectory(""),
m_recorderFullName("")
{}

arlCore::Recorder::~Recorder( void )
{}

bool arlCore::Recorder::getRecorderFilename( std::string &fileName )
{
    bool b = m_recorderHandle.is_open();
    if(b) fileName = m_recorderFullName;
    else fileName = "";
    return b;
}

bool arlCore::Recorder::getRecorderDirectory( std::string &directory )
{
    bool b = m_recorderHandle.is_open();
    if(b) directory = m_recorderDirectory;
    else directory = "";
    return b;
}

bool arlCore::Recorder::startRecorder( long int nbSnapshots, bool afterRender )
{
    m_nbSnapshots = nbSnapshots;
    if( m_recorderHandle.is_open() && m_nbSnapshots!=0 )
        m_recorderFlag=true;
    else m_recorderFlag=false;
    m_recorderAfterRender = afterRender;
    return m_recorderFlag;
}

bool arlCore::Recorder::stopRecorder( void )
{
    m_recorderFlag=false;
    return m_recorderFlag;
}

bool arlCore::Recorder::isRecording( void )
{
    return m_recorderFlag;
}

void arlCore::Recorder::closeRecorder( void )
{
    stopRecorder();
    m_recorderHandle.close();
}
