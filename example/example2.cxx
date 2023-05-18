#include <fitemall.hpp>

#include "fitemall_config.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

#include <TFile.h>
#include <TH1.h>
#include <TKey.h>
#include <TMath.h>
#include <TROOT.h>

#ifdef HAS_ROOTTOOLS
#include <RootTools.h>
#endif

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << " Usage: " << argv[0] << " file.root file_with_params" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // load custom functions definitions
#ifdef HAS_ROOTTOOLS
    RootTools::MyMath();
#endif

    // create params output filename
    size_t pflen = strlen(argv[2]);
    char* opf = new char[pflen + 5];
    sprintf(opf, "%s%s", argv[2], ".out");

    // create fitting factory
    fea::fitter ff;
    ff.init_fitter_from_file(argv[2], opf);
    delete[] opf;

    // uncomment this to print all entries
    // ff.print();

    // open root file
    TFile* file = TFile::Open(argv[1], "READ");

    TFile* ofile = nullptr;
    if (argc >= 4)
    {
        ofile = TFile::Open(argv[3], "RECREATE");
        ofile->cd();
    }

    // do fitting for each TH1 object found
    TKey* key = nullptr;
    TIter nextkey(file->GetListOfKeys());
    while ((key = (TKey*)nextkey()))
    {
        const char* classname = key->GetClassName();
        TClass* cl = gROOT->GetClass(classname);
        if (!cl) continue;

        TObject* obj = key->ReadObj();
        printf("*** %s\n", obj->GetName());
        if (obj->InheritsFrom("TH1"))
        {
            TH1* h = (TH1*)obj->Clone(obj->GetName());
            ff.fit(h);
            if (ofile) h->Write();
        }
    }

    ff.export_fitter_to_file();

    file->Close();

    if (ofile)
    {
        ofile->Write();
        ofile->Close();
    }

    return 0;
}
