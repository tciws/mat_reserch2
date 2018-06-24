# -*- coding:utf-8 -*-
import time
start = time.time()
f = open('6.log','r')

for i in f:
    i = i.split(",")
    if(len(i)>1):
        i = i[1].split("value=")
        #value.append(i[1])
        print(i[1])
f.close()
elapsed_time = time.time() - start
print ("elapsed_time:{0}".format(elapsed_time) + "[sec]")
