begin_version
3
end_version
begin_metric
0
end_metric
10
begin_variable
var0
-1
4
Atom at(rover1, waypoint1)
Atom at(rover1, waypoint2)
Atom at(rover1, waypoint3)
Atom at(rover1, waypoint4)
end_variable
begin_variable
var1
-1
2
Atom at_rock_sample(waypoint1)
Atom have_rock_analysis(rover1, waypoint1)
end_variable
begin_variable
var2
-1
2
Atom at_rock_sample(waypoint2)
Atom have_rock_analysis(rover1, waypoint2)
end_variable
begin_variable
var3
-1
2
Atom at_rock_sample(waypoint3)
Atom have_rock_analysis(rover1, waypoint3)
end_variable
begin_variable
var4
-1
2
Atom at_soil_sample(waypoint2)
Atom have_soil_analysis(rover1, waypoint2)
end_variable
begin_variable
var5
-1
2
Atom at_soil_sample(waypoint3)
Atom have_soil_analysis(rover1, waypoint3)
end_variable
begin_variable
var6
-1
2
Atom empty(rover1store)
Atom full(rover1store)
end_variable
begin_variable
var7
-1
2
Atom communicated_soil_data(waypoint2)
NegatedAtom communicated_soil_data(waypoint2)
end_variable
begin_variable
var8
-1
2
Atom communicated_rock_data(waypoint3)
NegatedAtom communicated_rock_data(waypoint3)
end_variable
begin_variable
var9
-1
2
Atom communicated_rock_data(waypoint2)
NegatedAtom communicated_rock_data(waypoint2)
end_variable
0
begin_state
1
0
0
0
0
0
0
1
1
1
end_state
begin_goal
3
7 0
8 0
9 0
end_goal
23
begin_operator
communicate_rock_data rover1 general waypoint2 waypoint2 waypoint1
2
0 1
2 1
1
0 9 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint2 waypoint3 waypoint1
2
0 2
2 1
1
0 9 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint2 waypoint4 waypoint1
2
0 3
2 1
1
0 9 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint3 waypoint2 waypoint1
2
0 1
3 1
1
0 8 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint3 waypoint3 waypoint1
2
0 2
3 1
1
0 8 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint3 waypoint4 waypoint1
2
0 3
3 1
1
0 8 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint2 waypoint2 waypoint1
2
0 1
4 1
1
0 7 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint2 waypoint3 waypoint1
2
0 2
4 1
1
0 7 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint2 waypoint4 waypoint1
2
0 3
4 1
1
0 7 -1 0
1
end_operator
begin_operator
drop rover1 rover1store
0
1
0 6 1 0
1
end_operator
begin_operator
navigate rover1 waypoint1 waypoint4
0
1
0 0 0 3
1
end_operator
begin_operator
navigate rover1 waypoint2 waypoint3
0
1
0 0 1 2
1
end_operator
begin_operator
navigate rover1 waypoint2 waypoint4
0
1
0 0 1 3
1
end_operator
begin_operator
navigate rover1 waypoint3 waypoint2
0
1
0 0 2 1
1
end_operator
begin_operator
navigate rover1 waypoint3 waypoint4
0
1
0 0 2 3
1
end_operator
begin_operator
navigate rover1 waypoint4 waypoint1
0
1
0 0 3 0
1
end_operator
begin_operator
navigate rover1 waypoint4 waypoint2
0
1
0 0 3 1
1
end_operator
begin_operator
navigate rover1 waypoint4 waypoint3
0
1
0 0 3 2
1
end_operator
begin_operator
sample_rock rover1 rover1store waypoint1
1
0 0
2
0 1 0 1
0 6 0 1
1
end_operator
begin_operator
sample_rock rover1 rover1store waypoint2
1
0 1
2
0 2 0 1
0 6 0 1
1
end_operator
begin_operator
sample_rock rover1 rover1store waypoint3
1
0 2
2
0 3 0 1
0 6 0 1
1
end_operator
begin_operator
sample_soil rover1 rover1store waypoint2
1
0 1
2
0 4 0 1
0 6 0 1
1
end_operator
begin_operator
sample_soil rover1 rover1store waypoint3
1
0 2
2
0 5 0 1
0 6 0 1
1
end_operator
0
