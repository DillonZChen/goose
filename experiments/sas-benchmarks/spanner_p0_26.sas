begin_version
3
end_version
begin_metric
0
end_metric
24
begin_variable
var0
-1
11
Atom at(bob, gate)
Atom at(bob, location1)
Atom at(bob, location2)
Atom at(bob, location3)
Atom at(bob, location4)
Atom at(bob, location5)
Atom at(bob, location6)
Atom at(bob, location7)
Atom at(bob, location8)
Atom at(bob, location9)
Atom at(bob, shed)
end_variable
begin_variable
var1
-1
2
Atom at(spanner9, location4)
Atom carrying(bob, spanner9)
end_variable
begin_variable
var2
-1
2
Atom at(spanner8, location5)
Atom carrying(bob, spanner8)
end_variable
begin_variable
var3
-1
2
Atom at(spanner7, location8)
Atom carrying(bob, spanner7)
end_variable
begin_variable
var4
-1
2
Atom at(spanner6, location4)
Atom carrying(bob, spanner6)
end_variable
begin_variable
var5
-1
2
Atom at(spanner5, location6)
Atom carrying(bob, spanner5)
end_variable
begin_variable
var6
-1
2
Atom at(spanner4, location3)
Atom carrying(bob, spanner4)
end_variable
begin_variable
var7
-1
2
Atom at(spanner3, location7)
Atom carrying(bob, spanner3)
end_variable
begin_variable
var8
-1
2
Atom at(spanner2, location9)
Atom carrying(bob, spanner2)
end_variable
begin_variable
var9
-1
2
Atom at(spanner1, location7)
Atom carrying(bob, spanner1)
end_variable
begin_variable
var10
-1
2
Atom usable(spanner1)
NegatedAtom usable(spanner1)
end_variable
begin_variable
var11
-1
2
Atom usable(spanner2)
NegatedAtom usable(spanner2)
end_variable
begin_variable
var12
-1
2
Atom usable(spanner3)
NegatedAtom usable(spanner3)
end_variable
begin_variable
var13
-1
2
Atom usable(spanner4)
NegatedAtom usable(spanner4)
end_variable
begin_variable
var14
-1
2
Atom usable(spanner5)
NegatedAtom usable(spanner5)
end_variable
begin_variable
var15
-1
2
Atom usable(spanner6)
NegatedAtom usable(spanner6)
end_variable
begin_variable
var16
-1
2
Atom usable(spanner7)
NegatedAtom usable(spanner7)
end_variable
begin_variable
var17
-1
2
Atom usable(spanner8)
NegatedAtom usable(spanner8)
end_variable
begin_variable
var18
-1
2
Atom usable(spanner9)
NegatedAtom usable(spanner9)
end_variable
begin_variable
var19
-1
2
Atom loose(nut1)
Atom tightened(nut1)
end_variable
begin_variable
var20
-1
2
Atom loose(nut2)
Atom tightened(nut2)
end_variable
begin_variable
var21
-1
2
Atom loose(nut3)
Atom tightened(nut3)
end_variable
begin_variable
var22
-1
2
Atom loose(nut4)
Atom tightened(nut4)
end_variable
begin_variable
var23
-1
2
Atom loose(nut5)
Atom tightened(nut5)
end_variable
0
begin_state
10
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
0
0
0
end_state
begin_goal
5
19 1
20 1
21 1
22 1
23 1
end_goal
64
begin_operator
pickup_spanner location3 spanner4 bob
1
0 3
1
0 6 0 1
1
end_operator
begin_operator
pickup_spanner location4 spanner6 bob
1
0 4
1
0 4 0 1
1
end_operator
begin_operator
pickup_spanner location4 spanner9 bob
1
0 4
1
0 1 0 1
1
end_operator
begin_operator
pickup_spanner location5 spanner8 bob
1
0 5
1
0 2 0 1
1
end_operator
begin_operator
pickup_spanner location6 spanner5 bob
1
0 6
1
0 5 0 1
1
end_operator
begin_operator
pickup_spanner location7 spanner1 bob
1
0 7
1
0 9 0 1
1
end_operator
begin_operator
pickup_spanner location7 spanner3 bob
1
0 7
1
0 7 0 1
1
end_operator
begin_operator
pickup_spanner location8 spanner7 bob
1
0 8
1
0 3 0 1
1
end_operator
begin_operator
pickup_spanner location9 spanner2 bob
1
0 9
1
0 8 0 1
1
end_operator
begin_operator
tighten_nut gate spanner1 bob nut1
2
0 0
9 1
2
0 19 0 1
0 10 0 1
1
end_operator
begin_operator
tighten_nut gate spanner1 bob nut2
2
0 0
9 1
2
0 20 0 1
0 10 0 1
1
end_operator
begin_operator
tighten_nut gate spanner1 bob nut3
2
0 0
9 1
2
0 21 0 1
0 10 0 1
1
end_operator
begin_operator
tighten_nut gate spanner1 bob nut4
2
0 0
9 1
2
0 22 0 1
0 10 0 1
1
end_operator
begin_operator
tighten_nut gate spanner1 bob nut5
2
0 0
9 1
2
0 23 0 1
0 10 0 1
1
end_operator
begin_operator
tighten_nut gate spanner2 bob nut1
2
0 0
8 1
2
0 19 0 1
0 11 0 1
1
end_operator
begin_operator
tighten_nut gate spanner2 bob nut2
2
0 0
8 1
2
0 20 0 1
0 11 0 1
1
end_operator
begin_operator
tighten_nut gate spanner2 bob nut3
2
0 0
8 1
2
0 21 0 1
0 11 0 1
1
end_operator
begin_operator
tighten_nut gate spanner2 bob nut4
2
0 0
8 1
2
0 22 0 1
0 11 0 1
1
end_operator
begin_operator
tighten_nut gate spanner2 bob nut5
2
0 0
8 1
2
0 23 0 1
0 11 0 1
1
end_operator
begin_operator
tighten_nut gate spanner3 bob nut1
2
0 0
7 1
2
0 19 0 1
0 12 0 1
1
end_operator
begin_operator
tighten_nut gate spanner3 bob nut2
2
0 0
7 1
2
0 20 0 1
0 12 0 1
1
end_operator
begin_operator
tighten_nut gate spanner3 bob nut3
2
0 0
7 1
2
0 21 0 1
0 12 0 1
1
end_operator
begin_operator
tighten_nut gate spanner3 bob nut4
2
0 0
7 1
2
0 22 0 1
0 12 0 1
1
end_operator
begin_operator
tighten_nut gate spanner3 bob nut5
2
0 0
7 1
2
0 23 0 1
0 12 0 1
1
end_operator
begin_operator
tighten_nut gate spanner4 bob nut1
2
0 0
6 1
2
0 19 0 1
0 13 0 1
1
end_operator
begin_operator
tighten_nut gate spanner4 bob nut2
2
0 0
6 1
2
0 20 0 1
0 13 0 1
1
end_operator
begin_operator
tighten_nut gate spanner4 bob nut3
2
0 0
6 1
2
0 21 0 1
0 13 0 1
1
end_operator
begin_operator
tighten_nut gate spanner4 bob nut4
2
0 0
6 1
2
0 22 0 1
0 13 0 1
1
end_operator
begin_operator
tighten_nut gate spanner4 bob nut5
2
0 0
6 1
2
0 23 0 1
0 13 0 1
1
end_operator
begin_operator
tighten_nut gate spanner5 bob nut1
2
0 0
5 1
2
0 19 0 1
0 14 0 1
1
end_operator
begin_operator
tighten_nut gate spanner5 bob nut2
2
0 0
5 1
2
0 20 0 1
0 14 0 1
1
end_operator
begin_operator
tighten_nut gate spanner5 bob nut3
2
0 0
5 1
2
0 21 0 1
0 14 0 1
1
end_operator
begin_operator
tighten_nut gate spanner5 bob nut4
2
0 0
5 1
2
0 22 0 1
0 14 0 1
1
end_operator
begin_operator
tighten_nut gate spanner5 bob nut5
2
0 0
5 1
2
0 23 0 1
0 14 0 1
1
end_operator
begin_operator
tighten_nut gate spanner6 bob nut1
2
0 0
4 1
2
0 19 0 1
0 15 0 1
1
end_operator
begin_operator
tighten_nut gate spanner6 bob nut2
2
0 0
4 1
2
0 20 0 1
0 15 0 1
1
end_operator
begin_operator
tighten_nut gate spanner6 bob nut3
2
0 0
4 1
2
0 21 0 1
0 15 0 1
1
end_operator
begin_operator
tighten_nut gate spanner6 bob nut4
2
0 0
4 1
2
0 22 0 1
0 15 0 1
1
end_operator
begin_operator
tighten_nut gate spanner6 bob nut5
2
0 0
4 1
2
0 23 0 1
0 15 0 1
1
end_operator
begin_operator
tighten_nut gate spanner7 bob nut1
2
0 0
3 1
2
0 19 0 1
0 16 0 1
1
end_operator
begin_operator
tighten_nut gate spanner7 bob nut2
2
0 0
3 1
2
0 20 0 1
0 16 0 1
1
end_operator
begin_operator
tighten_nut gate spanner7 bob nut3
2
0 0
3 1
2
0 21 0 1
0 16 0 1
1
end_operator
begin_operator
tighten_nut gate spanner7 bob nut4
2
0 0
3 1
2
0 22 0 1
0 16 0 1
1
end_operator
begin_operator
tighten_nut gate spanner7 bob nut5
2
0 0
3 1
2
0 23 0 1
0 16 0 1
1
end_operator
begin_operator
tighten_nut gate spanner8 bob nut1
2
0 0
2 1
2
0 19 0 1
0 17 0 1
1
end_operator
begin_operator
tighten_nut gate spanner8 bob nut2
2
0 0
2 1
2
0 20 0 1
0 17 0 1
1
end_operator
begin_operator
tighten_nut gate spanner8 bob nut3
2
0 0
2 1
2
0 21 0 1
0 17 0 1
1
end_operator
begin_operator
tighten_nut gate spanner8 bob nut4
2
0 0
2 1
2
0 22 0 1
0 17 0 1
1
end_operator
begin_operator
tighten_nut gate spanner8 bob nut5
2
0 0
2 1
2
0 23 0 1
0 17 0 1
1
end_operator
begin_operator
tighten_nut gate spanner9 bob nut1
2
0 0
1 1
2
0 19 0 1
0 18 0 1
1
end_operator
begin_operator
tighten_nut gate spanner9 bob nut2
2
0 0
1 1
2
0 20 0 1
0 18 0 1
1
end_operator
begin_operator
tighten_nut gate spanner9 bob nut3
2
0 0
1 1
2
0 21 0 1
0 18 0 1
1
end_operator
begin_operator
tighten_nut gate spanner9 bob nut4
2
0 0
1 1
2
0 22 0 1
0 18 0 1
1
end_operator
begin_operator
tighten_nut gate spanner9 bob nut5
2
0 0
1 1
2
0 23 0 1
0 18 0 1
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
walk location6 location7 bob
0
1
0 0 6 7
1
end_operator
begin_operator
walk location7 location8 bob
0
1
0 0 7 8
1
end_operator
begin_operator
walk location8 location9 bob
0
1
0 0 8 9
1
end_operator
begin_operator
walk location9 gate bob
0
1
0 0 9 0
1
end_operator
begin_operator
walk shed location1 bob
0
1
0 0 10 1
1
end_operator
0
