#!/usr/bin/env python3
import pprint


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

pprint.pprint(dics)

mean=[]

for i in range(101):
    result_kernel=0
    result_user=0
    result_diff=0
    for j in range(len(dics)):
        result_kernel+=dics[j][i][1]
        result_user+=dics[j][i][2]
        result_diff+=dics[j][i][3]
    mean.append((i, result_kernel/len(dics), result_user/len(dics), result_diff/len(dics)))

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