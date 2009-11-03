/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <boost/version.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/convenience.hpp>

#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>

// for PID
#ifdef WIN32
	#include <wtypes.h>
	#include <windows.h>
	#include <process.h>
#else
	#include <sys/types.h>
	#include <unistd.h>
    #include <signal.h>
#endif

#include <fwCore/base.hpp>

#include "fwTools/System.hpp"

#include <boost/functional/hash.hpp>

namespace fwTools
{

::boost::shared_ptr<System> System::m_instance;



System::~System()
{
	using boost::filesystem::exists;
	using boost::filesystem::remove_all;
	if ( ! m_dumpFolder.empty() && exists(m_dumpFolder) )
	{
		remove_all(m_dumpFolder);
	}
	eraseDumpFolderOfZombies();

}



const ::boost::shared_ptr<System> System::getDefault() throw()
{
	if(m_instance.get() == 0)
	{
		m_instance = ::boost::shared_ptr<System>(new System());
	}
	return m_instance;
}



int System::getPID() const
{
	int pid=0;
	#ifdef WIN32
	  pid = _getpid();
	#else
	  pid = getpid();
	#endif

	return pid;

}


std::string getPersonalizedTmp()
{
#ifdef WIN32
	char *userEnv = getenv("USERNAME"); // warning cygwin create USER env. var
#else
	char *userEnv = getenv("USER");
#endif

	assert(userEnv);

	boost::hash<std::string> string_hash;
	std::size_t hashValue = string_hash(userEnv);

	std::ostringstream path;
	path << "fwDumpFolder-" << hashValue;
//	std::string path = "fwDumpFolder-" + std::string(userEnv);
	return path.str();
}




boost::filesystem::path System::getTemporaryFolder() throw()
{
	using boost::filesystem::path;

	static path tmpdir;

	if ( tmpdir != path() )
	{
		return tmpdir; // path already setted
	}


	#ifdef WIN32
	char *userEnvTmp = getenv("TMP") ;
	char *userEnvTemp = getenv("TEMP") ;
	
	std::string EnvTmpStr(userEnvTmp);
	std::string EnvTempStr(userEnvTemp);

	if ((EnvTmpStr.find("'")!= std::string::npos) || (EnvTempStr.find("'")!= std::string::npos))
	{
		std::string str = "An apostrophe causes problem to the XML library. One is in the user name, change your TMP environment variable.";
		OSLM_ERROR("PROBLEM : " << str );
	}
	if( userEnvTmp )
	{
		tmpdir = path(userEnvTmp) / getPersonalizedTmp() ;
		OSLM_INFO("Temp Folder : " << tmpdir.string() );
		create_directories(tmpdir);
		return tmpdir ;

	}

	if( userEnvTemp )
	{
		tmpdir = path(userEnvTemp) / getPersonalizedTmp() ;
		OSLM_INFO("Temp Folder : " << tmpdir.string() );		
		create_directories(tmpdir);
		return tmpdir ;
	}
	// FIXME allow a more configurable choice for DumpFolder
	path DriveD("D:\\",boost::filesystem::native);
	path DriveC("C:\\",boost::filesystem::native);

	if ( exists( DriveD ) )
	{
		tmpdir = DriveD / getPersonalizedTmp();

		try
		{
			create_directory( tmpdir );
			create_directory( tmpdir / "writabletest" );
			remove( tmpdir / "writabletest" );
			return tmpdir;
		}
		catch(...)
		{
			// OK attempt fail use default behavior
		}

	}

	// default behavior
	tmpdir = DriveC / path(getPersonalizedTmp(),boost::filesystem::native);

	#else
	tmpdir = path("/tmp") /  path( getPersonalizedTmp() );
	#endif

	create_directories(tmpdir);
	return tmpdir;
}




System::System()
{
	int pid=getPID();
	assert ( isProcessRunning(pid) ); // verify pid is OK
	m_dumpFolder = System::getTemporaryFolder() / boost::lexical_cast<std::string>(pid);

	using boost::filesystem::exists;
	using boost::filesystem::create_directory;

	// create TemporaryFolder if necessary
	if (!exists(System::getTemporaryFolder()))
	{
		create_directory(System::getTemporaryFolder());
	}

	// create dumpFolder if necessary
	if (!exists(m_dumpFolder))
	{
		create_directory(m_dumpFolder);
	}

	if (!exists(m_dumpFolder))
	{
		std::string str = "DumpableDataBuffer unable to use";
		str+= m_dumpFolder.string() + " directory to dump Image memory buffer";
		throw std::ios_base::failure(str);
	}
}

boost::filesystem::path System::getDumpFolder() throw()
{
	return m_dumpFolder;
}



void System::eraseDumpFolderOfZombies() const
{

	 namespace fs = boost::filesystem;
     fs::directory_iterator end_itr; // default  construction yields past-the-end
	 fs::directory_iterator iter_dir( getTemporaryFolder() );
	 for ( ; iter_dir != end_itr; ++iter_dir )
	 {
#if BOOST_VERSION < 103600
     		std::string pidStr = (*iter_dir).leaf();
#else
     		std::string pidStr = (*iter_dir).filename();
#endif
     		try
     		{
     			int pid = boost::lexical_cast<int>(pidStr);
     			if (!isProcessRunning(pid))
     			{
     				eraseDumpFolder(pid);
     			}
     		}
     		catch (...)
     		{
     			// nothing to do ignore this directory
     		}
	 }

}

void System::cleanDumpFolder() const
{
	 // remove all files in subfolder
     using boost::filesystem::directory_iterator;
     directory_iterator end_itr; // default  construction yields past-the-end
	 directory_iterator iter_dir( m_dumpFolder );
	 for ( ; iter_dir != end_itr; ++iter_dir )
	 {
     		boost::filesystem::remove_all(*iter_dir);
	 }

}

void System::eraseDumpFolder(int pid) const
{
	assert ( !isProcessRunning(pid) );

	namespace fs = boost::filesystem;

	fs::path dumpFolderPID= System::getTemporaryFolder() / boost::lexical_cast<std::string>(pid);

	fs::remove_all(dumpFolderPID);
}

bool System::isProcessRunning(int pid)
{
	#ifdef WIN32
		HANDLE hProcess;
		//hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pid);hProcess==INVALID_HANDLE_VALUE
		hProcess = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, pid);
		if ( hProcess==INVALID_HANDLE_VALUE ) // FIXME find the correct windows value
		{
			CloseHandle( hProcess );
			return false;
		}
		else if ( hProcess==0 )
		{
			return false;
		}
		else
		{
			CloseHandle( hProcess );
			return true;
		}
	#else
	  	return kill(pid,0) == 0 ;
	#endif

	return true;

}



} //end namespace fwTools
