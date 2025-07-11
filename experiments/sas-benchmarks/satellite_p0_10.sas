begin_version
3
end_version
begin_metric
0
end_metric
29
begin_variable
var0
-1
2
Atom power_on(ins1)
NegatedAtom power_on(ins1)
end_variable
begin_variable
var1
-1
2
Atom power_on(ins6)
NegatedAtom power_on(ins6)
end_variable
begin_variable
var2
-1
2
Atom power_on(ins7)
NegatedAtom power_on(ins7)
end_variable
begin_variable
var3
-1
2
Atom power_avail(sat5)
NegatedAtom power_avail(sat5)
end_variable
begin_variable
var4
-1
2
Atom power_on(ins5)
NegatedAtom power_on(ins5)
end_variable
begin_variable
var5
-1
2
Atom power_on(ins8)
NegatedAtom power_on(ins8)
end_variable
begin_variable
var6
-1
2
Atom power_avail(sat4)
NegatedAtom power_avail(sat4)
end_variable
begin_variable
var7
-1
2
Atom power_avail(sat3)
NegatedAtom power_avail(sat3)
end_variable
begin_variable
var8
-1
2
Atom power_on(ins2)
NegatedAtom power_on(ins2)
end_variable
begin_variable
var9
-1
2
Atom power_avail(sat2)
NegatedAtom power_avail(sat2)
end_variable
begin_variable
var10
-1
2
Atom power_on(ins3)
NegatedAtom power_on(ins3)
end_variable
begin_variable
var11
-1
2
Atom power_avail(sat1)
NegatedAtom power_avail(sat1)
end_variable
begin_variable
var12
-1
2
Atom power_on(ins4)
NegatedAtom power_on(ins4)
end_variable
begin_variable
var13
-1
6
Atom pointing(sat5, dir1)
Atom pointing(sat5, dir2)
Atom pointing(sat5, dir3)
Atom pointing(sat5, dir4)
Atom pointing(sat5, dir5)
Atom pointing(sat5, dir6)
end_variable
begin_variable
var14
-1
6
Atom pointing(sat4, dir1)
Atom pointing(sat4, dir2)
Atom pointing(sat4, dir3)
Atom pointing(sat4, dir4)
Atom pointing(sat4, dir5)
Atom pointing(sat4, dir6)
end_variable
begin_variable
var15
-1
6
Atom pointing(sat3, dir1)
Atom pointing(sat3, dir2)
Atom pointing(sat3, dir3)
Atom pointing(sat3, dir4)
Atom pointing(sat3, dir5)
Atom pointing(sat3, dir6)
end_variable
begin_variable
var16
-1
6
Atom pointing(sat2, dir1)
Atom pointing(sat2, dir2)
Atom pointing(sat2, dir3)
Atom pointing(sat2, dir4)
Atom pointing(sat2, dir5)
Atom pointing(sat2, dir6)
end_variable
begin_variable
var17
-1
6
Atom pointing(sat1, dir1)
Atom pointing(sat1, dir2)
Atom pointing(sat1, dir3)
Atom pointing(sat1, dir4)
Atom pointing(sat1, dir5)
Atom pointing(sat1, dir6)
end_variable
begin_variable
var18
-1
2
Atom calibrated(ins8)
NegatedAtom calibrated(ins8)
end_variable
begin_variable
var19
-1
2
Atom calibrated(ins7)
NegatedAtom calibrated(ins7)
end_variable
begin_variable
var20
-1
2
Atom calibrated(ins6)
NegatedAtom calibrated(ins6)
end_variable
begin_variable
var21
-1
2
Atom calibrated(ins5)
NegatedAtom calibrated(ins5)
end_variable
begin_variable
var22
-1
2
Atom calibrated(ins4)
NegatedAtom calibrated(ins4)
end_variable
begin_variable
var23
-1
2
Atom calibrated(ins3)
NegatedAtom calibrated(ins3)
end_variable
begin_variable
var24
-1
2
Atom calibrated(ins2)
NegatedAtom calibrated(ins2)
end_variable
begin_variable
var25
-1
2
Atom calibrated(ins1)
NegatedAtom calibrated(ins1)
end_variable
begin_variable
var26
-1
2
Atom have_image(dir5, mod1)
NegatedAtom have_image(dir5, mod1)
end_variable
begin_variable
var27
-1
2
Atom have_image(dir4, mod1)
NegatedAtom have_image(dir4, mod1)
end_variable
begin_variable
var28
-1
2
Atom have_image(dir1, mod1)
NegatedAtom have_image(dir1, mod1)
end_variable
0
begin_state
1
1
1
0
1
1
0
0
1
0
1
0
1
2
4
3
5
2
1
1
1
1
1
1
1
1
1
1
1
end_state
begin_goal
3
26 0
27 0
28 0
end_goal
198
begin_operator
calibrate sat1 ins4 dir6
2
17 5
12 0
1
0 22 -1 0
1
end_operator
begin_operator
calibrate sat2 ins3 dir2
2
16 1
10 0
1
0 23 -1 0
1
end_operator
begin_operator
calibrate sat3 ins2 dir5
2
15 4
8 0
1
0 24 -1 0
1
end_operator
begin_operator
calibrate sat4 ins5 dir6
2
14 5
4 0
1
0 21 -1 0
1
end_operator
begin_operator
calibrate sat4 ins8 dir3
2
14 2
5 0
1
0 18 -1 0
1
end_operator
begin_operator
calibrate sat5 ins1 dir6
2
13 5
0 0
1
0 25 -1 0
1
end_operator
begin_operator
calibrate sat5 ins6 dir6
2
13 5
1 0
1
0 20 -1 0
1
end_operator
begin_operator
calibrate sat5 ins7 dir1
2
13 0
2 0
1
0 19 -1 0
1
end_operator
begin_operator
switch_off ins1 sat5
0
2
0 3 -1 0
0 0 0 1
1
end_operator
begin_operator
switch_off ins2 sat3
0
2
0 7 -1 0
0 8 0 1
1
end_operator
begin_operator
switch_off ins3 sat2
0
2
0 9 -1 0
0 10 0 1
1
end_operator
begin_operator
switch_off ins4 sat1
0
2
0 11 -1 0
0 12 0 1
1
end_operator
begin_operator
switch_off ins5 sat4
0
2
0 6 -1 0
0 4 0 1
1
end_operator
begin_operator
switch_off ins6 sat5
0
2
0 3 -1 0
0 1 0 1
1
end_operator
begin_operator
switch_off ins7 sat5
0
2
0 3 -1 0
0 2 0 1
1
end_operator
begin_operator
switch_off ins8 sat4
0
2
0 6 -1 0
0 5 0 1
1
end_operator
begin_operator
switch_on ins1 sat5
0
3
0 25 -1 1
0 3 0 1
0 0 -1 0
1
end_operator
begin_operator
switch_on ins2 sat3
0
3
0 24 -1 1
0 7 0 1
0 8 -1 0
1
end_operator
begin_operator
switch_on ins3 sat2
0
3
0 23 -1 1
0 9 0 1
0 10 -1 0
1
end_operator
begin_operator
switch_on ins4 sat1
0
3
0 22 -1 1
0 11 0 1
0 12 -1 0
1
end_operator
begin_operator
switch_on ins5 sat4
0
3
0 21 -1 1
0 6 0 1
0 4 -1 0
1
end_operator
begin_operator
switch_on ins6 sat5
0
3
0 20 -1 1
0 3 0 1
0 1 -1 0
1
end_operator
begin_operator
switch_on ins7 sat5
0
3
0 19 -1 1
0 3 0 1
0 2 -1 0
1
end_operator
begin_operator
switch_on ins8 sat4
0
3
0 18 -1 1
0 6 0 1
0 5 -1 0
1
end_operator
begin_operator
take_image sat1 dir1 ins4 mod1
3
22 0
17 0
12 0
1
0 28 -1 0
1
end_operator
begin_operator
take_image sat1 dir4 ins4 mod1
3
22 0
17 3
12 0
1
0 27 -1 0
1
end_operator
begin_operator
take_image sat1 dir5 ins4 mod1
3
22 0
17 4
12 0
1
0 26 -1 0
1
end_operator
begin_operator
take_image sat2 dir1 ins3 mod1
3
23 0
16 0
10 0
1
0 28 -1 0
1
end_operator
begin_operator
take_image sat2 dir4 ins3 mod1
3
23 0
16 3
10 0
1
0 27 -1 0
1
end_operator
begin_operator
take_image sat2 dir5 ins3 mod1
3
23 0
16 4
10 0
1
0 26 -1 0
1
end_operator
begin_operator
take_image sat3 dir1 ins2 mod1
3
24 0
15 0
8 0
1
0 28 -1 0
1
end_operator
begin_operator
take_image sat3 dir4 ins2 mod1
3
24 0
15 3
8 0
1
0 27 -1 0
1
end_operator
begin_operator
take_image sat3 dir5 ins2 mod1
3
24 0
15 4
8 0
1
0 26 -1 0
1
end_operator
begin_operator
take_image sat4 dir1 ins5 mod1
3
21 0
14 0
4 0
1
0 28 -1 0
1
end_operator
begin_operator
take_image sat4 dir1 ins8 mod1
3
18 0
14 0
5 0
1
0 28 -1 0
1
end_operator
begin_operator
take_image sat4 dir4 ins5 mod1
3
21 0
14 3
4 0
1
0 27 -1 0
1
end_operator
begin_operator
take_image sat4 dir4 ins8 mod1
3
18 0
14 3
5 0
1
0 27 -1 0
1
end_operator
begin_operator
take_image sat4 dir5 ins5 mod1
3
21 0
14 4
4 0
1
0 26 -1 0
1
end_operator
begin_operator
take_image sat4 dir5 ins8 mod1
3
18 0
14 4
5 0
1
0 26 -1 0
1
end_operator
begin_operator
take_image sat5 dir1 ins1 mod1
3
25 0
13 0
0 0
1
0 28 -1 0
1
end_operator
begin_operator
take_image sat5 dir1 ins6 mod1
3
20 0
13 0
1 0
1
0 28 -1 0
1
end_operator
begin_operator
take_image sat5 dir1 ins7 mod1
3
19 0
13 0
2 0
1
0 28 -1 0
1
end_operator
begin_operator
take_image sat5 dir4 ins1 mod1
3
25 0
13 3
0 0
1
0 27 -1 0
1
end_operator
begin_operator
take_image sat5 dir4 ins6 mod1
3
20 0
13 3
1 0
1
0 27 -1 0
1
end_operator
begin_operator
take_image sat5 dir4 ins7 mod1
3
19 0
13 3
2 0
1
0 27 -1 0
1
end_operator
begin_operator
take_image sat5 dir5 ins1 mod1
3
25 0
13 4
0 0
1
0 26 -1 0
1
end_operator
begin_operator
take_image sat5 dir5 ins6 mod1
3
20 0
13 4
1 0
1
0 26 -1 0
1
end_operator
begin_operator
take_image sat5 dir5 ins7 mod1
3
19 0
13 4
2 0
1
0 26 -1 0
1
end_operator
begin_operator
turn_to sat1 dir1 dir2
0
1
0 17 1 0
1
end_operator
begin_operator
turn_to sat1 dir1 dir3
0
1
0 17 2 0
1
end_operator
begin_operator
turn_to sat1 dir1 dir4
0
1
0 17 3 0
1
end_operator
begin_operator
turn_to sat1 dir1 dir5
0
1
0 17 4 0
1
end_operator
begin_operator
turn_to sat1 dir1 dir6
0
1
0 17 5 0
1
end_operator
begin_operator
turn_to sat1 dir2 dir1
0
1
0 17 0 1
1
end_operator
begin_operator
turn_to sat1 dir2 dir3
0
1
0 17 2 1
1
end_operator
begin_operator
turn_to sat1 dir2 dir4
0
1
0 17 3 1
1
end_operator
begin_operator
turn_to sat1 dir2 dir5
0
1
0 17 4 1
1
end_operator
begin_operator
turn_to sat1 dir2 dir6
0
1
0 17 5 1
1
end_operator
begin_operator
turn_to sat1 dir3 dir1
0
1
0 17 0 2
1
end_operator
begin_operator
turn_to sat1 dir3 dir2
0
1
0 17 1 2
1
end_operator
begin_operator
turn_to sat1 dir3 dir4
0
1
0 17 3 2
1
end_operator
begin_operator
turn_to sat1 dir3 dir5
0
1
0 17 4 2
1
end_operator
begin_operator
turn_to sat1 dir3 dir6
0
1
0 17 5 2
1
end_operator
begin_operator
turn_to sat1 dir4 dir1
0
1
0 17 0 3
1
end_operator
begin_operator
turn_to sat1 dir4 dir2
0
1
0 17 1 3
1
end_operator
begin_operator
turn_to sat1 dir4 dir3
0
1
0 17 2 3
1
end_operator
begin_operator
turn_to sat1 dir4 dir5
0
1
0 17 4 3
1
end_operator
begin_operator
turn_to sat1 dir4 dir6
0
1
0 17 5 3
1
end_operator
begin_operator
turn_to sat1 dir5 dir1
0
1
0 17 0 4
1
end_operator
begin_operator
turn_to sat1 dir5 dir2
0
1
0 17 1 4
1
end_operator
begin_operator
turn_to sat1 dir5 dir3
0
1
0 17 2 4
1
end_operator
begin_operator
turn_to sat1 dir5 dir4
0
1
0 17 3 4
1
end_operator
begin_operator
turn_to sat1 dir5 dir6
0
1
0 17 5 4
1
end_operator
begin_operator
turn_to sat1 dir6 dir1
0
1
0 17 0 5
1
end_operator
begin_operator
turn_to sat1 dir6 dir2
0
1
0 17 1 5
1
end_operator
begin_operator
turn_to sat1 dir6 dir3
0
1
0 17 2 5
1
end_operator
begin_operator
turn_to sat1 dir6 dir4
0
1
0 17 3 5
1
end_operator
begin_operator
turn_to sat1 dir6 dir5
0
1
0 17 4 5
1
end_operator
begin_operator
turn_to sat2 dir1 dir2
0
1
0 16 1 0
1
end_operator
begin_operator
turn_to sat2 dir1 dir3
0
1
0 16 2 0
1
end_operator
begin_operator
turn_to sat2 dir1 dir4
0
1
0 16 3 0
1
end_operator
begin_operator
turn_to sat2 dir1 dir5
0
1
0 16 4 0
1
end_operator
begin_operator
turn_to sat2 dir1 dir6
0
1
0 16 5 0
1
end_operator
begin_operator
turn_to sat2 dir2 dir1
0
1
0 16 0 1
1
end_operator
begin_operator
turn_to sat2 dir2 dir3
0
1
0 16 2 1
1
end_operator
begin_operator
turn_to sat2 dir2 dir4
0
1
0 16 3 1
1
end_operator
begin_operator
turn_to sat2 dir2 dir5
0
1
0 16 4 1
1
end_operator
begin_operator
turn_to sat2 dir2 dir6
0
1
0 16 5 1
1
end_operator
begin_operator
turn_to sat2 dir3 dir1
0
1
0 16 0 2
1
end_operator
begin_operator
turn_to sat2 dir3 dir2
0
1
0 16 1 2
1
end_operator
begin_operator
turn_to sat2 dir3 dir4
0
1
0 16 3 2
1
end_operator
begin_operator
turn_to sat2 dir3 dir5
0
1
0 16 4 2
1
end_operator
begin_operator
turn_to sat2 dir3 dir6
0
1
0 16 5 2
1
end_operator
begin_operator
turn_to sat2 dir4 dir1
0
1
0 16 0 3
1
end_operator
begin_operator
turn_to sat2 dir4 dir2
0
1
0 16 1 3
1
end_operator
begin_operator
turn_to sat2 dir4 dir3
0
1
0 16 2 3
1
end_operator
begin_operator
turn_to sat2 dir4 dir5
0
1
0 16 4 3
1
end_operator
begin_operator
turn_to sat2 dir4 dir6
0
1
0 16 5 3
1
end_operator
begin_operator
turn_to sat2 dir5 dir1
0
1
0 16 0 4
1
end_operator
begin_operator
turn_to sat2 dir5 dir2
0
1
0 16 1 4
1
end_operator
begin_operator
turn_to sat2 dir5 dir3
0
1
0 16 2 4
1
end_operator
begin_operator
turn_to sat2 dir5 dir4
0
1
0 16 3 4
1
end_operator
begin_operator
turn_to sat2 dir5 dir6
0
1
0 16 5 4
1
end_operator
begin_operator
turn_to sat2 dir6 dir1
0
1
0 16 0 5
1
end_operator
begin_operator
turn_to sat2 dir6 dir2
0
1
0 16 1 5
1
end_operator
begin_operator
turn_to sat2 dir6 dir3
0
1
0 16 2 5
1
end_operator
begin_operator
turn_to sat2 dir6 dir4
0
1
0 16 3 5
1
end_operator
begin_operator
turn_to sat2 dir6 dir5
0
1
0 16 4 5
1
end_operator
begin_operator
turn_to sat3 dir1 dir2
0
1
0 15 1 0
1
end_operator
begin_operator
turn_to sat3 dir1 dir3
0
1
0 15 2 0
1
end_operator
begin_operator
turn_to sat3 dir1 dir4
0
1
0 15 3 0
1
end_operator
begin_operator
turn_to sat3 dir1 dir5
0
1
0 15 4 0
1
end_operator
begin_operator
turn_to sat3 dir1 dir6
0
1
0 15 5 0
1
end_operator
begin_operator
turn_to sat3 dir2 dir1
0
1
0 15 0 1
1
end_operator
begin_operator
turn_to sat3 dir2 dir3
0
1
0 15 2 1
1
end_operator
begin_operator
turn_to sat3 dir2 dir4
0
1
0 15 3 1
1
end_operator
begin_operator
turn_to sat3 dir2 dir5
0
1
0 15 4 1
1
end_operator
begin_operator
turn_to sat3 dir2 dir6
0
1
0 15 5 1
1
end_operator
begin_operator
turn_to sat3 dir3 dir1
0
1
0 15 0 2
1
end_operator
begin_operator
turn_to sat3 dir3 dir2
0
1
0 15 1 2
1
end_operator
begin_operator
turn_to sat3 dir3 dir4
0
1
0 15 3 2
1
end_operator
begin_operator
turn_to sat3 dir3 dir5
0
1
0 15 4 2
1
end_operator
begin_operator
turn_to sat3 dir3 dir6
0
1
0 15 5 2
1
end_operator
begin_operator
turn_to sat3 dir4 dir1
0
1
0 15 0 3
1
end_operator
begin_operator
turn_to sat3 dir4 dir2
0
1
0 15 1 3
1
end_operator
begin_operator
turn_to sat3 dir4 dir3
0
1
0 15 2 3
1
end_operator
begin_operator
turn_to sat3 dir4 dir5
0
1
0 15 4 3
1
end_operator
begin_operator
turn_to sat3 dir4 dir6
0
1
0 15 5 3
1
end_operator
begin_operator
turn_to sat3 dir5 dir1
0
1
0 15 0 4
1
end_operator
begin_operator
turn_to sat3 dir5 dir2
0
1
0 15 1 4
1
end_operator
begin_operator
turn_to sat3 dir5 dir3
0
1
0 15 2 4
1
end_operator
begin_operator
turn_to sat3 dir5 dir4
0
1
0 15 3 4
1
end_operator
begin_operator
turn_to sat3 dir5 dir6
0
1
0 15 5 4
1
end_operator
begin_operator
turn_to sat3 dir6 dir1
0
1
0 15 0 5
1
end_operator
begin_operator
turn_to sat3 dir6 dir2
0
1
0 15 1 5
1
end_operator
begin_operator
turn_to sat3 dir6 dir3
0
1
0 15 2 5
1
end_operator
begin_operator
turn_to sat3 dir6 dir4
0
1
0 15 3 5
1
end_operator
begin_operator
turn_to sat3 dir6 dir5
0
1
0 15 4 5
1
end_operator
begin_operator
turn_to sat4 dir1 dir2
0
1
0 14 1 0
1
end_operator
begin_operator
turn_to sat4 dir1 dir3
0
1
0 14 2 0
1
end_operator
begin_operator
turn_to sat4 dir1 dir4
0
1
0 14 3 0
1
end_operator
begin_operator
turn_to sat4 dir1 dir5
0
1
0 14 4 0
1
end_operator
begin_operator
turn_to sat4 dir1 dir6
0
1
0 14 5 0
1
end_operator
begin_operator
turn_to sat4 dir2 dir1
0
1
0 14 0 1
1
end_operator
begin_operator
turn_to sat4 dir2 dir3
0
1
0 14 2 1
1
end_operator
begin_operator
turn_to sat4 dir2 dir4
0
1
0 14 3 1
1
end_operator
begin_operator
turn_to sat4 dir2 dir5
0
1
0 14 4 1
1
end_operator
begin_operator
turn_to sat4 dir2 dir6
0
1
0 14 5 1
1
end_operator
begin_operator
turn_to sat4 dir3 dir1
0
1
0 14 0 2
1
end_operator
begin_operator
turn_to sat4 dir3 dir2
0
1
0 14 1 2
1
end_operator
begin_operator
turn_to sat4 dir3 dir4
0
1
0 14 3 2
1
end_operator
begin_operator
turn_to sat4 dir3 dir5
0
1
0 14 4 2
1
end_operator
begin_operator
turn_to sat4 dir3 dir6
0
1
0 14 5 2
1
end_operator
begin_operator
turn_to sat4 dir4 dir1
0
1
0 14 0 3
1
end_operator
begin_operator
turn_to sat4 dir4 dir2
0
1
0 14 1 3
1
end_operator
begin_operator
turn_to sat4 dir4 dir3
0
1
0 14 2 3
1
end_operator
begin_operator
turn_to sat4 dir4 dir5
0
1
0 14 4 3
1
end_operator
begin_operator
turn_to sat4 dir4 dir6
0
1
0 14 5 3
1
end_operator
begin_operator
turn_to sat4 dir5 dir1
0
1
0 14 0 4
1
end_operator
begin_operator
turn_to sat4 dir5 dir2
0
1
0 14 1 4
1
end_operator
begin_operator
turn_to sat4 dir5 dir3
0
1
0 14 2 4
1
end_operator
begin_operator
turn_to sat4 dir5 dir4
0
1
0 14 3 4
1
end_operator
begin_operator
turn_to sat4 dir5 dir6
0
1
0 14 5 4
1
end_operator
begin_operator
turn_to sat4 dir6 dir1
0
1
0 14 0 5
1
end_operator
begin_operator
turn_to sat4 dir6 dir2
0
1
0 14 1 5
1
end_operator
begin_operator
turn_to sat4 dir6 dir3
0
1
0 14 2 5
1
end_operator
begin_operator
turn_to sat4 dir6 dir4
0
1
0 14 3 5
1
end_operator
begin_operator
turn_to sat4 dir6 dir5
0
1
0 14 4 5
1
end_operator
begin_operator
turn_to sat5 dir1 dir2
0
1
0 13 1 0
1
end_operator
begin_operator
turn_to sat5 dir1 dir3
0
1
0 13 2 0
1
end_operator
begin_operator
turn_to sat5 dir1 dir4
0
1
0 13 3 0
1
end_operator
begin_operator
turn_to sat5 dir1 dir5
0
1
0 13 4 0
1
end_operator
begin_operator
turn_to sat5 dir1 dir6
0
1
0 13 5 0
1
end_operator
begin_operator
turn_to sat5 dir2 dir1
0
1
0 13 0 1
1
end_operator
begin_operator
turn_to sat5 dir2 dir3
0
1
0 13 2 1
1
end_operator
begin_operator
turn_to sat5 dir2 dir4
0
1
0 13 3 1
1
end_operator
begin_operator
turn_to sat5 dir2 dir5
0
1
0 13 4 1
1
end_operator
begin_operator
turn_to sat5 dir2 dir6
0
1
0 13 5 1
1
end_operator
begin_operator
turn_to sat5 dir3 dir1
0
1
0 13 0 2
1
end_operator
begin_operator
turn_to sat5 dir3 dir2
0
1
0 13 1 2
1
end_operator
begin_operator
turn_to sat5 dir3 dir4
0
1
0 13 3 2
1
end_operator
begin_operator
turn_to sat5 dir3 dir5
0
1
0 13 4 2
1
end_operator
begin_operator
turn_to sat5 dir3 dir6
0
1
0 13 5 2
1
end_operator
begin_operator
turn_to sat5 dir4 dir1
0
1
0 13 0 3
1
end_operator
begin_operator
turn_to sat5 dir4 dir2
0
1
0 13 1 3
1
end_operator
begin_operator
turn_to sat5 dir4 dir3
0
1
0 13 2 3
1
end_operator
begin_operator
turn_to sat5 dir4 dir5
0
1
0 13 4 3
1
end_operator
begin_operator
turn_to sat5 dir4 dir6
0
1
0 13 5 3
1
end_operator
begin_operator
turn_to sat5 dir5 dir1
0
1
0 13 0 4
1
end_operator
begin_operator
turn_to sat5 dir5 dir2
0
1
0 13 1 4
1
end_operator
begin_operator
turn_to sat5 dir5 dir3
0
1
0 13 2 4
1
end_operator
begin_operator
turn_to sat5 dir5 dir4
0
1
0 13 3 4
1
end_operator
begin_operator
turn_to sat5 dir5 dir6
0
1
0 13 5 4
1
end_operator
begin_operator
turn_to sat5 dir6 dir1
0
1
0 13 0 5
1
end_operator
begin_operator
turn_to sat5 dir6 dir2
0
1
0 13 1 5
1
end_operator
begin_operator
turn_to sat5 dir6 dir3
0
1
0 13 2 5
1
end_operator
begin_operator
turn_to sat5 dir6 dir4
0
1
0 13 3 5
1
end_operator
begin_operator
turn_to sat5 dir6 dir5
0
1
0 13 4 5
1
end_operator
0
