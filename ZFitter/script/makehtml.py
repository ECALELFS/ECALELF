#script to write an html file for each category, used in report.sh
#this places the mc and data plots side by side for comparison
import os, sys, fnmatch


from optparse import OptionParser
parser = OptionParser()
parser.add_option("-s","--mcPath",action="store", type="str")
parser.add_option("-f","--imgFormat",action="store", type="str")

parser.add_option("-d","--dataPath",action="store", type="str")

(options,args)=parser.parse_args()
print(options.mcPath);
print(options.dataPath);


out = open('Report.html','w')
mcfils=[]
datafils=[]

for root, dirs, files in os.walk(options.mcPath):
  for f in fnmatch.filter(files,'*.'+options.imgFormat):
     mcfils.append(f)

for root, dirs, files in os.walk(options.dataPath):
  for f in fnmatch.filter(files,'*.'+options.imgFormat):
     datafils.append(f)


mcfils.sort()
datafils.sort()

out.write('<html>\n')
out.write('<body>\n')
out.write('<h1>Z Calibration Fit report</h1><br/>\n')

for mcf in mcfils:
   name=os.path.basename(mcf).split('.'+options.imgFormat)[0]
   out.write('<a name='+name+'>\n')
   out.write('<h2>'+name+'</h2><br>\n')
   out.write('<a href=\"calibplots/mc/'+mcf+'\"><img width=\"500\" src=\"calibplots/mc/'+mcf+'\"></a>\n')
   out .write('<a href=\"calibplots/data/'+mcf+'\"><img width=\"500\" src=\"calibplots/data/'+mcf+'\"></a>\n')
