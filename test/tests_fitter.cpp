#include <gtest/gtest.h>

#include "hellofitty.hpp"

#include "details.hpp"

#include <TH1.h>

#include <memory>
#include <string>
#include <utility>

TEST(TestsFitter, PrefixSuffixTest)
{
    const std::string pat1 = "pref1_*";
    const std::string pat2 = "p_*_suff1";

    const std::string tn1 = "test_name";
    const std::string tn2 = "replaced";

    ASSERT_STREQ("pref1_test_name", hf::tools::format_name(tn1, pat1));
    ASSERT_STREQ("pref1_replaced", hf::tools::format_name(tn2, pat1));

    ASSERT_STREQ("p_test_name_suff1", hf::tools::format_name(tn1, pat2));
    ASSERT_STREQ("p_replaced_suff1", hf::tools::format_name(tn2, pat2));
}

TEST(TestsFitter, InsertParameters)
{
    hf::fitter fitter;

    const auto fit1 = fitter.find_fit("name1");
    ASSERT_EQ(fit1, nullptr);

    auto hf1 = hf::fit_entry(0, 1);
    hf1.add_function("1");
    hf1.add_function("0");
    fitter.insert_parameter("name1", hf1);

    const auto fit2 = fitter.find_fit("name1");
    ASSERT_NE(fit2, nullptr);

    fitter.clear();
}

TEST(TestsFitter, FitFinding)
{
    hf::fitter fitter;

    TH1I* h_foo = new TH1I("h_foo", "foo", 10, 0, 10);
    const auto fit1 = fitter.find_fit(h_foo);
    ASSERT_EQ(fit1, nullptr);

    hf::fit_entry hfp_defaults(1, 10);
    ASSERT_EQ(hfp_defaults.add_function("gaus(0)"), 0);
    fitter.set_generic_entry(hfp_defaults);

    const auto fit2 = fitter.find_fit(h_foo);
    ASSERT_EQ(fit2, nullptr);

    fitter.clear();
    delete h_foo;
}

TEST(TestsFitter, Fitting)
{
    hf::fitter fitter;

    TH1I* h_foo = new TH1I("h_foo", "foo", 10, 0, 10);
    const auto fit1 = fitter.fit(h_foo, "", "");
    ASSERT_EQ(fit1.first, false);

    hf::fit_entry hfp_defaults(1, 10);
    ASSERT_EQ(hfp_defaults.add_function("gaus(0)"), 0);
    fitter.set_generic_entry(hfp_defaults);

    const auto fit2 = fitter.fit(h_foo, "", "");
    ASSERT_EQ(fit2.first, false);

    fitter.clear();
    delete h_foo;
}

TEST(TestsFitter, NameDecorator)
{
    hf::fitter fitter;

    auto hfp_foo = hf::fit_entry(1, 10);
    ASSERT_EQ(hfp_foo.add_function("gaus(0)"), 0);

    fitter.insert_parameter("h_foo", hfp_foo);

    fitter.set_name_decorator("*_foo");

    TH1I* h_foo = new TH1I("h", "foo", 10, 0, 10);

    const auto entry1 = fitter.find_fit(h_foo);
    ASSERT_NE(entry1, nullptr);

    fitter.clear();
    delete h_foo;
}

TEST(TestsFitter, FunctionDecorator)
{
    hf::fitter fitter;

    fitter.set_function_decorator("*_test");

    fitter.clear();
}
