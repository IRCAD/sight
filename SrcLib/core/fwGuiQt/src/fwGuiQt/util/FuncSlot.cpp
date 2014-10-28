#include <fwGuiQt/util/FuncSlot.hpp>

namespace fwGuiQt
{

namespace util
{

void doNothing()
{}

FuncSlot::FuncSlot() : m_func( doNothing )
{
}

void FuncSlot::trigger()
{
    m_func();
}

} // namespace util

} // namespace fwGuiQt

