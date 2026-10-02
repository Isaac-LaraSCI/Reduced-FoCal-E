#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TPad.h"

#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <utility>

struct Sample {
    double energy;
    const char* file;
};

struct Hit {
    double x;
    double y;
    double e;
};

struct Result {
    double fwhm;
    double err;
};

double crossing(TH1D* h, int b1, int b2, double y)
{
    double x1 = h->GetBinCenter(b1);
    double x2 = h->GetBinCenter(b2);
    double y1 = h->GetBinContent(b1);
    double y2 = h->GetBinContent(b2);

    if (std::abs(y2 - y1) < 1.0e-12) return 0.5 * (x1 + x2);

    return x1 + (y - y1) * (x2 - x1) / (y2 - y1);
}

double getFWHM(TH1D* h)
{
    double maxVal = h->GetMaximum();
    double halfMax = 0.5 * maxVal;

    int maxBin = h->GetMaximumBin();
    int nBins = h->GetNbinsX();

    int leftBelow = -1;
    int leftAbove = -1;
    int rightAbove = -1;
    int rightBelow = -1;

    for (int i = maxBin; i >= 2; --i) {
        if (h->GetBinContent(i) >= halfMax &&
            h->GetBinContent(i - 1) < halfMax) {
            leftBelow = i - 1;
            leftAbove = i;
            break;
        }
    }

    for (int i = maxBin; i <= nBins - 1; ++i) {
        if (h->GetBinContent(i) >= halfMax &&
            h->GetBinContent(i + 1) < halfMax) {
            rightAbove = i;
            rightBelow = i + 1;
            break;
        }
    }

    double xLeft = crossing(h, leftBelow, leftAbove, halfMax);
    double xRight = crossing(h, rightBelow, rightAbove, halfMax);

    return xRight - xLeft;
}

bool getCenter(const std::vector<Hit>& hits, double& x0, double& y0)
{
    const double macroPitch = 1.1;
    const double radius = 11.0;

    std::map<std::pair<int, int>, double> macroE;

    for (const auto& h : hits) {
        int ix = static_cast<int>(std::floor(h.x / macroPitch));
        int iy = static_cast<int>(std::floor(h.y / macroPitch));
        macroE[{ix, iy}] += h.e;
    }

    auto maxCell = macroE.begin()->first;
    double maxE = macroE.begin()->second;

    for (const auto& item : macroE) {
        if (item.second > maxE) {
            maxE = item.second;
            maxCell = item.first;
        }
    }

    double xMax = (maxCell.first + 0.5) * macroPitch;
    double yMax = (maxCell.second + 0.5) * macroPitch;

    double sumE = 0.0;
    double sumX = 0.0;
    double sumY = 0.0;

    for (const auto& item : macroE) {
        double x = (item.first.first + 0.5) * macroPitch;
        double y = (item.first.second + 0.5) * macroPitch;
        double e = item.second;

        double dx = x - xMax;
        double dy = y - yMax;

        if (std::sqrt(dx * dx + dy * dy) > radius) continue;

        sumE += e;
        sumX += e * x;
        sumY += e * y;
    }

    x0 = sumX / sumE;
    y0 = sumY / sumE;

    return true;
}

Result analyzeLayer(const char* filename, int targetLayer, int index)
{
    const double pitch = 0.1;
    const double ySliceHalfWidth = 0.5;
    const double profileMin = -10.0;
    const double profileMax = 10.0;
    const double binWidth = 0.2;
    const int nBins = static_cast<int>((profileMax - profileMin) / binWidth);

    TFile* f = TFile::Open(filename);
    TTree* tree = (TTree*)f->Get("Cell");

    int eventID;
    int layerMarker;
    int row;
    int col;
    double edep;

    tree->SetBranchAddress("eventID", &eventID);
    tree->SetBranchAddress("Layer_Marker", &layerMarker);
    tree->SetBranchAddress("row", &row);
    tree->SetBranchAddress("col", &col);
    tree->SetBranchAddress("cellEdep_MeV", &edep);

    std::map<int, std::vector<Hit>> events;

    Long64_t nEntries = tree->GetEntries();

    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);

        if (layerMarker != targetLayer) continue;
        if (edep <= 0.0) continue;

        double x = (col + 0.5) * pitch;
        double y = (row + 0.5) * pitch;

        events[eventID].push_back({x, y, edep});
    }

    TH1D* h = new TH1D(
        Form("h_%d_%d", index, targetLayer),
        "",
        nBins,
        profileMin,
        profileMax
    );

    h->SetDirectory(nullptr);

    for (const auto& ev : events) {
        double x0;
        double y0;

        getCenter(ev.second, x0, y0);

        for (const auto& hcell : ev.second) {
            if (std::abs(hcell.y - y0) < ySliceHalfWidth) {
                h->Fill(hcell.x - x0, hcell.e);
            }
        }
    }

    double integral = h->Integral("width");

    if (integral > 0.0) {
        h->Scale(1.0 / integral);
    }

    Result r;
    r.fwhm = getFWHM(h);
    r.err = binWidth;

    f->Close();

    return r;
}

void styleGraph(TGraphErrors* gr, int marker, int color, const char* title)
{
    gr->SetTitle(title);
    gr->SetMarkerStyle(marker);
    gr->SetMarkerSize(1.0);
    gr->SetMarkerColor(color);
    gr->SetLineColor(color);
    gr->SetLineWidth(1);

    gr->GetXaxis()->SetLimits(0.0, 300.0);
    gr->GetXaxis()->CenterTitle();
    gr->GetYaxis()->CenterTitle();

    gr->GetXaxis()->SetTitleSize(0.045);
    gr->GetYaxis()->SetTitleSize(0.045);
    gr->GetXaxis()->SetLabelSize(0.040);
    gr->GetYaxis()->SetLabelSize(0.040);

    gr->GetXaxis()->SetTitleOffset(1.05);
    gr->GetYaxis()->SetTitleOffset(1.25);
}

void root_file_extraction()
{
    gStyle->SetOptStat(0);
    gStyle->SetEndErrorSize(4);
    gStyle->SetTitleFontSize(0.045);

    const int layer5Marker = 9;
    const int layer10Marker = 19;

    std::vector<Sample> samples = {
        {20.0,  "../files/20GeV_10cm.root"},
        {60.0,  "../files/60GeV_10cm.root"},
        {80.0,  "../files/80GeV_10cm.root"},
        {100.0, "../files/100GeV_10cm.root"},
        {120.0, "../files/120GeV_10cm.root"},
        {150.0, "../files/150GeV_10cm.root"},
        {197.0, "../files/197GeV_10cm.root"},
        {243.0, "../files/243GeV_10cm.root"},
        {287.0, "../files/287GeV_10cm.root"}
    };

    std::vector<double> x5, y5, ex5, ey5;
    std::vector<double> x10, y10, ex10, ey10;

    for (size_t i = 0; i < samples.size(); ++i) {
        Result r5 = analyzeLayer(samples[i].file, layer5Marker, i);
        Result r10 = analyzeLayer(samples[i].file, layer10Marker, i + 1000);

        x5.push_back(samples[i].energy);
        y5.push_back(r5.fwhm);
        ex5.push_back(0.0);
        ey5.push_back(r5.err);

        x10.push_back(samples[i].energy);
        y10.push_back(r10.fwhm);
        ex10.push_back(0.0);
        ey10.push_back(r10.err);
    }

    double yMax = 0.0;

    for (double v : y5) {
        if (v > yMax) yMax = v;
    }

    for (double v : y10) {
        if (v > yMax) yMax = v;
    }

    yMax *= 1.20;

    if (yMax < 2.5) {
        yMax = 2.5;
    }

    TGraphErrors* gr5 = new TGraphErrors(
        x5.size(),
        x5.data(),
        y5.data(),
        ex5.data(),
        ey5.data()
    );

    TGraphErrors* gr10 = new TGraphErrors(
        x10.size(),
        x10.data(),
        y10.data(),
        ex10.data(),
        ey10.data()
    );

    TCanvas* c = new TCanvas(
        "c_shower_width_fwhm",
        "Shower Width FWHM",
        1600,
        650
    );

    c->Divide(2, 1);

    c->cd(1);
    gPad->SetGrid();
    gPad->SetTickx(1);
    gPad->SetTicky(1);
    gPad->SetLeftMargin(0.14);
    gPad->SetRightMargin(0.05);
    gPad->SetBottomMargin(0.13);
    gPad->SetTopMargin(0.10);

    styleGraph(
        gr5,
        20,
        kBlue + 1,
        "FoCal-E Pixel Layer 5;Electron energy (GeV);Shower Width FWHM (mm)"
    );

    gr5->Draw("APL");
    gr5->GetYaxis()->SetRangeUser(0.0, yMax);

    TLegend* leg5 = new TLegend(0.56, 0.72, 0.88, 0.84);
    leg5->SetBorderSize(0);
    leg5->SetFillColor(kWhite);
    leg5->SetFillStyle(1001);
    leg5->SetTextSize(0.032);
    leg5->AddEntry(gr5, "Simulation", "lp");
    leg5->Draw();

    c->cd(2);
    gPad->SetGrid();
    gPad->SetTickx(1);
    gPad->SetTicky(1);
    gPad->SetLeftMargin(0.14);
    gPad->SetRightMargin(0.05);
    gPad->SetBottomMargin(0.13);
    gPad->SetTopMargin(0.10);

    styleGraph(
        gr10,
        25,
        kRed + 1,
        "FoCal-E Pixel Layer 10;Electron energy (GeV);Shower Width FWHM (mm)"
    );

    gr10->Draw("APL");
    gr10->GetYaxis()->SetRangeUser(0.0, yMax);

    TLegend* leg10 = new TLegend(0.56, 0.72, 0.88, 0.84);
    leg10->SetBorderSize(0);
    leg10->SetFillColor(kWhite);
    leg10->SetFillStyle(1001);
    leg10->SetTextSize(0.032);
    leg10->AddEntry(gr10, "Simulation", "lp");
    leg10->Draw();

    c->Update();
    c->SaveAs("shower_width_fwhm_by_high_granularity_layer.pdf");
}