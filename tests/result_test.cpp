#include <catch2/catch.hpp>
#include <stdexcept>

#include "ara/core/result.h"

struct ErrorTest
{
    ErrorTest(int intVal) : intVal_{intVal}, stringVal_{"Init"} {}
    ErrorTest(std::string& stringVal) : intVal_{0}, stringVal_{stringVal} {}
    ErrorTest(int intVal, std::string& stringVal)
      : intVal_{intVal}, stringVal_{stringVal}
    {}

    void ThrowAsException() const
    {
        throw std::runtime_error("ErrorTest exception");
    }

    int         getIntVal() const { return intVal_; }
    std::string getStringVal() const { return stringVal_; }


 private:
    int         intVal_;
    std::string stringVal_;
};

bool operator==(ErrorTest const& lhs, ErrorTest const& rhs)
{
    return lhs.getIntVal() == rhs.getIntVal();
}

struct ValueTest
{
    ValueTest(int intVal) : intVal_{intVal}, stringVal_{"Init"} {}
    ValueTest(std::string& stringVal) : intVal_{0}, stringVal_{stringVal} {}
    ValueTest(int intVal, std::string& stringVal)
      : intVal_{intVal}, stringVal_{stringVal}
    {}

    int operator+(int valueToAdd)
    {
        intVal_ += valueToAdd;
        return intVal_;
    }

    int         getIntVal() const { return intVal_; }
    std::string getStringVal() const { return stringVal_; }

 private:
    int         intVal_;
    std::string stringVal_;
};

TEST_CASE("Constructs and destructs a Result based on value",
          "[SWS_CORE], [SWS_CORE_00721], [SWS_CORE_00722], [SWS_CORE_00731]")
{
    ValueTest                    value(10);
    ara::core::Result<ValueTest> resultLValue(value);
    CHECK(resultLValue.HasValue());

    ara::core::Result<ValueTest> resultRValue(ValueTest(10));
    CHECK(resultRValue.HasValue());
}

TEST_CASE("Constructs and destructs a Result based on error",
          "[SWS_CORE], [SWS_CORE_00723], [SWS_CORE_00724], [SWS_CORE_00731]")
{
    ErrorTest                               error(10);
    ara::core::Result<ValueTest, ErrorTest> resultLValue(error);
    CHECK(! resultLValue.HasValue());

    ara::core::Result<ValueTest, ErrorTest> resultRValue(ErrorTest(10));
    CHECK(! resultRValue.HasValue());
}

TEST_CASE("Copy and Move constructs a Result based on other instance",
          "[SWS_CORE], [SWS_CORE_00725], [SWS_CORE_00726]")
{
    ValueTest                    value(10);
    ara::core::Result<ValueTest> resultBasedOnValue(value);

    ara::core::Result<ValueTest> copiedResultBasedOnValue(resultBasedOnValue);
    CHECK(copiedResultBasedOnValue.HasValue());

    ara::core::Result<ValueTest> movedResultBasedOnValue(
      std::move(resultBasedOnValue));
    CHECK(movedResultBasedOnValue.HasValue());

    ErrorTest                               error(10);
    ara::core::Result<ValueTest, ErrorTest> resultBasedOnError(error);

    ara::core::Result<ValueTest, ErrorTest> copiedResultBasedOnError(
      resultBasedOnError);
    CHECK(! copiedResultBasedOnError.HasValue());

    ara::core::Result<ValueTest, ErrorTest> movedResultBasedOnError(
      std::move(resultBasedOnError));
    CHECK(! movedResultBasedOnError.HasValue());
}

TEST_CASE("Constuct a Result by FromValue method",
          "[SWS_CORE], [SWS_CORE_00731], [SWS_CORE_00732], [SWS_CORE_00733]")
{
    const ValueTest value(10);
    auto resultLValue = ara::core::Result<ValueTest>::FromValue(value);
    auto resultRValue = ara::core::Result<ValueTest>::FromValue(ValueTest(15));
    std::string testString = "Test string";
    auto        resultFromArguments =
      ara::core::Result<ValueTest>::FromValue(10, testString);

    CHECK(resultLValue.HasValue());
    CHECK(resultRValue.HasValue());
    CHECK(resultFromArguments.HasValue());
}

TEST_CASE("Constuct a Result by FromError method",
          "[SWS_CORE], [SWS_CORE_00734], [SWS_CORE_00735], [SWS_CORE_00736]")
{
    const ErrorTest error(10);
    auto            resultLValue =
      ara::core::Result<ValueTest, ErrorTest>::FromError(error);
    auto resultRValue =
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(15));
    std::string testString = "Test string";
    auto        resultFromArguments =
      ara::core::Result<ValueTest, ErrorTest>::FromError(10, testString);

    CHECK(! resultLValue.HasValue());
    CHECK(! resultRValue.HasValue());
    CHECK(! resultFromArguments.HasValue());
}

TEST_CASE("Copy-assign", "[SWS_CORE], [SWS_CORE_00741]")
{
    // Result1 has a value, Result2 has a value
    auto resultWithValue1 =
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(10));
    auto resultWithValue2 =
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(20));

    CHECK(resultWithValue1.HasValue());
    CHECK(resultWithValue2.HasValue());
    CHECK(resultWithValue1.Value().getIntVal() == 10);
    CHECK(resultWithValue2.Value().getIntVal() == 20);

    resultWithValue1 = resultWithValue2;

    CHECK(resultWithValue1.Value().getIntVal() == 20);
    CHECK(resultWithValue2.Value().getIntVal() == 20);

    // Result1 doesn't have a value, Result2 doesn't have a value
    auto resultWithError1 =
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(30));
    auto resultWithError2 =
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(40));

    CHECK(! resultWithError1.HasValue());
    CHECK(! resultWithError2.HasValue());
    CHECK(resultWithError1.Error().getIntVal() == 30);
    CHECK(resultWithError2.Error().getIntVal() == 40);

    resultWithError1 = resultWithError2;

    CHECK(resultWithError1.Error().getIntVal() == 40);
    CHECK(resultWithError2.Error().getIntVal() == 40);

    // Result1 has a value, Result2 doesn't have a value
    resultWithValue1 = resultWithError1;
    CHECK(! resultWithValue1.HasValue());
    CHECK(resultWithValue1.Error().getIntVal() == 40);

    // Result1 doesn't have a value, Result2 has a value
    resultWithError2 = resultWithValue2;
    CHECK(resultWithError2.HasValue());
    CHECK(resultWithError2.Value().getIntVal() == 20);
}

TEST_CASE("Move-assign", "[SWS_CORE], [SWS_CORE_00742]")
{
    // Result1 has a value, Result2 has a value
    auto resultWithValue1 =
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(10));

    CHECK(resultWithValue1.HasValue());
    CHECK(resultWithValue1.Value().getIntVal() == 10);

    resultWithValue1 =
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(20));

    CHECK(resultWithValue1.Value().getIntVal() == 20);

    // Result1 doesn't have a value, Result2 doesn't have a value
    auto resultWithError1 =
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(30));

    CHECK(! resultWithError1.HasValue());
    CHECK(resultWithError1.Error().getIntVal() == 30);

    resultWithError1 =
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(40));

    CHECK(resultWithError1.Error().getIntVal() == 40);

    // Result1 has a value, Result2 doesn't have a value
    resultWithValue1 =
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(50));
    CHECK(! resultWithValue1.HasValue());
    CHECK(resultWithValue1.Error().getIntVal() == 50);

    // Result1 doesn't have a value, Result2 has a value
    resultWithError1 =
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(60));
    CHECK(resultWithError1.HasValue());
    CHECK(resultWithError1.Value().getIntVal() == 60);
}

TEST_CASE("EmplaceValue method", "[SWS_CORE], [SWS_CORE_00743]")
{
    // Result already has a value
    auto resultWithValue1 =
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(10));

    CHECK(resultWithValue1.Value().getIntVal() == 10);
    CHECK(resultWithValue1.Value().getStringVal() == "Init");

    std::string message = "New value message";
    resultWithValue1.EmplaceValue(20, message);

    CHECK(resultWithValue1.HasValue());
    CHECK(resultWithValue1.Value().getIntVal() == 20);
    CHECK(resultWithValue1.Value().getStringVal() == message);

    // Result already has an error
    auto resultWithError1 =
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(10));
    CHECK(! resultWithError1.HasValue());
    CHECK(resultWithError1.Value().getIntVal() == 10);
    CHECK(resultWithError1.Value().getStringVal() == "Init");

    resultWithError1.EmplaceValue(20, message);
    CHECK(resultWithError1.HasValue());
    CHECK(resultWithError1.Value().getIntVal() == 20);
    CHECK(resultWithError1.Value().getStringVal() == message);
}

TEST_CASE("EmplaceError method", "[SWS_CORE], [SWS_CORE_00744]")
{
    // Result already has a value
    auto resultWithValue1 =
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(10));

    CHECK(resultWithValue1.Value().getIntVal() == 10);
    CHECK(resultWithValue1.Value().getStringVal() == "Init");

    std::string message = "New value message";
    resultWithValue1.EmplaceError(20, message);

    CHECK(! resultWithValue1.HasValue());
    CHECK(resultWithValue1.Error().getIntVal() == 20);
    CHECK(resultWithValue1.Error().getStringVal() == message);

    // Result already has an error
    auto resultWithError1 =
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(10));
    CHECK(! resultWithError1.HasValue());

    resultWithError1.EmplaceError(20, message);
    CHECK(! resultWithError1.HasValue());
    CHECK(resultWithError1.Error().getIntVal() == 20);
    CHECK(resultWithError1.Error().getStringVal() == message);
}

TEST_CASE("Swap method", "[SWS_CORE], [SWS_CORE_00745]")
{
    int         value1        = 11;
    int         value2        = 12;
    int         error1        = 21;
    int         error2        = 22;
    std::string valueMessage1 = "Value Message1";
    std::string valueMessage2 = "Value Message2";
    std::string errorMessage1 = "Error Message1";
    std::string errorMessage2 = "Error Message2";

    auto resultWithValue1 = ara::core::Result<ValueTest, ErrorTest>::FromValue(
      ValueTest(value1, valueMessage1));
    auto resultWithValue2 = ara::core::Result<ValueTest, ErrorTest>::FromValue(
      ValueTest(value2, valueMessage2));
    CHECK(resultWithValue1.HasValue());
    CHECK(resultWithValue2.HasValue());
    CHECK(resultWithValue1.Value().getIntVal() == value1);
    CHECK(resultWithValue2.Value().getIntVal() == value2);
    CHECK(resultWithValue1.Value().getStringVal() == valueMessage1);
    CHECK(resultWithValue2.Value().getStringVal() == valueMessage2);

    auto resultWithError1 = ara::core::Result<ValueTest, ErrorTest>::FromError(
      ErrorTest(error1, errorMessage1));
    auto resultWithError2 = ara::core::Result<ValueTest, ErrorTest>::FromError(
      ErrorTest(error2, errorMessage2));
    CHECK(! resultWithError1.HasValue());
    CHECK(! resultWithError2.HasValue());
    CHECK(resultWithError1.Error().getIntVal() == error1);
    CHECK(resultWithError2.Error().getIntVal() == error2);
    CHECK(resultWithError1.Error().getStringVal() == errorMessage1);
    CHECK(resultWithError2.Error().getStringVal() == errorMessage2);

    // Result1 has a value, Result2 has a value
    resultWithValue1.Swap(resultWithValue2);
    CHECK(resultWithValue1.HasValue());
    CHECK(resultWithValue2.HasValue());
    CHECK(resultWithValue1.Value().getIntVal() == value2);
    CHECK(resultWithValue2.Value().getIntVal() == value1);
    CHECK(resultWithValue1.Value().getStringVal() == valueMessage2);
    CHECK(resultWithValue2.Value().getStringVal() == valueMessage1);

    // Result1 doesn't have a value, Result2 doesn't have a value
    resultWithError1.Swap(resultWithError2);
    CHECK(! resultWithError1.HasValue());
    CHECK(! resultWithError2.HasValue());
    CHECK(resultWithError1.Error().getIntVal() == error2);
    CHECK(resultWithError2.Error().getIntVal() == error1);
    CHECK(resultWithError1.Error().getStringVal() == errorMessage2);
    CHECK(resultWithError2.Error().getStringVal() == errorMessage1);

    // Result1 doesn't have a value, Result2 has a value
    resultWithError1.Swap(resultWithValue1);
    CHECK(resultWithError1.HasValue());
    CHECK(! resultWithValue1.HasValue());
    CHECK(resultWithError1.Value().getIntVal() == value2);
    CHECK(resultWithValue1.Error().getIntVal() == error2);
    CHECK(resultWithError1.Value().getStringVal() == valueMessage2);
    CHECK(resultWithValue1.Error().getStringVal() == errorMessage2);

    // Result1 has a value, Result2 doesn't have a value
    resultWithValue2.Swap(resultWithError2);
    CHECK(resultWithError2.HasValue());
    CHECK(! resultWithValue2.HasValue());
    CHECK(resultWithError2.Value().getIntVal() == value1);
    CHECK(resultWithValue2.Error().getIntVal() == error1);
    CHECK(resultWithError2.Value().getStringVal() == valueMessage1);
    CHECK(resultWithValue2.Error().getStringVal() == errorMessage1);
}

TEST_CASE(
  "Value getters and checkers",
  "[SWS_CORE], [SWS_CORE_00751], [SWS_CORE_00752], [SWS_CORE_00753], [SWS_CORE_00759], [SWS_CORE_00754], [SWS_CORE_00755], [SWS_CORE_00756]")
{
    auto resultWithValue1 =
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(10));
    CHECK(resultWithValue1.HasValue());
    CHECK(resultWithValue1);

    ValueTest value1 = *resultWithValue1;
    CHECK(value1.getIntVal() == 10);

    ValueTest value2(
      *(ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(10))));
    CHECK(value2.getIntVal() == 10);

    CHECK(resultWithValue1->getIntVal() == 10);

    CHECK(resultWithValue1.Value().getIntVal() == 10);
    auto value3(
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(10)).Value());
    CHECK(value3.getIntVal() == 10);
    //
}

TEST_CASE("Error getters", "[SWS_CORE], [SWS_CORE_00757], [SWS_CORE_00758]")
{
    auto resultWithError1 =
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(10));
    ErrorTest error(
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(10)).Error());

    CHECK(resultWithError1.Error().getIntVal() == error.getIntVal());
}

TEST_CASE("ValueOr method", "[SWS_CORE], [SWS_CORE_00761], [SWS_CORE_00762]")
{
    auto resultWithValue =
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(10));
    auto resultWithError =
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(20));

    auto value1 = resultWithValue.ValueOr(88);
    auto value2 = resultWithError.ValueOr(88);

    CHECK(value1.getIntVal() == 10);
    CHECK(value2.getIntVal() == 88);

    ValueTest value3(
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(10))
        .ValueOr(88));
    ValueTest value4(
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(20))
        .ValueOr(88));

    CHECK(value3.getIntVal() == 10);
    CHECK(value4.getIntVal() == 88);
}

TEST_CASE("ErrorOr method", "[SWS_CORE], [SWS_CORE_00763]")
{
    auto resultWithValue =
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(10));
    auto resultWithError =
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(20));

    auto error1 = resultWithValue.ErrorOr(88);
    auto error2 = resultWithError.ErrorOr(88);

    CHECK(error1.getIntVal() == 88);
    CHECK(error2.getIntVal() == 20);
}

TEST_CASE("CheckError method", "[SWS_CORE], [SWS_CORE_00765]")
{
    auto resultWithValue =
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(10));
    CHECK(! resultWithValue.CheckError(20));

    auto resultWithError =
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(20));
    CHECK(resultWithError.CheckError(20));
    CHECK(! resultWithError.CheckError(30));
}

#if __cpp_exceptions == 199711
TEST_CASE("ValueOrThrow method",
          "[SWS_CORE], [SWS_CORE_00766], [SWS_CORE_00769]")
{
    auto resultWithValue =
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(10));
    auto resultWithError =
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(20));

    bool exceptionThrown = false;

    CHECK(resultWithValue.ValueOrThrow().getIntVal() == 10);
    ValueTest value(
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(10))
        .ValueOrThrow());
    CHECK(value.getIntVal() == 10);

    try
    {
        value = resultWithError.ValueOrThrow();
    }
    catch (std::runtime_error&)
    {
        exceptionThrown = true;
    }

    CHECK(exceptionThrown);

    exceptionThrown = false;

    try
    {
        auto value1(
          ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(20))
            .ValueOrThrow());
    }
    catch (std::runtime_error&)
    {
        exceptionThrown = true;
    }

    CHECK(exceptionThrown);
}
#endif  // __cpp_exceptions == 199711

TEST_CASE("Resolve method", "[SWS_CORE], [SWS_CORE_00767]")
{
    auto resultWithValue =
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(10));
    auto value1 = resultWithValue.Resolve([](ErrorTest const&) { return 50; });
    CHECK(value1.getIntVal() == 10);

    auto resultWithError =
      ara::core::Result<ValueTest, ErrorTest>::FromError(ErrorTest(20));
    auto value2 = resultWithError.Resolve([](ErrorTest const&) { return 50; });
    CHECK(value2.getIntVal() == 50);
}

TEST_CASE("Bind method", "[SWS_CORE], [SWS_CORE_00768]")
{
    auto resultWithValue =
      ara::core::Result<ValueTest, ErrorTest>::FromValue(ValueTest(10));
    auto value1 =
      resultWithValue.Bind([](ValueTest val) -> ValueTest { return val + 50; });
    CHECK(value1.Value().getIntVal() == 60);

    auto value2 = resultWithValue.Bind(
      [](ValueTest val) -> ara::core::Result<ValueTest, ErrorTest> {
          ValueTest toReturn(val + 100);
          return ara::core::Result<ValueTest, ErrorTest>(toReturn);
      });
    CHECK(value2.Value().getIntVal() == 110);
}

TEST_CASE(
  "Specialization for value_type=void: Constructs and destructs a Result with void value",
  "[SWS_CORE], [SWS_CORE_00801], [SWS_CORE_00825]")
{
    ara::core::Result<void> result;

    CHECK(result.HasValue());
}

TEST_CASE(
  "Specialization for value_type=void: Constructs and destructs a Result based on error",
  "[SWS_CORE], [SWS_CORE_00823], [SWS_CORE_00825]")
{
    ErrorTest                          error(10);
    ara::core::Result<void, ErrorTest> resultLValue(error);
    CHECK(! resultLValue.HasValue());

    ara::core::Result<void, ErrorTest> resultRValue(ErrorTest(10));
    CHECK(! resultRValue.HasValue());
}

TEST_CASE(
  "Specialization for value_type=void: Copy and Move constructs a Result based on other instance",
  "[SWS_CORE], [SWS_CORE_00785], [SWS_CORE_00826]")
{
    ara::core::Result<void> resultBasedOnValue;

    ara::core::Result<void> copiedResultBasedOnValue(resultBasedOnValue);
    CHECK(copiedResultBasedOnValue.HasValue());

    ara::core::Result<void> movedResultBasedOnValue(
      std::move(resultBasedOnValue));
    CHECK(movedResultBasedOnValue.HasValue());

    ErrorTest                          error(10);
    ara::core::Result<void, ErrorTest> resultBasedOnError(error);

    ara::core::Result<void, ErrorTest> copiedResultBasedOnError(
      resultBasedOnError);
    CHECK(! copiedResultBasedOnError.HasValue());

    ara::core::Result<void, ErrorTest> movedResultBasedOnError(
      std::move(resultBasedOnError));
    CHECK(! movedResultBasedOnError.HasValue());
}

TEST_CASE(
  "Specialization for value_type=void: Constuct a Result by FromValue method",
  "[SWS_CORE], [SWS_CORE_00831]")
{
    auto value = ara::core::Result<void>::FromValue();
    CHECK(value.HasValue());
}

TEST_CASE(
  "Specialization for value_type=void: Constuct a Result by FromError method",
  "[SWS_CORE], [SWS_CORE_00834], [SWS_CORE_00835], [SWS_CORE_00836]")
{
    const ErrorTest error(10);
    auto resultLValue = ara::core::Result<void, ErrorTest>::FromError(error);
    auto resultRValue =
      ara::core::Result<void, ErrorTest>::FromError(ErrorTest(15));
    std::string testString = "Test string";
    auto        resultFromArguments =
      ara::core::Result<void, ErrorTest>::FromError(10, testString);

    CHECK(! resultLValue.HasValue());
    CHECK(! resultRValue.HasValue());
    CHECK(! resultFromArguments.HasValue());
}

TEST_CASE("Specialization for value_type=void: Copy-assign",
          "[SWS_CORE], [SWS_CORE_00841]")
{
    auto resultWithValue1 = ara::core::Result<void, ErrorTest>::FromValue();
    auto resultWithValue2 = ara::core::Result<void, ErrorTest>::FromValue();

    CHECK(resultWithValue1.HasValue());
    CHECK(resultWithValue2.HasValue());

    // Result1 doesn't have a value, Result2 doesn't have a value
    auto resultWithError1 =
      ara::core::Result<void, ErrorTest>::FromError(ErrorTest(30));
    auto resultWithError2 =
      ara::core::Result<void, ErrorTest>::FromError(ErrorTest(40));

    CHECK(! resultWithError1.HasValue());
    CHECK(! resultWithError2.HasValue());
    CHECK(resultWithError1.Error().getIntVal() == 30);
    CHECK(resultWithError2.Error().getIntVal() == 40);

    resultWithError1 = resultWithError2;

    CHECK(resultWithError1.Error().getIntVal() == 40);
    CHECK(resultWithError2.Error().getIntVal() == 40);

    // Result1 has a value, Result2 doesn't have a value
    resultWithValue1 = resultWithError1;
    CHECK(! resultWithValue1.HasValue());
    CHECK(resultWithValue1.Error().getIntVal() == 40);

    // Result1 doesn't have a value, Result2 has a value
    resultWithError2 = resultWithValue2;
    CHECK(resultWithError2.HasValue());
}

TEST_CASE("Specialization for value_type=void: Move-assign",
          "[SWS_CORE], [SWS_CORE_00842]")
{
    auto resultWithValue1 = ara::core::Result<void, ErrorTest>::FromValue();

    CHECK(resultWithValue1.HasValue());

    auto resultWithError1 =
      ara::core::Result<void, ErrorTest>::FromError(ErrorTest(30));
    CHECK(! resultWithError1.HasValue());
    CHECK(resultWithError1.Error().getIntVal() == 30);

    // Result1 doesn't have a value, Result2 doesn't have a value
    resultWithError1 =
      ara::core::Result<void, ErrorTest>::FromError(ErrorTest(40));
    CHECK(resultWithError1.Error().getIntVal() == 40);

    // Result1 has a value, Result2 doesn't have a value
    resultWithValue1 =
      ara::core::Result<void, ErrorTest>::FromError(ErrorTest(50));
    CHECK(! resultWithValue1.HasValue());
    CHECK(resultWithValue1.Error().getIntVal() == 50);

    // Result1 doesn't have a value, Result2 has a value
    resultWithError1 = ara::core::Result<void, ErrorTest>::FromValue();
    CHECK(resultWithError1.HasValue());
}

TEST_CASE("Specialization for value_type=void: EmplaceValue method",
          "[SWS_CORE], [SWS_CORE_00843]")
{
    // Result already has a value
    auto resultWithValue1 = ara::core::Result<void, ErrorTest>::FromValue();
    CHECK(resultWithValue1.HasValue());

    // Result already has an error
    auto resultWithError1 =
      ara::core::Result<void, ErrorTest>::FromError(ErrorTest(10));
    CHECK(! resultWithError1.HasValue());

    resultWithError1.EmplaceValue();
    CHECK(resultWithError1.HasValue());
}

TEST_CASE("Specialization for value_type=void: EmplaceError method",
          "[SWS_CORE], [SWS_CORE_00844]")
{
    // Result already has a value
    auto resultWithValue1 = ara::core::Result<void, ErrorTest>::FromValue();

    std::string message = "New value message";
    resultWithValue1.EmplaceError(20, message);

    CHECK(! resultWithValue1.HasValue());
    CHECK(resultWithValue1.Error().getIntVal() == 20);
    CHECK(resultWithValue1.Error().getStringVal() == message);

    // Result already has an error
    auto resultWithError1 =
      ara::core::Result<void, ErrorTest>::FromError(ErrorTest(10));
    CHECK(! resultWithError1.HasValue());

    resultWithError1.EmplaceError(20, message);
    CHECK(! resultWithError1.HasValue());
    CHECK(resultWithError1.Error().getIntVal() == 20);
    CHECK(resultWithError1.Error().getStringVal() == message);
}

TEST_CASE("Specialization for value_type=void: Swap method",
          "[SWS_CORE], [SWS_CORE_00845]")
{
    int         error1        = 21;
    int         error2        = 22;
    std::string valueMessage1 = "Value Message1";
    std::string valueMessage2 = "Value Message2";
    std::string errorMessage1 = "Error Message1";
    std::string errorMessage2 = "Error Message2";

    auto resultWithValue1 = ara::core::Result<void, ErrorTest>::FromValue();
    auto resultWithValue2 = ara::core::Result<void, ErrorTest>::FromValue();
    CHECK(resultWithValue1.HasValue());
    CHECK(resultWithValue2.HasValue());

    auto resultWithError1 = ara::core::Result<void, ErrorTest>::FromError(
      ErrorTest(error1, errorMessage1));
    auto resultWithError2 = ara::core::Result<void, ErrorTest>::FromError(
      ErrorTest(error2, errorMessage2));
    CHECK(! resultWithError1.HasValue());
    CHECK(! resultWithError2.HasValue());
    CHECK(resultWithError1.Error().getIntVal() == error1);
    CHECK(resultWithError2.Error().getIntVal() == error2);
    CHECK(resultWithError1.Error().getStringVal() == errorMessage1);
    CHECK(resultWithError2.Error().getStringVal() == errorMessage2);

    // Result1 has a value, Result2 has a value
    resultWithValue1.Swap(resultWithValue2);
    CHECK(resultWithValue1.HasValue());
    CHECK(resultWithValue2.HasValue());

    // Result1 doesn't have a value, Result2 doesn't have a value
    resultWithError1.Swap(resultWithError2);
    CHECK(! resultWithError1.HasValue());
    CHECK(! resultWithError2.HasValue());
    CHECK(resultWithError1.Error().getIntVal() == error2);
    CHECK(resultWithError2.Error().getIntVal() == error1);
    CHECK(resultWithError1.Error().getStringVal() == errorMessage2);
    CHECK(resultWithError2.Error().getStringVal() == errorMessage1);

    // Result1 doesn't have a value, Result2 has a value
    resultWithError1.Swap(resultWithValue1);
    CHECK(resultWithError1.HasValue());
    CHECK(! resultWithValue1.HasValue());
    CHECK(resultWithValue1.Error().getIntVal() == error2);
    CHECK(resultWithValue1.Error().getStringVal() == errorMessage2);

    // Result1 has a value, Result2 doesn't have a value
    resultWithValue2.Swap(resultWithError2);
    CHECK(resultWithError2.HasValue());
    CHECK(! resultWithValue2.HasValue());
    CHECK(resultWithValue2.Error().getIntVal() == error1);
    CHECK(resultWithValue2.Error().getStringVal() == errorMessage1);
}

TEST_CASE(
  "Specialization for value_type=void: Value getters and checkers",
  "[SWS_CORE], [SWS_CORE_00851], [SWS_CORE_00852], [SWS_CORE_00853], [SWS_CORE_00855]")
{
    auto resultWithValue1 = ara::core::Result<void, ErrorTest>::FromValue();
    CHECK(resultWithValue1.HasValue());

    CHECK(resultWithValue1);

    // just for coverage
    *resultWithValue1;
    resultWithValue1.Value();
}

TEST_CASE("Specialization for value_type=void: Error getters",
          "[SWS_CORE], [SWS_CORE_00857], [SWS_CORE_00858]")
{
    auto resultWithError1 =
      ara::core::Result<void, ErrorTest>::FromError(ErrorTest(10));
    ErrorTest error(
      ara::core::Result<void, ErrorTest>::FromError(ErrorTest(10)).Error());

    CHECK(resultWithError1.Error().getIntVal() == error.getIntVal());
}

TEST_CASE("Specialization for value_type=void: ValueOr method",
          "[SWS_CORE], [SWS_CORE_00861]")
{
    auto resultWithValue = ara::core::Result<void, ErrorTest>::FromValue();
    CHECK(resultWithValue.HasValue());

    // just for coverage
    resultWithValue.ValueOr(5);
}

TEST_CASE("Specialization for value_type=void: ErrorOr method",
          "[SWS_CORE], [SWS_CORE_00863]")
{
    auto resultWithValue = ara::core::Result<void, ErrorTest>::FromValue();
    auto resultWithError =
      ara::core::Result<void, ErrorTest>::FromError(ErrorTest(20));

    auto error1 = resultWithValue.ErrorOr(88);
    auto error2 = resultWithError.ErrorOr(88);

    CHECK(error1.getIntVal() == 88);
    CHECK(error2.getIntVal() == 20);
}

TEST_CASE("Specialization for value_type=void: CheckError method",
          "[SWS_CORE], [SWS_CORE_00865]")
{
    auto resultWithValue = ara::core::Result<void, ErrorTest>::FromValue();
    CHECK(! resultWithValue.CheckError(20));

    auto resultWithError =
      ara::core::Result<void, ErrorTest>::FromError(ErrorTest(20));
    CHECK(resultWithError.CheckError(20));
    CHECK(! resultWithError.CheckError(30));
}

#if __cpp_exceptions == 199711
TEST_CASE("Specialization for value_type=void: ValueOrThrow method",
          "[SWS_CORE], [SWS_CORE_00866]")
{
    auto resultWithValue = ara::core::Result<void, ErrorTest>::FromValue();
    auto resultWithError =
      ara::core::Result<void, ErrorTest>::FromError(ErrorTest(20));

    // just for coverage
    resultWithValue.ValueOrThrow();

    bool exceptionThrown = false;

    try
    {
        resultWithError.ValueOrThrow();
    }
    catch (std::runtime_error&)
    {
        exceptionThrown = true;
    }

    CHECK(exceptionThrown);
}
#endif  // __cpp_exceptions == 199711

TEST_CASE("Specialization for value_type=void: Resolve method",
          "[SWS_CORE], [SWS_CORE_00867]")
{
    auto resultWithValue = ara::core::Result<void, ErrorTest>::FromValue();
    // just for coverage
    resultWithValue.Resolve([](ErrorTest const&) { return; });

    auto resultWithError =
      ara::core::Result<void, ErrorTest>::FromError(ErrorTest(20));
    // just for coverage
    resultWithError.Resolve([](ErrorTest const&) { return; });
}

TEST_CASE("Compare two Result instances for equality",
          "[SWS_CORE], [SWS_CORE_00780]")
{
    CHECK(ara::core::Result<int, ErrorTest>::FromValue(10)
          == ara::core::Result<int, ErrorTest>::FromValue(10));

    CHECK(! (ara::core::Result<int, ErrorTest>::FromValue(10)
             == ara::core::Result<int, ErrorTest>::FromValue(20)));

    CHECK(! (ara::core::Result<int, ErrorTest>::FromError(10)
             == ara::core::Result<int, ErrorTest>::FromValue(10)));
}

TEST_CASE("Compare two Result instances for inequality",
          "[SWS_CORE], [SWS_CORE_00781]")
{
    CHECK(! (ara::core::Result<int, ErrorTest>::FromValue(10)
             != ara::core::Result<int, ErrorTest>::FromValue(10)));

    CHECK(ara::core::Result<int, ErrorTest>::FromValue(10)
          != ara::core::Result<int, ErrorTest>::FromValue(20));

    CHECK(ara::core::Result<int, ErrorTest>::FromError(10)
          != ara::core::Result<int, ErrorTest>::FromValue(10));
}

TEST_CASE(
  "Compare a Result instance for equality to a value",
  "[SWS_CORE], [SWS_CORE_00782], [SWS_CORE_00783], [SWS_CORE_00784], [SWS_CORE_00785]")
{
    CHECK(ara::core::Result<int, ErrorTest>::FromValue(10) == 10);
    CHECK(! (ara::core::Result<int, ErrorTest>::FromError(10) == 10));

    CHECK(10 == ara::core::Result<int, ErrorTest>::FromValue(10));
    CHECK(! (10 == ara::core::Result<int, ErrorTest>::FromError(10)));

    CHECK(ara::core::Result<int, ErrorTest>::FromValue(10) != 15);
    CHECK(15 != ara::core::Result<int, ErrorTest>::FromValue(10));

    CHECK(ara::core::Result<int, ErrorTest>::FromError(10) != 10);
    CHECK(10 != ara::core::Result<int, ErrorTest>::FromError(10));
}

TEST_CASE(
  "Compare a Result instance for equality to an error",
  "[SWS_CORE], [SWS_CORE_00786], [SWS_CORE_00787], [SWS_CORE_00788], [SWS_CORE_00789]")
{
    CHECK(ara::core::Result<int, ErrorTest>::FromError(10) == ErrorTest(10));
    CHECK(
      ! (ara::core::Result<int, ErrorTest>::FromValue(10) == ErrorTest(10)));

    CHECK(ErrorTest(10) == ara::core::Result<int, ErrorTest>::FromError(10));
    CHECK(
      ! (ErrorTest(10) == ara::core::Result<int, ErrorTest>::FromValue(10)));

    CHECK(ara::core::Result<int, ErrorTest>::FromError(10) != ErrorTest(15));
    CHECK(ErrorTest(15) != ara::core::Result<int, ErrorTest>::FromError(10));

    CHECK(ara::core::Result<int, ErrorTest>::FromValue(10) != ErrorTest(10));
    CHECK(ErrorTest(10) != ara::core::Result<int, ErrorTest>::FromValue(10));
}

TEST_CASE("Swap the contents of the two given arguments",
          "[SWS_CORE], [SWS_CORE_00796]")
{
    int         value1        = 11;
    int         value2        = 12;
    int         error1        = 21;
    int         error2        = 22;
    std::string valueMessage1 = "Value Message1";
    std::string valueMessage2 = "Value Message2";
    std::string errorMessage1 = "Error Message1";
    std::string errorMessage2 = "Error Message2";

    auto resultWithValue1 = ara::core::Result<ValueTest, ErrorTest>::FromValue(
      ValueTest(value1, valueMessage1));
    auto resultWithValue2 = ara::core::Result<ValueTest, ErrorTest>::FromValue(
      ValueTest(value2, valueMessage2));
    CHECK(resultWithValue1.HasValue());
    CHECK(resultWithValue2.HasValue());
    CHECK(resultWithValue1.Value().getIntVal() == value1);
    CHECK(resultWithValue2.Value().getIntVal() == value2);
    CHECK(resultWithValue1.Value().getStringVal() == valueMessage1);
    CHECK(resultWithValue2.Value().getStringVal() == valueMessage2);

    auto resultWithError1 = ara::core::Result<ValueTest, ErrorTest>::FromError(
      ErrorTest(error1, errorMessage1));
    auto resultWithError2 = ara::core::Result<ValueTest, ErrorTest>::FromError(
      ErrorTest(error2, errorMessage2));
    CHECK(! resultWithError1.HasValue());
    CHECK(! resultWithError2.HasValue());
    CHECK(resultWithError1.Error().getIntVal() == error1);
    CHECK(resultWithError2.Error().getIntVal() == error2);
    CHECK(resultWithError1.Error().getStringVal() == errorMessage1);
    CHECK(resultWithError2.Error().getStringVal() == errorMessage2);

    // Result1 has a value, Result2 has a value
    swap(resultWithValue1, resultWithValue2);
    CHECK(resultWithValue1.HasValue());
    CHECK(resultWithValue2.HasValue());
    CHECK(resultWithValue1.Value().getIntVal() == value2);
    CHECK(resultWithValue2.Value().getIntVal() == value1);
    CHECK(resultWithValue1.Value().getStringVal() == valueMessage2);
    CHECK(resultWithValue2.Value().getStringVal() == valueMessage1);

    // Result1 doesn't have a value, Result2 doesn't have a value
    swap(resultWithError1, resultWithError2);
    CHECK(! resultWithError1.HasValue());
    CHECK(! resultWithError2.HasValue());
    CHECK(resultWithError1.Error().getIntVal() == error2);
    CHECK(resultWithError2.Error().getIntVal() == error1);
    CHECK(resultWithError1.Error().getStringVal() == errorMessage2);
    CHECK(resultWithError2.Error().getStringVal() == errorMessage1);

    // Result1 doesn't have a value, Result2 has a value
    swap(resultWithError1, resultWithValue1);
    CHECK(resultWithError1.HasValue());
    CHECK(! resultWithValue1.HasValue());
    CHECK(resultWithError1.Value().getIntVal() == value2);
    CHECK(resultWithValue1.Error().getIntVal() == error2);
    CHECK(resultWithError1.Value().getStringVal() == valueMessage2);
    CHECK(resultWithValue1.Error().getStringVal() == errorMessage2);

    // Result1 has a value, Result2 doesn't have a value
    swap(resultWithValue2, resultWithError2);
    CHECK(resultWithError2.HasValue());
    CHECK(! resultWithValue2.HasValue());
    CHECK(resultWithError2.Value().getIntVal() == value1);
    CHECK(resultWithValue2.Error().getIntVal() == error1);
    CHECK(resultWithError2.Value().getStringVal() == valueMessage1);
    CHECK(resultWithValue2.Error().getStringVal() == errorMessage1);
}
