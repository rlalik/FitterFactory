#include <gtest/gtest.h>

#include <hellofitty.hpp>
#include <hellofitty_config.h>

TEST(TestsTools, SourceSelect)
{
    auto true_in = tests_src_path + "test_input.txt";
    auto true_out = tests_src_path + "test_output.txt";

    auto fake_in = tests_src_path + "fake_input.txt";
    auto fake_out = tests_src_path + "fake_output.txt";

    auto newer_one = build_path + "hellofitty_config.h";

    ASSERT_EQ(hf::tools::select_source(fake_in.c_str(), fake_out.c_str()), hf::tools::source::none);

    ASSERT_EQ(hf::tools::select_source(true_in.c_str(), fake_out.c_str()), hf::tools::source::only_reference);

    ASSERT_EQ(hf::tools::select_source(fake_in.c_str(), true_out.c_str()), hf::tools::source::only_auxiliary);

    ASSERT_EQ(hf::tools::select_source(true_in.c_str(), newer_one.c_str()), hf::tools::source::auxiliary);

    ASSERT_EQ(hf::tools::select_source(newer_one.c_str(), true_out.c_str()), hf::tools::source::reference);
}

TEST(TestsTools, FormatDetection)
{
    ASSERT_EQ(hf::tools::detect_format("hist_1 gaus(0) 0  0  1 10  1  2 : 1 3  3 F 2 5"), hf::format_version::v1);

    ASSERT_EQ(hf::tools::detect_format("hist_1 1 10 0 gaus(0) | 1  2 : 1 3  3 F 2 5"), hf::format_version::v2);
}
