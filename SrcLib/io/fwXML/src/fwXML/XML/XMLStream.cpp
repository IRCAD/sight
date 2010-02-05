/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwXML/XML/XMLStream.hpp"
#include <iostream>






namespace fwXML
{

XMLStream::XMLStream()
{
}



XMLStream::~XMLStream()
{
}



int streamIOWrite(void * context,  const char * buffer, int len)
{
    std::ostream *pOs = static_cast< std::ostream * >( context );
    assert(pOs);
    return   pOs->write(buffer, len)?len:-1;

}



int streamIOClose(void * context)
{
    std::ostream *pOs = static_cast< std::ostream * >( context );
    assert(pOs);
    pOs->flush();
    return   0;

}



void XMLStream::toStream( xmlDocPtr doc, std::ostream &os )
{
    void *IOContexte = &os;

    char *encoding="UTF-8";
    int formatWithSpace=1;
    xmlCharEncodingHandlerPtr handler = xmlFindCharEncodingHandler(encoding);
    //xmlCharEncodingHandlerPtr handler = xmlGetCharEncodingHandler(XML_CHAR_ENCODING_UTF8); doesnt work :/

    assert(handler);

    //xmlThrDefIndentTreeOutput(4);
    xmlOutputBufferPtr outBuff = xmlOutputBufferCreateIO( streamIOWrite , streamIOClose, IOContexte, handler);
    xmlSaveFormatFileTo( outBuff, doc, encoding, formatWithSpace );


}


}
