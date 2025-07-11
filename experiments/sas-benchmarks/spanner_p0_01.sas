begin_version
3
end_version
begin_metric
0
end_metric
4
begin_variable
var0
-1
6
Atom at(bob, gate)
Atom at(bob, location1)
Atom at(bob, location2)
Atom at(bob, location3)
Atom at(bob, location4)
Atom at(bob, shed)
end_variable
begin_variable
var1
-1
2
Atom at(spanner1, location1)
Atom carrying(bob, spanner1)
end_variable
begin_variable
var2
-1
2
Atom loose(nut1)
NegatedAtom loose(nut1)
end_variable
begin_variable
var3
-1
2
Atom tightened(nut1)
Atom usable(spanner1)
end_variable
2
begin_mutex_group
2
2 0
3 0
end_mutex_group
begin_mutex_group
2
2 0
3 0
end_mutex_group
begin_state
5
0
0
1
end_state
begin_goal
1
3 0
end_goal
7
begin_operator
pickup_spanner location1 spanner1 bob
1
0 1
1
0 1 0 1
1
end_operator
begin_operator
tighten_nut gate spanner1 bob nut1
2
0 0
1 1
2
0 2 0 1
0 3 1 0
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
walk location4 gate bob
0
1
0 0 4 0
1
end_operator
begin_operator
walk shed location1 bob
0
1
0 0 5 1
1
end_operator
0
