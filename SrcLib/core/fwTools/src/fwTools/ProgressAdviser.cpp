/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTools/ProgressAdviser.hpp"

namespace fwTools
{

ProgressAdviser::ProgressAdviser()
{
}

ProgressAdviser::~ProgressAdviser()
{
}

void ProgressAdviser::addHandler(const ProgessHandler &handler)
{
    // append a new ProgressHandler
    m_progressSignal.connect(handler);
}

void ProgressAdviser::notifyProgress(float percent, const std::string& msg) const
{
    m_progressSignal(percent, msg);
}

}
