#!/usr/bin/env python
# *********************************************************************
# * Copyright (C) 2014 Luca Baldini (luca.baldini@pi.infn.it)         *
# *                                                                   *
# * For the license terms see the file LICENCE, distributed           *
# * along with this software.                                         *
# *********************************************************************
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


import ROOT

# Top-level ROOT settings.
TEXT_FONT = 42
TEXT_SIZE = 0.06
if TEXT_FONT % 10 == 3:
    TEXT_SIZE_SCALE = 31.
else:
    TEXT_SIZE_SCALE = 0.06
LABEL_TEXT_SIZE = 0.9*TEXT_SIZE
LEGEND_TEXT_SIZE = 0.75*TEXT_SIZE
SMALL_TEXT_SIZE = 0.8*TEXT_SIZE
SMALLER_TEXT_SIZE = 0.7*TEXT_SIZE
SMALLEST_TEXT_SIZE = 0.6*TEXT_SIZE
CANVAS_DEF_WIDTH  = 840
CANVAS_DEF_HEIGHT = 600
CANVAS_RIGHT_MARGIN = 0.13
CANVAS_TOP_MARGIN = 0.1
CANVAS_LEFT_MARGIN = 0.130*TEXT_SIZE/TEXT_SIZE_SCALE
CANVAS_BOTTOM_MARGIN = 0.130*TEXT_SIZE/TEXT_SIZE_SCALE

ROOT.gROOT.SetStyle('Plain')
STYLE = ROOT.gStyle
STYLE.SetOptTitle(0)
STYLE.SetOptStat(0000)

# More setting
STYLE.SetCanvasDefW(CANVAS_DEF_WIDTH)
STYLE.SetCanvasDefH(CANVAS_DEF_HEIGHT)
# Text Font and Precision
# The text font code is combination of the font number and the precision.
#
#   Text font code = 10*fontnumber + precision
#
# Font numbers must be between 1 and 14.
#
# The precision can be:
# 0 fast hardware fonts (steps in the size)
# 1 scalable and rotatable hardware fonts (see below)
# 2 scalable and rotatable hardware fonts
# 3 scalable and rotatable hardware fonts. Text size is given in pixels. 
STYLE.SetTextFont(TEXT_FONT)
STYLE.SetTextSize(TEXT_SIZE)
STYLE.SetTitleFont(TEXT_FONT, 'XYZ')
STYLE.SetTitleSize(TEXT_SIZE, 'XYZ')
STYLE.SetLabelFont(TEXT_FONT, 'XYZ')
STYLE.SetLabelSize(LABEL_TEXT_SIZE, 'XYZ')
STYLE.SetTitleYOffset(1.08)
STYLE.SetTitleXOffset(1.08)
STYLE.SetTitleOffset(1.0, 'Z')
STYLE.SetLegendBorderSize(0)
STYLE.SetPadRightMargin(CANVAS_RIGHT_MARGIN)
STYLE.SetPadTopMargin(CANVAS_TOP_MARGIN)
STYLE.SetPadLeftMargin(CANVAS_LEFT_MARGIN)
STYLE.SetPadBottomMargin(CANVAS_BOTTOM_MARGIN)
STYLE.SetStatBorderSize(0)
STYLE.SetStatFont(TEXT_FONT)
STYLE.SetStatFontSize(TEXT_SIZE)
STYLE.SetGridColor(ROOT.kGray + 1)
STYLE.SetGridStyle(2)
STYLE.SetStatStyle(0)
STYLE.SetMarkerStyle(1)
STYLE.SetLineWidth(2)
STYLE.SetHistLineWidth(2)


#from gRootColorPalette import DEFAULT_COLOR_PALETTE 
#DEFAULT_COLOR_PALETTE.createGradientColorTable(STYLE)


# Pool to save objects.
ROOT_OBJECT_POOL = []

def store(rootObject):
    ROOT_OBJECT_POOL.append(rootObject)
