(match($1,"phi_")) {
	cat=substr($1,5);
	phis[cat]=$3;
}

(match($1,"rho_")) {
	cat=substr($1,5);
	rhos[cat]=$3;
}
(match($1, "constTerm_") || match($1, "alpha_")){
	store[NR]=$0
}
(!match($1,"phi_") && !match($1,"rho_") && !match($1,"constTerm_")&& !match($1,"alpha_")){
	print $0
}

END {
	e_mean["absEta_0_1-highR9"] = 6.60
	e_mean["absEta_0_1-lowR9"] = 6.73
	e_mean["absEta_1_1.4442-highR9"] = 6.52
	e_mean["absEta_1_1.4442-lowR9"] = 6.72
	e_mean["absEta_1.566_2-highR9"] = 6.76
	e_mean["absEta_1.566_2-lowR9"] = 6.77
	e_mean["absEta_2_2.5-highR9"] = 6.54
	e_mean["absEta_2_2.5-lowR9"] = 6.73
	for(cat in phis)
	{
		if(match(cat,"gold")){
			r9="highR9";
		}
		else if(match(cat,"bad")){
			r9="lowR9";
		}
		match(cat,/(absEta_[0-9.]*_[0-9.]*)-?/,eta)

		constTerm=rhos[cat]*sin(phis[cat])
		alpha=rhos[cat]*e_mean[eta[1] "-" r9]*cos(phis[cat])
		print "constTerm_" cat " = " constTerm " +/- 0.0 L(0. - 0.05)"
		print "alpha_" cat " = " alpha " +/- 0.0 L(0. - 0.15)"
	}
#if there are no phi/alpha terms, print out constTerm and alpha
#this preserves the file if nothing is found
	if(length(phis) == 0) {
		for (i in store)
			print store[i]
	}
}
