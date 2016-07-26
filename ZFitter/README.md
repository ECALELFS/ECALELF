### Running `ZFitter` validation 
```bash
./script/validation.sh  -f data/validation/test_75X.dat  \
                        --invMass_var invMass_SC \
                        --validation --slides --baseDir test150715/
```

### Making validation history plots
- The validation plots can be optained by running the `python/stability.py` 
which takes as input the tex table produced by the `ZFitter` and the runRange.dat
that is produced by the runDivide option of the ZFitter. 
- The list of option of the script can be obtained by running :
```sh 
python/stability.py --help 
```

- You can run a small test using :
```sh 
python/stability.py --run-range python/test/test_run_interval_100000.dat \
                    --stability python/test/monitoring_test.tex \
                    --inv-mass  "invMass_e5x5"
```
