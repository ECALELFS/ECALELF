reset

fin = 'output.dat'

regions = system("grep -v '#' ".fin." | awk -F - '{print $1}' | sort | uniq | tr '\n' ' '")
regions = "EB mod1 mod2 mod3 mod4"
print 'regions found: '.regions
varnames = "mass"
varcols  = "16   14"

set terminal pdfcairo enhanced color solid font "Helvetica,16" size 5,4

set auto fix
set offsets graph 0.05, graph 0.05, graph 0.1, graph 0.1
set grid lc rgb '#888888'

save '.setup'

do for [i=1:words(regions)] {
        do for [j=1:words(varnames)] {
                reg = word(regions, i)
                vra = word(varnames, j)
                ftmp = "tmp_".vra."_".reg.".dat"
                system("grep ".reg." ".fin." | tr '-' ' ' | tr '_' '-'> ".ftmp)
                oplot = 'plot_'.vra.'_'.reg.'.pdf'
                set out oplot
                set bmargin 5
                set xtics rotate by 90 offset 0., -3. font ',10'
                set xlabel 'Run range [..., ...)' offset 0, -1.75
                set ylabel 'median of m_{ll}  (GeV)'
                p [][85:95] ftmp u 0:(column(int(word(varcols, j)))):xticlabel(stringcolumn(2)) not w p pt 7 lt 6 ps .75
                set out
                #system('latexmk '.oplot)
                histos = system("awk '{print $1\"_\"$2\" \"$".word(varcols, j)."}' ".ftmp." | tr '-' '_' | tr '\n' ' '")
                print histos
                do for [pl=1:words(histos):2] {
                        xval = word(histos, pl + 1)
                        reset
                        load '.setup'
                        set out 'plot_'.vra.'_'.word(histos, pl).'.pdf'
                        idx = 1000 * i + 100 * j + pl
                        print 'arrow: ', xval, ' region:', word(histos, pl), ' ', idx
                        set arrow idx from first xval, graph 0 to first xval, graph 1 nohead lt 7
                        set samples 1000
                        p [85:95] 'gp_histos/h_'.vra.'_'.word(histos, pl).'.dat' not w p, '' not sm cspl
                        set out
                        unset arrow idx
                }
        }
}

set macros
cmd = "p "
do for [j=1:words(varnames)] {
        do for [i=1:words(regions)] {
                reg = word(regions, i)
                vra = word(varnames, j)
                cmd = cmd." 'tmp_".vra."_".reg.".dat' u 0:(column(int(word(varcols, ".j.")))):xticlabel(stringcolumn(2)) t '".reg."' w linespoint pt 7 ps .5, "
        }
        cmd = cmd.' 1/0 not'
        oplot = 'plot_'.vra.'_all_regions.pdf'
        set out oplot
        set bmargin 5
        set xtics rotate by 90 offset 0., -3. font ',10'
        set xlabel 'Run range [..., ...)' offset 0, -1.75
        set ylabel 'median of m_{ll}  (GeV)'
        set key outside above
        eval cmd
        set out
}
