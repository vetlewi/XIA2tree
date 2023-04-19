
vector<double> calibrateSpectra(TH2 *m)
{
	TSpectrum spec;
	char tmp[1024];
	vector<double> peakPos;
	for ( int i = 0 ; i < m->GetYaxis()->GetNbins() ; ++i ){
		sprintf(tmp, "px_%d", i);
		TH1 *h = m->ProjectionX(tmp, i+1, i+1);
		spec.Search(h);
		int n_peaks = spec.GetNPeaks();
		cout << i << ": " << n_peaks << endl;

		double param[3] = {spec.GetPositionY()[0], spec.GetPositionX()[0], 1.0};
		//TF1 *fit = new TF1("total", "[0]*[2]/((x*x - [1]*[1])*(x*x - [1]*[1]) + [2]*[2])", spec.GetPositionX()[0]-10, spec.GetPositionX()[0]+10);
		TF1 *fit = new TF1("total", "gaus(0)", spec.GetPositionX()[0]-2, spec.GetPositionX()[0]+2);

		for (Int_t k=0; k<3; k++) {
        	fit->SetParameter(k, param[k]);
        }

        h->Fit("total", "bR");
		peakPos.push_back(-fit->GetParameter(1));
	}

	printf("t0_labr =\t");
	for (auto pos : peakPos){
		printf("%2.6f\t", pos);
	}
	return peakPos;
}
