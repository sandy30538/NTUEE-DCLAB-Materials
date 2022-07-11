#!/usr/bin/env python
from serial import Serial, EIGHTBITS, PARITY_NONE, STOPBITS_ONE
from sys import argv
from struct import unpack, pack

assert len(argv) == 2
s = Serial(
    port=argv[1],
    baudrate=115200,
    bytesize=EIGHTBITS,
    parity=PARITY_NONE,
    stopbits=STOPBITS_ONE,
    xonxoff=False,
    rtscts=False
)

fp_pat  = open('./random_pattern.bin','rb') 
fp_gold = open('./pattern_ans.txt','r')
fp_out  = open('output_result.bin','wb')
assert fp_pat and fp_gold and fp_out

pat = fp_pat.read()
lines = fp_gold.readlines()
assert len(pat) % 64 == 0       #ref 32bytes, read 32bytes
set_num = (len(pat) / 64)
print ("Open files successfully!")
print ("Total pattern num: " + str(set_num))
print ('============')

err_count = 0
for i in range(0, len(pat), 64):
    print ('Pattern ' + str(i/64) +'\n')
    s.write(pat[i:i+32])       # Send in ref seq
    s.write(pat[i+32:i+64])    # Send in read seq
    
    out = s.read(31)           # Receive output result
    fp_out.write(out)
    
    count   = 0
    vals    = unpack("{}B".format(31), out)
    column  = 0
    row     = 0
    score   = 0
    for val in vals:
        if ((count > 6) and (count <= 14)):
            column = (column<<8) | val
        elif ((count > 14) and (count <= 22)):
            row = (row<<8) | val
        elif(count > 22):
            score = (score<<8) | val
            
        count = count + 1    
    
    print ('*Golden*')
    print ('Score: \t' + str(lines[int(i/64)*3+1]))
    print ("Row: \t" + str(lines[int(i/64)*3+2]))
    print ("Column: " + str(lines[int(i/64)*3+3]))
    
    print ('*Output*')
    print ('Score: \t' + str(score))
    print ("Row: \t" + str(row))
    print ("Column: " + str(column))
    
    if (int(lines[int(i/64)*3+1]) != score or int(lines[int(i/64)*3+2]) != row or int(lines[int(i/64)*3+3]) != column):
        err_count = err_count + 1
        print('error')
    else:
        print('correct!')
    print ('============')

if (err_count == 0):
    print('\nYou have passed all patterns!\n')
elif (err_count == 1):
    print('\nThere is 1 error...\n')
else:
    print('\nThere are ' + str(err_count) + ' errors...\n')

fp_pat.close()
fp_gold.close()
fp_out.close()