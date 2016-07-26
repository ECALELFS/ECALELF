# -*- coding: utf-8 -*-
import numpy as np
import matplotlib.pyplot as plt
from   matplotlib.ticker import NullFormatter
from   matplotlib.ticker import MultipleLocator, FormatStrFormatter
import matplotlib.dates as dates
import pandas as pd
import glob, os, re
import datetime
import re
from math import sqrt

ecal_regions = [
    "EB",
    "EB-gold",
    "EB-bad",
    "EB-absEta_0_1",
    "EB-absEta_1_1.4442",
    "EE",
    "EE-gold",
    "EE-bad",
    "EE-absEta_1.566_2",
    "EE-absEta_2_2.5"
]

region_labels = {
    "EB"      : "EB",
    "EB-gold" : "EB $R_{9} > 0.94$",
    "EB-bad"  : "EB $R_{9} < 0.94$",
    "EB-absEta_0_1" : "EB $\mid\eta\mid < 1$",
    "EB-absEta_1_1.4442" : "EB $1 < \mid\eta\mid < 1.4442$",
    "EE"      : "EE",
    "EE-gold" : "EE $R_{9} > 0.94$",
    "EE-bad" : "EE $R_{9} < 0.94$",
    "EE-absEta_1.566_2" : "EE $1.566 < \mid\eta\mid < 2$",
    "EE-absEta_2_2.5" : "EE $2 < \mid\eta\mid < 2.5$"
}

var_labels = {  
    "DeltaM_data" : "$\Delta m$",
    "DeltaP"      : "$\Delta p$",
    "width_data"  : "$\sigma_{CB}$",
    "rescaledWidth_data" : "$\sigma_{CB}$ (Rescaled)",
    "additionalSmearing" : "Additional Smearing",
    "chi2data"           : "$\chi^{2}$",
    "events_lumi"        : "events/lumi",
    "sigmaeff_data"      : "$\sigma_{eff}$"
} 

format_fig_output = ['pdf','png']

def read_run_range(path = "", file = ""):

    raw_    = {
        'run_number': [],
        'Nevents'   : [],
        'UnixTime'  : []
    }
    columns = ['run_number', 'Nevents', 'UnixTime']
    with open(path+file) as f:
        for line in f.read().split('\n'):
            if len(line)==0: continue
            values = line.split('\t')
            raw_['run_number'].append(values[0])
            raw_['Nevents'   ].append(values[1])
            raw_['UnixTime'  ].append(values[2])

    data_ = pd.DataFrame(raw_)

    #Transform the entries
    data_.Nevents = [float(x) for x in data_.Nevents]
    data_[ "UnixTime_min" ] = [ int(x.split('-')[0]) for x in data_.UnixTime ]
    data_[ "UnixTime_max" ] = [ int(x.split('-')[1]) for x in data_.UnixTime ]
    data_[ "run_min" ] = [ int(x.split('-')[0]) for x in data_.run_number ]
    data_[ "run_max" ] = [ int(x.split('-')[1]) for x in data_.run_number ]
    data_[ "date_min"] = [ datetime.datetime.fromtimestamp(x).strftime('%Y-%m-%d %H:%M:%S') for x in data_.UnixTime_min ]
    data_[ "date_max"] = [ datetime.datetime.fromtimestamp(x).strftime('%Y-%m-%d %H:%M:%S') for x in data_.UnixTime_max ]
    data_['time'] = pd.to_datetime(data_['date_max'], format='%Y-%m-%d')

    return data_


def append_variables(path='',file='',data=None,category=''):

    data_ = data

    text = open(path+'/'+file).read()
    text = re.sub('\\pm','~',text)
    text = re.sub(r'[\\£@#${}^]','',text)

    #Parse top line for variables
    variables = text.split("\n")[0].split(' & ')
    
    for i in range(2,len(variables),1):
        values = []
        errors = []
        for line in text.split('\n'):
            
            if line.split('-runNumber')[0] == category:

                value = line.split(' & ')[i]

                if '--' in value: value = '0'

                if len(value.split('~')) == 2:
                    values.append(float(value.split('~')[0]))
                    errors.append(float(value.split('~')[1]))
                elif len(value.split('~')) == 1:
                    values.append(float(value))
                    errors.append(0.0)

        if 'mc' in variables[i]:
            variables[i] = variables[i].replace("mc","MC")
        if '/' in variables[i]:
            variables[i] = variables[i].replace("/","_")
            
        data_[variables[i]] = values
        data_[variables[i]+'_err'] = errors

    return data_


def plot_stability( time = None, datavalues = None, mcvalue = None, dataerrors = None, mcerror = None, label = '', category = '', path = "", evenX = False):

    left, width = 0.1, 1.0
    bottom, height = 0.1, 0.5
    rect_hist = [left+width+0.01, bottom, 0.2, height]
    rect_plot = [left, bottom, width, height]

    nullfmt = NullFormatter()

    fig = plt.figure()

    ax_plot = plt.axes(rect_plot)
    ax_hist = plt.axes(rect_hist)

    ax_hist.yaxis.set_major_formatter(nullfmt)

    xPlaceholder = range(1,1+len(time),1)
    if evenX:
        ax_plot.errorbar(xPlaceholder,datavalues,yerr=dataerrors,capthick=0,marker='o',ms=5,ls='None',)
    else:
        ax_plot.errorbar(time,datavalues,yerr=dataerrors,capthick=0,marker='o',ms=5,ls='None',)

    # customise the axes
    timevar = time.name
    if timevar == 'time':
        ax_plot.xaxis.set_minor_locator(dates.DayLocator(interval=2))
        ax_plot.xaxis.set_minor_formatter(dates.DateFormatter('%d\n%a'))
        ax_plot.xaxis.set_major_locator(dates.MonthLocator())
        ax_plot.xaxis.set_major_formatter(dates.DateFormatter('\n\n\n%b\n%Y'))
    elif (timevar == 'run_max' or timevar == 'run_min') and not evenX:
        majorLocator = MultipleLocator(125)
        minorLocator = MultipleLocator(62.5)
        ax_plot.xaxis.set_major_locator(majorLocator)
        ax_plot.xaxis.set_minor_locator(minorLocator)
        majorFormatter = FormatStrFormatter('%d')
        ax_plot.xaxis.set_major_formatter(majorFormatter)
        xlabels = ax_plot.get_xticklabels()
        plt.setp(xlabels, rotation=90, fontsize=10)
    elif (timevar == 'run_max' or timevar == 'run_min') and evenX:
        majorLocator = MultipleLocator(2)
        minorLocator = MultipleLocator(1)
        ax_plot.xaxis.set_major_locator(majorLocator)
        ax_plot.xaxis.set_minor_locator(minorLocator)
        xlabels = ax_plot.get_xticks().tolist()

        for i in range(2,len(time),2):
            xlabels[i/2+1] = time.tolist()[i-1]

        for i in range(len(xlabels)):
            if xlabels[i] < 200000: xlabels[i] = ''

        ax_plot.set_xticklabels(xlabels)
        xlabels = ax_plot.get_xticklabels()
        plt.setp(xlabels, rotation=90, fontsize=10)

    ax_plot.xaxis.grid(True, which="minor")
    ax_plot.yaxis.grid()
    ax_hist.xaxis.grid(True, which="minor")
    ax_hist.yaxis.grid()
    ax_hist.xaxis.set_ticks([])

    #Get and set the limits for the histogram
    ymin = round(datavalues.min()) - 1
    ymax = round(datavalues.max()) + 1
    ax_plot.set_ylim((ymin,ymax))
    ax_hist.set_ylim((ymin,ymax))

    ax_plot.set_ylabel(label)

    nbin = 20
    y,_,_ = ax_hist.hist(datavalues, bins=nbin, orientation='horizontal', histtype='stepfilled', alpha=0.6)
    hmax = y.max()
    ax_hist.set_xlim((0,hmax*1.1))

    ax_plot.set_title(region_labels[category] + "    " + label)

    #Annotate with mean and std dev
    npVals = np.asarray(datavalues)
    ax_hist.annotate('Mean = {:3.3f}'.format(np.mean(npVals)),(hmax/6,ymin-(ymax-ymin)*0.1),fontsize=11,annotation_clip=False,xycoords='data')
    ax_hist.annotate('Std dev. = {:3.3f}'.format(np.std(npVals)),(hmax/6,ymin-(ymax-ymin)*0.175),fontsize=11,annotation_clip=False,xycoords='data')
    
    #Add line for the MC 
    if (mcvalue > -999):
        xmin,xmax = 0,1
        ax_plot.axhline(y=mcvalue+mcerror,
                        xmin=xmin,xmax=xmax,c='red',linewidth=1,clip_on=True,linestyle='dashed')
        ax_plot.axhline(y=mcvalue-mcerror,
                        xmin=xmin,xmax=xmax,c='red',linewidth=1,clip_on=True,linestyle='dashed')
        ax_plot.axhline(y=mcvalue,
                        xmin=xmin,xmax=xmax,c='blue',linewidth=1,clip_on=True,linestyle='solid',label='MC')
        ax_hist.annotate('MC = {:3.3f} $\pm$ {:3.3f}'.format(mcvalue,mcerror),(hmax/6,ymin-(ymax-ymin)*0.25),fontsize=11,annotation_clip=False,xycoords='data')
    
    #Legend
    # legend = ax_plot.legend(loc='lower center',numpoints=1)
    # if (mcvalue > -999):
    #     legend.get_texts()[1].set_text('Data')
    # else:
    #     legend.get_texts()[0].set_text('Data')
    
    #Save
    if evenX:
        timevar = timevar + '_even'
        
    if not os.path.exists(path+'/'+timevar):
        os.makedirs(path+'/'+timevar)

    pdfName = category + '_' + re.sub(r'[\\@#$/{}^]','', label) + '.pdf'
    pdfName = re.sub(r'[ ]','_',pdfName)

    print 'Saving plot: ' + path + timevar + '/' + pdfName
    for fr in format_fig_output :
        plt.savefig( path + timevar + '/' + pdfName,
                     format='pdf',orientation='landscape',
                     dpi=900,papertype='a0',pad_inches=0.1,
                     bbox_inches='tight')

    plt.close(fig)








