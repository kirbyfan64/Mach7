///
/// \file config.hpp
///
/// This file defines various macros that are compiler or platform specific.
///
/// \autor Yuriy Solodkyy <yuriy.solodkyy@gmail.com>
///
/// This file is a part of the XTL framework (http://parasol.tamu.edu/xtl/).
/// Copyright (C) 2005-2011 Texas A&M University.
/// All rights reserved.
///

#pragma once

#include <cassert>

//------------------------------------------------------------------------------

/// Type capable of representing bit offsets and bit counts in intptr_t
typedef unsigned char  bit_offset_t;
//typedef size_t  bit_offset_t;

/// The smallest integral type capable of representing the amount N of different 
/// vtbl pointers in the program. Roughly N should be equal to some constant c
/// multiplied by the amount of different classes polymorphic classes in the 
/// program. Constant c accounts for potential multiple inheritance.
typedef unsigned short vtbl_count_t;
//typedef size_t vtbl_count_t;

//------------------------------------------------------------------------------

const bit_offset_t min_log_size = 3;
const bit_offset_t max_log_size = 16;
const vtbl_count_t min_expected_size = 1<<min_log_size;

//------------------------------------------------------------------------------

/// Uncomment this macro definition if you'd like to do some performance tracing
#define XTL_TRACE_PERFORMANCE
/// Uncomment this macro definition if you'd like to do some performance tracing
#define XTL_DUMP_PERFORMANCE
/// Uncomment to use Pearson hash
//#define XTL_USE_PEARSON_HASH

//------------------------------------------------------------------------------

/// Several choices for configuring syntax:
/// Default syntax Match,Case,Que,Or,Otherwise,EndMatch are resolved to
/// - 'G' - Generic switch (default)
/// - 'P' - Polymorphic switch
/// - 'K' - Kind switch: the-only-match
/// - 'F' - Kind switch: nearly-best-match
/// - 'U' - Union switch
/// - 'E' - Exception switch
#if !defined(XTL_DEFAULT_SYNTAX)
  #define XTL_DEFAULT_SYNTAX 'P'
#endif

//------------------------------------------------------------------------------

/// Another choice is whether user would like to have Otherwise statement, as if
/// she doesn't, we can use default: to enter the cascading-if, which accordingly
/// to our measurements gives about 5% performance benefit than entering via a
/// dedicated constant.

/// Choice of exception handling strategies:
/// - Assume extractors won't throw and thus generate not exception wrappers
/// - Assume extractors might throw, in which case:
///    * Propagate throw further
///    * Treat it as failed match
/// Note that in any case exceptions inside statements associated with case 
/// clauses are not intercepted in any way.
#if !defined(XTL_EXTRACTORS_MIGHT_THROW)
  /// By default we assume that for a given code using our library extractors 
  /// won't throw and thus we do not generate the necessary exception handling code
  #define XTL_EXTRACTORS_MIGHT_THROW 0
#endif

#if XTL_EXTRACTORS_MIGHT_THROW
  #if !defined(EXTRACTORS_PROPAGATE_THROW)
  #define EXTRACTORS_PROPAGATE_THROW 0
  #endif
#endif

//------------------------------------------------------------------------------

/// Another choice is whether library code should try to benefit from memoized_cast 
/// or just use dynamic_cast 
#if !defined(XTL_USE_MEMOIZED_CAST)
  #define XTL_USE_MEMOIZED_CAST 0
#endif

//------------------------------------------------------------------------------

#if defined(XTL_TRACE_PERFORMANCE)
    #define XTL_TRACE_PERFORMANCE_ONLY(...) __VA_ARGS__
#else
    #define XTL_TRACE_PERFORMANCE_ONLY(...)
#endif

//------------------------------------------------------------------------------

#if defined(XTL_DUMP_PERFORMANCE)
    #define XTL_DUMP_PERFORMANCE_ONLY(...) __VA_ARGS__
#else
    #define XTL_DUMP_PERFORMANCE_ONLY(...)
#endif

//------------------------------------------------------------------------------

#if !defined(VTBL_DEFAULT_CACHE_BITS)
    #define VTBL_DEFAULT_CACHE_BITS 7
#endif

//------------------------------------------------------------------------------

#if !defined(VTBL_IRRELEVANT_BITS)
/// \note The following section is compiler and platform specific for 
///       optimization purposes.
/// \note As subsequent experiments showed, this value may depend on the number
///       of virtual functions in the class as well. Chosing this value smaller 
///       or larger than necessary, increases the number of collisions and 
///       necessarily degrades performance.
/// FIX: Make this less empirical!
#if defined(_MSC_VER)
    #if defined(_WIN64)
        #if defined(_DEBUG)
            /// vtbl in MSVC in x64 Debug   builds seem to be alligned by 8 bytes
            #define VTBL_IRRELEVANT_BITS 3
        #else
            /// vtbl in MSVC in x64 Release builds seem to be alligned by 16 bytes
            #define VTBL_IRRELEVANT_BITS 4
        #endif
    #else
        #if defined(_DEBUG)
            /// vtbl in MSVC in x86 Debug   builds seem to be alligned by 4 bytes
            #define VTBL_IRRELEVANT_BITS 2
        #else
            /// vtbl in MSVC in x86 Release builds seem to be alligned by 8 bytes
            #define VTBL_IRRELEVANT_BITS 3
        #endif
    #endif
#else
    /// vtbl in G++ seem to be alligned by 16 bytes
    #define VTBL_IRRELEVANT_BITS 4
    /// When i defined more virtual functions it became 3 for some reason
    //#define VTBL_IRRELEVANT_BITS 3
#endif
#endif

//------------------------------------------------------------------------------

#if defined(_DEBUG)
#define XTL_DEBUG_ONLY(s) s
/// Our own version of assert macro because of the fact that normal assert was 
/// not always removed in the release builds.
#define XTL_ASSERT(x) if (!(x)) { std::cerr << #x " in file " << __FILE__ << '[' << __LINE__ << ']'; std::exit(42); }
/// Our own version of assert macro because of the fact that normal assert was 
/// not always removed in the release builds.
#define XTL_VERIFY(x) if (!(x)) std::cerr << #x " in file " << __FILE__ << '[' << __LINE__ << ']'

#else
#define XTL_DEBUG_ONLY(s)
/// Our own version of assert macro because of the fact that normal assert was 
/// not always removed in the release builds.
#define XTL_ASSERT(x)
/// Our own version of assert macro because of the fact that normal assert was 
/// not always removed in the release builds.
#define XTL_VERIFY(x) if (!(x)) std::cerr << #x " in file " << __FILE__ << '[' << __LINE__ << ']'

#endif

//------------------------------------------------------------------------------

/// A macro we use in functions with auto as a return type. It helps us avoid 
/// duplication of expression when both the expression in decltype() and in the
/// only return statement are the same.
/// \example auto foo(T1 t1, T2 t2) -> XTL_RETURN(t1 + t2)
/// \note We use ... (__VA_ARGS__ parameters) to allow expressions 
///       containing comma as argument. Essentially this is a one arg macro
#define XTL_RETURN(...) decltype(__VA_ARGS__) { return (__VA_ARGS__); }

//------------------------------------------------------------------------------

/// The code that our macros generate may produce some variables that are not
/// necessarily used by the user. This macro is to suppress unused variable warning
#define XTL_UNUSED(x) (void)x;

//------------------------------------------------------------------------------

#if !defined(__func__) && defined(_MSC_VER)
    #define __func__ __FUNCTION__
#endif

//------------------------------------------------------------------------------

/// When the compiler supports Visual C++ like __COUNTER__ macro that is resolved 
/// to consequitive number each time it is used, we use it, otherwise we approximate 
/// it with line number. This means that multiple uses of this macro within the same
/// line should be memoized in a constant to avoid differences in behavior as well as
/// the user cannot rely on consequitiveness of numbers even when the counter is 
/// supported because other macros may use the counter as well.
#if defined(_MSC_VER) || defined(__GNUC__)
    #define XTL_COUNTER __COUNTER__
#else
    #define XTL_COUNTER __LINE__
#endif

//------------------------------------------------------------------------------

/// Helper macro for the following one to resolve macros inside its arguments
#define _XTL_STRING_LITERAL(x)  #x
/// Macro to stringize some expression.
#define XTL_STRING_LITERAL(x)   _XTL_STRING_LITERAL(x)

//------------------------------------------------------------------------------

#ifdef _MSC_VER
    #define XTL_CONCATENATE(A,B) A ## B
#else
    /// Somehow operator ## == (or any other) results in error in GCC, so we use 
    /// a trick to let users build operator name when they need.
    #define XTL_IDENTITY(A) A
    #define XTL_CONCATENATE(A,B) XTL_IDENTITY(A)XTL_IDENTITY(B)
#endif

//------------------------------------------------------------------------------

/// FIX: Yet another version that works in different case, but hasn't been merged
///      with the above yet.
#define XTL_CONCAT(arg1, arg2)  XTL_CONCAT1(arg1, arg2)
#define XTL_CONCAT1(arg1, arg2) XTL_CONCAT2(arg1, arg2)
#define XTL_CONCAT2(arg1, arg2) arg1##arg2

//------------------------------------------------------------------------------

/// MS Visual C++ 2005 (in which variadic macros became supported) till at 
/// least MS Visual C++ 2010 has a bug in passing __VA_ARGS__ to subsequent
/// macros as a single token, which results in:
///     warning C4003: not enough actual parameters for macro 'XTL_ARG_N'
/// and incorrect behavior. The workaround used here is described at:
/// https://connect.microsoft.com/VisualStudio/feedback/details/380090/variadic-macro-replacement
#define XTL_APPLY_VARIADIC_MACRO(macro,tuple) macro tuple

//------------------------------------------------------------------------------

#define XTL_RSEQ_N() 63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
#define    XTL_ARG_N( _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, _61,_62,_63,      N,...) N 
#define XTL_ARG_N_EX( _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, _61,_62,_63,Dummy,N,...) N 

#ifdef _MSC_VER

    /// A macro used to count a number of variadic arguments.
    /// \note Using this macro without arguments violates 6.10.3p4 of ISO C99 
    ///       and thus it cannot be used to detect zero arguments. 
    /// \note This macro was invented by Laurent Deniau and can be found here:
    /// \see  http://groups.google.com/group/comp.std.c/browse_thread/thread/77ee8c8f92e4a3fb/346fc464319b1ee5
    #define XTL_NARG(...)  XTL_APPLY_VARIADIC_MACRO(XTL_NARG_,(__VA_ARGS__,XTL_RSEQ_N()))
    #define XTL_NARG_(...) XTL_APPLY_VARIADIC_MACRO(XTL_ARG_N,(__VA_ARGS__))

    /// The same as above but assumes a dummy first argument that is not counted
    /// in order to deal with the fact that regular @XTL_NARG cannot cope with 
    /// zero arguments.
    /// \note We need here 0 to be a real 0 and not something that evaluates to 0
    ///       as is done in some solutions to this problem of NARG, because we 
    ///       append this number to a name to form another macro!
    #define XTL_NARG_EX(...)  XTL_APPLY_VARIADIC_MACRO(XTL_NARG_EX_,(__VA_ARGS__,XTL_RSEQ_N()))
    #define XTL_NARG_EX_(...) XTL_APPLY_VARIADIC_MACRO(XTL_ARG_N_EX,(__VA_ARGS__))

#else

    /// A macro used to count a number of variadic arguments.
    /// \note Using this macro without arguments violates 6.10.3p4 of ISO C99 
    ///       and thus it cannot be used to detect zero arguments. 
    /// \note This macro was invented by Laurent Deniau and can be found here:
    /// \see  http://groups.google.com/group/comp.std.c/browse_thread/thread/77ee8c8f92e4a3fb/346fc464319b1ee5
    #define XTL_NARG(...)  XTL_NARG_(__VA_ARGS__,XTL_RSEQ_N()) 
    #define XTL_NARG_(...) XTL_ARG_N(__VA_ARGS__) 

    /// The same as above but assumes a dummy first argument that is not counted
    /// in order to deal with the fact that regular @XTL_NARG cannot cope with 
    /// zero arguments.
    /// \note We need here 0 to be a real 0 and not something that evaluates to 0
    ///       as is done in some solutions to this problem of NARG, because we 
    ///       append this number to a name to form another macro!
    #define XTL_NARG_EX(...)  XTL_NARG_EX_(__VA_ARGS__,XTL_RSEQ_N()) 
    #define XTL_NARG_EX_(...) XTL_ARG_N_EX(__VA_ARGS__)

#endif

//------------------------------------------------------------------------------

#if !defined(XTL_ARR_SIZE)
    #define XTL_ARR_SIZE(a) (sizeof(a)/sizeof((a)[0]))
#endif

//------------------------------------------------------------------------------

#if defined(__GNUC__)
    #define XTL_SUPPORTS_VLA
#elif defined(_MSC_VER)
    #define XTL_SUPPORTS_ALLOCA
#endif

//------------------------------------------------------------------------------

#if   defined(XTL_SUPPORTS_VLA)
    #define XTL_VLA(v,T,n,N)  T v[n]
    #define XTL_VLAZ(v,T,n,N) T v[n]; std::memset(v,0,n*sizeof(T))
#elif defined(XTL_SUPPORTS_ALLOCA)
    #define XTL_VLA(v,T,n,N)  T* v = static_cast<T*>(alloca(n*sizeof(T)))
    #define XTL_VLAZ(v,T,n,N) T* v = static_cast<T*>(alloca(n*sizeof(T))); std::memset(v,0,n*sizeof(T))
#else
    #define XTL_VLA(v,T,n,N)  T v[N]
    #define XTL_VLAZ(v,T,n,N) T v[N] = {}
#endif

//------------------------------------------------------------------------------

#if defined(_MSC_VER)
    #define XTL_MSC_ONLY(...)     __VA_ARGS__
    #define XTL_NON_MSC_ONLY(...)
#else
    #define XTL_MSC_ONLY(...)
    #define XTL_NON_MSC_ONLY(...) __VA_ARGS__
#endif

//------------------------------------------------------------------------------

#if defined(__GNUC__)
    #define XTL_GCC_ONLY(...)     __VA_ARGS__
    #define XTL_NON_GCC_ONLY(...)
#else
    #define XTL_GCC_ONLY(...)
    #define XTL_NON_GCC_ONLY(...) __VA_ARGS__
#endif

//------------------------------------------------------------------------------

#if defined(_MSC_VER)

    /// Macros to use compiler's branch hinting. 
    /// \note These macros are only to be used in Case macro expansion, not in 
    ///       user's code since they explicitly expect a pointer argument
    /// \note We use ... (__VA_ARGS__ parameters) to allow expressions 
    ///       containing comma as argument. Essentially this is a one arg macro
    #define   XTL_LIKELY(...) (__VA_ARGS__)
    #define XTL_UNLIKELY(...) (__VA_ARGS__)

    /// A macro that is supposed to be put before the function definition whose inlining should be disabled
    #define XTL_DO_NOT_INLINE_BEGIN __pragma(auto_inline (off))
    /// A macro that is supposed to be put after  the function definition whose inlining should be disabled
    #define XTL_DO_NOT_INLINE_END   __pragma(auto_inline (on))
        
#else
    
    /// Macros to use compiler's branch hinting. 
    /// \note These macros are only to be used in Case macro expansion, not in 
    ///       user's code since they explicitly expect a pointer argument
    /// \note We use ... (__VA_ARGS__ parameters) to allow expressions 
    ///       containing comma as argument. Essentially this is a one arg macro
    #define   XTL_LIKELY(...) (__builtin_expect((long int)(__VA_ARGS__), 1))
    #define XTL_UNLIKELY(...) (__builtin_expect((long int)(__VA_ARGS__), 0))

    /// A macro that is supposed to be put before the function definition whose inlining should be disabled
    #define XTL_DO_NOT_INLINE_BEGIN __attribute__ ((noinline))
    /// A macro that is supposed to be put after  the function definition whose inlining should be disabled
    #define XTL_DO_NOT_INLINE_END

#endif

//------------------------------------------------------------------------------

/// Apparently in C++0x typename can be used to annotate types even in 
/// non-template context. If indeed so, this is what we need to avoid duplication
/// of macros depending on whether they are used in templated and non-templated 
/// context.
///
/// From: C++0x 14.2[5]
/// A name prefixed by the keyword template shall be a template-id or the name shall refer to a class template.
/// [ Note: The keyword template may not be applied to non-template members of class templates. -end
/// note ] [ Note: As is the case with the typename prefix, the template prefix is allowed in cases where it is
/// not strictly necessary; i.e., when the nested-name-specifier or the expression on the left of the -> or . is not
/// dependent on a template-parameter, or the use does not appear in the scope of a template. -end note ]
#define XTL_CPP0X_TYPENAME XTL_NON_MSC_ONLY(typename)
#define XTL_CPP0X_TEMPLATE XTL_NON_GCC_ONLY(template)

//------------------------------------------------------------------------------

/// The following set of macros was copied verbatim from a response by Moritz Beutel
/// here: https://forums.embarcadero.com/message.jspa?messageID=320338
/// It is used for suppressing , (comma) when argument is added to __VA_ARGS__, which
/// might be empty.
#define UCL_PP_AND(a,b) UCL_PP_CAT3(_UCL_PP_AND_, a, b)
#define UCL_PP_NOT(a) UCL_PP_CAT2(_UCL_PP_NOT_, a)
#define UCL_PP_IF(a,b,...) UCL_PP_CAT2(_UCL_PP_IF_, a)(b, __VA_ARGS__)
 
#define UCL_PP_EMPTY()
#define UCL_PP_EXPAND(...) __VA_ARGS__
#define UCL_PP_1ST(a,...) a
#define UCL_PP_2ND(a,b,...) b
#define UCL_PP_REMOVE_1ST(a,...) __VA_ARGS__
#define UCL_PP_PAIR(a,...) a __VA_ARGS__
#define UCL_PP_CAT2(a,...) UCL_PP_CAT2_(a, __VA_ARGS__)
#define UCL_PP_CAT3(a,b,...) UCL_PP_CAT3_(a, b, __VA_ARGS__)
 
    // The two macros below are inspired by Laurent Deniau's posting on comp.lang.c from 2006/09/27
    // http://groups.google.com/group/comp.lang.c/browse_thread/thread/578912299f8f87ce#msg_937356effc43f569
#define UCL_PP_IS_EMPTY(...) \
    UCL_PP_AND(UCL_PP_IS_LIST(__VA_ARGS__ ()),UCL_PP_NOT(UCL_PP_IS_LIST(__VA_ARGS__ _)))
#define UCL_PP_IS_LIST(...) \
    UCL_PP_PAIR(UCL_PP_1ST, (UCL_PP_CAT2(UCL_PP_IS_LIST_RET_, UCL_PP_IS_LIST_TST_ __VA_ARGS__)))
 
 
    // implementation details
 
#define UCL_PP_IS_LIST_TST_(...) 1
#define UCL_PP_IS_LIST_RET_UCL_PP_IS_LIST_TST_ 0,
#define UCL_PP_IS_LIST_RET_1 1,
 
#define UCL_PP_CAT2_(a,...) UCL_PP_EXPAND(a ## __VA_ARGS__)
#define UCL_PP_CAT3_(a,b,...) UCL_PP_EXPAND(a ## b ## __VA_ARGS__)
 
#define _UCL_PP_AND_00 0
#define _UCL_PP_AND_01 0
#define _UCL_PP_AND_10 0
#define _UCL_PP_AND_11 1
 
#define _UCL_PP_IF_0(a,...) __VA_ARGS__
#define _UCL_PP_IF_1(a,...) a
 
#define _UCL_PP_NOT_0 1
#define _UCL_PP_NOT_1 0

//------------------------------------------------------------------------------