#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TMath.h"
#include "TF1.h"
#include "TAxis.h"
#include "TROOT.h"

#include <iostream>
#include <vector>
#include <string>

struct Sample {
    double energyGeV;
    std::string filename;
};

const double layerWidth_t = 0.96;

double GammaProfile(double* x, double* p)
{
    double layer = x[0];
    double t = layerWidth_t * layer;

    double QE = p[0];
    double alpha = p[1];
    double beta = p[2];
    double Q0 = p[3];

    double value =
        QE *
        beta *
        TMath::Power(beta * t, alpha - 1.0) *
        TMath::Exp(-beta * t) /
        TMath::Gamma(alpha)
        + Q0;

    return value;
}

void Longitudinal_Deposition()
{
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);

    // PLOT FORMATTING

    gStyle->SetCanvasColor(kWhite);
    gStyle->SetPadColor(kWhite);
    gStyle->SetFrameFillColor(kWhite);
    gStyle->SetTitleFillColor(kWhite);

    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);

    std::vector<Sample> samples = {
        {20.0, "../files/20GeV_10cm.root"},
        {60.0, "../files/60GeV_10cm.root"},
        {80.0, "../files/80GeV_10cm.root"},
        {100.0, "../files/100GeV_10cm.root"},
        {120.0, "../files/120GeV_10cm.root"},
        {150.0, "../files/150GeV_10cm.root"},
        {197.0, "../files/197GeV_10cm.root"},
        {243.0, "../files/243GeV_10cm.root"},
        {287.0, "../files/287GeV_10cm.root"}
    };

    const int nFoCalLayers = 20;
    const double halfLayerWidth = 0.5;
    const double MeV_to_pC = 0.0445;

    TCanvas* c = new TCanvas("c", "FoCal-E Longitudinal Charge Profile", 1100, 700);

    c->SetFillColor(kWhite);
    c->SetFrameFillColor(kWhite);
    c->SetBorderMode(0);
    c->SetFrameBorderMode(0);
    c->SetTicks(1, 1);
    c->SetGridx();
    c->SetGridy();

    TMultiGraph* mg = new TMultiGraph();

    // LEGEND FORMATTING
    TLegend* leg = new TLegend(0.68, 0.56, 0.90, 0.88);
    leg->SetFillColor(kWhite);
    leg->SetFillStyle(1001);
    leg->SetBorderSize(1);
    leg->SetTextSize(0.035);

    // CURVES
    
    int colors[] = {
        kBlack,
        kRed,
        kGreen + 2,
        kBlue,
        kOrange + 7,
        kMagenta,
        kCyan + 1,
        kGreen + 3,
        kBlue + 2
    };

    std::vector<TF1*> fits;

    for (int s = 0; s < (int)samples.size(); s++) {

        TFile* f = TFile::Open(samples[s].filename.c_str(), "READ");
        TTree* cell = (TTree*)f->Get("Cell");

        int eventID = 0;
        int layer = 0;
        int row = 0;
        int col = 0;
        double edep = 0.0;

        cell->SetBranchAddress("eventID", &eventID);
        cell->SetBranchAddress("Layer_Marker", &layer);
        cell->SetBranchAddress("row", &row);
        cell->SetBranchAddress("col", &col);
        cell->SetBranchAddress("cellEdep_MeV", &edep);

        Long64_t nEntries = cell->GetEntries();

        int maxEventID = -1;

        for (Long64_t i = 0; i < nEntries; i++) {
            cell->GetEntry(i);

            if (eventID > maxEventID) {
                maxEventID = eventID;
            }
        }

        int nRuns = maxEventID + 1;

        std::cout << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
        std::cout << "File: " << samples[s].filename << std::endl;
        std::cout << "Energy = " << samples[s].energyGeV << " GeV" << std::endl;
        std::cout << "Number of runs/events = " << nRuns << std::endl;
        std::cout << "Cell entries = " << nEntries << std::endl;

        std::vector<std::vector<double>> q(
            nRuns,
            std::vector<double>(nFoCalLayers + 1, 0.0)
        );

        for (Long64_t i = 0; i < nEntries; i++) {
            cell->GetEntry(i);

            if (eventID < 0 || eventID >= nRuns) continue;
            if (layer % 2 == 0) continue;
            
            int focalLayer = (layer + 1) / 2;
            if (focalLayer < 1 || focalLayer > nFoCalLayers) continue;
                double charge_pC = edep * MeV_to_pC;
                q[eventID][focalLayer] += charge_pC;
        }

        double x[nFoCalLayers];
        double y[nFoCalLayers];
        double ex[nFoCalLayers];
        double ey[nFoCalLayers];

        double totalMeanSignal = 0.0;
        double maxMeanSignal = 0.0;
        int maxLayer = 1;

        for (int L = 1; L <= nFoCalLayers; L++) {

            double sumQ = 0.0;
            double sumQ2 = 0.0;

            for (int run = 0; run < nRuns; run++) {
                double Q = q[run][L];

                sumQ += Q;
                sumQ2 += Q * Q;
            }

            double meanQ = sumQ / nRuns;
            double meanQ2 = sumQ2 / nRuns;

            double variance = meanQ2 - meanQ * meanQ;

            if (variance < 0.0) {
                variance = 0.0;
            }

            double sigmaQ =
                TMath::Sqrt(variance) /
                TMath::Sqrt((double)nRuns);

            x[L - 1] = L;
            y[L - 1] = meanQ;
            ex[L - 1] = halfLayerWidth;
            ey[L - 1] = sigmaQ;

            totalMeanSignal += meanQ;

            if (meanQ > maxMeanSignal) {
                maxMeanSignal = meanQ;
                maxLayer = L;
            }

            std::cout << "FoCal-E layer " << L
                      << "  t = " << layerWidth_t * L
                      << "  <Q> = " << meanQ
                      << " pC"
                      << "  sigma_Q = " << sigmaQ
                      << " pC"
                      << std::endl;
        }

        TGraphErrors* gr = new TGraphErrors(
            nFoCalLayers,
            x,
            y,
            ex,
            ey
        );

        int colorIndex = s % 9;

        gr->SetName(Form("gr_%.0fGeV", samples[s].energyGeV));
        gr->SetTitle(Form("%.0f GeV", samples[s].energyGeV));

        gr->SetMarkerStyle(20);
        gr->SetMarkerSize(1.0);
        gr->SetMarkerColor(colors[colorIndex]);
        gr->SetLineColor(colors[colorIndex]);
        gr->SetLineWidth(1);

        // DATA POINTS + ERROR BARS
        mg->Add(gr, "P");

        leg->AddEntry(
            gr,
            Form("%.0f GeV", samples[s].energyGeV),
            "p"
        );

        // FITTING        
        TF1* fit = new TF1(
            Form("gamma_fit_%.0fGeV", samples[s].energyGeV),
            GammaProfile, 1.0, 20.0, 4);

        double tMaxGuess = layerWidth_t * maxLayer;
        double betaGuess = 0.55;
        double alphaGuess = 1.0 + betaGuess * tMaxGuess;

        double QEGuess = totalMeanSignal;
        double Q0Guess = 0.0;

        fit->SetParNames("Q_E", "alpha", "beta", "Q0");

        fit->SetParameters(
            QEGuess,
            alphaGuess,
            betaGuess,
            Q0Guess
        );

        fit->SetParLimits(0, 0.0, 1.0e9);
        fit->SetParLimits(1, 1.01, 50.0);
        fit->SetParLimits(2, 0.01, 10.0);
        fit->SetParLimits(3, -10.0, 10.0);

        fit->SetLineColor(colors[colorIndex]);
        fit->SetLineStyle(2);
        fit->SetLineWidth(2);

        gr->Fit(fit, "RQ");

        fits.push_back(fit);

        // FIT PARAMETER DISPLAYS
        std::cout << std::endl;
        std::cout << "Gamma-profile fit results for "
                  << samples[s].energyGeV
                  << " GeV:" << std::endl;

        std::cout << "  Q_E   = "
                  << fit->GetParameter(0)
                  << " +/- "
                  << fit->GetParError(0)
                  << std::endl;

        std::cout << "  alpha = "
                  << fit->GetParameter(1)
                  << " +/- "
                  << fit->GetParError(1)
                  << std::endl;

        std::cout << "  beta  = "
                  << fit->GetParameter(2)
                  << " +/- "
                  << fit->GetParError(2)
                  << std::endl;

        std::cout << "  Q0    = "
                  << fit->GetParameter(3)
                  << " +/- "
                  << fit->GetParError(3)
                  << std::endl;

        std::cout << "  chi2/ndf = "
                  << fit->GetChisquare()
                  << " / "
                  << fit->GetNDF()
                  << std::endl;

        f->Close();
    }

    // COSMETICS (AXES, TITLES)
    mg->SetTitle("FoCal-E Longitudinal Charge Profile;FoCal-E Layer (~0.96 X/X_{0} = t);FoCal-E layer signal (pC)");

    mg->Draw("A");

    mg->GetXaxis()->SetLimits(0.0, 21.0);
    mg->GetYaxis()->SetRangeUser(0.0, 20.0);

    mg->GetXaxis()->SetTitle("FoCal-E Layer (~0.96 X/X_{0} = t)");
    mg->GetYaxis()->SetTitle("FoCal-E layer signal (pC)");



    // DRAWING FITTED GAMMA PROFILES 
    for (int i = 0; i < (int)fits.size(); i++) {
        fits[i]->Draw("same");
    }

    c->SetTicks(1, 1);
    c->Modified();
    c->Update();

    leg->AddEntry((TObject*)0, "--- #Gamma fit", "");
    leg->Draw();

    c->Modified();
    c->Update();
}