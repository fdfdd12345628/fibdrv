#!/usr/bin/env python3
import pprint
import numpy as np

expect = [0, 1]
result = []
result_split = []
dics = []

for i in range(2, 101):
    expect.append(expect[i - 1] + expect[i - 2])
with open('out', 'r') as f:
    tmp = f.readline()
    while (tmp):
        result.append(tmp)
        tmp = f.readline()
    f.close()
for r in result:
    if (r.find('Num') != -1):
        
        result_split.append(r.split(' '))
        num=int(result_split[-1][1][:-1])
        kernel=int((result_split[-1][3][:-1]))
        user=int(float(result_split[-1][5][:-1]))
        diff=int(float(result_split[-1][7][:-1]))
        if num==0:
            dics.append([])
        '''
        k = int(result_split[-1][5].split(',')[0])
        f0 = int(result_split[-1][9].split('.')[0])
        '''
        dics[-1].append((num, kernel, user, diff))

# pprint.pprint(dics)

mean=[]

data=np.array(dics)

print(data[:, 1:2,:])

def outlier_filter(datas, threshold = 2):
    datas = np.array(datas)
    z = np.abs((datas - datas.mean()) / datas.std())
    return datas[z < threshold]

def data_processing(data_set, n):
    catgories = data_set[0].shape[0]
    samples = data_set[0].shape[1]
    final = np.zeros((catgories, samples))

    for c in range(catgories):        
        for s in range(samples):
            final[c][s] =                                                    \
                outlier_filter([data_set[i][c][s] for i in range(n)]).mean()
    return final

for i in range(101):
    kernel_mean = outlier_filter(data[:, i:i+1, 1]).mean()
    user_mean = outlier_filter(data[:, i:i+1, 2]).mean()
    diff_mean = outlier_filter(data[:, i:i+1, 3]).mean()
    mean.append((i, kernel_mean, user_mean, diff_mean))

print("result:")
pprint.pprint(mean)
'''for i in dics:
    print("Num: "+str(i[0]))
    print("Kernel: "+str(i[1]))
    print("User: "+str(i[2]))
'''

import csv

with open('output.csv', 'w+') as fp:
    writer=csv.writer(fp)
    writer.writerows(mean)