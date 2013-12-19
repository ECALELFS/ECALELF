(NF!=0){
  cat=$1
  rho=$2
  rho_err=$3
  phi=$4
  phi_err=$5
  Emean=$6
  printf("#%s\t%.3f\t%.3f\n", cat, rho*sin(phi), rho*Emean*cos(phi))
  printf("constTerm_%s = %.3f +/- 0.00 L(0 - 0.03)\n", cat, rho*sin(phi))
  printf("alpha_%s     = %.3f +/- 0.00 L(0 - 0.20)\n", cat, rho*Emean*cos(phi))
}
