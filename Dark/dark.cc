#include "dark.h"

//------------------------------
void Noise(){
  
  InitNoise();
  
  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    cookedTree->GetEntry(iEntry);

    hMean_Cooked->Fill(mean_mV);
    hPPV_Cooked->Fill(peak_mV-min_mV);
    hPeak_Cooked->Fill(peak_mV);
    hMin_Cooked->Fill(min_mV);
    hMin_Peak_Cooked->Fill(min_mV,peak_mV);
    
   }
  
  // find peak of mean voltage in mV
  int     max_bin_mean = hMean_Cooked->GetMaximumBin();
  TAxis * x_axis       = hMean_Cooked->GetXaxis();
  float   peak_mean_mV = x_axis->GetBinCenter(max_bin_mean);
  
  thresh_mV       = 10.0; // ideally 1/4 of 1 p.e.
  th_low_mV       = 5.0;  // 
  
  noise_thresh_mV = peak_mean_mV - thresh_mV;
  noise_th_low_mV = peak_mean_mV - th_low_mV;
  
  // standard threshold rel mean peak
  int thresh_bin   = hMin_Cooked->FindBin(noise_thresh_mV);
  int noise_counts = hMin_Cooked->Integral(0,thresh_bin);
  
  float noise_rate = (float)noise_counts/nentries;
  noise_rate = noise_rate/Length_ns * 1.0e9;

  // low threshold rel mean peak
  thresh_bin   = hMin_Cooked->FindBin(noise_th_low_mV);
  noise_counts = hMin_Cooked->Integral(0,thresh_bin);
  
  float noise_rate_low = (float)noise_counts/nentries;
  noise_rate_low = noise_rate_low/Length_ns * 1.0e9;
  
  printf("\n Mean voltage %.2f mV \n",peak_mean_mV);
  printf("\n Rate @ %.2f mV  %.2f Hz \n",noise_th_low_mV,noise_rate_low);
  printf("\n Rate @ %.2f mV  %.2f Hz \n",noise_thresh_mV,noise_rate);

  SaveNoise();

}

void InitNoise(){
  
  printf("\n ------------------------------ \n");
  printf("\n Analysing Noise   \n");
  
  float range = (float)roundf(Range_V)*1000.;
  
  // for amp not 10x, scale bin width 
  float binWidth =  Wave_To_Amp_Scaled_Wave(mVPerBin);
  float minX     = -range/2.;
  float maxX     =  range/2.;
  int   nBins    = 0;
  
  // fix binning and set number of bins
  Set_THF_Params(&minX,&maxX,&binWidth,&nBins);
  
  hMean_Cooked = new TH1F("hMean_Cooked",
			  ";mean voltage (mV);Counts",
			  nBins,minX,maxX);

  hPeak_Cooked =  new TH1F("hPeak_Cooked",
			   ";peak voltage (mV);Counts",
			   nBins,minX,maxX);
  
  hMin_Cooked =  new TH1F("hMin_Cooked",
			  ";min voltage (mV);Counts",
			  nBins,minX,maxX);

  hMin_Peak_Cooked =  new TH2F("hMin_Peak_Cooked",
			       "peak vs min ; min voltage (mV);peak voltage (mV)",
			       nBins,minX,maxX,
			       nBins,minX,maxX);

  // prepare for range starting at zero
  minX = 0.0;
  maxX = range/2.;
  nBins  = 0;
  
  Set_THF_Params(&minX,&maxX,&binWidth,&nBins);
  
  hPPV_Cooked =  new TH1F("hPPV_Cooked",
			  ";peak to peak voltage (mV);Counts",
			  nBins,minX,maxX);

}


void SaveNoise(string outPath){

  string sys_command = "mkdir -p ";
  sys_command += outPath;
  gSystem->Exec(sys_command.c_str());
  
  printf("\n Saving Noise Monitoring Plots \n\n");

  InitCanvas();
  
  gPad->SetLogy();
  
  hMean_Cooked->SetAxisRange(-30., 120.,"X");
  hMean_Cooked->SetMinimum(0.1);
  hMean_Cooked->Draw();

  string outName = outPath + "hMean_Cooked.pdf";
  canvas->SaveAs(outName.c_str());  
  
  hPPV_Cooked->SetAxisRange(-5.0, 145.,"X");
  hPPV_Cooked->SetMinimum(0.1);
  hPPV_Cooked->Draw();
  
  outName = outPath + "hPPV_Cooked.pdf";
  canvas->SaveAs(outName.c_str());
  
  hPeak_Cooked->SetAxisRange(-20.,80.,"X");
  hPeak_Cooked->SetMinimum(0.1);
  hPeak_Cooked->Draw();
  outName = outPath + "hPeak_Cooked.pdf";
  canvas->SaveAs(outName.c_str());
  
  
  hMin_Cooked->SetAxisRange(-30.,20.,"X");
  hMin_Cooked->SetMinimum(0.1);
  hMin_Cooked->Draw();

  TLine * l_thresh = new TLine(noise_thresh_mV,1,noise_thresh_mV,1000);
  l_thresh->SetLineStyle(2);
  l_thresh->SetLineColor(kRed);
  l_thresh->SetLineWidth(2);
  l_thresh->Draw();
  
  TLine * l_th_low = new TLine(noise_th_low_mV,1,noise_th_low_mV,1000);
  l_th_low->SetLineStyle(2);
  l_th_low->SetLineColor(kBlue);
  l_th_low->SetLineWidth(2);
  l_th_low->Draw();
  
  outName = outPath + "hMin_Cooked.pdf";
  canvas->SaveAs(outName.c_str());
  
  gPad->SetLogy(false);
  gPad->SetLogz();
  
  hMin_Peak_Cooked->SetAxisRange(-25., 15.,"X");
  hMin_Peak_Cooked->SetAxisRange(-15., 50.,"Y");
  
  hMin_Peak_Cooked->Draw("colz");

  outName = outPath + "hMin_Peak_Cooked.pdf";
  canvas->SaveAs(outName.c_str());

  gPad->SetLogz(false);
  
  delete canvas;
  
}

std::pair<double,std::vector<double>> base(int iEntry){

  // determine waveform, mean amplitude in mV

  std::vector<double> amplitude;
  for( int iSamp = 0 ; iSamp < NSamples; iSamp++){
    amplitude.push_back(ADC_To_Wave(ADC->at(iSamp)));
  }
  
  double mean = std::accumulate(amplitude.begin(), amplitude.end(), 0.0);
  mean /= NSamples;
    
  // baseline subtraction
  
  std::vector<double> base_sub;
  for( int iSamp = 0; iSamp < NSamples; iSamp++){
    base_sub.push_back(amplitude[iSamp]-mean);
  }
  
  // first linear regression
  
  TH1F* lin_reg1 = new TH1F("lin_reg1","First linear slope correction",base_sub.size(),0,Length_ns);
  for (int iSamp = 0; iSamp < NSamples; iSamp++){
    lin_reg1->SetBinContent(iSamp+1,base_sub[iSamp]);
  }
  
  lin_reg1->Fit("pol1","Q");
  
  double p0_lin1 = lin_reg1->GetFunction("pol1")->GetParameter(0);
  double p1_lin1 = lin_reg1->GetFunction("pol1")->GetParameter(1);
  
  delete lin_reg1;
  
  // first linear regression correction
  
  std::vector<double> base_lin1;
  for( int iSamp = 0; iSamp < NSamples; iSamp++){
    base_lin1.push_back(base_sub[iSamp] - p0_lin1 - (p1_lin1*iSamp*nsPerSamp));
  }
  
  // variance/stdev
  
  double var = 0.;
  
  for (int iSamp = 0; iSamp < NSamples; iSamp++){
    var += pow(base_lin1[iSamp]-mean,2);
  }

  var /= NSamples;
  double sdev = sqrt(var);
  
  // outlier removal
  
  std::vector<double> base_out;
  //printf("base_out = [");
  for (int iSamp = 0; iSamp < NSamples; iSamp++){
    if (sqrt(pow(base_lin1[iSamp]-mean,2)) >= 1*sdev){
      continue;
    }
    else
      base_out.push_back(base_lin1[iSamp]);
  }
  
  // second mean baseline
  
  double mean2 = std::accumulate(base_out.begin(), base_out.end(), 0.0);
  mean2 /= NSamples;
    
  // second baseline subtraction
  std::vector<double> base_all_sub2;
  std::vector<double> base_sub2;
  int NSamples_removed = base_out.size();
  for( int iSamp = 0; iSamp < NSamples_removed; iSamp++){
    base_sub2.push_back(base_out[iSamp]-mean2); //apply to data with AND without outlier
  }
  //printf("]\n");
  for( int iSamp = 0; iSamp < NSamples; iSamp++){
    base_all_sub2.push_back(base_lin1[iSamp]-mean2); //apply to data with AND without outlier
  }

  // second linear regression
  
  TH1F* lin_reg2 = new TH1F("lin_reg2","Second linear slope correction",NSamples_removed,0,nsPerSamp*NSamples_removed);
  for (int iSamp = 0; iSamp < NSamples_removed; iSamp++){
    lin_reg2->SetBinContent(iSamp+1,base_sub2[iSamp]);
  }
  
  lin_reg2->Fit("pol1","Q");
  
  double p0_lin2 = lin_reg2->GetFunction("pol1")->GetParameter(0);
  double p1_lin2 = lin_reg2->GetFunction("pol1")->GetParameter(1);
  
  delete lin_reg2;
  
  // second linear regression correction
  
  std::vector<double> base_lin_all2;
  for( int iSamp = 0; iSamp < NSamples; iSamp++){
    base_lin_all2.push_back(base_all_sub2[iSamp] - p0_lin2 - (p1_lin2*iSamp*nsPerSamp)); //Fit to data without outlier, correct data with outlier
  }

  std::vector<double> base_lin2;// = base_sub2;
  //printf("baseline2 = [");
  for( int iSamp = 0; iSamp < NSamples_removed; iSamp++){
    base_lin2.push_back(base_sub2[iSamp] - p0_lin2 - (p1_lin2*iSamp*nsPerSamp)); //Fit to data without outlier, correct data with outlier
  }
  
  // final mean to give baseline in mV -> surely need whole corrected data set?
  
  double baseline = std::accumulate(base_lin2.begin(), base_lin2.end(), 0.0);
  baseline /= NSamples;
  
  return std::make_pair(baseline,base_lin_all2);

}

int peak_rise(float thresh_mV, int nbins){

  double thresh = base_mV+0.25*peak_mV;//base_mV + thresh_mV;
  
  std::vector<double> amplitude;

  for( short iSamp = 0 ; iSamp < NSamples; iSamp++)
    amplitude.push_back(ADC_To_Wave(ADC->at(iSamp)));
    
  int bins = 0;
    
  for( int iSamp_peak = peak_samp; iSamp_peak > peak_samp - nbins; iSamp_peak--){
    if(amplitude[iSamp_peak] > thresh)
      bins++;
    else
      break;
  }
    
  if(bins == 0)
    return 0;  
  else if(bins == nbins)
    return 0;
  else
    return 1;
    
  //first analysis uses 6 bins, base_mV + thresh_mV, 10 mV thresh, no bins == 0 condition
  
}

void Dark(float thresh_mV){
  
  InitDark();
  
  float darkRate = 0;
  float darkRateErr = 0;
  float darkRate_noise = 0;
  float darkRateErr_noise = 0;
  
  TFile* results = new TFile("dark_results.root","RECREATE");  
  TTree* Dark = new TTree("Dark","Dark");
  Dark->Branch("darkRate",&darkRate,"darkRate/F");
  Dark->Branch("darkRateErr",&darkRateErr,"darkRateErr/F");
  Dark->Branch("darkRate_noise",&darkRate_noise,"darkRate/F");
  Dark->Branch("darkRateErr_noise",&darkRateErr_noise,"darkRateErr/F");
  
  int nDark = 0;
  int nDark_noise = 0;
  
  int rejected = 0;
  
  int rise_rej = 0;
  int av_neg_rej = 0;
  int av_pos_rej = 0;
  int peak_low = 0;
  int peak_high = 0;
  
  std::ofstream rejected_waveforms;
  rejected_waveforms.open("rejected_waveforms.csv");
  rejected_waveforms << "Rejected waveform at entry\n";

  std::ofstream dark_csv;
  dark_csv.open ("dark_hits.csv");
  dark_csv << "Count at entry\n";
  
  for (int iEntry = 0; iEntry < nentries; iEntry++) {
    cookedTree->GetEntry(iEntry);
    if(peak_mV > thresh_mV)
      nDark_noise++;

    // Noise Rejection 
    if( min_mV < -2.5 && peak_mV < thresh_mV){
      continue;}
    
    if( peak_mV < -2*min_mV && peak_mV > thresh_mV ){
      continue;}
      
    if( peak_mV < 2*min_mV && peak_mV > thresh_mV ){
      continue;}
      
    hD_Peak->Fill(peak_mV);
    hD_Min_Peak->Fill(min_mV,peak_mV);
    
    if( peak_mV < thresh_mV){
      peak_low++;
      continue;}
    
    std::pair<double,std::vector<double>> base_corrections = base(iEntry);
    double baseline = base_corrections.first;
    std::vector<double> wave_corrected = base_corrections.second;
    auto max_wave = std::max_element(wave_corrected.begin(), wave_corrected.end());
    double max_mV = *max_wave;

    if(max_mV > 80+baseline){
      rejected_waveforms << iEntry << "\n";
      rejected++;
      peak_high++;
      continue;}

    if(max_mV < thresh_mV+baseline){
      rejected_waveforms << iEntry << "\n";
      rejected++;
      peak_low++;
      continue;}
    
    int rise = peak_rise();
    
    if(!rise){
      rejected_waveforms << iEntry << "\n";
      rise_rej++;
      continue;}
    
    dark_csv << iEntry << "\n";
    
    nDark++;
    
  }

  rejected_waveforms.close();
  dark_csv.close();
  
  std::ofstream rej_count;
  rej_count.open("rejected_types.csv");
  rej_count << "peak_low,av_neg_rej,av_pos_rej,peak_high,rise_rej\n";
  rej_count << peak_low << "," << av_neg_rej << "," << av_pos_rej << "," << peak_high << "," << rise_rej;
  rej_count.close();

  float darkErr = sqrt(nDark);

  darkRate = (float)nDark/(nentries-rejected);
  darkRate = darkRate/Length_ns * 1.0e9;
  darkRateErr = darkErr/nDark * darkRate;
  
  printf("\n \n nentries = %d \n",nentries);
  printf("\n %i rejected 'dark counts'\n",rejected);
  printf("\n dark counts (noise rejected) = %d +/- %.0f \n",nDark,darkErr);
  printf("\n dark rate   (noise rejected) = %.0f +/- %.0f Hz \n",darkRate,darkRateErr);
  
  std::ofstream dark_results;
  dark_results.open ("dark_results.txt");
  dark_results << "dark counts (noise rejected) = " << nDark << " +/- " << darkErr << "\n";
  dark_results << "dark noise (noise rejected) = " << darkRate << " +/- " << darkRateErr << " Hz\n";
  dark_results.close();
  
  float darkErr_noise = sqrt(nDark_noise);
  
  darkRate_noise = (float)nDark_noise/nentries;
  darkRate_noise = darkRate_noise/Length_ns * 1.0e9;
  darkRateErr_noise = darkErr_noise/nDark_noise * darkRate_noise;
  
  printf("\n dark counts (with noise) = %d +/- %.0f \n",nDark_noise,darkErr_noise);
  printf("\n dark rate   (with noise) = %.0f +/- %.0f Hz\n\n",darkRate_noise,darkRateErr_noise);
  
  std::ofstream dark_results_noise;
  dark_results_noise.open ("dark_results.txt", std::ios_base::app);
  dark_results_noise << "dark counts (noise) = " << nDark_noise << " +/- " << darkErr_noise << "\n";
  dark_results_noise << "dark noise (noise) = " << darkRate_noise << " +/- " << darkRateErr_noise << " Hz\n";
  dark_results_noise.close();
  
  SaveDark();
  
  Dark->Fill();
  Dark->Write();
  results->Close();
  
}

void InitDark(){
  
  printf("\n ------------------------------ \n");
  printf("\n Dark Counts Analysis           \n");
    
  float range = (float)roundf(Range_V)*1000.;

  float max      =  range/2;
  float min      = -range/2;
  float binWidth = Wave_To_Amp_Scaled_Wave(mVPerBin);
  int   nBins    = 0;

  //  fix binning and set number of bins
  Set_THF_Params(&min,&max,&binWidth,&nBins);
  
  hD_Peak = new TH1F("hD_Peak",
		     "hD_Peak;peak voltage (mV);Counts",
		     nBins,min,max);
  
  hD_Min_Peak = new TH2F("hD_Min_Peak",
			 "hD_Min_Peak;min voltage (mV);peak voltage (mV)",
			 nBins,min,max,
			 nBins,min,max);

}

void SaveDark(string outPath){

  string sys_command = "mkdir -p ";
  sys_command += outPath;
  gSystem->Exec(sys_command.c_str());

  InitCanvas();

  TLegend *leg = new TLegend(0.21,0.2,0.31,0.9);
    
  leg->SetTextSize(0.025);
  leg->SetHeader("Baseline start","C");
  
  leg->SetMargin(0.4); 

  gPad->SetLogy();
  
  hD_Peak->SetAxisRange(-5., 75.,"X");
  hD_Peak->SetMinimum(0.1);
  hD_Peak->Draw();
  
  TLine * lVert = new TLine(10,0,10,20);
  lVert->SetLineColor(kBlue);
  lVert->SetLineWidth(2);
  lVert->SetLineStyle(2);
  lVert->Draw();

  string outName = outPath + "hD_Peak.pdf";
  canvas->SaveAs(outName.c_str());

  gPad->SetLogy(false);

  gPad->SetLogz();
  hD_Min_Peak->SetAxisRange(-25.,25.,"X");
  hD_Min_Peak->SetAxisRange(-5., 45.,"Y");
  
  gPad->SetGrid(1, 1);
  hD_Min_Peak->Draw("col");
  
  gPad->SetLogz();
  
  outName = outPath + "hD_Min_Peak.pdf";
  canvas->SaveAs(outName.c_str());

  gPad->SetGrid(0,0);
  
  gPad->SetLogz(false);

  delete canvas;
  
}

float ADC_To_Wave(short ADC){

  float wave = ADC * mVPerBin;

  wave -= Range_V*1000./2.;
  
  wave = Wave_To_Amp_Scaled_Wave(wave);
  
  return wave;
}

float Wave_To_Amp_Scaled_Wave(float wave){
  return wave/AmpGain*10.;
}

void Set_THF_Params(float * minX, 
				     float * maxX,
				     float * binWidth,
				     int   * nBins){
  
  if     (*nBins==0)
    *nBins = (int)roundf((*maxX - *minX)/(*binWidth));
  else if(*nBins > 0 && *binWidth < 1.0E-10)
    *binWidth = (*maxX - *minX)/(*nBins);
  else
    fprintf(stderr,"\n Error in Set_THF_Params \n");
  
  *nBins += 1;
  *minX -= 0.5*(*binWidth);
  *maxX += 0.5*(*binWidth);

}

void InitCanvas(float w,float h){
  
  canvas = new TCanvas();
  canvas->SetWindowSize(w,h);
  
}

int GetEntry(int entry)
{
// Read contents of entry.
   if (!cookedTree) return 0;
   return cookedTree->GetEntry(entry);
}

void Init()
{

  InitMeta();
  InitCooked();
  
  rand3 = new TRandom3(0);
 
  InitCanvas();
  
}

string GetMetaTreeID(){
  return "Meta_Data";
}

string GetCookedTreeID(){
  string CookedTreeID = FileID;
  return "Cooked_" + CookedTreeID;
}

void InitMeta(){
  
  printf("\n ------------------------------ \n");
  printf("\n Initialising Meta Data \n");
  printf("\n   %s \n",GetMetaTreeID().c_str());

  inFile->GetObject(GetMetaTreeID().c_str(),metaTree);
  
  if (!metaTree){
    fprintf( stderr, "\n Error: no meta tree  \n ");
    fprintf( stderr, "\n Was this file created with the latest cook_raw ? \n ");
    return;
  }

  metaTree->SetBranchAddress("SampFreq",&SampFreq,&b_SampFreq);
  metaTree->SetBranchAddress("NSamples",&NSamples,&b_NSamples);
  metaTree->SetBranchAddress("NADCBins",&NADCBins,&b_NADCBins);
  metaTree->SetBranchAddress("Range_V",&Range_V,&b_Range_V);
  metaTree->SetBranchAddress("nsPerSamp",&nsPerSamp,&b_nsPerSamp);
  metaTree->SetBranchAddress("mVPerBin",&mVPerBin,&b_mVPerBin);
  metaTree->SetBranchAddress("Length_ns",&Length_ns,&b_Length_ns);
  metaTree->SetBranchAddress("AmpGain",&AmpGain,&b_AmpGain);
  metaTree->SetBranchAddress("FirstMaskBin",&FirstMaskBin,&b_FirstMaskBin);
  metaTree->SetBranchAddress("FileID",&FileID,&b_FileID);
  
  metaTree->SetBranchAddress("Run",&Run,&b_Run);
  metaTree->SetBranchAddress("PMT",&PMT,&b_PMT);
  metaTree->SetBranchAddress("Loc",&Loc,&b_Loc);
  metaTree->SetBranchAddress("Test",&Test,&b_Test);
  metaTree->SetBranchAddress("HVStep",&HVStep,&b_HVStep);

  metaTree->GetEntry(0);
  
  printf("\n ------------------------------ \n");
  printf("\n FileID = %s ",FileID);
  printf("\n Run    = %d ",Run);
  printf("\n PMT    = %d ",PMT);
  printf("\n Loc    = %d ",Loc);
  printf("\n Test   = %c ",Test);
  printf("\n HVStep = %d \n",HVStep);
  
  printf("\n ------------------------------ \n");

}

void InitCooked(){
  
  inFile->GetObject(GetCookedTreeID().c_str(),cookedTree);
  
  if (cookedTree == 0){
    fprintf( stderr, "\n Warning: No cooked data tree");
  }
  
  printf("\n ------------------------------ \n");
  printf("\n Initialising Cooked Data \n");
  printf("\n   %s \n",GetCookedTreeID().c_str());
  
  if (!cookedTree){
    fprintf( stderr, "\n Error: no cooked tree  \n ");
    return;
  }

  cookedTree->SetMakeClass(1);
  
  cookedTree->SetBranchAddress("ADC",&ADC, &b_ADC);
  cookedTree->SetBranchAddress("peak_mV",&peak_mV, &b_peak_mV);
  cookedTree->SetBranchAddress("peak_samp",&peak_samp, &b_peak_samp);
  cookedTree->SetBranchAddress("min_mV",&min_mV, &b_min_mV);
  cookedTree->SetBranchAddress("mean_mV",&mean_mV, &b_mean_mV);
  cookedTree->SetBranchAddress("start_s",&start_s, &b_start_s);
  cookedTree->SetBranchAddress("base_mV",&base_mV, &b_base_mV);
  
  nentries64_t = cookedTree->GetEntriesFast();
  
  if( nentries64_t > INT_MAX ){
      fprintf(stderr,
	      "\n Error, nentries = (%lld) > INT_MAX unsupported \n ",
	      nentries64_t);
      return;
  }
  else
    nentries = (int)nentries64_t;
  
  
  printf("\n ------------------------------ \n");
  
  return;
}

void PrintMetaData(){ 

  printf("\n ------------------------------ \n");
  printf("\n Printing Meta Data \n");

  if (!metaTree) return;
  metaTree->Show(0);
  
}

int main(int argc, char** argv){

    const char* file = argv[1];
    inFile = new TFile(file,"READ");
    InitMeta();
    InitCooked();
    InitCanvas();
    PrintMetaData();
    Noise();
    Dark(10);
    delete cookedTree;
    delete metaTree;
    delete inFile;

    return 0;
}
