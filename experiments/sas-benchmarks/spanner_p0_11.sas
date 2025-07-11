begin_version
3
end_version
begin_metric
0
end_metric
11
begin_variable
var0
-1
8
Atom at(bob, gate)
Atom at(bob, location1)
Atom at(bob, location2)
Atom at(bob, location3)
Atom at(bob, location4)
Atom at(bob, location5)
Atom at(bob, location6)
Atom at(bob, shed)
end_variable
begin_variable
var1
-1
2
Atom at(spanner4, location2)
Atom carrying(bob, spanner4)
end_variable
begin_variable
var2
-1
2
Atom at(spanner3, location4)
Atom carrying(bob, spanner3)
end_variable
begin_variable
var3
-1
2
Atom at(spanner2, location6)
Atom carrying(bob, spanner2)
end_variable
begin_variable
var4
-1
2
Atom at(spanner1, location4)
Atom carrying(bob, spanner1)
end_variable
begin_variable
var5
-1
2
Atom usable(spanner1)
NegatedAtom usable(spanner1)
end_variable
begin_variable
var6
-1
2
Atom usable(spanner2)
NegatedAtom usable(spanner2)
end_variable
begin_variable
var7
-1
2
Atom usable(spanner3)
NegatedAtom usable(spanner3)
end_variable
begin_variable
var8
-1
2
Atom usable(spanner4)
NegatedAtom usable(spanner4)
end_variable
begin_variable
var9
-1
2
Atom loose(nut1)
Atom tightened(nut1)
end_variable
begin_variable
var10
-1
2
Atom loose(nut2)
Atom tightened(nut2)
end_variable
0
begin_state
7
0
0
0
0
0
0
0
0
0
0
end_state
begin_goal
2
9 1
10 1
end_goal
19
begin_operator
pickup_spanner location2 spanner4 bob
1
0 2
1
0 1 0 1
1
end_operator
begin_operator
pickup_spanner location4 spanner1 bob
1
0 4
1
0 4 0 1
1
end_operator
begin_operator
pickup_spanner location4 spanner3 bob
1
0 4
1
0 2 0 1
1
end_operator
begin_operator
pickup_spanner location6 spanner2 bob
1
0 6
1
0 3 0 1
1
end_operator
begin_operator
tighten_nut gate spanner1 bob nut1
2
0 0
4 1
2
0 9 0 1
0 5 0 1
1
end_operator
begin_operator
tighten_nut gate spanner1 bob nut2
2
0 0
4 1
2
0 10 0 1
0 5 0 1
1
end_operator
begin_operator
tighten_nut gate spanner2 bob nut1
2
0 0
3 1
2
0 9 0 1
0 6 0 1
1
end_operator
begin_operator
tighten_nut gate spanner2 bob nut2
2
0 0
3 1
2
0 10 0 1
0 6 0 1
1
end_operator
begin_operator
tighten_nut gate spanner3 bob nut1
2
0 0
2 1
2
0 9 0 1
0 7 0 1
1
end_operator
begin_operator
tighten_nut gate spanner3 bob nut2
2
0 0
2 1
2
0 10 0 1
0 7 0 1
1
end_operator
begin_operator
tighten_nut gate spanner4 bob nut1
2
0 0
1 1
2
0 9 0 1
0 8 0 1
1
end_operator
begin_operator
tighten_nut gate spanner4 bob nut2
2
0 0
1 1
2
0 10 0 1
0 8 0 1
1
end_operator
begin_operator
walk location1 location2 bob
0
1
0 0 1 2
1
end_operator
begin_operator
walk location2 location3 bob
0
1
0 0 2 3
1
end_operator
begin_operator
walk location3 location4 bob
0
1
0 0 3 4
1
end_operator
begin_operator
walk location4 location5 bob
0
1
0 0 4 5
1
end_operator
begin_operator
walk location5 location6 bob
0
1
0 0 5 6
1
end_operator
begin_operator
walk location6 gate bob
0
1
0 0 6 0
1
end_operator
begin_operator
walk shed location1 bob
0
1
0 0 7 1
1
end_operator
0
