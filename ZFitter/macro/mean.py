import ROOT
import math

counter_barrel=0
mean_barrel=0

counter_endcap=0
mean_endcap=0

with open('test/dato/10June_Golden_plus_diff_DCS/loose/invMass_SC_pho_regrCorr/table/step2-invMass_SC_pho_regrCorr-loose-Et_20-noPF-HggRunEtaR9.dat') as file_res:
    for line in file_res: 
        numbers_str = line.split()  
        value= float(numbers_str[4])
        category= numbers_str[0]
        #print value
        #print category
        if (("0_1" in category) or ("1_1.4442" in category)): 
            mean_barrel+=value
            counter_barrel+=1
        else:
            mean_endcap+=value
            counter_endcap+=1

corr_EB=mean_barrel/counter_barrel
corr_EE=mean_endcap/counter_endcap
mass_corr_EBEB=corr_EB
mass_corr_EBEE=math.sqrt(corr_EB*corr_EE)
print "mean correction barrel is ", corr_EB
print "mean correction endcap is ", corr_EE
print "mass correction EBEB is ", mass_corr_EBEB
print "mass correction EBEE is ", mass_corr_EBEE
print("**********: EBEB -> 1 - corr: %.3lf" %(1 - mass_corr_EBEB))
print("**********: EBEE -> 1 - corr: %.3lf" %(1 - mass_corr_EBEE))
