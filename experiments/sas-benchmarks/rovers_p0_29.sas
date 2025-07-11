begin_version
3
end_version
begin_metric
0
end_metric
22
begin_variable
var0
-1
10
Atom at(rover4, waypoint1)
Atom at(rover4, waypoint10)
Atom at(rover4, waypoint2)
Atom at(rover4, waypoint3)
Atom at(rover4, waypoint4)
Atom at(rover4, waypoint5)
Atom at(rover4, waypoint6)
Atom at(rover4, waypoint7)
Atom at(rover4, waypoint8)
Atom at(rover4, waypoint9)
end_variable
begin_variable
var1
-1
10
Atom at(rover3, waypoint1)
Atom at(rover3, waypoint10)
Atom at(rover3, waypoint2)
Atom at(rover3, waypoint3)
Atom at(rover3, waypoint4)
Atom at(rover3, waypoint5)
Atom at(rover3, waypoint6)
Atom at(rover3, waypoint7)
Atom at(rover3, waypoint8)
Atom at(rover3, waypoint9)
end_variable
begin_variable
var2
-1
10
Atom at(rover1, waypoint1)
Atom at(rover1, waypoint10)
Atom at(rover1, waypoint2)
Atom at(rover1, waypoint3)
Atom at(rover1, waypoint4)
Atom at(rover1, waypoint5)
Atom at(rover1, waypoint6)
Atom at(rover1, waypoint7)
Atom at(rover1, waypoint8)
Atom at(rover1, waypoint9)
end_variable
begin_variable
var3
-1
3
Atom at_soil_sample(waypoint1)
Atom have_soil_analysis(rover3, waypoint1)
Atom have_soil_analysis(rover4, waypoint1)
end_variable
begin_variable
var4
-1
3
Atom at_soil_sample(waypoint2)
Atom have_soil_analysis(rover3, waypoint2)
Atom have_soil_analysis(rover4, waypoint2)
end_variable
begin_variable
var5
-1
3
Atom at_soil_sample(waypoint5)
Atom have_soil_analysis(rover3, waypoint5)
Atom have_soil_analysis(rover4, waypoint5)
end_variable
begin_variable
var6
-1
3
Atom at_soil_sample(waypoint8)
Atom have_soil_analysis(rover3, waypoint8)
Atom have_soil_analysis(rover4, waypoint8)
end_variable
begin_variable
var7
-1
4
Atom at_rock_sample(waypoint1)
Atom have_rock_analysis(rover1, waypoint1)
Atom have_rock_analysis(rover3, waypoint1)
Atom have_rock_analysis(rover4, waypoint1)
end_variable
begin_variable
var8
-1
4
Atom at_rock_sample(waypoint10)
Atom have_rock_analysis(rover1, waypoint10)
Atom have_rock_analysis(rover3, waypoint10)
Atom have_rock_analysis(rover4, waypoint10)
end_variable
begin_variable
var9
-1
4
Atom at_rock_sample(waypoint5)
Atom have_rock_analysis(rover1, waypoint5)
Atom have_rock_analysis(rover3, waypoint5)
Atom have_rock_analysis(rover4, waypoint5)
end_variable
begin_variable
var10
-1
4
Atom at_rock_sample(waypoint6)
Atom have_rock_analysis(rover1, waypoint6)
Atom have_rock_analysis(rover3, waypoint6)
Atom have_rock_analysis(rover4, waypoint6)
end_variable
begin_variable
var11
-1
2
Atom empty(rover1store)
Atom full(rover1store)
end_variable
begin_variable
var12
-1
2
Atom empty(rover3store)
Atom full(rover3store)
end_variable
begin_variable
var13
-1
4
Atom at_rock_sample(waypoint8)
Atom have_rock_analysis(rover1, waypoint8)
Atom have_rock_analysis(rover3, waypoint8)
Atom have_rock_analysis(rover4, waypoint8)
end_variable
begin_variable
var14
-1
4
Atom at_rock_sample(waypoint9)
Atom have_rock_analysis(rover1, waypoint9)
Atom have_rock_analysis(rover3, waypoint9)
Atom have_rock_analysis(rover4, waypoint9)
end_variable
begin_variable
var15
-1
2
Atom empty(rover4store)
Atom full(rover4store)
end_variable
begin_variable
var16
-1
2
Atom communicated_rock_data(waypoint9)
NegatedAtom communicated_rock_data(waypoint9)
end_variable
begin_variable
var17
-1
2
Atom communicated_rock_data(waypoint8)
NegatedAtom communicated_rock_data(waypoint8)
end_variable
begin_variable
var18
-1
2
Atom communicated_rock_data(waypoint6)
NegatedAtom communicated_rock_data(waypoint6)
end_variable
begin_variable
var19
-1
2
Atom communicated_rock_data(waypoint5)
NegatedAtom communicated_rock_data(waypoint5)
end_variable
begin_variable
var20
-1
2
Atom communicated_rock_data(waypoint10)
NegatedAtom communicated_rock_data(waypoint10)
end_variable
begin_variable
var21
-1
2
Atom communicated_rock_data(waypoint1)
NegatedAtom communicated_rock_data(waypoint1)
end_variable
0
begin_state
2
3
3
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
1
1
1
1
1
1
end_state
begin_goal
6
16 0
17 0
18 0
19 0
20 0
21 0
end_goal
141
begin_operator
communicate_rock_data rover1 general waypoint1 waypoint4 waypoint8
2
2 4
7 1
1
0 21 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint1 waypoint5 waypoint8
2
2 5
7 1
1
0 21 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint10 waypoint4 waypoint8
2
2 4
8 1
1
0 20 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint10 waypoint5 waypoint8
2
2 5
8 1
1
0 20 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint5 waypoint4 waypoint8
2
2 4
9 1
1
0 19 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint5 waypoint5 waypoint8
2
2 5
9 1
1
0 19 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint6 waypoint4 waypoint8
2
2 4
10 1
1
0 18 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint6 waypoint5 waypoint8
2
2 5
10 1
1
0 18 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint8 waypoint4 waypoint8
2
2 4
13 1
1
0 17 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint8 waypoint5 waypoint8
2
2 5
13 1
1
0 17 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint9 waypoint4 waypoint8
2
2 4
14 1
1
0 16 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint9 waypoint5 waypoint8
2
2 5
14 1
1
0 16 -1 0
1
end_operator
begin_operator
communicate_rock_data rover3 general waypoint1 waypoint4 waypoint8
2
1 4
7 2
1
0 21 -1 0
1
end_operator
begin_operator
communicate_rock_data rover3 general waypoint1 waypoint5 waypoint8
2
1 5
7 2
1
0 21 -1 0
1
end_operator
begin_operator
communicate_rock_data rover3 general waypoint10 waypoint4 waypoint8
2
1 4
8 2
1
0 20 -1 0
1
end_operator
begin_operator
communicate_rock_data rover3 general waypoint10 waypoint5 waypoint8
2
1 5
8 2
1
0 20 -1 0
1
end_operator
begin_operator
communicate_rock_data rover3 general waypoint5 waypoint4 waypoint8
2
1 4
9 2
1
0 19 -1 0
1
end_operator
begin_operator
communicate_rock_data rover3 general waypoint5 waypoint5 waypoint8
2
1 5
9 2
1
0 19 -1 0
1
end_operator
begin_operator
communicate_rock_data rover3 general waypoint6 waypoint4 waypoint8
2
1 4
10 2
1
0 18 -1 0
1
end_operator
begin_operator
communicate_rock_data rover3 general waypoint6 waypoint5 waypoint8
2
1 5
10 2
1
0 18 -1 0
1
end_operator
begin_operator
communicate_rock_data rover3 general waypoint8 waypoint4 waypoint8
2
1 4
13 2
1
0 17 -1 0
1
end_operator
begin_operator
communicate_rock_data rover3 general waypoint8 waypoint5 waypoint8
2
1 5
13 2
1
0 17 -1 0
1
end_operator
begin_operator
communicate_rock_data rover3 general waypoint9 waypoint4 waypoint8
2
1 4
14 2
1
0 16 -1 0
1
end_operator
begin_operator
communicate_rock_data rover3 general waypoint9 waypoint5 waypoint8
2
1 5
14 2
1
0 16 -1 0
1
end_operator
begin_operator
communicate_rock_data rover4 general waypoint1 waypoint4 waypoint8
2
0 4
7 3
1
0 21 -1 0
1
end_operator
begin_operator
communicate_rock_data rover4 general waypoint1 waypoint5 waypoint8
2
0 5
7 3
1
0 21 -1 0
1
end_operator
begin_operator
communicate_rock_data rover4 general waypoint10 waypoint4 waypoint8
2
0 4
8 3
1
0 20 -1 0
1
end_operator
begin_operator
communicate_rock_data rover4 general waypoint10 waypoint5 waypoint8
2
0 5
8 3
1
0 20 -1 0
1
end_operator
begin_operator
communicate_rock_data rover4 general waypoint5 waypoint4 waypoint8
2
0 4
9 3
1
0 19 -1 0
1
end_operator
begin_operator
communicate_rock_data rover4 general waypoint5 waypoint5 waypoint8
2
0 5
9 3
1
0 19 -1 0
1
end_operator
begin_operator
communicate_rock_data rover4 general waypoint6 waypoint4 waypoint8
2
0 4
10 3
1
0 18 -1 0
1
end_operator
begin_operator
communicate_rock_data rover4 general waypoint6 waypoint5 waypoint8
2
0 5
10 3
1
0 18 -1 0
1
end_operator
begin_operator
communicate_rock_data rover4 general waypoint8 waypoint4 waypoint8
2
0 4
13 3
1
0 17 -1 0
1
end_operator
begin_operator
communicate_rock_data rover4 general waypoint8 waypoint5 waypoint8
2
0 5
13 3
1
0 17 -1 0
1
end_operator
begin_operator
communicate_rock_data rover4 general waypoint9 waypoint4 waypoint8
2
0 4
14 3
1
0 16 -1 0
1
end_operator
begin_operator
communicate_rock_data rover4 general waypoint9 waypoint5 waypoint8
2
0 5
14 3
1
0 16 -1 0
1
end_operator
begin_operator
drop rover1 rover1store
0
1
0 11 1 0
1
end_operator
begin_operator
drop rover3 rover3store
0
1
0 12 1 0
1
end_operator
begin_operator
drop rover4 rover4store
0
1
0 15 1 0
1
end_operator
begin_operator
navigate rover1 waypoint1 waypoint4
0
1
0 2 0 4
1
end_operator
begin_operator
navigate rover1 waypoint1 waypoint5
0
1
0 2 0 5
1
end_operator
begin_operator
navigate rover1 waypoint10 waypoint3
0
1
0 2 1 3
1
end_operator
begin_operator
navigate rover1 waypoint10 waypoint5
0
1
0 2 1 5
1
end_operator
begin_operator
navigate rover1 waypoint2 waypoint4
0
1
0 2 2 4
1
end_operator
begin_operator
navigate rover1 waypoint2 waypoint5
0
1
0 2 2 5
1
end_operator
begin_operator
navigate rover1 waypoint2 waypoint6
0
1
0 2 2 6
1
end_operator
begin_operator
navigate rover1 waypoint3 waypoint10
0
1
0 2 3 1
1
end_operator
begin_operator
navigate rover1 waypoint4 waypoint1
0
1
0 2 4 0
1
end_operator
begin_operator
navigate rover1 waypoint4 waypoint2
0
1
0 2 4 2
1
end_operator
begin_operator
navigate rover1 waypoint4 waypoint6
0
1
0 2 4 6
1
end_operator
begin_operator
navigate rover1 waypoint4 waypoint7
0
1
0 2 4 7
1
end_operator
begin_operator
navigate rover1 waypoint4 waypoint8
0
1
0 2 4 8
1
end_operator
begin_operator
navigate rover1 waypoint4 waypoint9
0
1
0 2 4 9
1
end_operator
begin_operator
navigate rover1 waypoint5 waypoint1
0
1
0 2 5 0
1
end_operator
begin_operator
navigate rover1 waypoint5 waypoint10
0
1
0 2 5 1
1
end_operator
begin_operator
navigate rover1 waypoint5 waypoint2
0
1
0 2 5 2
1
end_operator
begin_operator
navigate rover1 waypoint5 waypoint8
0
1
0 2 5 8
1
end_operator
begin_operator
navigate rover1 waypoint6 waypoint2
0
1
0 2 6 2
1
end_operator
begin_operator
navigate rover1 waypoint6 waypoint4
0
1
0 2 6 4
1
end_operator
begin_operator
navigate rover1 waypoint7 waypoint4
0
1
0 2 7 4
1
end_operator
begin_operator
navigate rover1 waypoint7 waypoint9
0
1
0 2 7 9
1
end_operator
begin_operator
navigate rover1 waypoint8 waypoint4
0
1
0 2 8 4
1
end_operator
begin_operator
navigate rover1 waypoint8 waypoint5
0
1
0 2 8 5
1
end_operator
begin_operator
navigate rover1 waypoint9 waypoint4
0
1
0 2 9 4
1
end_operator
begin_operator
navigate rover1 waypoint9 waypoint7
0
1
0 2 9 7
1
end_operator
begin_operator
navigate rover3 waypoint1 waypoint4
0
1
0 1 0 4
1
end_operator
begin_operator
navigate rover3 waypoint1 waypoint9
0
1
0 1 0 9
1
end_operator
begin_operator
navigate rover3 waypoint10 waypoint3
0
1
0 1 1 3
1
end_operator
begin_operator
navigate rover3 waypoint10 waypoint5
0
1
0 1 1 5
1
end_operator
begin_operator
navigate rover3 waypoint10 waypoint9
0
1
0 1 1 9
1
end_operator
begin_operator
navigate rover3 waypoint2 waypoint3
0
1
0 1 2 3
1
end_operator
begin_operator
navigate rover3 waypoint2 waypoint4
0
1
0 1 2 4
1
end_operator
begin_operator
navigate rover3 waypoint2 waypoint5
0
1
0 1 2 5
1
end_operator
begin_operator
navigate rover3 waypoint2 waypoint6
0
1
0 1 2 6
1
end_operator
begin_operator
navigate rover3 waypoint3 waypoint10
0
1
0 1 3 1
1
end_operator
begin_operator
navigate rover3 waypoint3 waypoint2
0
1
0 1 3 2
1
end_operator
begin_operator
navigate rover3 waypoint4 waypoint1
0
1
0 1 4 0
1
end_operator
begin_operator
navigate rover3 waypoint4 waypoint2
0
1
0 1 4 2
1
end_operator
begin_operator
navigate rover3 waypoint4 waypoint7
0
1
0 1 4 7
1
end_operator
begin_operator
navigate rover3 waypoint4 waypoint8
0
1
0 1 4 8
1
end_operator
begin_operator
navigate rover3 waypoint5 waypoint10
0
1
0 1 5 1
1
end_operator
begin_operator
navigate rover3 waypoint5 waypoint2
0
1
0 1 5 2
1
end_operator
begin_operator
navigate rover3 waypoint5 waypoint8
0
1
0 1 5 8
1
end_operator
begin_operator
navigate rover3 waypoint6 waypoint2
0
1
0 1 6 2
1
end_operator
begin_operator
navigate rover3 waypoint7 waypoint4
0
1
0 1 7 4
1
end_operator
begin_operator
navigate rover3 waypoint7 waypoint9
0
1
0 1 7 9
1
end_operator
begin_operator
navigate rover3 waypoint8 waypoint4
0
1
0 1 8 4
1
end_operator
begin_operator
navigate rover3 waypoint8 waypoint5
0
1
0 1 8 5
1
end_operator
begin_operator
navigate rover3 waypoint9 waypoint1
0
1
0 1 9 0
1
end_operator
begin_operator
navigate rover3 waypoint9 waypoint10
0
1
0 1 9 1
1
end_operator
begin_operator
navigate rover3 waypoint9 waypoint7
0
1
0 1 9 7
1
end_operator
begin_operator
navigate rover4 waypoint1 waypoint4
0
1
0 0 0 4
1
end_operator
begin_operator
navigate rover4 waypoint1 waypoint5
0
1
0 0 0 5
1
end_operator
begin_operator
navigate rover4 waypoint10 waypoint3
0
1
0 0 1 3
1
end_operator
begin_operator
navigate rover4 waypoint10 waypoint5
0
1
0 0 1 5
1
end_operator
begin_operator
navigate rover4 waypoint2 waypoint4
0
1
0 0 2 4
1
end_operator
begin_operator
navigate rover4 waypoint2 waypoint5
0
1
0 0 2 5
1
end_operator
begin_operator
navigate rover4 waypoint2 waypoint6
0
1
0 0 2 6
1
end_operator
begin_operator
navigate rover4 waypoint3 waypoint10
0
1
0 0 3 1
1
end_operator
begin_operator
navigate rover4 waypoint3 waypoint6
0
1
0 0 3 6
1
end_operator
begin_operator
navigate rover4 waypoint4 waypoint1
0
1
0 0 4 0
1
end_operator
begin_operator
navigate rover4 waypoint4 waypoint2
0
1
0 0 4 2
1
end_operator
begin_operator
navigate rover4 waypoint4 waypoint6
0
1
0 0 4 6
1
end_operator
begin_operator
navigate rover4 waypoint4 waypoint7
0
1
0 0 4 7
1
end_operator
begin_operator
navigate rover4 waypoint5 waypoint1
0
1
0 0 5 0
1
end_operator
begin_operator
navigate rover4 waypoint5 waypoint10
0
1
0 0 5 1
1
end_operator
begin_operator
navigate rover4 waypoint5 waypoint2
0
1
0 0 5 2
1
end_operator
begin_operator
navigate rover4 waypoint5 waypoint8
0
1
0 0 5 8
1
end_operator
begin_operator
navigate rover4 waypoint6 waypoint2
0
1
0 0 6 2
1
end_operator
begin_operator
navigate rover4 waypoint6 waypoint3
0
1
0 0 6 3
1
end_operator
begin_operator
navigate rover4 waypoint6 waypoint4
0
1
0 0 6 4
1
end_operator
begin_operator
navigate rover4 waypoint7 waypoint4
0
1
0 0 7 4
1
end_operator
begin_operator
navigate rover4 waypoint7 waypoint9
0
1
0 0 7 9
1
end_operator
begin_operator
navigate rover4 waypoint8 waypoint5
0
1
0 0 8 5
1
end_operator
begin_operator
navigate rover4 waypoint9 waypoint7
0
1
0 0 9 7
1
end_operator
begin_operator
sample_rock rover1 rover1store waypoint1
1
2 0
2
0 7 0 1
0 11 0 1
1
end_operator
begin_operator
sample_rock rover1 rover1store waypoint10
1
2 1
2
0 8 0 1
0 11 0 1
1
end_operator
begin_operator
sample_rock rover1 rover1store waypoint5
1
2 5
2
0 9 0 1
0 11 0 1
1
end_operator
begin_operator
sample_rock rover1 rover1store waypoint6
1
2 6
2
0 10 0 1
0 11 0 1
1
end_operator
begin_operator
sample_rock rover1 rover1store waypoint8
1
2 8
2
0 13 0 1
0 11 0 1
1
end_operator
begin_operator
sample_rock rover1 rover1store waypoint9
1
2 9
2
0 14 0 1
0 11 0 1
1
end_operator
begin_operator
sample_rock rover3 rover3store waypoint1
1
1 0
2
0 7 0 2
0 12 0 1
1
end_operator
begin_operator
sample_rock rover3 rover3store waypoint10
1
1 1
2
0 8 0 2
0 12 0 1
1
end_operator
begin_operator
sample_rock rover3 rover3store waypoint5
1
1 5
2
0 9 0 2
0 12 0 1
1
end_operator
begin_operator
sample_rock rover3 rover3store waypoint6
1
1 6
2
0 10 0 2
0 12 0 1
1
end_operator
begin_operator
sample_rock rover3 rover3store waypoint8
1
1 8
2
0 13 0 2
0 12 0 1
1
end_operator
begin_operator
sample_rock rover3 rover3store waypoint9
1
1 9
2
0 14 0 2
0 12 0 1
1
end_operator
begin_operator
sample_rock rover4 rover4store waypoint1
1
0 0
2
0 7 0 3
0 15 0 1
1
end_operator
begin_operator
sample_rock rover4 rover4store waypoint10
1
0 1
2
0 8 0 3
0 15 0 1
1
end_operator
begin_operator
sample_rock rover4 rover4store waypoint5
1
0 5
2
0 9 0 3
0 15 0 1
1
end_operator
begin_operator
sample_rock rover4 rover4store waypoint6
1
0 6
2
0 10 0 3
0 15 0 1
1
end_operator
begin_operator
sample_rock rover4 rover4store waypoint8
1
0 8
2
0 13 0 3
0 15 0 1
1
end_operator
begin_operator
sample_rock rover4 rover4store waypoint9
1
0 9
2
0 14 0 3
0 15 0 1
1
end_operator
begin_operator
sample_soil rover3 rover3store waypoint1
1
1 0
2
0 3 0 1
0 12 0 1
1
end_operator
begin_operator
sample_soil rover3 rover3store waypoint2
1
1 2
2
0 4 0 1
0 12 0 1
1
end_operator
begin_operator
sample_soil rover3 rover3store waypoint5
1
1 5
2
0 5 0 1
0 12 0 1
1
end_operator
begin_operator
sample_soil rover3 rover3store waypoint8
1
1 8
2
0 6 0 1
0 12 0 1
1
end_operator
begin_operator
sample_soil rover4 rover4store waypoint1
1
0 0
2
0 3 0 2
0 15 0 1
1
end_operator
begin_operator
sample_soil rover4 rover4store waypoint2
1
0 2
2
0 4 0 2
0 15 0 1
1
end_operator
begin_operator
sample_soil rover4 rover4store waypoint5
1
0 5
2
0 5 0 2
0 15 0 1
1
end_operator
begin_operator
sample_soil rover4 rover4store waypoint8
1
0 8
2
0 6 0 2
0 15 0 1
1
end_operator
0
