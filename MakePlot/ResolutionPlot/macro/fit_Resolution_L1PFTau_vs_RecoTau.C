// double CB symmetric (two-sided tails)
double CrystalBall
(double* x, double* par){ 
  //http://en.wikipedia.org/wiki/Crystal_Ball_function 
  double xcur = x[0]; 
  double alpha = par[0]; 
  double n = par[1]; 
  double mu = par[2]; 
  double sigma = par[3]; 
  double N = par[4]; 
  TF1* exp = new TF1("exp","exp(x)",1e-20,1e20); 
  double A; double B; 
  if (alpha < 0){ 
    A = pow((n/(-1*alpha)),n)*exp->Eval((-1)*alpha*alpha/2); 
    B = n/(-1*alpha) + alpha;} 
  else { 
    A = pow((n/alpha),n)*exp->Eval((-1)*alpha*alpha/2); 
    B = n/alpha - alpha;} 
    double f; 
  if (TMath::Abs((xcur-mu)/sigma) < TMath::Abs(alpha) ) 
    f = N*exp->Eval((-1)*(xcur-mu)*(xcur-mu)/(2*sigma*sigma)); 
  else if (((xcur-mu)/sigma) < (-1.)*alpha )
    f = N*A*pow((B- (xcur-mu)/sigma),(-1*n)); // left tail
  else
    f = N*A*pow( (B- (mu-xcur)/sigma),(-1*n)); // right tail
  delete exp; 
  return f; 
} 

// double CB asymmetric
double DoubleCrystalBall
(double* x, double* par){ 
  //http://en.wikipedia.org/wiki/Crystal_Ball_function 
  double xcur = x[0]; 
  double alphaL = par[0]; 
  double nL = par[1]; 
  double alphaR = par[2]; 
  double nR = par[3]; 

  double mu = par[4]; 
  double sigma = par[5]; 
  double N = par[6]; 
 
  TF1* exp = new TF1("exp","exp(x)",1e-20,1e20); 
  double AL; double BL; double AR; double BR; 
 
  if (alphaL < 0){ 
    AL = pow((nL/(-1*alphaL)),nL)*exp->Eval((-1)*alphaL*alphaL/2); 
    BL = nL/(-1*alphaL) + alphaL;} 
  else { 
    AL = pow((nL/alphaL),nL)*exp->Eval((-1)*alphaL*alphaL/2); 
    BL = nL/alphaL - alphaL;} 

  if (alphaR < 0){ 
    AR = pow((nR/(-1*alphaR)),nR)*exp->Eval((-1)*alphaR*alphaR/2); 
    BR = nR/(-1*alphaR) + alphaR;} 
  else { 
    AR = pow((nR/alphaR),nR)*exp->Eval((-1)*alphaR*alphaR/2); 
    BR = nR/alphaR - alphaR;} 
   

    double f; 
  if ( ((xcur-mu)/sigma) > (-1.)*alphaL  && ((xcur-mu)/sigma) < (1.)*alphaR) 
    f = N*exp->Eval((-1)*(xcur-mu)*(xcur-mu)/(2*sigma*sigma)); 
  
  // left
  else if ( ((xcur-mu)/sigma) <= (-1.)*alphaL )
    f = N*AL*pow((BL- (xcur-mu)/sigma),(-1*nL)); // left tail
  //right
  else
    f = N*AR*pow( (BR- (mu-xcur)/sigma),(-1*nR)); // right tail
  delete exp; 
  return f; 
} 

void fit_Resolution_L1PFTau_vs_RecoTau()
{
  TFile* fIn = new TFile ("../results/plotOutput_NTuple_VBFHToTauTau_20190423.root");
  TFile* fOut = new TFile ("../results/plotOutputFitted_NTuple_VBFHToTauTau_20190423.root", "recreate");
  vector<TString> fitSingleCB = {"hEta_Data", "hPhi_Data", "hEta_Data_barrel", "hPhi_Data_barrel", "hEta_Data_endcap", "hPhi_Data_endcap"};
  vector<TString> fitDoubleCB = {"hEt_Data", "hEt_Data_barrel", "hEt_Data_endcap"};
    
  TH1::SetDefaultSumw2();
    
  TF1* CBFunc = new TF1("CBFunc",&CrystalBall,-0.3,0.3,5);
  CBFunc->SetParameters(3, 1, 0, 0.05, 0.06);    
  for (TString name : fitSingleCB)
    {
        cout << "... fitting: " << name << endl;
        TH1F* h = (TH1F*) fIn->Get(name);
        h->Scale(1./h->Integral());
        h->Fit("CBFunc");
        fOut->cd();
        h->Write();
    }

  TF1* CBFuncAsymm = new TF1("CBFuncAsymm",&DoubleCrystalBall,0.,3.,7);
  for (TString name : fitDoubleCB)
    {
      cout << "... fitting: " << name << endl;
      TH1F* h = (TH1F*) fIn->Get(name);
      h->Scale(1./h->Integral());
      CBFuncAsymm->SetParameters(0.9, 4.3, 1.3, 4.3, h->GetMean(), h->GetRMS(), 1.);
      h->Fit("CBFuncAsymm");
      fOut->cd();
      h->Write();
    }
  
  fOut->Close();
}
