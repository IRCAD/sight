/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTools/ProgressAdviser.hpp"

namespace fwTools {

ProgressAdviser::ProgressAdviser() {
    // TODO Auto-generated constructor stub

}

ProgressAdviser::~ProgressAdviser() {
    // TODO Auto-generated destructor stub
}


void   ProgressAdviser::addHandler(const ProgessHandler &handler)
{
    // append a new ProgressHandler
    m_progressSignal.connect(handler);
}


void   ProgressAdviser::notifyProgress(float percent,std::string msg)
{
    m_progressSignal(percent,msg);
}



}
