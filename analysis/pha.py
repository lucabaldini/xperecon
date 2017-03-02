
import ROOT
import numpy

G2FWHM = 2.3548200450309493

def fit_gauss(hist,nsigma=1.5, verbose=True):
    """
    """
    g0 = ROOT.TF1("g0", "gaus", 500, 10000)
    hist.Fit("g0", "RNQ")
    g = ROOT.TF1("g", "gaus", \
                 g0.GetParameter(1) - nsigma*g0.GetParameter(2),\
                 g0.GetParameter(1) + nsigma*g0.GetParameter(2))
    hist.Fit("g", "RQ")
    gPeak  = g.GetParameter(1)
    gSigma = g.GetParameter(2)
    gPeakErr = g.GetParError(1)
    gFWHM  = G2FWHM*(gSigma/gPeak)
    gFWHMErr = gFWHM*numpy.sqrt((g.GetParError(2)/gSigma)**2 + (gPeakErr/gPeak)**2)
    
    
    if verbose:
        print  "fitSimpleGauss:", gPeak, gSigma, "FWHM", gFWHM, \
            "Eres=", gSigma/gPeak,'\n'            
        
    return (gPeak, gFWHM, gPeakErr, gFWHMErr,gSigma)
    


class ixpePulseHeightCube(ROOT.TH3D):

    """Wrapper around a ROOT.TH3 object describing a count cube, i.e. a set
    of one-dimensional count histograms onto a two-dimensional x-y grid.
    """

    def __init__(self, name, title=None, num_side_bins=20, half_size=7.5,
                 num_pha_bins=200, min_pha=0., max_pha=10000, label='GEM'):
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
        self.label = label

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
        """Create the pulse-height histograms in all the spatial bins and
        fill the maps of fwhm and main peak position.
        """
        fwhm_hist = self.create_2d_hist('FWHM_%s'%self.label)
        main_peak_hist = self.create_2d_hist('MainPeak_%s'%self.label)
        main_peak_sigma_hist = self.create_2d_hist('MainPeakSigma_%s'%self.label)
        
        for i in range(self.GetNbinsX()):
            for j in range(self.GetNbinsY()):
                h = self.create_pha_histogram(i, j)
                if fit:
                    peak, fwhm, peak_err, fwhm_err, sigma = fit_gauss(h, verbose=False)
                    fwhm_hist.SetBinContent(i+1,j+1, fwhm)
                    main_peak_hist.SetBinContent(i+1,j+1, peak)
                    main_peak_sigma_hist.SetBinContent(i+1,j+1, sigma)
        self.__histogram_dict['MainPeak'] = main_peak_hist
        self.__histogram_dict['MainPeakSigma'] = main_peak_sigma_hist
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

    def draw(self, opts):
        """
        """
        self.c1 = ROOT.TCanvas('canvas1')
        #ROOT.gStyle.SetPaintTextFormat('.1f')
        self.__histogram_dict['MainPeak'].Draw(opts)
        self.c1.Update()
        self.c2 = ROOT.TCanvas('canvas2')
        #ROOT.gStyle.SetPaintTextFormat('.3f')
        self.__histogram_dict['FWHM'].Draw(opts)
        self.c2.Update()
     



