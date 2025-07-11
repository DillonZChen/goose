begin_version
3
end_version
begin_metric
0
end_metric
13
begin_variable
var0
-1
2
Atom power_avail(sat3)
NegatedAtom power_avail(sat3)
end_variable
begin_variable
var1
-1
2
Atom power_on(ins3)
NegatedAtom power_on(ins3)
end_variable
begin_variable
var2
-1
2
Atom power_avail(sat2)
NegatedAtom power_avail(sat2)
end_variable
begin_variable
var3
-1
2
Atom power_on(ins1)
NegatedAtom power_on(ins1)
end_variable
begin_variable
var4
-1
2
Atom power_avail(sat1)
NegatedAtom power_avail(sat1)
end_variable
begin_variable
var5
-1
2
Atom power_on(ins2)
NegatedAtom power_on(ins2)
end_variable
begin_variable
var6
-1
4
Atom pointing(sat3, dir1)
Atom pointing(sat3, dir2)
Atom pointing(sat3, dir3)
Atom pointing(sat3, dir4)
end_variable
begin_variable
var7
-1
4
Atom pointing(sat2, dir1)
Atom pointing(sat2, dir2)
Atom pointing(sat2, dir3)
Atom pointing(sat2, dir4)
end_variable
begin_variable
var8
-1
4
Atom pointing(sat1, dir1)
Atom pointing(sat1, dir2)
Atom pointing(sat1, dir3)
Atom pointing(sat1, dir4)
end_variable
begin_variable
var9
-1
2
Atom calibrated(ins3)
NegatedAtom calibrated(ins3)
end_variable
begin_variable
var10
-1
2
Atom calibrated(ins2)
NegatedAtom calibrated(ins2)
end_variable
begin_variable
var11
-1
2
Atom calibrated(ins1)
NegatedAtom calibrated(ins1)
end_variable
begin_variable
var12
-1
2
Atom have_image(dir2, mod1)
NegatedAtom have_image(dir2, mod1)
end_variable
0
begin_state
0
1
0
1
0
1
2
1
0
1
1
1
1
end_state
begin_goal
2
6 0
12 0
end_goal
48
begin_operator
calibrate sat1 ins2 dir2
2
8 1
5 0
1
0 10 -1 0
1
end_operator
begin_operator
calibrate sat2 ins1 dir2
2
7 1
3 0
1
0 11 -1 0
1
end_operator
begin_operator
calibrate sat3 ins3 dir1
2
6 0
1 0
1
0 9 -1 0
1
end_operator
begin_operator
switch_off ins1 sat2
0
2
0 2 -1 0
0 3 0 1
1
end_operator
begin_operator
switch_off ins2 sat1
0
2
0 4 -1 0
0 5 0 1
1
end_operator
begin_operator
switch_off ins3 sat3
0
2
0 0 -1 0
0 1 0 1
1
end_operator
begin_operator
switch_on ins1 sat2
0
3
0 11 -1 1
0 2 0 1
0 3 -1 0
1
end_operator
begin_operator
switch_on ins2 sat1
0
3
0 10 -1 1
0 4 0 1
0 5 -1 0
1
end_operator
begin_operator
switch_on ins3 sat3
0
3
0 9 -1 1
0 0 0 1
0 1 -1 0
1
end_operator
begin_operator
take_image sat1 dir2 ins2 mod1
3
10 0
8 1
5 0
1
0 12 -1 0
1
end_operator
begin_operator
take_image sat2 dir2 ins1 mod1
3
11 0
7 1
3 0
1
0 12 -1 0
1
end_operator
begin_operator
take_image sat3 dir2 ins3 mod1
3
9 0
6 1
1 0
1
0 12 -1 0
1
end_operator
begin_operator
turn_to sat1 dir1 dir2
0
1
0 8 1 0
1
end_operator
begin_operator
turn_to sat1 dir1 dir3
0
1
0 8 2 0
1
end_operator
begin_operator
turn_to sat1 dir1 dir4
0
1
0 8 3 0
1
end_operator
begin_operator
turn_to sat1 dir2 dir1
0
1
0 8 0 1
1
end_operator
begin_operator
turn_to sat1 dir2 dir3
0
1
0 8 2 1
1
end_operator
begin_operator
turn_to sat1 dir2 dir4
0
1
0 8 3 1
1
end_operator
begin_operator
turn_to sat1 dir3 dir1
0
1
0 8 0 2
1
end_operator
begin_operator
turn_to sat1 dir3 dir2
0
1
0 8 1 2
1
end_operator
begin_operator
turn_to sat1 dir3 dir4
0
1
0 8 3 2
1
end_operator
begin_operator
turn_to sat1 dir4 dir1
0
1
0 8 0 3
1
end_operator
begin_operator
turn_to sat1 dir4 dir2
0
1
0 8 1 3
1
end_operator
begin_operator
turn_to sat1 dir4 dir3
0
1
0 8 2 3
1
end_operator
begin_operator
turn_to sat2 dir1 dir2
0
1
0 7 1 0
1
end_operator
begin_operator
turn_to sat2 dir1 dir3
0
1
0 7 2 0
1
end_operator
begin_operator
turn_to sat2 dir1 dir4
0
1
0 7 3 0
1
end_operator
begin_operator
turn_to sat2 dir2 dir1
0
1
0 7 0 1
1
end_operator
begin_operator
turn_to sat2 dir2 dir3
0
1
0 7 2 1
1
end_operator
begin_operator
turn_to sat2 dir2 dir4
0
1
0 7 3 1
1
end_operator
begin_operator
turn_to sat2 dir3 dir1
0
1
0 7 0 2
1
end_operator
begin_operator
turn_to sat2 dir3 dir2
0
1
0 7 1 2
1
end_operator
begin_operator
turn_to sat2 dir3 dir4
0
1
0 7 3 2
1
end_operator
begin_operator
turn_to sat2 dir4 dir1
0
1
0 7 0 3
1
end_operator
begin_operator
turn_to sat2 dir4 dir2
0
1
0 7 1 3
1
end_operator
begin_operator
turn_to sat2 dir4 dir3
0
1
0 7 2 3
1
end_operator
begin_operator
turn_to sat3 dir1 dir2
0
1
0 6 1 0
1
end_operator
begin_operator
turn_to sat3 dir1 dir3
0
1
0 6 2 0
1
end_operator
begin_operator
turn_to sat3 dir1 dir4
0
1
0 6 3 0
1
end_operator
begin_operator
turn_to sat3 dir2 dir1
0
1
0 6 0 1
1
end_operator
begin_operator
turn_to sat3 dir2 dir3
0
1
0 6 2 1
1
end_operator
begin_operator
turn_to sat3 dir2 dir4
0
1
0 6 3 1
1
end_operator
begin_operator
turn_to sat3 dir3 dir1
0
1
0 6 0 2
1
end_operator
begin_operator
turn_to sat3 dir3 dir2
0
1
0 6 1 2
1
end_operator
begin_operator
turn_to sat3 dir3 dir4
0
1
0 6 3 2
1
end_operator
begin_operator
turn_to sat3 dir4 dir1
0
1
0 6 0 3
1
end_operator
begin_operator
turn_to sat3 dir4 dir2
0
1
0 6 1 3
1
end_operator
begin_operator
turn_to sat3 dir4 dir3
0
1
0 6 2 3
1
end_operator
0
