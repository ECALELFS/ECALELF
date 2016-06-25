import ROOT
#####READ A FILE###################
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

print "mean correction barrel is ", mean_barrel/counter_barrel
print "mean correction endcap is ",mean_endcap/counter_endcap
