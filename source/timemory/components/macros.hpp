// MIT License
//
// Copyright (c) 2020, The Regents of the University of California,
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

#pragma once

#include "timemory/components/metadata.hpp"
#include "timemory/components/properties.hpp"
#include "timemory/dll.hpp"
#include "timemory/macros.hpp"

#include <string>
#include <unordered_set>

//--------------------------------------------------------------------------------------//
//
#if defined(TIMEMORY_USE_EXTERN) && !defined(TIMEMORY_USE_COMPONENT_EXTERN)
#    define TIMEMORY_USE_COMPONENT_EXTERN
#endif

#if !defined(TIMEMORY_USE_COMPONENT_EXTERN) && !defined(TIMEMORY_COMPONENT_SOURCE) &&    \
    !defined(TIMEMORY_COMPONENT_HEADER_MODE)
#    define TIMEMORY_COMPONENT_HEADER_MODE
#endif

//--------------------------------------------------------------------------------------//
//
/**
 * \macro TIMEMORY_DECLARE_COMPONENT
 * \brief Declare a non-templated component type in the tim::component namespace
 */

#if !defined(TIMEMORY_DECLARE_COMPONENT)
#    define TIMEMORY_DECLARE_COMPONENT(NAME)                                             \
        namespace tim                                                                    \
        {                                                                                \
        namespace component                                                              \
        {                                                                                \
        struct NAME;                                                                     \
        }                                                                                \
        namespace trait                                                                  \
        {                                                                                \
        template <>                                                                      \
        struct is_component<component::NAME> : true_type                                 \
        {};                                                                              \
        }                                                                                \
        }
#endif
//
//--------------------------------------------------------------------------------------//
//
/**
 * \macro TIMEMORY_DECLARE_API_COMPONENTS
 * \brief Declare a non-templated component type in the tim::component namespace
 */

#if !defined(TIMEMORY_DECLARE_API_COMPONENTS)
#    define TIMEMORY_DECLARE_API_COMPONENTS(API, ...)                                    \
        namespace tim                                                                    \
        {                                                                                \
        namespace trait                                                                  \
        {                                                                                \
        template <>                                                                      \
        struct api_components<API, void>                                                 \
        {                                                                                \
            using type = type_list<__VA_ARGS__>;                                         \
        };                                                                               \
        }                                                                                \
        }
#endif
//
//--------------------------------------------------------------------------------------//
//
/**
 * \macro TIMEMORY_SET_COMPONENT_APIS
 * \brief Declare a component is part of one or more APIs
 */

#if !defined(TIMEMORY_SET_COMPONENT_API)
#    define TIMEMORY_SET_COMPONENT_API(COMP, ...)                                        \
        namespace tim                                                                    \
        {                                                                                \
        namespace trait                                                                  \
        {                                                                                \
        template <>                                                                      \
        struct component_apis<COMP>                                                      \
        {                                                                                \
            using type = type_list<__VA_ARGS__>;                                         \
        };                                                                               \
        }                                                                                \
        }
#endif
//
//--------------------------------------------------------------------------------------//
//
/**
 * \macro TIMEMORY_SET_TEMPLATE_COMPONENT_API
 * \brief Declare a component is part of one or more APIs
 */

#if !defined(TIMEMORY_SET_TEMPLATE_COMPONENT_API)
#    define TIMEMORY_SET_TEMPLATE_COMPONENT_API(TARGS, TSPECIAL, ...)                    \
        namespace tim                                                                    \
        {                                                                                \
        namespace trait                                                                  \
        {                                                                                \
        template <TARGS>                                                                 \
        struct component_apis<TSPECIAL>                                                  \
        {                                                                                \
            using type = type_list<__VA_ARGS__>;                                         \
        };                                                                               \
        }                                                                                \
        }
#endif
//
//--------------------------------------------------------------------------------------//
//
/**
 * \macro TIMEMORY_BUNDLE_INDEX
 * \brief Declare a bundle index
 */

#if !defined(TIMEMORY_BUNDLE_INDEX)
#    define TIMEMORY_BUNDLE_INDEX(NAME, IDX)                                             \
        namespace tim                                                                    \
        {                                                                                \
        namespace component                                                              \
        {                                                                                \
        static constexpr size_t NAME = IDX;                                              \
        }                                                                                \
        }
#endif
//
//--------------------------------------------------------------------------------------//
//
/**
 * \macro TIMEMORY_DECLARE_TEMPLATE_COMPONENT
 * \brief Declare a templated component type in the tim::component namespace
 */

#if !defined(TIMEMORY_DECLARE_TEMPLATE_COMPONENT)
#    define TIMEMORY_DECLARE_TEMPLATE_COMPONENT(NAME, ...)                               \
        namespace tim                                                                    \
        {                                                                                \
        namespace component                                                              \
        {                                                                                \
        template <__VA_ARGS__>                                                           \
        struct NAME;                                                                     \
        }                                                                                \
        }
#endif
//
//--------------------------------------------------------------------------------------//
//
/**
 * \macro TIMEMORY_TEMPLATE_COMPONENT
 * \brief Declare a templated component type in the tim::component namespace
 */
#if !defined(TIMEMORY_TEMPLATE_COMPONENT)
#    define TIMEMORY_TEMPLATE_COMPONENT(NAME, TEMPLATE_PARAM, ...)                       \
        namespace tim                                                                    \
        {                                                                                \
        namespace component                                                              \
        {                                                                                \
        template <TEMPLATE_PARAM>                                                        \
        struct NAME;                                                                     \
        }                                                                                \
        namespace trait                                                                  \
        {                                                                                \
        template <TEMPLATE_PARAM>                                                        \
        struct is_component<component::NAME<__VA_ARGS__>> : true_type                    \
        {};                                                                              \
        }                                                                                \
        }
#endif
//
//--------------------------------------------------------------------------------------//
//
/**
 * \macro TIMEMORY_COMPONENT_ALIAS
 * \brief Declare a non-templated alias to a component in the tim::component namespace
 */

#if !defined(TIMEMORY_COMPONENT_ALIAS)
#    define TIMEMORY_COMPONENT_ALIAS(NAME, ...)                                          \
        namespace tim                                                                    \
        {                                                                                \
        namespace component                                                              \
        {                                                                                \
        using NAME = __VA_ARGS__;                                                        \
        }                                                                                \
        }
#endif

//======================================================================================//

/**
 * \macro TIMEMORY_PROPERTY_SPECIALIZATION
 * \brief Specialization of the property specialization
 */

#if !defined(TIMEMORY_PROPERTY_SPECIALIZATION) && !defined(TIMEMORY_DISABLE_PROPERTIES)
#    define TIMEMORY_PROPERTY_SPECIALIZATION(TYPE, ENUM, ID, ...)                        \
        namespace tim                                                                    \
        {                                                                                \
        namespace component                                                              \
        {                                                                                \
        template <>                                                                      \
        struct properties<TYPE> : static_properties<TYPE>                                \
        {                                                                                \
            using type                        = TYPE;                                    \
            using value_type                  = TIMEMORY_COMPONENT;                      \
            static constexpr value_type value = ENUM;                                    \
            static const char*          enum_string() { return #ENUM; }                  \
            static const char*          id() { return ID; }                              \
            static const idset_t&       ids()                                            \
            {                                                                            \
                static auto _instance = []() {                                           \
                    auto _val = idset_t{ ID, __VA_ARGS__ };                              \
                    if(_val.find("") != _val.end())                                      \
                        _val.erase("");                                                  \
                    return _val;                                                         \
                }();                                                                     \
                return _instance;                                                        \
            }                                                                            \
            template <typename Archive>                                                  \
            void save(Archive& ar, const unsigned int) const                             \
            {                                                                            \
                ar(cereal::make_nvp("value", ENUM),                                      \
                   cereal::make_nvp("enum", std::string{ enum_string() }),               \
                   cereal::make_nvp("id", std::string{ id() }),                          \
                   cereal::make_nvp("ids", ids()));                                      \
            }                                                                            \
            template <typename Archive>                                                  \
            void load(Archive&, const unsigned int)                                      \
            {}                                                                           \
            TIMEMORY_COMPONENT operator()() { return ENUM; }                             \
                                                                                         \
            constexpr operator TIMEMORY_COMPONENT() const { return ENUM; }               \
        };                                                                               \
        template <>                                                                      \
        struct enumerator<ENUM> : properties<TYPE>                                       \
        {                                                                                \
            using type                  = TYPE;                                          \
            static constexpr bool value = ::tim::trait::is_available<TYPE>::value;       \
        };                                                                               \
        }                                                                                \
        }
#elif !defined(TIMEMORY_PROPERTY_SPECIALIZATION) && defined(TIMEMORY_DISABLE_PROPERTIES)
#    define TIMEMORY_PROPERTY_SPECIALIZATION(...)
#endif

//======================================================================================//

/**
 * \macro TIMEMORY_METADATA_SPECIALIZATION
 * \brief Specialization of the property specialization
 */

#if !defined(TIMEMORY_METADATA_SPECIALIZATION) && !defined(TIMEMORY_DISABLE_METADATA)
#    define TIMEMORY_METADATA_SPECIALIZATION(TYPE, LABEL, BASIC_DESC, ...)               \
        namespace tim                                                                    \
        {                                                                                \
        namespace component                                                              \
        {                                                                                \
        template <>                                                                      \
        struct metadata<TYPE>                                                            \
        {                                                                                \
            using type                        = TYPE;                                    \
            using value_type                  = TIMEMORY_COMPONENT;                      \
            static constexpr value_type value = properties<TYPE>::value;                 \
            static std::string          name() { return LABEL; }                         \
            static std::string          label() { return LABEL; }                        \
            static std::string          description() { return BASIC_DESC; }             \
            static std::string          extra_description()                              \
            {                                                                            \
                return TIMEMORY_JOIN(" ", __VA_ARGS__);                                  \
            }                                                                            \
        };                                                                               \
        }                                                                                \
        }
#elif !defined(TIMEMORY_METADATA_SPECIALIZATION) && defined(TIMEMORY_DISABLE_METADATA)
#    define TIMEMORY_METADATA_SPECIALIZATION(...)
#endif

//======================================================================================//

/**
 * \macro TIMEMORY_TOOLSET_ALIAS
 * \brief Creates an alias for a complex type when declaring the statistics type
 */

#if !defined(TIMEMORY_TOOLSET_ALIAS)
#    define TIMEMORY_TOOLSET_ALIAS(NAME, WRAPPER, ...)                                   \
        namespace tim                                                                    \
        {                                                                                \
        namespace component                                                              \
        {                                                                                \
        namespace aliases                                                                \
        {                                                                                \
        using NAME = WRAPPER<__VA_ARGS__>;                                               \
        }                                                                                \
        }                                                                                \
        }                                                                                \
        using tim::component::aliases::NAME;
#endif

//======================================================================================//
//
//      EXTERN TEMPLATE DECLARE AND INSTANTIATE
//
//======================================================================================//

#if !defined(_EXTERN_NAME_COMBINE)
#    define _EXTERN_NAME_COMBINE(X, Y) X##Y
#endif
//
//--------------------------------------------------------------------------------------//
//
#if !defined(_EXTERN_TUPLE_ALIAS)
#    define _EXTERN_TUPLE_ALIAS(Y) _EXTERN_NAME_COMBINE(extern_tuple_, Y)
#endif
//
//--------------------------------------------------------------------------------------//
//
#if !defined(_EXTERN_LIST_ALIAS)
#    define _EXTERN_LIST_ALIAS(Y) _EXTERN_NAME_COMBINE(extern_list_, Y)
#endif

//--------------------------------------------------------------------------------------//

#if !defined(TIMEMORY_INITIALIZE_STORAGE)
#    define TIMEMORY_INITIALIZE_STORAGE(...)                                             \
        namespace tim                                                                    \
        {                                                                                \
        namespace internal                                                               \
        {                                                                                \
        namespace initialization                                                         \
        {                                                                                \
        namespace                                                                        \
        {                                                                                \
        using namespace ::tim::component;                                                \
        namespace component = ::tim::component;                                          \
        auto _TIM_STORAGE_INIT(__COUNTER__) =                                            \
            ::tim::storage_initializer::get<__VA_ARGS__>();                              \
        }                                                                                \
        }                                                                                \
        }                                                                                \
        }
#endif

//--------------------------------------------------------------------------------------//
// backwards compatibility
#if !defined(TIMEMORY_STORAGE_INITIALIZER)
#    define TIMEMORY_STORAGE_INITIALIZER(...) TIMEMORY_INITIALIZE_STORAGE(__VA_ARGS__)
#endif

//--------------------------------------------------------------------------------------//
//
#if !defined(TIMEMORY_EXTERN_STORAGE_ALIASES)
#    define TIMEMORY_EXTERN_STORAGE_ALIASES                                              \
        namespace tim                                                                    \
        {                                                                                \
        namespace alias                                                                  \
        {                                                                                \
        template <typename T>                                                            \
        using storage_t = storage<T, typename T::value_type>;                            \
        template <typename T>                                                            \
        using storage_impl_t = impl::storage<T, trait::uses_value_storage<T>::value>;    \
        template <typename T>                                                            \
        using storage_deleter_t = impl::storage_deleter<storage_impl_t<T>>;              \
        template <typename T>                                                            \
        using storage_pointer_t =                                                        \
            std::unique_ptr<alias::storage_impl_t<T>, alias::storage_deleter_t<T>>;      \
        }                                                                                \
        }
#endif
//
//--------------------------------------------------------------------------------------//
//
#if !defined(TIMEMORY_DECLARE_EXTERN_STORAGE)
#    define TIMEMORY_DECLARE_EXTERN_STORAGE(TYPE)                                            \
        TIMEMORY_EXTERN_STORAGE_ALIASES                                                      \
        namespace tim                                                                        \
        {                                                                                    \
        extern template class impl::storage<TYPE,                                            \
                                            trait::uses_value_storage<TYPE>::value>;         \
        extern template class storage<TYPE, typename TYPE::value_type>;                      \
        extern template class singleton<alias::storage_impl_t<TYPE>,                         \
                                        alias::storage_pointer_t<TYPE>, TIMEMORY_API>;       \
        extern template storage_singleton<alias::storage_t<TYPE>>*                           \
                                            get_storage_singleton<alias::storage_t<TYPE>>(); \
        extern template storage_initializer storage_initializer::get<TYPE>();                \
        namespace node                                                                       \
        {                                                                                    \
        extern template struct data<TYPE>;                                                   \
        extern template struct graph<TYPE>;                                                  \
        extern template struct result<TYPE>;                                                 \
        extern template struct tree<TYPE>;                                                   \
        }                                                                                    \
        }
#endif
//
//--------------------------------------------------------------------------------------//
//
#if !defined(TIMEMORY_INSTANTIATE_EXTERN_STORAGE)
#    define TIMEMORY_INSTANTIATE_EXTERN_STORAGE(TYPE)                                    \
        TIMEMORY_EXTERN_STORAGE_ALIASES                                                  \
        namespace tim                                                                    \
        {                                                                                \
        template class impl::storage<TYPE, trait::uses_value_storage<TYPE>::value>;      \
        template class storage<TYPE, typename TYPE::value_type>;                         \
        template class singleton<alias::storage_impl_t<TYPE>,                            \
                                 alias::storage_pointer_t<TYPE>, TIMEMORY_API>;          \
        template storage_singleton<alias::storage_t<TYPE>>*                              \
                                     get_storage_singleton<alias::storage_t<TYPE>>();    \
        template storage_initializer storage_initializer::get<TYPE>();                   \
        namespace node                                                                   \
        {                                                                                \
        template struct data<TYPE>;                                                      \
        template struct graph<TYPE>;                                                     \
        template struct result<TYPE>;                                                    \
        template struct tree<TYPE>;                                                      \
        }                                                                                \
        }                                                                                \
        namespace                                                                        \
        {                                                                                \
        using namespace ::tim::component;                                                \
        namespace component                 = ::tim::component;                          \
        auto _TIM_STORAGE_INIT(__COUNTER__) = ::tim::storage_initializer::get<TYPE>();   \
        }
#endif

//======================================================================================//
//
//      EXTERN OPERATION DECLARE AND INSTANTIATE
//
//======================================================================================//
//
#if !defined(TIMEMORY_DECLARE_EXTERN_OPERATIONS)
#    define TIMEMORY_DECLARE_EXTERN_OPERATIONS(COMPONENT_NAME, HAS_DATA)                 \
        namespace tim                                                                    \
        {                                                                                \
        namespace component                                                              \
        {                                                                                \
        namespace factory                                                                \
        {                                                                                \
        extern template opaque           get_opaque<COMPONENT_NAME>();                   \
        extern template opaque           get_opaque<COMPONENT_NAME>(scope::config);      \
        extern template std::set<size_t> get_typeids<COMPONENT_NAME>();                  \
        }                                                                                \
        }                                                                                \
        namespace operation                                                              \
        {                                                                                \
        extern template struct add_secondary<COMPONENT_NAME>;                            \
        extern template struct add_statistics<COMPONENT_NAME>;                           \
        extern template struct assemble<COMPONENT_NAME>;                                 \
        extern template struct audit<COMPONENT_NAME>;                                    \
        extern template struct cache<COMPONENT_NAME>;                                    \
        extern template struct cleanup<COMPONENT_NAME>;                                  \
        extern template struct construct<COMPONENT_NAME>;                                \
        extern template struct copy<COMPONENT_NAME>;                                     \
        extern template struct derive<COMPONENT_NAME>;                                   \
        extern template struct fini<COMPONENT_NAME>;                                     \
        extern template struct fini_storage<COMPONENT_NAME>;                             \
        extern template struct echo_measurement<                                         \
            COMPONENT_NAME, trait::echo_enabled<COMPONENT_NAME>::value>;                 \
        extern template struct extra_serialization<COMPONENT_NAME>;                      \
        extern template struct get<COMPONENT_NAME>;                                      \
        extern template struct init<COMPONENT_NAME>;                                     \
        extern template struct init_storage<COMPONENT_NAME>;                             \
        extern template struct is_running<COMPONENT_NAME, true>;                         \
        extern template struct is_running<COMPONENT_NAME, false>;                        \
        extern template struct minus<COMPONENT_NAME>;                                    \
        extern template struct plus<COMPONENT_NAME>;                                     \
        extern template struct pop_node<COMPONENT_NAME>;                                 \
        extern template struct print<COMPONENT_NAME>;                                    \
        extern template struct print_header<COMPONENT_NAME>;                             \
        extern template struct print_statistics<COMPONENT_NAME>;                         \
        extern template struct print_storage<COMPONENT_NAME>;                            \
        extern template struct push_node<COMPONENT_NAME>;                                \
        extern template struct record<COMPONENT_NAME>;                                   \
        extern template struct reset<COMPONENT_NAME>;                                    \
        extern template struct serialization<COMPONENT_NAME>;                            \
        extern template struct set_prefix<COMPONENT_NAME>;                               \
        extern template struct set_scope<COMPONENT_NAME>;                                \
        extern template struct set_started<COMPONENT_NAME>;                              \
        extern template struct set_stopped<COMPONENT_NAME>;                              \
        extern template struct store<COMPONENT_NAME>;                                    \
        extern template struct finalize::dmp_get<                                        \
            COMPONENT_NAME, HAS_DATA && trait::is_available<COMPONENT_NAME>::value>;     \
        extern template struct finalize::get<                                            \
            COMPONENT_NAME, HAS_DATA && trait::is_available<COMPONENT_NAME>::value>;     \
        extern template struct finalize::merge<                                          \
            COMPONENT_NAME, HAS_DATA && trait::is_available<COMPONENT_NAME>::value>;     \
        extern template struct finalize::mpi_get<                                        \
            COMPONENT_NAME, HAS_DATA && trait::is_available<COMPONENT_NAME>::value>;     \
        extern template struct finalize::print<                                          \
            COMPONENT_NAME, HAS_DATA && trait::is_available<COMPONENT_NAME>::value>;     \
        extern template struct finalize::upc_get<                                        \
            COMPONENT_NAME, HAS_DATA && trait::is_available<COMPONENT_NAME>::value>;     \
        }                                                                                \
        }
#endif
//
//--------------------------------------------------------------------------------------//
//
#if !defined(TIMEMORY_INSTANTIATE_EXTERN_OPERATIONS)
#    define TIMEMORY_INSTANTIATE_EXTERN_OPERATIONS(COMPONENT_NAME, HAS_DATA)             \
        namespace tim                                                                    \
        {                                                                                \
        namespace component                                                              \
        {                                                                                \
        namespace factory                                                                \
        {                                                                                \
        template opaque           get_opaque<COMPONENT_NAME>();                          \
        template opaque           get_opaque<COMPONENT_NAME>(scope::config);             \
        template std::set<size_t> get_typeids<COMPONENT_NAME>();                         \
        }                                                                                \
        }                                                                                \
        namespace operation                                                              \
        {                                                                                \
        template struct add_secondary<COMPONENT_NAME>;                                   \
        template struct add_statistics<COMPONENT_NAME>;                                  \
        template struct assemble<COMPONENT_NAME>;                                        \
        template struct audit<COMPONENT_NAME>;                                           \
        template struct cache<COMPONENT_NAME>;                                           \
        template struct cleanup<COMPONENT_NAME>;                                         \
        template struct construct<COMPONENT_NAME>;                                       \
        template struct copy<COMPONENT_NAME>;                                            \
        template struct derive<COMPONENT_NAME>;                                          \
        template struct echo_measurement<COMPONENT_NAME,                                 \
                                         trait::echo_enabled<COMPONENT_NAME>::value>;    \
        template struct extra_serialization<COMPONENT_NAME>;                             \
        template struct fini<COMPONENT_NAME>;                                            \
        template struct fini_storage<COMPONENT_NAME>;                                    \
        template struct get<COMPONENT_NAME>;                                             \
        template struct init<COMPONENT_NAME>;                                            \
        template struct init_storage<COMPONENT_NAME>;                                    \
        template struct is_running<COMPONENT_NAME, true>;                                \
        template struct is_running<COMPONENT_NAME, false>;                               \
        template struct minus<COMPONENT_NAME>;                                           \
        template struct plus<COMPONENT_NAME>;                                            \
        template struct pop_node<COMPONENT_NAME>;                                        \
        template struct print<COMPONENT_NAME>;                                           \
        template struct print_header<COMPONENT_NAME>;                                    \
        template struct print_statistics<COMPONENT_NAME>;                                \
        template struct print_storage<COMPONENT_NAME>;                                   \
        template struct push_node<COMPONENT_NAME>;                                       \
        template struct record<COMPONENT_NAME>;                                          \
        template struct reset<COMPONENT_NAME>;                                           \
        template struct serialization<COMPONENT_NAME>;                                   \
        template struct set_prefix<COMPONENT_NAME>;                                      \
        template struct set_scope<COMPONENT_NAME>;                                       \
        template struct set_started<COMPONENT_NAME>;                                     \
        template struct set_stopped<COMPONENT_NAME>;                                     \
        template struct store<COMPONENT_NAME>;                                           \
        template struct finalize::dmp_get<                                               \
            COMPONENT_NAME, HAS_DATA && trait::is_available<COMPONENT_NAME>::value>;     \
        template struct finalize::get<                                                   \
            COMPONENT_NAME, HAS_DATA && trait::is_available<COMPONENT_NAME>::value>;     \
        template struct finalize::merge<                                                 \
            COMPONENT_NAME, HAS_DATA && trait::is_available<COMPONENT_NAME>::value>;     \
        template struct finalize::mpi_get<                                               \
            COMPONENT_NAME, HAS_DATA && trait::is_available<COMPONENT_NAME>::value>;     \
        template struct finalize::print<                                                 \
            COMPONENT_NAME, HAS_DATA && trait::is_available<COMPONENT_NAME>::value>;     \
        template struct finalize::upc_get<                                               \
            COMPONENT_NAME, HAS_DATA && trait::is_available<COMPONENT_NAME>::value>;     \
        }                                                                                \
        }
#endif

//======================================================================================//

#if defined(TIMEMORY_COMPONENT_SOURCE)
//
//--------------------------------------------------------------------------------------//
//
#    if !defined(TIMEMORY_EXTERN_OPERATIONS)
#        define TIMEMORY_EXTERN_OPERATIONS(COMPONENT_NAME, HAS_DATA)                     \
            TIMEMORY_INSTANTIATE_EXTERN_OPERATIONS(TIMEMORY_ESC(COMPONENT_NAME), HAS_DATA)
#    endif
//
//--------------------------------------------------------------------------------------//
//
#    if !defined(TIMEMORY_EXTERN_STORAGE)
#        define TIMEMORY_EXTERN_STORAGE(...)                                             \
            TIMEMORY_INSTANTIATE_EXTERN_STORAGE(__VA_ARGS__)
#    endif
//
//--------------------------------------------------------------------------------------//
//
#    if !defined(TIMEMORY_EXTERN_TEMPLATE)
#        define TIMEMORY_EXTERN_TEMPLATE(...) template __VA_ARGS__;
#    endif
//
//--------------------------------------------------------------------------------------//
//
#elif defined(TIMEMORY_USE_COMPONENT_EXTERN)
//
//--------------------------------------------------------------------------------------//
//
#    if !defined(TIMEMORY_EXTERN_OPERATIONS)
#        define TIMEMORY_EXTERN_OPERATIONS(COMPONENT_NAME, HAS_DATA)                     \
            TIMEMORY_DECLARE_EXTERN_OPERATIONS(TIMEMORY_ESC(COMPONENT_NAME), HAS_DATA)
#    endif
//
//--------------------------------------------------------------------------------------//
//
#    if !defined(TIMEMORY_EXTERN_STORAGE)
#        define TIMEMORY_EXTERN_STORAGE(...) TIMEMORY_DECLARE_EXTERN_STORAGE(__VA_ARGS__)
#    endif
//
//--------------------------------------------------------------------------------------//
//
#    if !defined(TIMEMORY_EXTERN_TEMPLATE)
#        define TIMEMORY_EXTERN_TEMPLATE(...) extern template __VA_ARGS__;
#    endif
//
//--------------------------------------------------------------------------------------//
//
#else
//
//--------------------------------------------------------------------------------------//
//
#    if !defined(TIMEMORY_EXTERN_OPERATIONS)
#        define TIMEMORY_EXTERN_OPERATIONS(...)
#    endif
//
//--------------------------------------------------------------------------------------//
//
#    if !defined(TIMEMORY_EXTERN_STORAGE)
#        define TIMEMORY_EXTERN_STORAGE(...)
#    endif
//
//--------------------------------------------------------------------------------------//
//
#    if !defined(TIMEMORY_EXTERN_TEMPLATE)
#        define TIMEMORY_EXTERN_TEMPLATE(...)
#    endif
//
//--------------------------------------------------------------------------------------//
//
#endif

//======================================================================================//

#if !defined(TIMEMORY_EXTERN_COMPONENT)
#    define TIMEMORY_EXTERN_COMPONENT(NAME, HAS_DATA, ...)                               \
        TIMEMORY_EXTERN_TEMPLATE(                                                        \
            struct tim::component::base<TIMEMORY_ESC(tim::component::NAME),              \
                                        __VA_ARGS__>)                                    \
        TIMEMORY_EXTERN_OPERATIONS(TIMEMORY_ESC(component::NAME), HAS_DATA)              \
        TIMEMORY_EXTERN_STORAGE(TIMEMORY_ESC(component::NAME))
#endif

//======================================================================================//

#if !defined(TIMEMORY_DECLARE_EXTERN_COMPONENT)
#    define TIMEMORY_DECLARE_EXTERN_COMPONENT(NAME, HAS_DATA, ...)                       \
        TIMEMORY_DECLARE_EXTERN_TEMPLATE(                                                \
            struct tim::component::base<TIMEMORY_ESC(tim::component::NAME),              \
                                        __VA_ARGS__>)                                    \
        TIMEMORY_DECLARE_EXTERN_OPERATIONS(TIMEMORY_ESC(component::NAME), HAS_DATA)      \
        TIMEMORY_DECLARE_EXTERN_STORAGE(TIMEMORY_ESC(component::NAME))
#endif

//======================================================================================//

#if !defined(TIMEMORY_INSTANTIATE_EXTERN_COMPONENT)
#    define TIMEMORY_INSTANTIATE_EXTERN_COMPONENT(NAME, HAS_DATA, ...)                   \
        TIMEMORY_INSTANTIATE_EXTERN_TEMPLATE(                                            \
            struct tim::component::base<TIMEMORY_ESC(tim::component::NAME),              \
                                        __VA_ARGS__>)                                    \
        TIMEMORY_INSTANTIATE_EXTERN_OPERATIONS(TIMEMORY_ESC(component::NAME), HAS_DATA)  \
        TIMEMORY_INSTANTIATE_EXTERN_STORAGE(TIMEMORY_ESC(component::NAME))
#endif

//======================================================================================//
