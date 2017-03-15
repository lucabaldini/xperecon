
import numpy

from scipy.optimize import curve_fit
from matplotlib import pyplot as plt

file_path = 'gaintrend_gpd20_run923_981.data'
t, peak, fwhm, dt, dpeak, dfwhm = numpy.loadtxt(file_path, unpack=True)


def falling_exp(x, C, D, tau, x0):
    """
    """
    return C + D*numpy.exp(-(x - x0)/tau)


def rising_exp(x, C, D, tau, x0):
    """
    """
    return C - D*numpy.exp(-(x - x0)/tau)

def comb_exp(x, C, D1, D2, tau1, tau2, x0):
    """
    """
    return C - D1*numpy.exp(-(x - x0)/tau1) + D2*numpy.exp(-(x - x0)/tau2)




_mask1 = (t > 35.)*(t < 210)
_t1 = t[_mask1]
_peak1 = peak[_mask1]

p0 = (4500, 1000, 35, 48.)
popt1, pcov1 = curve_fit(falling_exp, _t1, _peak1, p0)
print(popt1)

_mask2 = (t > 215.)*(t < 310)
_t2 = t[_mask2]
_peak2 = peak[_mask2]

p0 = (5000, 500, 215, 48.)
popt2, pcov2 = curve_fit(rising_exp, _t2, _peak2, p0)
print(popt2)

_mask3 = (t > 0.)*(t < 210)
_t3 = t[_mask3]
_peak3 = peak[_mask3]
p0 = (4600, 1200, 1500, 12., 40., 0)
popt3 = p0
popt3, pcov3 = curve_fit(comb_exp, t, peak, p0)
print(popt3)



plt.errorbar(t, peak, dpeak, fmt='o')
plt.plot(_t1, falling_exp(_t1, *popt1), lw=2, color='r')
plt.plot(_t2, rising_exp(_t2, *popt2), lw=2, color='g')
plt.plot(_t3, comb_exp(_t3, *popt3), lw=2)
plt.text(100, 5000, 'C = %.1f, $\\tau$ = %.1f h' % (popt1[0], popt1[2]),
         color='r')
plt.text(200, 4500, 'C = %.1f, $\\tau$ = %.1f h' % (popt2[0], popt2[2]),
         color='g')

plt.show()
