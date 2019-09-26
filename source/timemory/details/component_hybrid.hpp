// MIT License
//
// Copyright (c) 2019, The Regents of the University of California,
// through Lawrence Berkeley National Laboratory (subject to receipt of any
// required approvals from the U.S. Dept. of Energy).  All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "timemory/manager.hpp"
#include "timemory/mpl/filters.hpp"

//======================================================================================//
//
//      tim::get functions
//
namespace tim
{
//--------------------------------------------------------------------------------------//

template <typename _Tuple, typename _List,
          typename _Ret = decltype(std::tuple_cat(get(std::declval<_Tuple>()),
                                                  get(std::declval<_List>())))>
_Ret
get(const component_hybrid<_Tuple, _List>& _obj)
{
    return std::tuple_cat(get(_obj.get_lhs()), get(_obj.get_rhs()));
}

//--------------------------------------------------------------------------------------//

template <typename _Tuple, typename _List,
          typename _Ret = decltype(std::tuple_cat(get_labeled(std::declval<_Tuple>()),
                                                  get_labeled(std::declval<_List>())))>
_Ret
get_labeled(const component_hybrid<_Tuple, _List>& _obj)
{
    return std::tuple_cat(get_labeled(_obj.get_lhs()), get_labeled(_obj.get_rhs()));
}

}  // namespace tim

//--------------------------------------------------------------------------------------//