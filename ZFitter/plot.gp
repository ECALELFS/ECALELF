reset

fin_sele = 'output_r9.dat'
fin_dele = 'output_mass.dat'
dplot = 'plots'

regions_sele= system("grep -v '#' ".fin_sele." | awk -F _ '{print $4}' | awk '{print $1}' | sort | uniq | tr '\n' ' '")
regions_sele = "EB mod1 mod2 mod3 mod4"
regions_dele = system("grep -v '#' ".fin_dele." | awk -F _ '{print $4}' | awk '{print $1}' | sort | uniq | tr '\n' ' '")
print 'regions found for single electron: '.regions_sele.' and for double electron: '.regions_dele

varnames = "r9  sieie                                     mass"
varlabel = "R9  '{/Symbol s}_{i{/Symbol h}i{/Symbol h}}' 'm_{ll} (GeV)'"
varcols  = "3   3                                         3"

obscols  = "2                   3     4            5                  6       7      "
obslabel = "'Number of events'  Mean  'Std. Dev.'  'Effective Sigma'  Median  'Effective Mean'"

set terminal pdfcairo enhanced color solid font "Helvetica,16" size 5,4

set auto fix
set offsets graph 0.05, graph 0.05, graph 0.1, graph 0.1
set grid lc rgb '#888888'
set key samplen 1.5
set datafile missing '0'

save '.setup'

system("mkdir -p ".dplot." ".dplot."/points")

allftmp = 'tmp_f1 tmp_f2 tmp_t'
do for [j=1:words(varnames)] {
        vra = word(varnames, j)
        reggs = regions_sele
        if (vra eq 'mass') {
                reggs = regions_dele
        }
        do for [i=1:words(reggs)] {
                reg = word(reggs, i)
                fin = "output_".vra.".dat"
                ftmp = "tmp_".vra."_".reg.".dat"
                system("grep ".reg." ".fin." | awk -F _ '{$1=$2\"_\"$3; $2=\"\"; $3=\"\"; print $0}' | awk '{$2=\"\"; print $0}' | sed '2d' | tr '-' ' ' | tr '_' '-'> tmp_f1")
                system("sed '2d' output_time.dat > tmp_t")
                system("paste tmp_f1 tmp_t > ".ftmp)
                do for [k=1:words(obscols)] {
                        obs = word(obscols, k)
                        lab = system("echo ".word(obslabel, k)."| tr -d ' .' | tr '[:upper:]' '[:lower:]'")
                        oplot = dplot . '/plot_'.lab.'_'.vra.'_'.reg.'.pdf'
                        set x2tics rotate by 90 font ',9'
                        set xtics rotate by 45 offset -2., -1.5 nomirror
                        set key outside above
                        set out oplot
                        set bmargin 4
                        set tmargin 5.5
                        set timefmt "%s"
                        set xdata time
                        set format x "%d/%m"
#set xlabel 'Run range [..., ...)' offset 0, -1.75
                        set xlabel 'Time (day/month)' offset 0, -.5
                        set ylabel word(obslabel, k).' of '.word(varlabel, j)
                        p [][] ftmp u 12:(column(int(word(obscols, k)))):17:18:x2ticlabel(stringcolumn(1)) t reg w xerr pt 7 lt 6 ps .75
                        set out
                }
                #system('latexmk '.oplot)
                histos = system("awk '{print $1\" \"}' ".fin." | tr '-' '_' | tr '\n' ' '")
                print histos
                do for [pl=1:words(histos):2] {
                        xval = word(histos, pl + 1)
                        reset
                        load '.setup'
                        set out dplot . '/points/plot_'.word(histos, pl).'.pdf'
                        idx = 1000 * i + 100 * j + pl
                        print 'arrow: ', xval, ' region:', word(histos, pl), ' ', idx
                        set xlabel word(varlabel, j)
                        set arrow idx from first xval, graph 0 to first xval, graph 1 nohead lt 7
                        set samples 1000
#p [85:95] 'gp_histos/h_'.vra.'_'.word(histos, pl).'.dat' not w p, '' not sm cspl
#p [85:95] 'gp_histos/h_'.word(histos, pl).'.dat' not w p, '' not sm cspl
                        p 'gp_histos/h_'.word(histos, pl).'.dat' not w p, '' not sm cspl
                        set out
                        unset arrow idx
                }
                allftmp = allftmp.' '.ftmp
        }
}

set macros
do for [j=1:words(varnames)] {
        vra = word(varnames, j)
        reggs = regions_sele
        if (vra eq 'mass') {
                reggs = regions_dele
        }
        do for [k=1:words(obscols)] {
                cmd = "p "
                obs = 0 + word(obscols, k)
                do for [i=1:words(reggs)] {
                        reg = word(reggs, i)
                        cmd = cmd." 'tmp_".vra."_".reg.".dat' u 12:(column(".obs.")):17:18:x2ticlabel(stringcolumn(1)) t '".reg."' w xerr pt 7 ps .5, "
                }
                cmd = cmd.' 1/0 not'
                lab = system("echo ".word(obslabel, k)."| tr -d ' .' | tr '[:upper:]' '[:lower:]'")
                oplot = dplot . '/plot_'.lab.'_'.vra.'_all_regions.pdf'
                set x2tics rotate by 90 font ',9'
                set xtics rotate by 45 offset -2., -1.5 nomirror
                set key outside above
                set out oplot
                set bmargin 4
                set tmargin 5.5
                set timefmt "%s"
                set xdata time
                set format x "%d/%m"
#set xlabel 'Run range [..., ...)' offset 0, -1.75
                set xlabel 'Time (day/month)' offset 0, -.5
                set ylabel word(obslabel, k).' of '.word(varlabel, j)
                eval cmd
                set out
        }
}

reset
load '.setup'
system("cat output_nPV.dat | awk -F _ '{$1=$2\"_\"$3; $2=\"\"; $3=\"\"; print $0}' | sed '2d' | tr '-' ' ' | tr '_' '-'> tmp_f3")
system("sed '2d' output_time.dat > tmp_t")
system("paste tmp_f3 tmp_t > tmp_f4")
ftmp = 'tmp_f4'
allftmp = ""
allftmp = allftmp.' tmp_f3 tmp_f4 tmp_t'
do for [k=1:words(obscols)] {
        obs = word(obscols, k)
        lab = system("echo ".word(obslabel, k)."| tr -d ' .' | tr '[:upper:]' '[:lower:]'")
        oplot = dplot . '/plot_'.lab.'_nPV.pdf'
        set out oplot
        set bmargin 4
        set tmargin 5.5
        set key outside above
        set timefmt "%s"
        set xdata time
        set format x "%d/%m"
        set x2tics rotate by 90 font ',9'
        set xtics rotate by 45 offset -2., -1.5 nomirror
        set xlabel 'Time (day/month)' offset 0, -.5
        set ylabel word(obslabel, k).' of nPV'
        p [][] ftmp u 12:(column(int(word(obscols, k)))):17:18:x2ticlabel(stringcolumn(1)) not w xerr pt 7 lt 6 ps .75
        set out
}

system("rm ".allftmp)
system("cd ".dplot." ; for i in *pdf; do echo $i; pdftopng -r 200 $i; done")
system("cd ".dplot."/points ; for i in *pdf; do echo $i; pdftopng -r 200 $i; done")
