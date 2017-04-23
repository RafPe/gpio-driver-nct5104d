# gpio-driver-nct5104d
GPIO driver for nct5104d with PCEngines APU board support

# Using driver 
Driver has been compiled under Centos 7.3. In order to use the driver download the repository.

# Using nct5104dctl for chip management 
```
nct5104dctl -a get -r 0x07 
```

# Using nct5104dgpio for pin management 
```
nct5104dgpio --read-all
nct5104dgpio -r 1
nct5104dgpio -s 2 -d out
nct5104dgpio -s 5 -d in 
```