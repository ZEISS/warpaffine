// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include <warpafine_unittests_config.h>
#include "../libwarpaffine/cmdlineoptions.h"
#include "../libwarpaffine/document_info.h"
#include "../libwarpaffine/angle_utils.h"

// Helper to create argc/argv from a vector of strings
class ArgvHelper
{
public:
    explicit ArgvHelper(const std::vector<std::string>& args)
    {
        // First copy all strings
        for (const auto& arg : args)
        {
            this->args_.push_back(arg);
        }
        // Then create pointers (after all strings are in place to avoid reallocation)
        for (auto& arg : this->args_)
        {
            this->argv_.push_back(const_cast<char*>(arg.c_str()));
        }
    }

    int argc() const { return static_cast<int>(this->argv_.size()); }
    char** argv() { return this->argv_.data(); }

private:
    std::vector<std::string> args_;
    std::vector<char*> argv_;
};

TEST(CmdLineOptions, IlluminationAngleNotSpecified_ReturnsNullopt)
{
    CCmdLineOptions options;
    ArgvHelper args({ "warpaffine", "-s", "input.czi", "-d", "output.czi" });

    const auto result = options.Parse(args.argc(), args.argv());

    ASSERT_EQ(result, CCmdLineOptions::ParseResult::OK);
    EXPECT_FALSE(options.GetIlluminationAngleOverride().has_value());
}

TEST(CmdLineOptions, IlluminationAngleSpecified_ReturnsValue)
{
    CCmdLineOptions options;
    ArgvHelper args({ "warpaffine", "-s", "input.czi", "-d", "output.czi", "--illumination-angle", "45" });

    const auto result = options.Parse(args.argc(), args.argv());

    ASSERT_EQ(result, CCmdLineOptions::ParseResult::OK);
    ASSERT_TRUE(options.GetIlluminationAngleOverride().has_value());
    EXPECT_DOUBLE_EQ(options.GetIlluminationAngleOverride().value(), 45.0);
}

TEST(CmdLineOptions, IlluminationAngleAt0Degrees_IsValid)
{
    CCmdLineOptions options;
    ArgvHelper args({ "warpaffine", "-s", "input.czi", "-d", "output.czi", "--illumination-angle", "0" });

    const auto result = options.Parse(args.argc(), args.argv());

    ASSERT_EQ(result, CCmdLineOptions::ParseResult::OK);
    ASSERT_TRUE(options.GetIlluminationAngleOverride().has_value());
    EXPECT_DOUBLE_EQ(options.GetIlluminationAngleOverride().value(), 0.0);
}

TEST(CmdLineOptions, IlluminationAngleAt90Degrees_IsValid)
{
    CCmdLineOptions options;
    ArgvHelper args({ "warpaffine", "-s", "input.czi", "-d", "output.czi", "--illumination-angle", "90" });

    const auto result = options.Parse(args.argc(), args.argv());

    ASSERT_EQ(result, CCmdLineOptions::ParseResult::OK);
    ASSERT_TRUE(options.GetIlluminationAngleOverride().has_value());
    EXPECT_DOUBLE_EQ(options.GetIlluminationAngleOverride().value(), 90.0);
}

TEST(CmdLineOptions, IlluminationAngleNegative_IsInvalid)
{
    CCmdLineOptions options;
    ArgvHelper args({ "warpaffine", "-s", "input.czi", "-d", "output.czi", "--illumination-angle", "-1" });

    const auto result = options.Parse(args.argc(), args.argv());

    EXPECT_EQ(result, CCmdLineOptions::ParseResult::Error);
}

TEST(CmdLineOptions, IlluminationAngleAbove90_IsInvalid)
{
    CCmdLineOptions options;
    ArgvHelper args({ "warpaffine", "-s", "input.czi", "-d", "output.czi", "--illumination-angle", "91" });

    const auto result = options.Parse(args.argc(), args.argv());

    EXPECT_EQ(result, CCmdLineOptions::ParseResult::Error);
}

TEST(CmdLineOptions, IlluminationAngleFractional_IsValid)
{
    CCmdLineOptions options;
    ArgvHelper args({ "warpaffine", "-s", "input.czi", "-d", "output.czi", "--illumination-angle", "60.5" });

    const auto result = options.Parse(args.argc(), args.argv());

    ASSERT_EQ(result, CCmdLineOptions::ParseResult::OK);
    ASSERT_TRUE(options.GetIlluminationAngleOverride().has_value());
    EXPECT_DOUBLE_EQ(options.GetIlluminationAngleOverride().value(), 60.5);
}

// Test the DeskewDocumentInfo::SetIlluminationAngleInDegrees function
TEST(DeskewDocumentInfo, SetIlluminationAngleInDegrees_ConvertsCorrectly)
{
    DeskewDocumentInfo doc_info;

    doc_info.SetIlluminationAngleInDegrees(60.0);
    EXPECT_NEAR(doc_info.illumination_angle_in_radians, AngleUtils::DegreesToRadians(60.0), 1e-10);

    doc_info.SetIlluminationAngleInDegrees(45.0);
    EXPECT_NEAR(doc_info.illumination_angle_in_radians, AngleUtils::DegreesToRadians(45.0), 1e-10);

    doc_info.SetIlluminationAngleInDegrees(0.0);
    EXPECT_NEAR(doc_info.illumination_angle_in_radians, 0.0, 1e-10);

    doc_info.SetIlluminationAngleInDegrees(90.0);
    EXPECT_NEAR(doc_info.illumination_angle_in_radians, AngleUtils::DegreesToRadians(90.0), 1e-10);
}

TEST(DeskewDocumentInfo, DefaultIlluminationAngle_Is60Degrees)
{
    DeskewDocumentInfo doc_info;

    // Default should be 60 degrees
    EXPECT_NEAR(doc_info.illumination_angle_in_radians, AngleUtils::DegreesToRadians(60.0), 1e-10);
}

// Test the AngleUtils functions
TEST(AngleUtils, DegreesToRadians_ConvertsCorrectly)
{
    EXPECT_NEAR(AngleUtils::DegreesToRadians(0.0), 0.0, 1e-10);
    EXPECT_NEAR(AngleUtils::DegreesToRadians(90.0), M_PI / 2.0, 1e-10);
    EXPECT_NEAR(AngleUtils::DegreesToRadians(180.0), M_PI, 1e-10);
    EXPECT_NEAR(AngleUtils::DegreesToRadians(360.0), 2.0 * M_PI, 1e-10);
}

TEST(AngleUtils, RadiansToDegrees_ConvertsCorrectly)
{
    EXPECT_NEAR(AngleUtils::RadiansToDegrees(0.0), 0.0, 1e-10);
    EXPECT_NEAR(AngleUtils::RadiansToDegrees(M_PI / 2.0), 90.0, 1e-10);
    EXPECT_NEAR(AngleUtils::RadiansToDegrees(M_PI), 180.0, 1e-10);
    EXPECT_NEAR(AngleUtils::RadiansToDegrees(2.0 * M_PI), 360.0, 1e-10);
}

TEST(AngleUtils, RoundTrip_PreservesValue)
{
    const double original = 57.3;
    const double radians = AngleUtils::DegreesToRadians(original);
    const double back_to_degrees = AngleUtils::RadiansToDegrees(radians);
    EXPECT_NEAR(back_to_degrees, original, 1e-10);
}
