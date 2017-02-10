from  gem_run_dict import GEM_RUN_DICT as dictionary

import os
import ROOT
import __root__
import numpy


def parse_and_fit(gem, useExpression=False):
    file_str = ''
    for run in dictionary[gem]['unif_illumination_runs']:
        filepath = '../out/001_0000%s_data_TH5_outputfile.root'%run
        gain_trend_file_path = '/data/xpe/xperecon/analysis/gemNov2016Summary/gaintrend_%s.data'%gem
        file_str+='%s '%filepath
       
    output_file_path = '%s_uniformities.root'%gem
    if useExpression:
        expression = fit_gain_trend(gain_trend_file_path,gem)
        print "Using this expression ",expression
    cmd = 'python plotGEMUniformMap.py %s -o %s -l %s -x "%s"'%(file_str, output_file_path, gem, expression)
    os.system(cmd)


def fit_gain_trend(filepath, gem_label):
    time, peak, fwhm, timeErr, peakErr, fwhmErr = \
                        numpy.loadtxt(filepath, delimiter='\t', unpack=True)

    offset = fetch_offset_time(filepath)
    offset = float(offset)
    from matplotlib import pyplot as plt
    print "Ploting peak position for %s"%gem_label
    plt.errorbar(time, peak, xerr=timeErr, yerr=peakErr, marker='o',ls='none')
    pol_deg = 5
    pol_coef = numpy.polyfit(time, peak,deg=pol_deg)

    def time_trend_expr(pol_coef, offset, time_var='fTimeStamp'):
        """
        """
        expr = ''
        degree = len(pol_coef)
        for i, p in enumerate(pol_coef):
            if i < degree - 1:
                expr += '%f*((%s - %f)/3600.)**%d + ' %\
                        (p, time_var, offset, degree - i - 1)
            else:
                expr += '%f' % p
        expr = '(%s)/%f' % (expr, p) 
        return expr

    #offset = 1.48550e+09 #offset for gem 1422#1.48596e+09 Offset for gem 1422 
    expr =  time_trend_expr(pol_coef, offset)
    print expr
    
    plt.plot(time, numpy.polyval(pol_coef, time))
    plt.xlabel('Time since start of run [hr]')
    plt.ylabel('Pulse height')
    print "Here are the polynomial coefficients for the fit with pol degree 5",\
        pol_coef
    
    plt.savefig('gain_trend_%s_fit_pol_%s.png'%(gem_label,pol_deg))
    plt.show()

    return expr
    
def fetch_offset_time(gain_trend_file_path):
    gain_file = file(gain_trend_file_path,'r')
    for line in gain_file.readlines():
        if 'offset' in line:
            l = line.split()
            offset_time = l[5]

    return offset_time
     
def parse_and_plot2d(gem):
    #for gem in dictionary.keys():
    #for gem in ['gem_1420']:
    root_file_path = '%s_uniformities.root'%gem
        
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

def plot_gem_summary():
    gem_list = ['gem_1413','gem_1414','gem_1415','gem_1416','gem_1417','gem_1418',
                'gem_1419','gem_1420','gem_1421','gem_1422']
    
    cFWHM = ROOT.TCanvas("FWHM","FWHM",1400, 1050)
    cFWHM.Divide(4,3)

    cPeak = ROOT.TCanvas("Peak","Peak",1400, 1050)
    cPeak.Divide(4,3)
    
    pool = []
    for i,gem in enumerate(gem_list):
        root_file_path = '%s_uniformities.root'%gem
        f = ROOT.TFile(root_file_path)
        fwhm_label = "FWHM_%s"%gem
        main_peak_label = "MainPeak_%s"%gem
        h = f.Get(fwhm_label)
        g = f.Get(main_peak_label)
        cFWHM.cd(i+1)

        h.Draw('colz')
        h.SetTitle(fwhm_label)
        h.GetYaxis().SetTitle("fBaricenterY[0]")
        h.GetXaxis().SetTitle("fBaricenterX[0]")
        cFWHM.Update()
        
        cPeak.cd(i+1)
        g.Draw('colz')
        g.SetTitle(main_peak_label)
        g.GetYaxis().SetTitle("fBaricenterY[0]")
        g.GetXaxis().SetTitle("fBaricenterX[0]")
        cPeak.Update()
        
        pool.append(h)
        pool.append(g)
        pool.append(f)
        pool.append(fwhm_label)
        pool.append(main_peak_label)
    raw_input()

    
#for gem in ['gem_1419']:
#for gem in dictionary.keys():
#    parse_and_fit(gem, useExpression=True)
#    parse_and_plot2d(gem)

plot_gem_summary()
