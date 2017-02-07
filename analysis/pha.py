
import ROOT
import numpy



def fit_gauss(hist):
    """
    """
    peak = hist.GetBinCenter(hist.GetMaximumBin())
    rms = hist.GetRMS()
    g0 = ROOT.TF1("g0", "gaus", peak - rms, peak + rms)
    hist.Fit("g0", "R")
    try:
        fwhm = 235.*g0.GetParameter(2)/g0.GetParameter(1)
    except:
        print "ZeroDivision Error"
        fwhm = 0.0
    peak = g0.GetParameter(1)
    return peak, fwhm


class ixpePulseHeightCube(ROOT.TH3D):

    """Wrapper around a ROOT.TH3 object describing a count cube, i.e. a set
    of one-dimensional count histograms onto a two-dimensional x-y grid.
    """

    def __init__(self, name, title=None, num_side_bins=20, half_size=8.,
                 num_pha_bins=200, min_pha=0., max_pha=10000):
        """Constructor.
        """
        title = title or name
        ROOT.TH3D.__init__(self, name, title,
                           num_side_bins, -half_size, half_size,
                           num_side_bins, -half_size, half_size,
                           num_pha_bins, min_pha, max_pha)
        self.__histogram_dict = {}
        self.num_side_bins = num_side_bins
        self.half_size = half_size

    def pha_histogram_name(self, i, j):
        """
        """
        return 'pha_hist_%03d_%03d' % (i, j)

    def create_pha_histogram(self, i, j, overlap=0):
        """Create a one-dimensional pulse-height histogram corresponding to a
        given spatial bin (possibly with overlap).
        """
        imin = max(1, i + 1 - overlap)
        imax = min(i + 1 + overlap, self.GetNbinsX())
        self.GetXaxis().SetRange(imin, imax)
        self.GetXaxis().SetBit(ROOT.TAxis.kAxisRange)
        jmin = max(1, j + 1 - overlap)
        jmax = min(j + 1 + overlap, self.GetNbinsY())
        self.GetYaxis().SetRange(jmin, jmax)
        self.GetYaxis().SetBit(ROOT.TAxis.kAxisRange)
        hist = self.Project3D('z')
        hist.SetName(self.pha_histogram_name(i, j))
        self.__histogram_dict[(i, j)] = hist
        return hist

    def create_2d_hist(self,name):
        hist2d = ROOT.TH2F(name, name, self.num_side_bins, -self.half_size, self.half_size, self.num_side_bins, -self.half_size, self.half_size)
        return hist2d
    
    def project_pha(self, fit=True):
        """Create the pulse-height histograms in all the spatial bins.
        """
        fwhm_hist = self.create_2d_hist('FWHM')
        main_peak_hist = self.create_2d_hist('MainPeak')
        
        for i in range(self.GetNbinsX()):
            for j in range(self.GetNbinsY()):
                h = self.create_pha_histogram(i, j)
                if fit:
                    peak, fwhm = fit_gauss(h)
                    fwhm_hist.SetBinContent(i+1,j+1, fwhm)
                    main_peak_hist.SetBinContent(i+1,j+1, peak)
        self.__histogram_dict['MainPeak'] = main_peak_hist
        self.__histogram_dict['FWHM'] = fwhm_hist
     

        
    def pha_histogram(self, i, j):
        """
        """
        if not self.__histogram_dict.has_key((i, j)):
            self.create_pha_histogram(i, j)
        return self.__histogram_dict[(i, j)]

    def write(self, file_path):
        """
        """
        output_file = ROOT.TFile(file_path, 'RECREATE')
        self.Write()
        for hist in self.__histogram_dict.values():
            hist.Write()
        output_file.Close()

    

if __name__ == '__main__':
    file_path = '../out/001_0000574_data_TH5_outputfile.root'
    input_file = ROOT.TFile(file_path)
    tree = input_file.Get('tree')

    expr = 'fPHeight[0]:fBaricenterY[0]:fBaricenterX[0]'
    cut = ''
    num_events = 100000000
    cube = ixpePulseHeightCube('ccube', 'Count cube')
    tree.Project('ccube', expr, cut, '', num_events)
    cube.project_pha()
    h = cube.pha_histogram(10, 10)
    h.Draw()
    cube.write('./test_cube.root')
    
    """
    ccube = ROOT.TH3D('ccube', 'Count cube', num_bins, -side, side, num_bins,
                  -side, side, 200, 0, 10000)
tree.Project('ccube', 'fPHeight[0]:fBaricenterY[0]:fBaricenterX[0]', cut)

label ="GEM"

fwhm_list = []
main_peak_list = []

x_bins = numpy.linspace(-side, side, num_bins)
y_bins = numpy.linspace(-side, side, num_bins)

for i,x in enumerate(x_bins):
    for j,y in enumerate(y_bins):
        ccube.GetXaxis().SetRange(i + 1, i + 1)
        ccube.GetXaxis().SetBit(ROOT.TAxis.kAxisRange)
        ccube.GetYaxis().SetRange(j + 1, j + 1)
        ccube.GetYaxis().SetBit(ROOT.TAxis.kAxisRange)

        h = ccube.Project3D('z')
        c = ROOT.TCanvas("plot_%s"%label, "plot_%s"%label, 800, 600)
        c.cd()
        
        myTxt = ROOT.TLatex()
        myTxt.SetTextFont(42)
        
        print("\n FIT MAIN PEAK")
        peak = h.GetBinCenter(h.GetMaximumBin())
        rms  = h.GetRMS()
        g0 = ROOT.TF1("g0", "gaus", peak-rms, peak+rms)
        h.Fit("g0", "R")
        myTxt.DrawLatexNDC(0.6, 0.75, "Main Peak:")
        myTxt.DrawLatexNDC(0.6, 0.7, "Ave %d" %(g0.GetParameter(1)))
        try:
            fwhm = 235.*g0.GetParameter(2)/g0.GetParameter(1)
            myTxt.DrawLatexNDC(0.6, 0.65, "FWHM %.2f%%" %(fwhm))
        except:
            print "ZeroDivision Error"
            fwhm = 0.0
        fwhm_list.append(fwhm)
        main_peak_list.append(peak)
        
        g0.Draw("same")
        print("\n FIT ESC. PEAK")
        esc  = g0.GetParameter(1)/2
        rms1 = g0.GetParameter(2)
        g1 = ROOT.TF1("g1", "gaus", esc-rms1, esc+rms1)
        g1.SetLineColor(3)
        h.Fit("g1", "R")
        g1.Draw("same")
        myTxt.DrawLatexNDC(0.6, 0.6, "Esc Peak:")
        myTxt.DrawLatexNDC(0.6, 0.55, "Ave %d" %(g1.GetParameter(1)))
        try:
            fwhm1 = 235.*g1.GetParameter(2)/g1.GetParameter(1)
            myTxt.DrawLatexNDC(0.6, 0.5, "FWHM %.2f%%" %(fwhm1))
        except:
            print "ZeroDivision Error"
            fwhm1 = 0.0
        #line += "%s,%s,%s,%s,%s,%s\n"%(x,y,num_events,fwhm,peak,fwhm1)
        h.Draw()
        c.Update()
       
      #  raw_input()
"""
