reset

if (! exists("lbl")) lbl = "none"

fin_sele = 'output_r9_'.lbl.'.dat'
fin_dele = 'output_mass_'.lbl.'.dat'
dplot = 'out_plots_'.lbl

#regions_sele = system("grep -v '#' ".fin_sele." | awk -F _ '{print $4}' | awk '{print $1}' | sort | uniq | tr '\n' ' '")
if (! exists("regions_sele")) regions_sele = "EB mod1 mod2 mod3 mod4"
#regions_sele = "EB kFirstEB mod1 kMiddleEB mod2 mod3 mod4"
regions_dele = system("grep -v '#' ".fin_dele." | awk -F _ '{print $4}' | awk '{print $1}' | sort | uniq | tr '\n' ' '")
print 'regions found for single electron: '.regions_sele.' and for double electron: '.regions_dele

varnames = "r9    sieie                                     mass"
varlabel = "R9    '{/Symbol s}_{i{/Symbol h}i{/Symbol h}}' 'm_{ll} (GeV)'"
varcols  = "3     3                                         3"
xmin     = "0.85  *                                         75"
xmax     = "1.00  *                                         105"

obscols  = "2                   3      6       7                 4            5                "
obslabel = "'Number of events'  Mean   Median  'Effective Mean'  'Std. Dev.'  'Effective Sigma'"

set terminal pdfcairo enhanced color font "Helvetica,16" size 5,4

set auto fix
set offsets graph 0.05, graph 0.05, graph 0.1, graph 0.1
set grid lc rgb '#888888'
set key samplen 1.5
set datafile missing '0'

save '.setup'

system("mkdir -p ".dplot." ".dplot."/points")

allftmp = 'tmp_f1 tmp_t tmp_t2'
do for [j=1:words(varnames)] {
        vra = word(varnames, j)
        reggs = regions_sele
        if (vra eq 'mass') {
                reggs = regions_dele
        }
        do for [i=1:words(reggs)] {
                reg = word(reggs, i)
                fin = "output_".vra."_".lbl.".dat"
                ftmp = "tmp_".vra."_".reg.".dat"
#system("grep _".reg." ".fin." | awk -F _ '{$1=$2\"_\"$3; $2=\"\"; $3=\"\"; print $0}' | awk '{$2=\"\"; print $0}' | sed '2d' | tr '-' ' ' | tr '_' '-'> tmp_f1")
#system("sed '2d' output_time_".lbl.".dat > tmp_t")
                system("grep _".reg." ".fin." | awk -F _ '{$1=$2\"_\"$3; $2=\"\"; $3=\"\"; print $0}' | awk '{$2=\"\"; print $0}' | sed '/_999999/d' | tr '-' ' ' | tr '_' '-'> tmp_f1")
                system("sed '/_999999/d' output_time_".lbl.".dat > tmp_t")
                system("for rr in \`awk '{print $1}' tmp_f1 | awk -F '-' '{print $1\"_\"$2}'\`; do grep $rr output_time_".lbl.".dat ; done > tmp_t2");
                system("paste tmp_f1 tmp_t2 > ".ftmp)
                do for [k=1:words(obscols)] {
                        obs = word(obscols, k)
                        lab = system("echo ".word(obslabel, k)."| tr -d ' .' | tr '[:upper:]' '[:lower:]'")
                        oplot = dplot . '/plot_'.lab.'_'.vra.'_'.reg.'.pdf'
                        set xtics  nomirror rotate by 45 offset -2., -1.5
                        set x2tics nomirror rotate by 90 font ',3' tc rgb '#aaaaaa'
                        set key outside above
                        set out oplot
                        set bmargin 4
                        set tmargin 3
                        set timefmt "%s"
                        set xdata time
#set x2data time
                        set link x
                        set format x "%d/%m"
#set xlabel 'Run range [..., ...)' offset 0, -1.75
                        set xlabel 'Time (day/month)' offset 0, -.5
                        set ylabel word(obslabel, k).' of '.word(varlabel, j)
                        p [][] ftmp u 12:(column(int(word(obscols, k)))):17:18:x2ticlabel(stringcolumn(1)) t reg w xerr pt 7 lt 6 ps .5
                        set out
                }
                allftmp = allftmp.' '.ftmp
        }
        #system('latexmk '.oplot)
        awkcol = system("echo \\$".obscols." | sed -e 's/ [ ]*/\" \"$/g'")
        histos = system("awk '{print $1\" \"".awkcol."}' ".fin." | tr '-' '_' | tr '\n' ' '")
        histos = system("awk '{print $1\" \"}' ".fin." | tr '-' '_' | tr '\n' ' '")
        print histos
        if (vra eq "mass") {
        do for [pl=1:words(histos)] {
                reset
                load '.setup'
                set offsets graph 0.00, graph 0.00, graph 0.1, graph 0.0
                set out dplot . '/points/plot_'.word(histos, pl).'.pdf'
                print "going to plot: ", 'gp_histos_'.lbl.'/h_'.word(histos, pl).'.dat', " xmin: ", word(xmin, j), " xmax: ", word(xmax, j)
                fakekey = ''
                cmd = "p 'gp_histos_".lbl."/h_".word(histos, pl).".dat' u 1:2:4 not w yerr pt 7 ps .5 lt 6, '' not w histep lc rgb '#888888'"
                ave = 0
                do for [k=2:words(obscols)] {
                        obs  = word(obscols, k)
                        xval = system("grep ".word(histos, pl)." ".fin." | awk '{print $".obs."}'")
                        idx = 1000 * i + 100 * j + 10 * k + pl
                        print 'arrow: ', xval, ' region:', word(histos, pl), ' ', idx
                        if (k == 2) {
                                ave = xval
                        }
                        if (k == 5 || k == 6) {
                                ypos = 0.05 + (k % 3) * 0.025
                                set arrow idx from first ave - xval / 2, graph ypos to first ave + xval / 2, graph ypos heads size 0.5,90 lt k - 1
                        } else {
                                set arrow idx from first xval, graph 0 to first xval, graph 1 nohead lt k - 1
                        }
                        fakekey = ' , 1/0 t "'.word(obslabel, k).'" lw 2 lt '.sprintf("%d", k - 1)
                        cmd = cmd . fakekey
                }
                set samples 1000
#p [85:95] 'gp_histos/h_'.vra.'_'.word(histos, pl).'.dat' not w p, '' not sm cspl
#p [85:95] 'gp_histos/h_'.word(histos, pl).'.dat' not w p, '' not sm cspl
                if (vra eq "mass" || vra eq "r9") set xrange [word(xmin, j):word(xmax, j)] 
                set xlabel word(varlabel, j)
#p [89:91.5] 'gp_histos_'.lbl.'/h_'.word(histos, pl).'.dat' u 1:2:4 not w yerr pt 7 ps .5 lt 6, '' not w histep lc rgb '#888888' @fakekey
                eval cmd
                set out
#                        unset arrow idx
        }
        }
}

set macros
reset
load '.setup'
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
                oplot = dplot . '/plot_'.lab.'_'.vra.'_'.system("echo ".reggs." | tr ' ' '_'").'_regions.pdf'
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

# plot inclusive distributions from different regions on the same histogram
reset
load '.setup'
set macros
set grid lc rgb "#aaaaaa"
do for [j=1:words(varnames)] {
        vra = word(varnames, j)
        reggs = regions_sele
        if (vra eq 'mass') {
                reggs = regions_dele
        }
        fin = "output_".vra."_".lbl.".dat"
        cmd = "p [".word(xmin, j).":".word(xmax, j)."]"
        do for [i=1:words(reggs)] {
                reg = word(reggs, i)
                        ftmp = system("ls ")
                        histo = system("awk '{print $1}' ".fin." | tr '-' '_' | grep _999999 | grep _".reg)
                        print histo
                        stats "gp_histos_".lbl."/h_".histo.".dat" u 2 nooutput
                        M   = sprintf("%20.8f", STATS_sum)
                        cmd = cmd." 'gp_histos_".lbl."/h_".histo.".dat' u 1:($2 / ".M."):($4 / ".M.") t '".reg."' w yerr pt 7 ps .5, "
#                        cmd = cmd." 'gp_histos_".lbl."/h_".histo.".dat' u 1:($2 / ".M."):($4 / ".M.") t '".reg."' w l, "
        }
        cmd = cmd.' 1/0 not'
        oplot = dplot . '/plot_'.vra.'_'.system("echo ".reggs." | tr ' ' '_'").'.pdf'
        set key outside above
        set out oplot
        set xlabel word(varlabel, j) offset 0, -.5
        set ylabel "a.u.\n{/=12 (normalization to 1 on the full ".word(varlabel, j)." range)}"
        eval cmd
        set out
}

reset
load '.setup'
#system("cat output_nPV.dat | awk -F _ '{$1=$2\"_\"$3; $2=\"\"; $3=\"\"; print $0}' | sed '2d' | tr '-' ' ' | tr '_' '-'> tmp_f3")
#system("sed '2d' output_time.dat > tmp_t3")
system("cat output_nPV_".lbl.".dat | awk -F _ '{$1=$2\"_\"$3; $2=\"\"; $3=\"\"; print $0}' | sed '/_999999/d' | tr '-' ' ' | tr '_' '-'> tmp_f3")
system("sed '/_999999/d' output_time_".lbl.".dat > tmp_t3")
system("paste tmp_f3 tmp_t3 > tmp_f4")
ftmp = 'tmp_f4'
#allftmp = ""
allftmp = allftmp.' tmp_f3 tmp_f4 tmp_t3'
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
        p [][] ftmp u 12:(column(int(word(obscols, k)))):17:18:x2ticlabel(stringcolumn(1)) not w xerr pt 7 lt 6 ps .5
        set out
}




#system("rm ".allftmp)
#system("cd ".dplot." ; for i in *pdf; do echo $i; pdftopng -r 200 $i; done")
#system("cd ".dplot."/points ; for i in *pdf; do echo $i; pdftopng -r 200 $i; done")
