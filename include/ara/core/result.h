/**
 * Copyright (c) 2020
 * umlaut Software Development and contributors
 * 
 * SPDX-License-Identifier: MIT
 */

#ifndef ARA_CORE_RESULT_H_
#define ARA_CORE_RESULT_H_

#include <type_traits>
#include "ara/core/error_code.h"

namespace ara::core
{

/**
 * @brief This class is a type that contains either a value or an error.
 * 
 * @tparam T - the type of value
 * @tparam E - the type of error
 * 
 * @req {SWS_CORE_00701}
 */
template <typename T, typename E = ErrorCode>
class Result final
{
public:
/**
 * @brief Type alias for the type @c T of values.
 * 
 * @req {SWS_CORE_00711}
 */
using Result<T, E>::value_type = T;

/**
 * @brief Type alias for the type @c E of errors.
 * 
 * @req {SWS_CORE_00712}
 */
using Result<T, E>::error_type = E;

/**
 * @brief Construct a new @c Result from the specified value (given as lvalue).
 * 
 * @param t - the value to put into the @c Result
 * 
 * @req {SWS_CORE_00721}
 */
Result(T const &t);

/**
 * @brief Construct a new @c Result from the specified value (given as rvalue).
 * 
 * @param t - the value to put into the @c Result
 * 
 * @req {SWS_CORE_00722}
 */
Result(T const &&t);

/**
 * @brief Construct a new @c Result from the specified error (given as lvalue).
 * 
 * @param e - the error to put into the @c Result
 * 
 * @req {SWS_CORE_00723}
 */
Result(E const &e);

/**
 * @brief Construct a new @c Result from the specified error (given as rvalue).
 * 
 * @param e - the error to put into the @c Result
 * 
 * @req {SWS_CORE_00724}
 */
Result(E const &&e);

/**
 * @brief Copy-construct a new @c Result from another instance.
 * 
 * @param other - the other instance
 * 
 * @req {SWS_CORE_00725}
 */
Result(Result const &other);

/**
 * @brief Move-construct a new @c Result from another instance.
 * 
 * @param other - the other instance
 * 
 * @req {SWS_CORE_00726}
 */
Result(Result const &&other) noexcept(std::is_nothrow_move_constructible<T>::value && std::is_nothrow_move_constructible<E>::value);

/**
 * @brief Destructor.
 * 
 * @req {SWS_CORE_00727}
 */
// TODO: This destructor is trivial if std::is_trivially_destructible<T>::value && std::is_trivially_destructible<E>::value is true.
~Result();

/**
 * @brief Build a new @c Result from the specified value (given as lvalue).
 * 
 * @param t - the value to put into the @c Result
 * 
 * @return A @c Result that contains the value @c t
 * 
 * @req {SWS_CORE_00731}
 */
static Result FromValue(T const &t);

/**
 * @brief Build a new @c Result from the specified value (given as rvalue).
 * 
 * @param t - the value to put into the @c Result
 * 
 * @return A @c Result that contains the value @c t
 * 
 * @req {SWS_CORE_00732}
 */
static Result FromValue(T const &&t);

/**
 * @brief Build a new @c Result from a value that is constructed in-place from the given arguments.
 * 
 * @tparam Args... - the types of arguments given to this function
 * @param args - the arguments used for constructing the value
 * 
 * @return A @c Result that contains the value
 * 
 * @req {SWS_CORE_00733}
 */
// TODO: This function shall not participate in overload resolution unless:
// std::is_constructible<T, Args&&...>::value is true, and the first type of the expanded parameter pack is not T, and the
// first type of the expanded parameter pack is not a specialization of Result
template <typename... Args>
static Result FromValue(Args &&... args);

/**
 * @brief Build a new @c Result from the specified error (given as lvalue).
 * 
 * @param e - the error to put into the @c Result
 * 
 * @return A @c Result that contains the error @c e
 * 
 * @req {SWS_CORE_00734}
 */
static Result FromError(E const& e);

/**
 * @brief Build a new @c Result from the specified error (given as rvalue).
 * 
 * @param e - the error to put into the @c Result
 * 
 * @return A @c Result that contains the error @c e
 * 
 * @req {SWS_CORE_00735}
 */
static Result FromError(E &&e);

/**
 * @brief Build a new @c Result from an error that is constructed in-place from the given arguments.
 * 
 * @tparam Args... - the types of arguments given to this function
 * @param args - the arguments used for constructing the error
 * 
 * @return A @c Result that contains an error
 * 
 * @req {SWS_CORE_00736}
 */
//TODO: This function shall not participate in overload resolution unless:
// std::is_constructible<E, Args&&...>::value is true, and the first type of the expanded parameter pack is not E, and the
// first type of the expanded parameter pack is not a specialization of Result
template<typename... Args>
static FromError(Args &&... args);

/**
 * @brief Copy-assign another @c Result to this instance.
 * 
 * @param other - the other instance
 * 
 * @return @c *this, containing the contents of other
 * 
 * @req {SWS_CORE_00741}
 */
Result& operator=(Result const &other);

/**
 * @brief Move-assign another @c Result to this instance.
 * 
 * @param other - the other instance
 * 
 * @return @c *this, containing the contents of other
 * 
 * @req {SWS_CORE_00742}
 */
Result& operator=(Result &&other)
    noexcept(std::is_nothrow_move_constructible<T>::value && std::is_nothrow_move_assignable<T>::value
             && std::is_nothrow_move_constructible<E>::value && std::is_nothrow_move_assignable<E>::value);

/**
 * @brief Put a new value into this instance, constructed in-place from the given arguments.
 * 
 * @tparam Args... - the types of arguments given to this function
 * @param args - the arguments used for constructing the value
 * 
 * @req {SWS_CORE_00743}
 */
template<typename... Args>
void EmplaceValue(Args &&... args);

/**
 * @brief Put a new error into this instance, constructed in-place from the given arguments.
 * 
 * @tparam Args... - the types of arguments given to this function
 * @param args - the arguments used for constructing the error
 * 
 * @req {SWS_CORE_00744}
 */
template<typename... Args>
void EmplaceError(Args &&... args);

/**
 * @brief Exchange the contents of this instance with those of @c other.
 * 
 * @param other - the other instance
 * 
 * @req {SWS_CORE_00745}
 */
void Swap(Result &other)
    noexcept(std::is_nothrow_move_constructible<T> && std::is_nothrow_move_assignable<T>
             && std::is_nothrow_move_constructible<E> && std::is_nothrow_move_assignable<E>);

/**
 * @brief Check whether @c *this contains a value.
 * 
 * @return @c true if @c *this contains a value, @c false otherwise
 * 
 * @req {SWS_CORE_00751}
 */
bool HasValue() const noexcept;

/**
 * @brief Check whether @c *this contains a value.
 * 
 * @return @c true if @c *this contains a value, @c false otherwise
 * 
 * @req {SWS_CORE_00752}
 */
explicit operator bool() const noexcept;

/**
 * @brief Access the contained value.
 * This function’s behavior is undefined if @c *this does not contain a value.
 * 
 * @return A const_reference to the contained value
 * 
 * @req {SWS_CORE_00753}
 */
T const& operator *() const&;

/**
 * @brief Access the contained value.
 * This function’s behavior is undefined if @c *this does not contain a value.
 * 
 * @return An rvalue reference to the contained value
 * 
 * @req {SWS_CORE_00759}
 */
T&& operator *() &&;

/**
 * @brief Access the contained value.
 * This function’s behavior is undefined if @c *this does not contain a value.
 * 
 * @return A pointer to the contained value
 * 
 * @req {SWS_CORE_00754}
 */
T const* operator ->() const;

/**
 * @brief Access the contained value.
 * This function’s behavior is undefined if @c *this does not contain a value.
 * 
 * @return A const reference to the contained value
 * 
 * @req {SWS_CORE_00755}
 */
T const& Value() const &;

/**
 * @brief Access the contained value.
 * This function’s behavior is undefined if @c *this does not contain a value.
 * 
 * @return An rvalue reference to the contained value
 * 
 * @req {SWS_CORE_00756}
 */
T&& Value() &&;

/**
 * @brief Access the contained error.
 * The behavior of this function is undefined if @c *this does not contain an error.
 * 
 * @return A const reference to the contained error
 * 
 * @req {SWS_CORE_00757}
 */
E const& Error() const &; 

/**
 * @brief Access the contained error.
 * The behavior of this function is undefined if @c *this does not contain an error.
 * 
 * @return An rvalue reference to the contained error
 * 
 * @req {SWS_CORE_00758}
 */
E&& Error() &&; 

/**
 * @brief Return the contained value or the given default value.
 * If @c *this contains a value, it is returned. Otherwise, the specified default value is returned, static_cast’d to @c T.
 * 
 * @tparam U - the type of @c defaultValue
 * @param defaultValue - the value to use if @c *this does not contain a value
 * 
 * @return the value
 * 
 * @req {SWS_CORE_00761}
 */
template<typename U>
T ValueOr(U &&defaultValue)  const &;

/**
 * @brief Return the contained value or the given default value.
 * If @c *this contains a value, it is returned. Otherwise, the specified default value is returned, static_cast’d to @c T.
 * 
 * @tparam U - the type of @c defaultValue
 * @param defaultValue - the value to use if @c *this does not contain a value
 * 
 * @return the value
 * 
 * @req {SWS_CORE_00762}
 */
template<typename U>
T ValueOr(U &&defaultValue) &&;

/**
 * @brief Return the contained error or the given default error.
 * If @c *this contains an error, it is returned. Otherwise, the specified default error is returned, static_cast’d to @c E.
 * 
 * @tparam G - the type of @c defaultError
 * @param defaultError - the error to use if @c *this does not contain an error
 * 
 * @return the error
 * 
 * @req {SWS_CORE_00763}
 */
template<typename G>
E ErrorOr(G &&defaultError) const;

/**
 * @brief Return whether this instance contains the given error.
 * This call compares the argument @c error, static_cast’d to @c E, with the return value from @c Error().
 * 
 * @tparam G - the type of the error argument @c error
 * @param error - the error to check
 * 
 * @return @c true if @c *this contains an error that is equivalent to the given error, @c false otherwise
 *  
 * @req {SWS_CORE_00765}
 */
bool CheckError(G &&error) const;
};

} // namespace ara::core

#endif // ARA_CORE_RESULT_H_