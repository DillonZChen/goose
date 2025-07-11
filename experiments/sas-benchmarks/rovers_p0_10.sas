begin_version
3
end_version
begin_metric
0
end_metric
17
begin_variable
var0
-1
6
Atom at(rover2, waypoint1)
Atom at(rover2, waypoint2)
Atom at(rover2, waypoint3)
Atom at(rover2, waypoint4)
Atom at(rover2, waypoint5)
Atom at(rover2, waypoint6)
end_variable
begin_variable
var1
-1
2
Atom calibrated(camera2, rover2)
NegatedAtom calibrated(camera2, rover2)
end_variable
begin_variable
var2
-1
2
Atom calibrated(camera1, rover2)
NegatedAtom calibrated(camera1, rover2)
end_variable
begin_variable
var3
-1
2
Atom have_image(rover2, objective4, low_res)
NegatedAtom have_image(rover2, objective4, low_res)
end_variable
begin_variable
var4
-1
2
Atom communicated_image_data(objective4, low_res)
NegatedAtom communicated_image_data(objective4, low_res)
end_variable
begin_variable
var5
-1
2
Atom have_image(rover2, objective4, high_res)
NegatedAtom have_image(rover2, objective4, high_res)
end_variable
begin_variable
var6
-1
2
Atom communicated_image_data(objective4, high_res)
NegatedAtom communicated_image_data(objective4, high_res)
end_variable
begin_variable
var7
-1
2
Atom have_image(rover2, objective3, low_res)
NegatedAtom have_image(rover2, objective3, low_res)
end_variable
begin_variable
var8
-1
2
Atom communicated_image_data(objective3, low_res)
NegatedAtom communicated_image_data(objective3, low_res)
end_variable
begin_variable
var9
-1
6
Atom at(rover1, waypoint1)
Atom at(rover1, waypoint2)
Atom at(rover1, waypoint3)
Atom at(rover1, waypoint4)
Atom at(rover1, waypoint5)
Atom at(rover1, waypoint6)
end_variable
begin_variable
var10
-1
2
Atom at_soil_sample(waypoint1)
Atom have_soil_analysis(rover1, waypoint1)
end_variable
begin_variable
var11
-1
3
Atom at_rock_sample(waypoint4)
Atom have_rock_analysis(rover1, waypoint4)
Atom have_rock_analysis(rover2, waypoint4)
end_variable
begin_variable
var12
-1
2
Atom empty(rover1store)
Atom full(rover1store)
end_variable
begin_variable
var13
-1
2
Atom empty(rover2store)
Atom full(rover2store)
end_variable
begin_variable
var14
-1
3
Atom at_rock_sample(waypoint6)
Atom have_rock_analysis(rover1, waypoint6)
Atom have_rock_analysis(rover2, waypoint6)
end_variable
begin_variable
var15
-1
2
Atom communicated_soil_data(waypoint1)
NegatedAtom communicated_soil_data(waypoint1)
end_variable
begin_variable
var16
-1
2
Atom communicated_rock_data(waypoint6)
NegatedAtom communicated_rock_data(waypoint6)
end_variable
0
begin_state
3
1
1
1
1
1
1
1
1
5
0
0
0
0
0
1
1
end_state
begin_goal
5
4 0
6 0
8 0
15 0
16 0
end_goal
113
begin_operator
calibrate rover2 camera1 objective2 waypoint6
1
0 5
1
0 2 -1 0
1
end_operator
begin_operator
calibrate rover2 camera2 objective3 waypoint1
1
0 0
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover2 camera2 objective3 waypoint2
1
0 1
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover2 camera2 objective3 waypoint3
1
0 2
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover2 camera2 objective3 waypoint4
1
0 3
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover2 camera2 objective3 waypoint5
1
0 4
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover2 camera2 objective3 waypoint6
1
0 5
1
0 1 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective3 low_res waypoint2 waypoint3
2
0 1
7 0
1
0 8 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective3 low_res waypoint6 waypoint3
2
0 5
7 0
1
0 8 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective4 high_res waypoint2 waypoint3
2
0 1
5 0
1
0 6 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective4 high_res waypoint6 waypoint3
2
0 5
5 0
1
0 6 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective4 low_res waypoint2 waypoint3
2
0 1
3 0
1
0 4 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective4 low_res waypoint6 waypoint3
2
0 5
3 0
1
0 4 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint6 waypoint2 waypoint3
2
9 1
14 1
1
0 16 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint6 waypoint6 waypoint3
2
9 5
14 1
1
0 16 -1 0
1
end_operator
begin_operator
communicate_rock_data rover2 general waypoint6 waypoint2 waypoint3
2
0 1
14 2
1
0 16 -1 0
1
end_operator
begin_operator
communicate_rock_data rover2 general waypoint6 waypoint6 waypoint3
2
0 5
14 2
1
0 16 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint1 waypoint2 waypoint3
2
9 1
10 1
1
0 15 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint1 waypoint6 waypoint3
2
9 5
10 1
1
0 15 -1 0
1
end_operator
begin_operator
drop rover1 rover1store
0
1
0 12 1 0
1
end_operator
begin_operator
drop rover2 rover2store
0
1
0 13 1 0
1
end_operator
begin_operator
navigate rover1 waypoint1 waypoint4
0
1
0 9 0 3
1
end_operator
begin_operator
navigate rover1 waypoint1 waypoint5
0
1
0 9 0 4
1
end_operator
begin_operator
navigate rover1 waypoint2 waypoint3
0
1
0 9 1 2
1
end_operator
begin_operator
navigate rover1 waypoint2 waypoint4
0
1
0 9 1 3
1
end_operator
begin_operator
navigate rover1 waypoint3 waypoint2
0
1
0 9 2 1
1
end_operator
begin_operator
navigate rover1 waypoint4 waypoint1
0
1
0 9 3 0
1
end_operator
begin_operator
navigate rover1 waypoint4 waypoint2
0
1
0 9 3 1
1
end_operator
begin_operator
navigate rover1 waypoint4 waypoint6
0
1
0 9 3 5
1
end_operator
begin_operator
navigate rover1 waypoint5 waypoint1
0
1
0 9 4 0
1
end_operator
begin_operator
navigate rover1 waypoint5 waypoint6
0
1
0 9 4 5
1
end_operator
begin_operator
navigate rover1 waypoint6 waypoint4
0
1
0 9 5 3
1
end_operator
begin_operator
navigate rover1 waypoint6 waypoint5
0
1
0 9 5 4
1
end_operator
begin_operator
navigate rover2 waypoint1 waypoint4
0
1
0 0 0 3
1
end_operator
begin_operator
navigate rover2 waypoint1 waypoint5
0
1
0 0 0 4
1
end_operator
begin_operator
navigate rover2 waypoint2 waypoint3
0
1
0 0 1 2
1
end_operator
begin_operator
navigate rover2 waypoint2 waypoint4
0
1
0 0 1 3
1
end_operator
begin_operator
navigate rover2 waypoint3 waypoint2
0
1
0 0 2 1
1
end_operator
begin_operator
navigate rover2 waypoint4 waypoint1
0
1
0 0 3 0
1
end_operator
begin_operator
navigate rover2 waypoint4 waypoint2
0
1
0 0 3 1
1
end_operator
begin_operator
navigate rover2 waypoint5 waypoint1
0
1
0 0 4 0
1
end_operator
begin_operator
navigate rover2 waypoint5 waypoint6
0
1
0 0 4 5
1
end_operator
begin_operator
navigate rover2 waypoint6 waypoint5
0
1
0 0 5 4
1
end_operator
begin_operator
sample_rock rover1 rover1store waypoint4
1
9 3
2
0 11 0 1
0 12 0 1
1
end_operator
begin_operator
sample_rock rover1 rover1store waypoint6
1
9 5
2
0 14 0 1
0 12 0 1
1
end_operator
begin_operator
sample_rock rover2 rover2store waypoint4
1
0 3
2
0 11 0 2
0 13 0 1
1
end_operator
begin_operator
sample_rock rover2 rover2store waypoint6
1
0 5
2
0 14 0 2
0 13 0 1
1
end_operator
begin_operator
sample_soil rover1 rover1store waypoint1
1
9 0
2
0 10 0 1
0 12 0 1
1
end_operator
begin_operator
take_image rover2 waypoint1 objective1 camera1 colour
1
0 0
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint1 objective1 camera1 high_res
1
0 0
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint1 objective1 camera1 low_res
1
0 0
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint1 objective1 camera2 high_res
1
0 0
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint1 objective1 camera2 low_res
1
0 0
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint1 objective3 camera1 colour
1
0 0
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint1 objective3 camera1 high_res
1
0 0
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint1 objective3 camera1 low_res
1
0 0
2
0 2 0 1
0 7 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint1 objective3 camera2 high_res
1
0 0
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint1 objective3 camera2 low_res
1
0 0
2
0 1 0 1
0 7 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint2 objective3 camera1 colour
1
0 1
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint2 objective3 camera1 high_res
1
0 1
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint2 objective3 camera1 low_res
1
0 1
2
0 2 0 1
0 7 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint2 objective3 camera2 high_res
1
0 1
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint2 objective3 camera2 low_res
1
0 1
2
0 1 0 1
0 7 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint3 objective3 camera1 colour
1
0 2
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint3 objective3 camera1 high_res
1
0 2
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint3 objective3 camera1 low_res
1
0 2
2
0 2 0 1
0 7 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint3 objective3 camera2 high_res
1
0 2
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint3 objective3 camera2 low_res
1
0 2
2
0 1 0 1
0 7 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint3 objective4 camera1 colour
1
0 2
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint3 objective4 camera1 high_res
1
0 2
2
0 2 0 1
0 5 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint3 objective4 camera1 low_res
1
0 2
2
0 2 0 1
0 3 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint3 objective4 camera2 high_res
1
0 2
2
0 1 0 1
0 5 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint3 objective4 camera2 low_res
1
0 2
2
0 1 0 1
0 3 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint4 objective3 camera1 colour
1
0 3
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint4 objective3 camera1 high_res
1
0 3
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint4 objective3 camera1 low_res
1
0 3
2
0 2 0 1
0 7 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint4 objective3 camera2 high_res
1
0 3
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint4 objective3 camera2 low_res
1
0 3
2
0 1 0 1
0 7 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint4 objective4 camera1 colour
1
0 3
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint4 objective4 camera1 high_res
1
0 3
2
0 2 0 1
0 5 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint4 objective4 camera1 low_res
1
0 3
2
0 2 0 1
0 3 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint4 objective4 camera2 high_res
1
0 3
2
0 1 0 1
0 5 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint4 objective4 camera2 low_res
1
0 3
2
0 1 0 1
0 3 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint5 objective3 camera1 colour
1
0 4
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint5 objective3 camera1 high_res
1
0 4
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint5 objective3 camera1 low_res
1
0 4
2
0 2 0 1
0 7 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint5 objective3 camera2 high_res
1
0 4
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint5 objective3 camera2 low_res
1
0 4
2
0 1 0 1
0 7 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint5 objective4 camera1 colour
1
0 4
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint5 objective4 camera1 high_res
1
0 4
2
0 2 0 1
0 5 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint5 objective4 camera1 low_res
1
0 4
2
0 2 0 1
0 3 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint5 objective4 camera2 high_res
1
0 4
2
0 1 0 1
0 5 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint5 objective4 camera2 low_res
1
0 4
2
0 1 0 1
0 3 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint6 objective1 camera1 colour
1
0 5
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective1 camera1 high_res
1
0 5
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective1 camera1 low_res
1
0 5
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective1 camera2 high_res
1
0 5
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective1 camera2 low_res
1
0 5
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective2 camera1 colour
1
0 5
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective2 camera1 high_res
1
0 5
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective2 camera1 low_res
1
0 5
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective2 camera2 high_res
1
0 5
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective2 camera2 low_res
1
0 5
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective3 camera1 colour
1
0 5
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective3 camera1 high_res
1
0 5
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective3 camera1 low_res
1
0 5
2
0 2 0 1
0 7 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint6 objective3 camera2 high_res
1
0 5
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective3 camera2 low_res
1
0 5
2
0 1 0 1
0 7 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint6 objective4 camera1 colour
1
0 5
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective4 camera1 high_res
1
0 5
2
0 2 0 1
0 5 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint6 objective4 camera1 low_res
1
0 5
2
0 2 0 1
0 3 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint6 objective4 camera2 high_res
1
0 5
2
0 1 0 1
0 5 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint6 objective4 camera2 low_res
1
0 5
2
0 1 0 1
0 3 -1 0
1
end_operator
0
