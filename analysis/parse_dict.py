from  gem_run_dict import GEM_RUN_DICT as dictionary

import os
import ROOT
import __root__
import numpy


def parse_and_fit():
    for gem in dictionary.keys():
        file_str = ''
        for run in dictionary[gem]['unif_illumination_runs']:
            filepath = '../out/001_0000%s_data_TH5_outputfile.root'%run
            file_str+='%s '%filepath
       
        output_file_path = '%s_uniformities.root'%gem
        cmd = 'python plotGEMUniformMap.py %s -o %s -l %s'%(file_str, output_file_path, gem)
        os.system(cmd)


def fit_gain_trend(filepath, gem_label):
    time, peak, fwhm, timeErr, peakErr, fwhmErr = \
                        numpy.loadtxt(filepath, delimiter='\t', unpack=True)
    
    from matplotlib import pyplot as plt
    print "Ploting peak position for %s"%gem_label
    plt.errorbar(time, peak, xerr=timeErr, yerr=peakErr, marker='o',ls='none')
    pol_coef = numpy.polyfit(time, peak,deg=5)

    def time_trend_expr(pol_coef, offset, time_var='fTimeStamp'):
        """
        """
        expr = ''
        degree = len(pol_coef)
        for i, p in enumerate(pol_coef):
            if i < degree - 1:
                expr += '%f*(%s - %f)**%d + ' %\
                        (p, time_var, offset, degree - i - 1)
            else:
                expr += '%f' % p
        expr = '(%s)/%f' % (expr, p) 
        return expr

    offset = 1485500048.003290
    print time_trend_expr(pol_coef, offset)
    
    plt.plot(time, numpy.polyval(pol_coef, time))
    print "Here are the polynomial coefficients for the fir with pol degree 5",pol_coef
    plt.show()

    
    
    #raw_input()
        
def parse_and_plot2d():
    #for gem in dictionary.keys():
    for gem in ['gem_1420']:
        root_file_path = '%s_uniformities.root'%gem
        gain_trend_file_path = '/data/xpe/xperecon/analysis/gemNov2016Summary/gaintrend_%s.data'%gem
        #if gem=='gem_1420':
        #
        fit_gain_trend(gain_trend_file_path, gem)
        
        f = ROOT.TFile(root_file_path)
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
        #raw_input()
        c2 = ROOT.TCanvas(main_peak_label)
        peak = f.Get(main_peak_label)
        peak.SetTitle(main_peak_label)
        peak.GetYaxis().SetTitle("fBaricenterY[0]")
        peak.GetXaxis().SetTitle("fBaricenterX[0]")
        peak.Draw("colz")
        c2.SaveAs("%s.png"%main_peak_label)
        #raw_input()



parse_and_plot2d()
