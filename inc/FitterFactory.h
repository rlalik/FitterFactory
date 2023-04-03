/*
    FitterFactory - mass fitting tool for CERN's ROOT applications
    Copyright (C) 2015-2021  Rafał Lalik <rafallalik@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FITTERFACTORY_H
#define FITTERFACTORY_H

#include <map>
#include <memory>
#include <optional>

#include <TF1.h>
#include <TFormula.h>
#include <TString.h>

#if __cplusplus < 201402L
#define CONSTEXPRCXX14
#else
#define CONSTEXPRCXX14 constexpr
#endif

class TH1;

struct FitterFactoryImpl;

namespace FitterFactoryTools
{
enum class SelectedSource
{
    None,
    OnlyReference,
    OnlyAuxilary,
    Reference,
    Auxilary
};

auto selectSource(const char* filename, const char* auxname = 0)
    -> FitterFactoryTools::SelectedSource;

struct DrawProperties
{
#if __cplusplus >= 201703L
    std::optional<Int_t> line_color;
    std::optional<Int_t> line_width;
    std::optional<Int_t> line_style;
#else
    Int_t line_color{-1};
    Int_t line_width{-1};
    Int_t line_style{-1};
#endif

    DrawProperties& setLineColor(Int_t color)
    {
        line_color = color;
        return *this;
    }
    DrawProperties& setLineWidth(Int_t width)
    {
        line_width = width;
        return *this;
    }
    DrawProperties& setLineStyle(Int_t style)
    {
        line_style = style;
        return *this;
    }

    void applyStyle(TF1* f);
};

}; // namespace FitterFactoryTools

struct ParamValue
{
    double val{0}; // value
    double l{0};   // lower limit
    double u{0};   // upper limit
    enum class FitMode
    {
        Free,
        Fixed
    } mode{FitMode::Free};
    bool has_limits{false};

    constexpr ParamValue() = default;
    constexpr ParamValue(double val, ParamValue::FitMode mode) : val(val), mode(mode) {}
    constexpr ParamValue(double val, double l, double u, ParamValue::FitMode mode)
        : val(val), l(l), u(u), mode(mode), has_limits(true)
    {
    }
    void print() const;
};

using ParamVector = std::vector<ParamValue>;

class HistogramFit
{
public:
    TString hist_name;  // histogram name
    TString sig_string; // signal and background functions
    TString bkg_string;

    Double_t range_l; // function range
    Double_t range_u; // function range

    int rebin{0}; // rebin, 0 == no rebin
    bool fit_disabled{false};

    ParamVector pars;
    TF1 function_sig;
    TF1 function_bkg;
    TF1 function_sum;

    constexpr HistogramFit() = delete;
    HistogramFit(const TString& hist_name, const TString& formula_s, const TString& formula_b,
                 Double_t range_lower, Double_t range_upper);
    HistogramFit(HistogramFit&& other) = default;
    HistogramFit& operator=(HistogramFit&& other) = default;

    ~HistogramFit() = default;

    void init();
    auto clone(const TString& new_name) const -> std::unique_ptr<HistogramFit>;
    void setParam(Int_t par, ParamValue value);
    void setParam(Int_t par, Double_t val, ParamValue::FitMode mode);
    void setParam(Int_t par, Double_t val, Double_t l, Double_t u, ParamValue::FitMode mode);
    void print(bool detailed = false) const;
    bool load(TF1* f);

    void clear();

    TString exportEntry() const;

    static std::unique_ptr<HistogramFit> parseLineEntry(const TString& line);

    void push();
    void pop();
    void apply();
    void drop();

private:
    HistogramFit(const HistogramFit& hfp) = delete;
    HistogramFit& operator=(const HistogramFit& hfp) = delete;

private:
    std::vector<Double_t> backup_p; // backup for parameters
};

class FitterFactory
{
public:
    enum class PriorityMode
    {
        Reference,
        Auxilary,
        Newer
    };

    FitterFactory(PriorityMode mode = PriorityMode::Newer);
    virtual ~FitterFactory();

    void clear();

    void setFlags(PriorityMode new_mode);
    void setDefaultParameters(HistogramFit* defs);

    bool initFactoryFromFile(const char* filename, const char* auxname = 0);
    bool exportFactoryToFile();

    HistogramFit* findFit(TH1* hist) const;
    HistogramFit* findFit(const char* name) const;

    auto fit(TH1* hist, const char* pars = "B,Q", const char* gpars = "") -> bool;
    auto fit(HistogramFit* hfp, TH1* hist, const char* pars = "B,Q", const char* gpars = "")
        -> bool;

    void print() const;

    static auto setVerbose(bool verbose) -> void;

    auto setReplacement(const TString& src, const TString& dst) -> void;
    auto format_name(const TString& name, const TString& decorator) const -> TString;

    void insertParameters(std::unique_ptr<HistogramFit>&& hfp);
    void insertParameters(const TString& name, std::unique_ptr<HistogramFit>&& hfp);

    void setNameDecorator(const TString& decorator);
    void clearNameDecorator();

    void setFunctionDecorator(const TString& decorator);

    void setDrawBits(bool sum = true, bool sig = false, bool bkg = false);

    auto propSum() -> FitterFactoryTools::DrawProperties&;
    auto propSig() -> FitterFactoryTools::DrawProperties&;
    auto propBkg() -> FitterFactoryTools::DrawProperties&;

private:
    bool importParameters(const std::string& filename);
    bool exportParameters(const std::string& filename);
    std::unique_ptr<FitterFactoryImpl> d;
};

#endif // FITTERFACTORY_H
