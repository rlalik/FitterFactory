#include "parser.hpp"

#include "hellofitty.hpp"

#include <RtypesCore.h>
#include <TF1.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TString.h>

#include <memory>

namespace hf::parser
{
auto parse_line_entry_v1(const TString& line) -> std::unique_ptr<fit_entry>
{
    TString line_ = line;
    line_.ReplaceAll("\t", " ");
    auto arr = std::unique_ptr<TObjArray>(line_.Tokenize(" "));

    if (arr->GetEntries() < 6)
    {
        // std::cerr << "Error parsing line:\n " << line << "\n";
        return nullptr;
    };

    auto hfp = make_unique<fit_entry>(dynamic_cast<TObjString*>(arr->At(0))->String(),        // hist name
                                      dynamic_cast<TObjString*>(arr->At(1))->String(),        // func val
                                      dynamic_cast<TObjString*>(arr->At(2))->String(),        // func val
                                      dynamic_cast<TObjString*>(arr->At(4))->String().Atof(), // low range
                                      dynamic_cast<TObjString*>(arr->At(5))->String().Atof());

    auto npars = hfp->get_sum_func().GetNpar();

    Int_t step = 0;
    Int_t parnum = 0;

    auto entries = arr->GetEntries();
    for (int i = 6; i < entries; i += step, ++parnum)
    {
        Double_t l_, u_;
        param::fit_mode flag_;
        bool has_limits_ = false;

        if (parnum >= npars) { return nullptr; }

        const TString val = dynamic_cast<TObjString*>(arr->At(i))->String();
        const TString nval =
            ((i + 1) < arr->GetEntries()) ? dynamic_cast<TObjString*>(arr->At(i + 1))->String() : TString();

        auto par_ = val.Atof();
        if (nval == ":")
        {
            l_ = (i + 2) < arr->GetEntries() ? dynamic_cast<TObjString*>(arr->At(i + 2))->String().Atof() : 0;
            u_ = (i + 2) < arr->GetEntries() ? dynamic_cast<TObjString*>(arr->At(i + 3))->String().Atof() : 0;
            step = 4;
            flag_ = param::fit_mode::free;
            has_limits_ = true;
        }
        else if (nval == "F")
        {
            l_ = (i + 2) < arr->GetEntries() ? dynamic_cast<TObjString*>(arr->At(i + 2))->String().Atof() : 0;
            u_ = (i + 2) < arr->GetEntries() ? dynamic_cast<TObjString*>(arr->At(i + 3))->String().Atof() : 0;
            step = 4;
            flag_ = param::fit_mode::fixed;
            has_limits_ = true;
        }
        else if (nval == "f")
        {
            l_ = 0;
            u_ = 0;
            step = 2;
            flag_ = param::fit_mode::fixed;
            has_limits_ = false;
        }
        else
        {
            l_ = 0;
            u_ = 0;
            step = 1;
            flag_ = param::fit_mode::free;
            has_limits_ = false;
        }

        if (has_limits_) { hfp->set_param(parnum, par_, l_, u_, flag_); }
        else { hfp->set_param(parnum, par_, flag_); }
    }

    return hfp;
}

auto format_line_entry_v1(const hf::fit_entry* hist_fit) -> TString
{
    auto out = TString::Format("%c%s\t%s %s %d %.0f %.0f", hist_fit->get_flag_disabled() ? '@' : ' ',
                               hist_fit->get_name().Data(), hist_fit->get_sig_string().Data(),
                               hist_fit->get_bkg_string().Data(), hist_fit->get_flag_rebin(),
                               hist_fit->get_fit_range_min(), hist_fit->get_fit_range_max());
    auto limit = hist_fit->get_params_number();

    for (decltype(limit) i = 0; i < limit; ++i)
    {
        const TString val = TString::Format("%g", hist_fit->get_param(i).value);
        const TString min = TString::Format("%g", hist_fit->get_param(i).min);
        const TString max = TString::Format("%g", hist_fit->get_param(i).max);

        char sep{0};

        switch (hist_fit->get_param(i).mode)
        {
            case param::fit_mode::free:
                if (hist_fit->get_param(i).has_limits) { sep = ':'; }
                else { sep = ' '; }
                break;
            case param::fit_mode::fixed:
                if (hist_fit->get_param(i).has_limits) { sep = 'F'; }
                else { sep = 'f'; }
                break;
        }

        if (hist_fit->get_param(i).mode == param::fit_mode::free and hist_fit->get_param(i).has_limits == false)
        {
            out += TString::Format(" %s", val.Data());
        }
        else if (hist_fit->get_param(i).mode == param::fit_mode::fixed and hist_fit->get_param(i).has_limits == false)
        {
            out += TString::Format(" %s %c", val.Data(), sep);
        }
        else { out += TString::Format(" %s %c %s %s", val.Data(), sep, min.Data(), max.Data()); }
    }

    return out;
}

} // namespace hf::parser
