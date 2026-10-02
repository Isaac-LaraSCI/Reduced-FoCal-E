#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TString.h"
#include "TPad.h"

// Convention:
// Layer_Marker = 9  -> ACTUAL LAYER 10 -> HG Layer 1
// Layer_Marker = 19 -> ACTUAL LAYER 20 -> HG Layer 2

TH2D* makeHeatMap(const char* filename,
                  const char* histName,
                  int targetLayerMarker,
                  double zMax)
{
    TFile* file = TFile::Open(filename, "READ");
    TTree* cellTree = nullptr;
    file->GetObject("Cell", cellTree);

    int eventID = 0;
    int layerMarker = 0;
    int row = 0;
    int col = 0;
    double cellEdep_MeV = 0.0;

    cellTree->SetBranchAddress("eventID", &eventID);
    cellTree->SetBranchAddress("Layer_Marker", &layerMarker);
    cellTree->SetBranchAddress("row", &row);
    cellTree->SetBranchAddress("col", &col);
    cellTree->SetBranchAddress("cellEdep_MeV", &cellEdep_MeV);

    // AXIS RANGE IS DISPLAYED FROM 0 TO 200
    TH2D* h2 = new TH2D(histName,"",200, 0.0, 200.0,200, 0.0, 200.0);

    h2->SetDirectory(nullptr);

    Long64_t nEntries = cellTree->GetEntries();
    Long64_t selectedEntries = 0;

    for (Long64_t i = 0; i < nEntries; i++) {
        cellTree->GetEntry(i);

        if (layerMarker != targetLayerMarker) continue;
        if (row < 0 || row > 199) continue;
        if (col < 0 || col > 199) continue;

        h2->Fill(col, row, cellEdep_MeV);
        selectedEntries++;
    }

    std::cout << "File: " << filename << std::endl;
    std::cout << "Layer_Marker: " << targetLayerMarker << std::endl;
    std::cout << "Physical layer: " << targetLayerMarker + 1 << std::endl;
    std::cout << "Selected entries: " << selectedEntries << std::endl;

    h2->GetXaxis()->SetTitle("Column");
    h2->GetYaxis()->SetTitle("Row");
    h2->GetZaxis()->SetTitle("Deposited energy (MeV)");

    h2->GetXaxis()->CenterTitle();
    h2->GetYaxis()->CenterTitle();
    h2->GetZaxis()->CenterTitle();

    h2->GetXaxis()->SetTitleSize(0.045);
    h2->GetYaxis()->SetTitleSize(0.045);
    h2->GetZaxis()->SetTitleSize(0.040);

    h2->GetXaxis()->SetLabelSize(0.035);
    h2->GetYaxis()->SetLabelSize(0.035);
    h2->GetZaxis()->SetLabelSize(0.035);

    h2->GetXaxis()->SetTitleOffset(0.95);
    h2->GetYaxis()->SetTitleOffset(1.10);
    h2->GetZaxis()->SetTitleOffset(1.05);

    h2->SetMinimum(0.0);
    h2->SetMaximum(zMax);

    file->Close();

    return h2;
}

void drawOnePad(TH2D* h, const char* title)
{
    gPad->SetRightMargin(0.16);
    gPad->SetLeftMargin(0.12);
    gPad->SetBottomMargin(0.12);
    gPad->SetTopMargin(0.10);

    h->SetTitle(title);
    h->Draw("COLZ");
}

void Transverse_Deposition()
{
    gStyle->SetOptStat(0);
    gStyle->SetPalette(kViridis);

    const int layerMarker1 = 9;
    const int layerMarker2 = 19;

    const double zMax = 260.0;

    const char* file20  = "../files/20GeV_10cm.root";
    const char* file150 = "../files/150GeV_10cm.root";
    const char* file287 = "../files/287GeV_10cm.root";

    TH2D* h20_L1 = makeHeatMap(file20,"h20_layer9",layerMarker1,zMax);
    TH2D* h20_L2 = makeHeatMap(file20,"h20_layer19",layerMarker2,zMax);
    
    TH2D* h150_L1 = makeHeatMap(file150,"h150_layer9",layerMarker1,zMax);
    TH2D* h150_L2 = makeHeatMap(file150,"h150_layer19",layerMarker2,zMax);

    TH2D* h287_L1 = makeHeatMap(file287,"h287_layer9",layerMarker1,zMax);
    TH2D* h287_L2 = makeHeatMap(file287,"h287_layer19",layerMarker2,zMax);

    TCanvas* c = new TCanvas("c_six_energy_layer_heatmaps","High-Grain Layer Heat Maps",1100,1500);

    // 2 columns [LAYER], 3 rows [ENERGY]:
    c->Divide(2, 3, 0.01, 0.01);

    // ROW 1 [20 GeV]
    c->cd(1);
    drawOnePad(h20_L1, "20 GeV, High-Grain Layer 1");
    c->cd(2);
    drawOnePad(h20_L2, "20 GeV, High-Grain Layer 2");

    // ROW 2 [150 GeV]
    c->cd(3);
    drawOnePad(h150_L1, "150 GeV, High-Grain Layer 1");
    c->cd(4);
    drawOnePad(h150_L2, "150 GeV, High-Grain Layer 2");

    // ROW 3 [287 GeV]
    c->cd(5);
    drawOnePad(h287_L1, "287 GeV, High-Grain Layer 1");
    c->cd(6);
    drawOnePad(h287_L2, "287 GeV, High-Grain Layer 2");

    c->Update();
    // c->SaveAs("HighGrainLayers_3x2_EnergyRows_LayerColumns_heatmaps.pdf");
}