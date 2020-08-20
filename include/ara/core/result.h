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

namespace details
{
    template <typename T, typename E>
    class Result;

    template <std::size_t Index, typename... T>
    struct params {};

    template <std::size_t Index, typename First, typename... Rest>
    struct params<Index, First, Rest...> : params<Index - 1, Rest...> {};

    template <typename First, typename... Rest>
    struct params<0, First, Rest...>
    {
        using type = First;
    };

    template <typename... Args>
    using first_param_t = typename params<0, Args...>::type;

    template <typename T, typename = std::void_t<>>
    struct is_specialization_of_result : std::false_type {};

    template <typename T>
    struct is_specialization_of_result
        <T, std::void_t<Result<typename T::value_type, typename T::error_type>>>
        : std::conditional_t<std::is_same_v<T, Result<typename T::value_type, typename T::error_type>>, std::true_type, std::false_type>
    {
    }; 

    template <bool Condition, typename T = void>
    using requires_ = typename std::enable_if<Condition, T>::type;
} // namespace details

/**
 * @brief This class is a type that contains either a value or an error.
 * 
 * @tparam T the type of value
 * @tparam E the type of error
 * 
 * @req {SWS_CORE_00701}
 */
template <typename T, typename E = ErrorCode>
class Result final
{
    /**
     * @brief Type alias for the type @c T of values.
     * 
     * @req {SWS_CORE_00711}
     */
    using value_type = T;

    /**
     * @brief Type alias for the type @c E of errors.
     * 
     * @req {SWS_CORE_00712}
     */
    using error_type = E;

    union
    {
         /**
         * @brief Result's error
         */
        error_type error_;
        
        /**
         * @brief Result's value
         */
        value_type value_;
    };

    /**
     * @brief Indicates if value exists
     */
    bool hasValue_;
    
public:
    /**
     * @brief Construct a new @c Result from the specified value (given as lvalue).
     * 
     * @param t the value to put into the @c Result
     * 
     * @req {SWS_CORE_00721}
     */
    Result(value_type const &t) : value_ {t}, hasValue_ {true} {}

    /**
     * @brief Construct a new @c Result from the specified value (given as rvalue).
     * 
     * @param t the value to put into the @c Result
     * 
     * @req {SWS_CORE_00722}
     */
    Result(value_type const &&t) : value_ {t}, hasValue_ {true} {}

    /**
     * @brief Construct a new @c Result from the specified error (given as lvalue).
     * 
     * @param e the error to put into the @c Result
     * 
     * @req {SWS_CORE_00723}
     */
    Result(error_type const &e) : error_ {e}, hasValue_ {false} {}

    /**
     * @brief Construct a new @c Result from the specified error (given as rvalue).
     * 
     * @param e the error to put into the @c Result
     * 
     * @req {SWS_CORE_00724}
     */
    Result(error_type const &&e) : error_ {e}, hasValue_ {false} {}

    /**
     * @brief Copy-construct a new @c Result from another instance.
     * 
     * @param other the other instance
     * 
     * @req {SWS_CORE_00725}
     */
    Result(Result const &other)
    {
        this->hasValue_ = other.hasValue_;
        if (this->hasValue_)
        {
            EmplaceValue(other.value_);
        }
        else
        {
            EmplaceError(other.error_);
        }
    }

    /**
     * @brief Move-construct a new @c Result from another instance.
     * 
     * @param other the other instance
     * 
     * @req {SWS_CORE_00726}
     */
    Result(Result const &&other) noexcept(std::is_nothrow_move_constructible_v<value_type> && std::is_nothrow_move_constructible_v<error_type>)
    {
        this->hasValue_ = other.hasValue_;
        if (this->hasValue_)
        {
            EmplaceValue(std::move(other.value_));
        }
        else
        {
            EmplaceError(std::move(other.error_));
        }
    }

    /**
     * @brief Destructor.
     * 
     * @req {SWS_CORE_00727}
     */
    // FIXME: This destructor is trivial if std::is_trivially_destructible<E>::value is true.
    ~Result()
    {
        if (hasValue_)
        {
            value_.~value_type();
        }
        else
        {
            error_.~error_type();
        }
    }

    /**
     * @brief Build a new @c Result from the specified value (given as lvalue).
     * 
     * @param t the value to put into the @c Result
     * 
     * @return A @c Result that contains the value @c t
     * 
     * @req {SWS_CORE_00731}
     */
    static Result FromValue(value_type const &t)
    {
        return Result(t);
    }

    /**
     * @brief Build a new @c Result from the specified value (given as rvalue).
     * 
     * @param t the value to put into the @c Result
     * 
     * @return A @c Result that contains the value @c t
     * 
     * @req {SWS_CORE_00732}
     */
    static Result FromValue(value_type const &&t)
    {
        return Result(t);
    }

    /**
     * @brief Build a new @c Result from a value that is constructed in-place from the given arguments.
     * 
     * @tparam Args... the types of arguments given to this function
     * @param args the arguments used for constructing the value
     * 
     * @return A @c Result that contains the value
     * 
     * @req {SWS_CORE_00733}
     */
    template <typename... Args,
              details::requires_
              <
                  std::is_constructible_v<value_type, Args...> 
                  && !std::is_same_v<value_type, details::first_param_t<Args...>>
                  && !details::is_specialization_of_result<details::first_param_t<Args...>>::value
              > * = nullptr
             >           
    static Result FromValue(Args &&... args)
    {
        value_type* newValue = ::new (newValue) value_type(std::forward<Args>(args)...);
        return Result(*newValue);
    }

    /**
     * @brief Build a new @c Result from the specified error (given as lvalue).
     * 
     * @param e the error to put into the @c Result
     * 
     * @return A @c Result that contains the error @c e
     * 
     * @req {SWS_CORE_00734}
     */
    static Result FromError(error_type const& e)
    {
        return Result(e);
    }

    /**
     * @brief Build a new @c Result from the specified error (given as rvalue).
     * 
     * @param e the error to put into the @c Result
     * 
     * @return A @c Result that contains the error @c e
     * 
     * @req {SWS_CORE_00735}
     */
    static Result FromError(error_type &&e)
    {
        return Result(e);
    }

    /**
     * @brief Build a new @c Result from an error that is constructed in-place from the given arguments.
     * 
     * @tparam Args... the types of arguments given to this function
     * @param args the arguments used for constructing the error
     * 
     * @return A @c Result that contains an error
     * 
     * @req {SWS_CORE_00736}
     */
    template <typename... Args,
              details::requires_
                <
                    std::is_constructible_v<error_type, Args...> 
                    && !std::is_same_v<error_type, details::first_param_t<Args...>>
                    && !details::is_specialization_of_result<details::first_param_t<Args...>>::value
                > * = nullptr
              >    
    static Result FromError(Args &&... args)
    {
        error_type* error = ::new (error) error_type(std::forward<Args>(args)...);
        return Result(*error);
    }

    /**
     * @brief Copy-assign another @c Result to this instance.
     * 
     * @param other the other instance
     * 
     * @return @c *this, containing the contents of other
     * 
     * @req {SWS_CORE_00741}
     */
    Result& operator=(Result const &other)
    {
        if (*this == other)
        {
            return *this;
        }

        Swap(other);
        return *this;
    }

    /**
     * @brief Move-assign another @c Result to this instance.
     * 
     * @param other the other instance
     * 
     * @return @c *this, containing the contents of other
     * 
     * @req {SWS_CORE_00742}
     */
    Result& operator=(Result &&other)
        noexcept(std::is_nothrow_move_constructible_v<value_type> && std::is_nothrow_move_assignable_v<value_type>
                && std::is_nothrow_move_constructible_v<error_type> && std::is_nothrow_move_assignable_v<error_type>)
    {
        if (*this == other)
        {
            return *this;
        }

        Swap(std::move(other));
        return *this;
    }

    /**
     * @brief Put a new value into this instance, constructed in-place from the given arguments.
     * 
     * @tparam Args... the types of arguments given to this function
     * @param args the arguments used for constructing the value
     * 
     * @req {SWS_CORE_00743}
     */
    template <typename... Args>
    void EmplaceValue(Args &&... args)
    {
        if (hasValue_)
        {
            value_ = value_type(std::forward<Args>(args)...);
        }
        else
        {
            error_.~error_type();
            hasValue_ = true;
            ::new (&value_) value_type(std::forward<Args>(args)...);
        }
    }

    /**
     * @brief Put a new error into this instance, constructed in-place from the given arguments.
     * 
     * @tparam Args... the types of arguments given to this function
     * @param args the arguments used for constructing the error
     * 
     * @req {SWS_CORE_00744}
     */
    template <typename... Args>
    void EmplaceError(Args &&... args)
    {
        if (!hasValue_)
        {
            error_ = error_type(std::forward<Args>(args)...);
        }
        else
        {
            value_.~value_type();
            hasValue_ = false;
            ::new (&error_) error_type(std::forward<Args>(args)...);
        }
    }

    /**
     * @brief Exchange the contents of this instance with those of @c other.
     * 
     * @param other the other instance
     * 
     * @req {SWS_CORE_00745}
     */
    void Swap(Result &other)
        noexcept(std::is_nothrow_move_constructible_v<value_type> && std::is_nothrow_move_assignable_v<value_type>
                && std::is_nothrow_move_constructible_v<error_type> && std::is_nothrow_move_assignable_v<error_type>)
    {
        // both have a value
        if(HasValue() && other.HasValue())
        {
            std::swap(this.Value(), other.Value());
        }
        // both have an error
        else if(!HasValue() && !other.HasValue())
        {
            std::swap(this.Error(), other.Error());
        }
        else if (HasValue() && !other.HasValue())
        {
            auto tempValue = std::move(Value());
            auto tempError = std::move(Error());

            EmplaceError(tempError);
            other.EmplaceValue(tempValue);
        }
        else if (!HasValue() && other.HasValue())
        {
            other.Swap(*this);
        }
    }

    /**
     * @brief Check whether @c *this contains a value.
     * 
     * @return @c true if @c *this contains a value, @c false otherwise
     * 
     * @req {SWS_CORE_00751}
     */
    bool HasValue() const noexcept
    {
        return hasValue_;
    }

    /**
     * @brief Check whether @c *this contains a value.
     * 
     * @return @c true if @c *this contains a value, @c false otherwise
     * 
     * @req {SWS_CORE_00752}
     */
    explicit operator bool() const noexcept
    {
        return hasValue_;
    }

    /**
     * @brief Access the contained value.
     * This function’s behavior is undefined if @c *this does not contain a value.
     * 
     * @return A const_reference to the contained value
     * 
     * @req {SWS_CORE_00753}
     */
    value_type const& operator *() const&
    {
        if (!hasValue_)
        {
            // TODO: Add some warning
        }

        return value_;
    }

    /**
     * @brief Access the contained value.
     * This function’s behavior is undefined if @c *this does not contain a value.
     * 
     * @return An rvalue reference to the contained value
     * 
     * @req {SWS_CORE_00759}
     */
    value_type&& operator *() &&
    {
        if (!hasValue_)
        {
            // TODO: Add some warning
        }

        return value_;
    }

    /**
     * @brief Access the contained value.
     * This function’s behavior is undefined if @c *this does not contain a value.
     * 
     * @return A pointer to the contained value
     * 
     * @req {SWS_CORE_00754}
     */
    value_type const* operator ->() const
    {
        if (!hasValue_)
        {
            // TODO: Add some warning
        }

        return &value_;
    }

    /**
     * @brief Access the contained value.
     * This function’s behavior is undefined if @c *this does not contain a value.
     * 
     * @return A const reference to the contained value
     * 
     * @req {SWS_CORE_00755}
     */
    value_type const& Value() const &
    {
        if (!hasValue_)
        {
            // TODO: Add some warning
        }
        
        return value_;
    }

    /**
     * @brief Access the contained value.
     * This function’s behavior is undefined if @c *this does not contain a value.
     * 
     * @return An rvalue reference to the contained value
     * 
     * @req {SWS_CORE_00756}
     */
    value_type&& Value() &&
    {
        if (!hasValue_)
        {
            // TODO: Add some warning
        }
        
        return value_;
    }

    /**
     * @brief Access the contained error.
     * The behavior of this function is undefined if @c *this does not contain an error.
     * 
     * @return A const reference to the contained error
     * 
     * @req {SWS_CORE_00757}
     */
    error_type const& Error() const &
    {
        if (hasValue_)
        {
            // TODO: Add some warning
        }
        
        return error_;
    }

    /**
     * @brief Access the contained error.
     * The behavior of this function is undefined if @c *this does not contain an error.
     * 
     * @return An rvalue reference to the contained error
     * 
     * @req {SWS_CORE_00758}
     */
    error_type&& Error() && 
    {
        if (hasValue_)
        {
            // TODO: Add some warning
        }
        
        return error_;
    }

    /**
     * @brief Return the contained value or the given default value.
     * If @c *this contains a value, it is returned. Otherwise, the specified default value is returned, static_cast’d to @c value_type.
     * 
     * @tparam U the type of @c defaultValue
     * @param defaultValue the value to use if @c *this does not contain a value
     * 
     * @return the value
     * 
     * @req {SWS_CORE_00761}
     */
    template <typename U>
    value_type ValueOr(U &&defaultValue)  const &
    {
        if (hasValue_)
        {
            return value_;
        }

        return static_cast<value_type>(defaultValue);
    }

    /**
     * @brief Return the contained value or the given default value.
     * If @c *this contains a value, it is returned. Otherwise, the specified default value is returned, static_cast’d to @c value_type.
     * 
     * @tparam U the type of @c defaultValue
     * @param defaultValue the value to use if @c *this does not contain a value
     * 
     * @return the value
     * 
     * @req {SWS_CORE_00762}
     */
    template <typename U>
    value_type ValueOr(U &&defaultValue) &&
    {
        if (hasValue_)
        {
            return value_;
        }

        return static_cast<value_type>(defaultValue);
    }

    /**
     * @brief Return the contained error or the given default error.
     * If @c *this contains an error, it is returned. Otherwise, the specified default error is returned, static_cast’d to @c error_type.
     * 
     * @tparam G the type of @c defaultError
     * @param defaultError the error to use if @c *this does not contain an error
     * 
     * @return the error
     * 
     * @req {SWS_CORE_00763}
     */
    template <typename G>
    error_type ErrorOr(G &&defaultError) const
    {
        if (!hasValue_)
        {
            return error_;
        }

        return static_cast<error_type>(defaultError);
    }

    /**
     * @brief Return whether this instance contains the given error.
     * This call compares the argument @c error, static_cast’d to @c error_type, with the return value from @c Error().
     * 
     * @tparam G the type of the error argument @c error
     * @param error the error to check
     * 
     * @return @c true if @c *this contains an error that is equivalent to the given error, @c false otherwise
     *  
     * @req {SWS_CORE_00765}
     */
    template <typename G>
    bool CheckError(G &&error) const
    {
        if (hasValue_)
        {
            return false;
        }

        return static_cast<error_type>(error) == error_.Value() ? true : false;
    }

#if __cpp_exceptions == 199711
    /**
     * @brief Return the contained value or throw an exception.
     * This function does not participate in overload resolution when the compiler toolchain does not support C++ exceptions.
     * 
     * @exception The exception type associated with the contained error
     * @return A const reference to the contained value
     *  
     * @req {SWS_CORE_00766}
     */
    value_type const& ValueOrThrow() const &noexcept(false)
    {
        if (hasValue_)
        {
            return value_;
        }

        error_.ThrowAsException();
    }
#endif // __cpp_exceptions == 199711

#if __cpp_exceptions == 199711
    /**
     * @brief Return the contained value or throw an exception.
     * This function does not participate in overload resolution when the compiler toolchain does not support C++ exceptions.
     * 
     * @exception The exception type associated with the contained error
     * @return An rvalue reference to the contained value
     *  
     * @req {SWS_CORE_00769}
     */
    value_type&& ValueOrThrow() const &&noexcept(false)
    {
        if (hasValue_)
        {
            return value_;
        }

        error_.ThrowAsException();
    }
#endif // __cpp_exceptions == 199711

    /**
     * @brief Return the contained value or return the result of a function call.
     * If @c *this contains a value, it is returned. Otherwise, the specified callable is invoked and its return
     * value which is to be compatible to type @c value_type is returned from this function.
     * The Callable is expected to be compatible to this interface: <tt> value_type f(error_type const&); </tt>
     * 
     * @tparam F the type of the Callable f
     * @param f the Callable
     * 
     * @return the value
     *  
     * @req {SWS_CORE_00767}
     */
    template <typename F>
    value_type Resolve(F &&f) const
    {
        if (hasValue_)
        {
            return value_;
        }

        return f(error_);
    }

    /**
     * @brief Apply the given Callable to the value of this instance, and return a new @c Result with the result of the call.
     * The Callable is expected to be compatible to:
     * <tt> Result<value_type, error_type> f(value_type const&); </tt>
     * If this instance does not contain a value, a new @c Result<value_type,error_type> is still created and returned, 
     * with the original error contents of this instance being copied into the new instance.
     * 
     * @tparam F the type of the Callable f
     * @param f the Callable
     * 
     * @return A new @c Result instance of the possibly transformed type
     *  
     * @req {SWS_CORE_00768}
     */
    template <typename F,
              details::requires_<std::is_same_v<Result, decltype(F(value_))>> * = nullptr
             >
    Result Bind(F &&f)
    {
        if (hasValue_)
        {
            return f(value_);
        }

        return Result(error_);
    }

    /**
     * @brief Apply the given Callable to the value of this instance, and return a new @c Result with the result of the call.
     * The Callable is expected to be compatible to:
     * <tt> value_type f(value_type const&); </tt>
     * The return type of this function is @c Result<f(Value()),error_type>.
     * If this instance does not contain a value, a new @c Result<value_type,error_type> is still created and returned, 
     * with the original error contents of this instance being copied into the new instance.
     * 
     * @tparam F the type of the Callable f
     * @param f the Callable
     * 
     * @return A new @c Result instance of the possibly transformed type
     *  
     * @req {SWS_CORE_00768}
     */
    template <typename F,
              details::requires_<std::is_same_v<value_type, decltype(F(value_))>> * = nullptr
             >
    Result Bind(F &&f)
    {
        if (hasValue_)
        {
            return Result(f(value_));
        }

        return Result(error_);
    }
};

/**
 * @brief Specialization of class @c Result for @c void values.
 *
 * @req {SWS_CORE_00801}
 */
template <typename E>
class Result<void, E> final
{
    /**
     * @brief Type alias for the type @c T of values, always @c void for this specialization.
     *  
     * @req {SWS_CORE_00811}
     */
    using value_type = void;

    /**
     * @brief Type alias for the type @c E of errors.
     *  
     * @req {SWS_CORE_00812}
     */
    using error_type = E;
    
    /**
     * @brief Result's error
     */
    error_type error_;

    /**
     * @brief Indicates if value exists
     */
    bool hasValue_;

public:
    /**
     * @brief Construct a new @c Result with a "void" value.
     * 
     * @req {SWS_CORE_00821}
     */
    Result() noexcept;

    /**
     * @brief Construct a new @c Result from the specified error (given as lvalue).
     * 
     * @param e the error to put into the @c Result
     *  
     * @req {SWS_CORE_00823}
     */
    explicit Result (error_type const &e);

    /**
     * @brief Construct a new @c Result from the specified error (given as rvalue).
     * 
     * @param e the error to put into the @c Result
     *  
     * @req {SWS_CORE_00824}
     */
    explicit Result (error_type &&e);

    /**
     * @brief Copy-construct a new @c Result from another instance.
     * 
     * @param other the other instance
     *  
     * @req {SWS_CORE_00825}
     */
    Result (Result const &other);

    /**
     * @brief Move-construct a new @c Result from another instance.
     * 
     * @param other the other instance
     *  
     * @req {SWS_CORE_00826}
     */
    Result (Result &&other) noexcept(std::is_nothrow_move_constructible_v<error_type>);

    /**
     * @brief Destructor.
     * This destructor is trivial if @c std::is_trivially_destructible<error_type>::value is true.
     *  
     * @req {SWS_CORE_00827}
     */
    ~Result();

    /**
     * @brief Build a new @c Result with @c void as value.
     * 
     * @return A @c Result that contains a @c void value
     *  
     * @req {SWS_CORE_00831}
     */
    static Result FromValue();

    /**
     * @brief Build a new @c Result from the specified error (given as lvalue).
     * 
     * @param e the error to put into the @c Result
     * 
     * @return A @c Result that contains the error @c e
     *  
     * @req {SWS_CORE_00834}
     */
    static Result FromError(error_type const &e);

    /**
     * @brief Build a new Result from the specified error (given as rvalue).
     * 
     * @param e the error to put into the @c Result
     * 
     * @return A @c Result that contains the error @c e
     *  
     * @req {SWS_CORE_00835}
     */
    static Result FromError(error_type &&e);

    /**
     * @brief Build a new @c Result from an error that is constructed in-place from the given arguments.
     * 
     * @tparam Args... the types of arguments given to this function
     * @param args the parameter pack used for constructing the error
     * 
     * @return A @c Result that contains an error
     *  
     * @req {SWS_CORE_00836}
     */
    template <typename... Args,
              details::requires_
              <
                  std::is_constructible_v<error_type, Args...> 
                  && !std::is_same_v<error_type, details::first_param_t<Args...>>
                  && !details::is_specialization_of_result<details::first_param_t<Args...>>::value
              > * = nullptr
             >    
    static Result FromError(Args &&... args)
    {
        error_type* error = ::new (error) error_type(std::forward<Args>(args)...);
        return Result(*error);
    }

    /**
     * @brief Copy-assign another @c Result to this instance.
     * 
     * @param other the other instance
     * 
     * @return @c *this, containing the contents of @c other
     *  
     * @req {SWS_CORE_00841}
     */
    Result& operator=(Result const &other);

    /**
     * @brief Move-assign another  @c Result to this instance.
     * 
     * @param other the other instance
     * 
     * @return @c *this, containing the contents of @c other
     *  
     * @req {SWS_CORE_00842}
     */
    Result& operator=(Result &&other)
        noexcept(std::is_nothrow_move_constructible_v<error_type> && std::is_nothrow_move_assignable_v<error_type>);

    /**
     * @brief Put a new value into this instance, constructed in-place from the given arguments.
     * 
     * @tparam Args... the types of arguments given to this function
     * @param args the arguments used for constructing the value
     *  
     * @req {SWS_CORE_00843}
     */
    template <typename... Args>
    void EmplaceValue (Args &&... args) noexcept;

    /**
     * @brief Put a new error into this instance, constructed in-place from the given arguments.
     * 
     * @tparam Args... the types of arguments given to this function
     * @param args the arguments used for constructing the value
     *  
     * @req {SWS_CORE_00844}
     */
    template <typename... Args>
    void EmplaceError (Args &&... args);

    /**
     * @brief Exchange the contents of this instance with those of @c other.
     * 
     * @param other- the other instance
     *  
     * @req {SWS_CORE_00845}
     */
    void Swap (Result &other)
        noexcept(std::is_nothrow_move_constructible_v<error_type> && std::is_nothrow_move_assignable_v<error_type>);

    /**
     * @brief Check whether @c *this contains a value.
     * 
     * @return @c true if @c *this contains a value, @c false otherwise
     *  
     * @req {SWS_CORE_00851}
     */
    bool HasValue () const noexcept;

    /**
     * @brief Check whether @c *this contains a value.
     * 
     * @return @c true if @c *this contains a value, @c false otherwise
     *  
     * @req {SWS_CORE_00852}
     */
    explicit operator bool() const noexcept;

    /**
     * @brief Do nothing.
     * This function only exists for helping with generic programming.
     * The behavior of this function is undefined if @c *this does not contain a value.
     *  
     * @req {SWS_CORE_00853}
     */
    void operator *() const;

    /**
     * @brief Do nothing.
     * This function only exists for helping with generic programming.
     * The behavior of this function is undefined if @c *this does not contain a value.
     *  
     * @req {SWS_CORE_00855}
     */
    void Value () const;

    /**
     * @brief Access the contained error.
     * The behavior of this function is undefined if @c *this does not contain an error.
     * 
     * @return A const reference to the contained error
     *  
     * @req {SWS_CORE_00857}
     */
    error_type const& Error() const &;

    /**
     * @brief Access the contained error.
     * The behavior of this function is undefined if @c *this does not contain an error.
     * 
     * @return An rvalue reference to the contained error
     *  
     * @req {SWS_CORE_00858}
     */
    error_type&& Error() &&;

    /**
     * @brief Do nothing.
     * This function only exists for helping with generic programming.
     * 
     * @tparam U the type of @c defaultValue
     * @param defaultValue the value to use if @c *this does not contain a value
     *  
     * @req {SWS_CORE_00861}
     */
    template <typename U>
    void ValueOr (U &&defaultValue) const {}

    /**
     * @brief Return the contained error or the given default error.
     * If @c *this contains an error, it is returned. Otherwise, the specified default error is returned, static_cast’d to @c error_type.
     * 
     * @tparam G the type of @c defaultError
     * @param defaultError the error to use if @c *this does not contain an error
     * 
     * @return The error
     *  
     * @req {SWS_CORE_00863}
     */
    template <typename G>
    error_type ErrorOr (G &&defaultError) const;

    /**
     * @brief Return whether this instance contains the given error.
     * This call compares the argument error, static_cast’d to @c error_type, with the return value from Error().
     * 
     * @tparam G the type of the error argument @c error
     * @param error the error to check
     * 
     * @return @c true if @c *this contains an error that is equivalent to the given @c error, @c false otherwise
     *  
     * @req {SWS_CORE_00865}
     */
    template <typename G>
    bool CheckError (G &&error) const;

#if __cpp_exceptions == 199711
// This function does not participate in overload resolution when the compiler toolchain does not support C++ exceptions.
    /**
     * @brief Return the contained value or throw an exception.
     * 
     * @exception the exception type associated with the contained error
     *  
     * @req {SWS_CORE_00866}
     */
    void ValueOrThrow () const noexcept(false);
#endif // __cpp_exceptions == 199711

    /**
     * @brief Do nothing or call a function.
     * If @c *this contains a value, this function does nothing. Otherwise, the specified callable is invoked.
     * The Callable is expected to be compatible to this interface: <tt>void f(error_type const&);</tt>
     * This function only exists for helping with generic programming.
     * 
     * @tparam F the type of the Callable @c f
     * @param f the Callable
     *  
     * @req {SWS_CORE_00867}
     */
    template <typename F>
    void Resolve (F &&f) const;
};

/**
 * @brief Compare two @c Result instances for equality.
 * A @c Result that contains a value is unequal to every Result containing an error.
 * A @c Result is equal to another @c Result only if both contain the same type, and the value of that type compares equal.
 * 
 * @param lhs the left hand side of the comparison
 * @param rhs the right hand side of the comparison
 * 
 * @return @c true if the two instances compare equal, @c false otherwise
 *  
 * @req {SWS_CORE_00780}
 */
template <typename T, typename E>
bool operator==(Result<T, E> const &lhs, Result<T, E> const &rhs)
{
    if (lhs.HasValue() && rhs.HasValue())
    {
        return lhs.Value() == rhs.Value();
    }

    return false;
}

/**
 * @brief Compare two @c Result instances for inequality.
 * A @c Result that contains a value is unequal to every Result containing an error.
 * A @c Result is equal to another @c Result only if both contain the same type, and the value of that type compares equal.
 * 
 * @param lhs the left hand side of the comparison
 * @param rhs the right hand side of the comparison
 * 
 * @return @c true if the two instances compare unequal, @c false otherwise
 *  
 * @req {SWS_CORE_00781}
 */
template <typename T, typename E>
bool operator!=(Result<T, E> const &lhs, Result<T, E> const &rhs)
{
    return !(lhs == rhs);
}

/**
 * @brief Compare a @c Result instance for equality to a value.
 * A @c Result that contains no value is unequal to every value.
 * A @c Result is equal to a value only if the @c Result contains a value of the same type, and the values compare equal.
 * 
 * @param lhs the @c Result instance
 * @param rhs the value to compare with
 * 
 * @return @c true if the @c Result’s value compares equal to the @c rhs value, @c false otherwise
 *  
 * @req {SWS_CORE_00782}
 */
template <typename T, typename E>
bool operator==(Result<T, E> const &lhs, T const &rhs)
{
    if (lhs.HasValue())
    {
        return lhs.Value() == rhs;
    }

    return false;
}

/**
 * @brief Compare a @c Result instance for equality to a value.
 * A @c Result that contains no value is unequal to every value.
 * A @c Result is equal to a value only if the @c Result contains a value of the same type, and the values compare equal.
 * 
 * @param lhs the value to compare with
 * @param rhs the @c Result instance
 * 
 * @return @c true if the @c Result’s value compares equal to the @c lhs value, @c false otherwise
 *  
 * @req {SWS_CORE_00783}
 */
template <typename T, typename E>
bool operator==(T const &lhs, Result<T, E> const &rhs)
{
    return rhs == lhs;
}

/**
 * @brief Compare a @c Result instance for inequality to a value.
 * A Result that contains no value is unequal to every value.
 * A @c Result is equal to a value only if the @c Result contains a value of the same type, and the values compare equal.
 * 
 * @param lhs the @c Result instance
 * @param rhs the value to compare with
 * 
 * @return @c true if the @c Result’s value compares unequal to the @c rhs value, @c false otherwise
 *  
 * @req {SWS_CORE_00784}
 */
template <typename T, typename E>
bool operator!=(Result<T, E> const &lhs, T const &rhs)
{
    return !(lhs == rhs);
}

/**
 * @brief Compare a @c Result instance for inequality to a value.
 * A @c Result that contains no value is unequal to every value.
 * A @c Result is equal to a value only if the @c Result contains a value of the same type, and the values compare equal.
 * 
 * @param lhs the value to compare with
 * @param rhs the @c Result instance
 * 
 * @return @c true if the @c Result’s value compares unequal to the @c lhs value, @c false otherwise
 *  
 * @req {SWS_CORE_00785}
 */
template <typename T, typename E>
bool operator!=(T const &lhs, Result<T, E> const &rhs)
{
    return !(rhs == lhs);
}

/**
 * @brief Compare a @c Result instance for equality to an error.
 * A @c Result that contains no error is unequal to every error.
 * A @c Result is equal to an error only if the @c Result contains an error of the same type, and the errors compare equal.
 * 
 * @param lhs the @c Result instance
 * @param rhs the error to compare with
 * 
 * @return @c true if the @c Result’s error compares equal to the @c rhs error, @c false otherwise
 *  
 * @req {SWS_CORE_00786}
 */
template <typename T, typename E>
bool operator==(Result<T, E> const &lhs, E const &rhs)
{
    if (!lhs.HasValue())
    {
        return lhs.Error() == rhs;
    }

    return false;
}

/**
 * @brief Compare a @c Result instance for equality to an error.
 * A @c Result that contains no error is unequal to every error.
 * A @c Result is equal to an error only if the @c Result contains an error of the same type, and the errors compare equal.
 * 
 * @param lhs the error to compare with
 * @param rhs the @c Result instance
 * 
 * @return @c true if the @c Result’s error compares equal to the @c lhs error, @c false otherwise
 *  
 * @req {SWS_CORE_00787}
 */
template <typename T, typename E>
bool operator==(E const &lhs, Result<T, E> const &rhs)
{
    return rhs == lhs;
}

/**
 * @brief Compare a @c Result instance for inequality to an error.
 * A @c Result that contains no error is unequal to every error.
 * A @c Result is equal to an error only if the @c Result contains an error of the same type, and the errors compare equal.
 * 
 * @param lhs the @c Result instance
 * @param rhs the error to compare with
 * 
 * @return @c true if the @c Result’s error compares unequal to the @c rhs error, @c false otherwise
 *  
 * @req {SWS_CORE_00788}
 */
template <typename T, typename E>
bool operator!=(Result<T, E> const &lhs, E const &rhs)
{
    return !(lhs == rhs);
}

/**
 * @brief Compare a @c Result instance for inequality to an error.
 * A @c Result that contains no error is unequal to every error.
 * A @c Result is equal to an error only if the @c Result contains an error of the same type, and the errors compare equal.
 * 
 * @param lhs the error to compare with
 * @param rhs the @c Result instance
 * 
 * @return @c true if the @c Result’s error compares unequal to the @c lhs error, @c false otherwise
 *  
 * @req {SWS_CORE_00789}
 */
template <typename T, typename E>
bool operator!=(E const &lhs, Result<T, E> const &rhs)
{
   return !(rhs == lhs);
}

/**
 * @brief Swap the contents of the two given arguments.
 * 
 * @param lhs one instance
 * @param rhs another instance
 *  
 * @req {SWS_CORE_00796}
 */
template <typename T, typename E>
void swap(Result<T, E> &lhs, Result<T, E> &rhs) noexcept(noexcept(lhs.Swap(rhs)))
{
    lhs.Swap(rhs);
}

} // namespace ara::core

#endif // ARA_CORE_RESULT_H_
