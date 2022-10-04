/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/graph/named_graph.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/noncopyable.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/comparison/greater_equal.hpp>
#include <boost/preprocessor/control/expr_if.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/facilities/is_empty_variadic.hpp>
#include <boost/preprocessor/facilities/overload.hpp>
#include <boost/preprocessor/variadic/elem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/range/combine.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/type_traits.hpp>
#include <boost/unordered_map.hpp>

#include <glm/glm.hpp>

#include <algorithm>
#include <any>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <deque>
#include <exception>
#include <filesystem>
#include <functional>
#include <future>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <ranges>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
