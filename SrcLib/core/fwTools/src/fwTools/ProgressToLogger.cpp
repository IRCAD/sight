/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include "fwTools/ProgressToLogger.hpp"

namespace fwTools {

ProgressToLogger::ProgressToLogger(std::string prefix, std::string postfix)
: m_prefix(prefix),
  m_postfix(postfix)
{
}

ProgressToLogger::~ProgressToLogger() {
        // TODO Auto-generated destructor stub
}


void ProgressToLogger::operator()(float percent,std::string msg)
{
        // fixme io precision
        OSLM_INFO( m_prefix << msg << " " << (int) (percent*100) << m_postfix);
}

}
