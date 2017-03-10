
import numpy

from scipy.optimize import curve_fit
from matplotlib import pyplot as plt

file_path = 'gaintrend_gpd20_run923_986.data'
try:
    t, peak, fwhm, dt, dpeak, dfwhm = numpy.loadtxt(file_path, unpack=True)
except:
    t, peak, fwhm, rate, dt, dpeak, dfwhm, drate = numpy.loadtxt(file_path, unpack=True)
    

def falling_exp(x, C, D, tau, x0):
    """
    """
    return C + D*numpy.exp(-(x - x0)/tau)


def rising_exp(x, C, D, tau, x0):
    """
    """
    return C - D*numpy.exp(-(x - x0)/tau)


_mask1 = (t > 35.)*(t < 210)
_t1 = t[_mask1]
_peak1 = peak[_mask1]
_rate1 = rate[_mask1]

p0 = (4500, 1000, 35, 48.)
popt1, pcov1 = curve_fit(falling_exp, _t1, _peak1, p0)
print(popt1)

_mask2 = (t > 215.)*(t < 310)
_t2 = t[_mask2]
_peak2 = peak[_mask2]
_rate2 = rate[_mask2]

p0 = (5000, 500, 215, 48.)
popt2, pcov2 = curve_fit(rising_exp, _t2, _peak2, p0)
print(popt2)

_mask3 = (t > 310.)*(t < 340)
_t3 = t[_mask3]
_peak3 = peak[_mask3]
_rate3 = rate[_mask3]

p0 = (4500, 1000, 35, 300.)
popt3, pcov3 = curve_fit(falling_exp, _t3, _peak3, p0)
print(popt3)

plt.figure()
plt.errorbar(t, peak, dpeak, fmt='o')
plt.plot(_t1, falling_exp(_t1, *popt1), lw=2, color='r')
plt.plot(_t2, rising_exp(_t2, *popt2), lw=2, color='g')
plt.plot(_t3, falling_exp(_t3, *popt3), lw=2, color='blue')
plt.text(100, 5000, 'C = %.1f, $\\tau$ = %.1f h' % (popt1[0], popt1[2]),
         color='r')
plt.text(200, 4500, 'C = %.1f, $\\tau$ = %.1f h' % (popt2[0], popt2[2]),
         color='g')
plt.text(275, 5000, 'C = %.1f, $\\tau$ = %.1f h' % (popt3[0], popt3[2]),
         color='blue')



r = _rate1.mean(), _rate2.mean(), _rate3.mean()
C = popt1[0], popt2[0] , popt3[0] 
tau = popt1[2], popt2[2] , popt3[2] 

plt.figure()
plt.plot(r, C, 'o')
plt.xlabel('Rate [Hz]')
plt.ylabel('C [ADC counts]')

plt.figure()
plt.plot(r, tau, 'o')
plt.xlabel('Rate [Hz]')
plt.ylabel('$\\tau$ [h]')


plt.show()
