from  gem_run_dict import GEM_RUN_DICT as dictionary

import os
import ROOT
import __root__

def parse_and_fit():
    for gem in dictionary.keys():
        file_str = ''
        for run in dictionary[gem]['unif_illumination_runs']:
            filepath = '../out/001_0000%s_data_TH5_outputfile.root'%run
            file_str+='%s '%filepath
       
        output_file_path = '%s_uniformities.root'%gem
        cmd = 'python plotGEMUniformMap.py %s -o %s -l %s'%(file_str, output_file_path, gem)
        os.system(cmd)


def parse_and_plot2d():
    for gem in dictionary.keys():
        output_file_path = '%s_uniformities.root'%gem
        f = ROOT.TFile(output_file_path)
        fwhm_label = "FWHM_%s"%gem
        main_peak_label = "MainPeak_%s"%gem
        c1 = ROOT.TCanvas(fwhm_label)
        fwhm = f.Get(fwhm_label)
        fwhm.SetTitle(fwhm_label)
        fwhm.GetYaxis().SetTitle("fBaricenterY[0]")
        fwhm.GetXaxis().SetTitle("fBaricenterX[0]")
        fwhm.Draw("colz")
        c1.Update()
        c1.SaveAs("%s.png"%fwhm_label)
        raw_input()
        c2 = ROOT.TCanvas(main_peak_label)
        peak = f.Get(main_peak_label)
        peak.SetTitle(main_peak_label)
        peak.GetYaxis().SetTitle("fBaricenterY[0]")
        peak.GetXaxis().SetTitle("fBaricenterX[0]")
        peak.Draw("colz")
        c2.SaveAs("%s.png"%main_peak_label)
        raw_input()



parse_and_plot2d()
