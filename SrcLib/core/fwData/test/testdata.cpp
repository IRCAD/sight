/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility> // pair.h

#include <fwData/Image.hpp>
#include <fwData/ProcessObject.hpp>
#include <fwData/Float.hpp>
#include <fwData/String.hpp>
#include <fwData/Factory.hpp>

#include "codeGenerator/FieldParser.hpp"
#include "codeGenerator/HppGenerator.hpp"
#include "codeGenerator/CppGenerator.hpp"
#include "codeGenerator/SaveGenerator.hpp"
#include "codeGenerator/LoadGenerator.hpp"
#include "codeGenerator/MfoToDataConvertGenerator.hpp"
#include "codeGenerator/DataToMfoConvertGenerator.hpp"

#include <fwCore/base.hpp>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

::boost::shared_ptr< ::fwData::ProcessObject > NewTestWithIOInitialized(const std::string _name , bool initialize )
{
	 ::boost::shared_ptr< ::fwData::ProcessObject > op = fwTools::Factory::New< ::fwData::ProcessObject >() ;
	if( _name == "mean" )
	{
		 ::boost::shared_ptr< ::fwData::String > name( new ::fwData::String ) ;
		name->value() = "mean" ;
		op->setFieldSingleElement("ID_NAME",  name) ;

		if( initialize  )
		{
			op->getInputs()->setFieldSingleElement( "INPUTIMAGE" ,  			fwTools::Factory::New< ::fwData::Image >()) ;
			op->getInputs()->setFieldSingleElement( "SIGMA_X" ,  				fwTools::Factory::New< ::fwData::Float >()) ;	
			op->getInputs()->setFieldSingleElement( "SIGMA_Y" ,  				fwTools::Factory::New< ::fwData::Float >()) ;	
			op->getInputs()->setFieldSingleElement( "SIGMA_Z" ,  				fwTools::Factory::New< ::fwData::Float >()) ;	
			op->getOutputs()->setFieldSingleElement( "OUTPUTIMAGE" ,  	fwTools::Factory::New< ::fwData::Image >()) ;
		}
	}
	if( _name == "erode" )
	{
		 ::boost::shared_ptr< ::fwData::String > name( new ::fwData::String ) ;
		name->value() = "erode" ;
		op->setFieldSingleElement("ID_NAME",  name) ;

		if( initialize  )
		{
			op->getInputs()->setFieldSingleElement( "INPUTIMAGE" ,  			fwTools::Factory::New< ::fwData::Image >()) ;
			op->getInputs()->setFieldSingleElement( "SIGMA_X" ,  				fwTools::Factory::New< ::fwData::Float >()) ;	
			op->getInputs()->setFieldSingleElement( "SIGMA_Y" ,  				fwTools::Factory::New< ::fwData::Float >()) ;	
			op->getInputs()->setFieldSingleElement( "SIGMA_Z" ,  				fwTools::Factory::New< ::fwData::Float >()) ;	
			op->getOutputs()->setFieldSingleElement( "OUTPUTIMAGE" ,  	fwTools::Factory::New< ::fwData::Image >()) ;
		}
	}
	if( _name == "threshold" )
	{
		 ::boost::shared_ptr< ::fwData::String > name( new ::fwData::String ) ;
		name->value() = "threshold" ;
		op->setFieldSingleElement("ID_NAME",  name) ;

		if( initialize  )
		{
			op->getInputs()->setFieldSingleElement( "INPUTIMAGE" ,  			fwTools::Factory::New< ::fwData::Image >()) ;
			op->getInputs()->setFieldSingleElement( "THRESHOLD" , 			fwTools::Factory::New< ::fwData::Float >()) ;	
			op->getOutputs()->setFieldSingleElement( "OUTPUTIMAGE" ,  	fwTools::Factory::New< ::fwData::Image >()) ;
		}
	}
	
	return op ;
}
//------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	
	 ::boost::shared_ptr< ::fwData::ProcessObject > meanOp 			= NewTestWithIOInitialized( "mean" , true ) ;
	 ::boost::shared_ptr< ::fwData::ProcessObject > erodeOp 			= NewTestWithIOInitialized( "erode" , true ) ;
	 ::boost::shared_ptr< ::fwData::ProcessObject > thresholdOp 	= NewTestWithIOInitialized( "threshold" , true ) ;
	
	
	SLM_TRACE("TRACE");
	SLM_DEBUG("DEBUG");
	SLM_INFO("INFO");
	
    if (argc != 4)
	{
		std::cout << "Syntax error => \"./testdata -cpp(hpp,mfo2data,data2mfo,load,save) fields.txt className\"" << std::endl;
		return 0;
	}

	std::string typeGeneration (argv[1]);
	std::string pathFile (argv[2]);
	std::string className (argv[3]);
	
	std::vector< std::pair< std::string, std::string > > fieldAndType = codeGenerator::fieldParser(pathFile);
	
	if(typeGeneration == "-hpp" )
	{
		codeGenerator::generateHPP(fieldAndType, className);
	}
	else if(typeGeneration == "-cpp" )
	{
		codeGenerator::generateCPP(fieldAndType, className);
	}
	else if(typeGeneration == "-mfo2data" )
	{
		codeGenerator::generateMfoToDataConvert(fieldAndType, className);
	}
	else if(typeGeneration == "-data2mfo" )
	{
		codeGenerator::generateDataToMfoConvert(fieldAndType, className);
	}
	else if(typeGeneration == "-load" )
	{
		codeGenerator::generateLoad(fieldAndType, className);
	}
	else if(typeGeneration == "-save" )
	{
		codeGenerator::generateSave(fieldAndType, className);
	}
	
	
}

