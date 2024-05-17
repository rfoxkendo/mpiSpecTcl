#!/usr/bin/env python
import io
import sys, os
sys.path.append(os.getcwd())

import pandas as pd
import numpy as np
from scipy.optimize import curve_fit

import fit_factory

class GausFit:
    def __init__(self, amplitude, mean, standard_deviation):
        self.amplitude = amplitude
        self.mean = mean
        self.standard_deviation = standard_deviation

    # function defined by the user
    def gauss(self, x, amplitude, mean, standard_deviation):
        return amplitude*np.exp(-(x-mean)**2.0 / (2*standard_deviation**2))

    # implementation of the fitting algorithm
    def start(self, x, y, xmin, xmax, fitpar, axis, fit_results):
        fitln =None
        if (fitpar[0] != 0.0):
            self.amplitude = fitpar[0]
        else:
            self.amplitude = 2000
        if (fitpar[1] != 0.0):
            self.mean = fitpar[1]
        else:
            self.mean = xmin+(xmax-xmin)/2
        if (fitpar[2] != 0.0):
            self.standard_deviation = fitpar[2]
        else:
            self.standard_deviation = self.mean/10

        p_init = [self.amplitude, self.mean, self.standard_deviation]
        #sometime doesn't converge with maxfev iterations and raise an error in curve_fit, this closes CutiePie window.
        #consider changing library
        popt, pcov = curve_fit(self.gauss, x, y, p0=p_init, maxfev=1000000)

        # plotting fit curve and printing results
        try:
            x_fit = np.linspace(x[0],x[-1], 10000)
            y_fit = self.gauss(x_fit, *popt)

            fitln, = axis.plot(x_fit,y_fit, 'r-')
            for i in range(len(popt)):
                s = 'Par['+str(i)+']: '+str(round(popt[i],3))+'+/-'+str(round(pcov[i][i],3))
                fit_results.append(s)
        except:
            pass
        return fitln

class GausFitBuilder:
    def __init__(self):
        self._instance = None

    def __call__(self, amplitude = 1000, mean = 100, standard_deviation = 10):
        if not self._instance:
            self._instance = GausFit(amplitude, mean, standard_deviation)
        return self._instance
