begin_version
3
end_version
begin_metric
0
end_metric
15
begin_variable
var0
-1
7
Atom at(rover1, waypoint1)
Atom at(rover1, waypoint2)
Atom at(rover1, waypoint3)
Atom at(rover1, waypoint4)
Atom at(rover1, waypoint5)
Atom at(rover1, waypoint6)
Atom at(rover1, waypoint7)
end_variable
begin_variable
var1
-1
2
Atom calibrated(camera2, rover1)
NegatedAtom calibrated(camera2, rover1)
end_variable
begin_variable
var2
-1
2
Atom have_image(rover1, objective5, low_res)
NegatedAtom have_image(rover1, objective5, low_res)
end_variable
begin_variable
var3
-1
2
Atom communicated_image_data(objective5, low_res)
NegatedAtom communicated_image_data(objective5, low_res)
end_variable
begin_variable
var4
-1
2
Atom have_image(rover1, objective3, low_res)
NegatedAtom have_image(rover1, objective3, low_res)
end_variable
begin_variable
var5
-1
2
Atom communicated_image_data(objective3, low_res)
NegatedAtom communicated_image_data(objective3, low_res)
end_variable
begin_variable
var6
-1
2
Atom have_image(rover1, objective2, low_res)
NegatedAtom have_image(rover1, objective2, low_res)
end_variable
begin_variable
var7
-1
2
Atom communicated_image_data(objective2, low_res)
NegatedAtom communicated_image_data(objective2, low_res)
end_variable
begin_variable
var8
-1
2
Atom calibrated(camera1, rover1)
NegatedAtom calibrated(camera1, rover1)
end_variable
begin_variable
var9
-1
2
Atom have_image(rover1, objective5, colour)
NegatedAtom have_image(rover1, objective5, colour)
end_variable
begin_variable
var10
-1
2
Atom communicated_image_data(objective5, colour)
NegatedAtom communicated_image_data(objective5, colour)
end_variable
begin_variable
var11
-1
2
Atom have_image(rover1, objective4, high_res)
NegatedAtom have_image(rover1, objective4, high_res)
end_variable
begin_variable
var12
-1
2
Atom communicated_image_data(objective4, high_res)
NegatedAtom communicated_image_data(objective4, high_res)
end_variable
begin_variable
var13
-1
2
Atom have_image(rover1, objective3, high_res)
NegatedAtom have_image(rover1, objective3, high_res)
end_variable
begin_variable
var14
-1
2
Atom communicated_image_data(objective3, high_res)
NegatedAtom communicated_image_data(objective3, high_res)
end_variable
0
begin_state
0
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
1
1
1
end_state
begin_goal
6
3 0
5 0
7 0
10 0
12 0
14 0
end_goal
112
begin_operator
calibrate rover1 camera1 objective1 waypoint2
1
0 1
1
0 8 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective1 waypoint3
1
0 2
1
0 8 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective1 waypoint4
1
0 3
1
0 8 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective1 waypoint5
1
0 4
1
0 8 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective1 waypoint6
1
0 5
1
0 8 -1 0
1
end_operator
begin_operator
calibrate rover1 camera2 objective5 waypoint3
1
0 2
1
0 1 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective2 low_res waypoint2 waypoint1
2
0 1
6 0
1
0 7 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective2 low_res waypoint4 waypoint1
2
0 3
6 0
1
0 7 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 high_res waypoint2 waypoint1
2
0 1
13 0
1
0 14 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 high_res waypoint4 waypoint1
2
0 3
13 0
1
0 14 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 low_res waypoint2 waypoint1
2
0 1
4 0
1
0 5 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 low_res waypoint4 waypoint1
2
0 3
4 0
1
0 5 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 high_res waypoint2 waypoint1
2
0 1
11 0
1
0 12 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 high_res waypoint4 waypoint1
2
0 3
11 0
1
0 12 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective5 colour waypoint2 waypoint1
2
0 1
9 0
1
0 10 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective5 colour waypoint4 waypoint1
2
0 3
9 0
1
0 10 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective5 low_res waypoint2 waypoint1
2
0 1
2 0
1
0 3 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective5 low_res waypoint4 waypoint1
2
0 3
2 0
1
0 3 -1 0
1
end_operator
begin_operator
navigate rover1 waypoint1 waypoint2
0
1
0 0 0 1
1
end_operator
begin_operator
navigate rover1 waypoint2 waypoint1
0
1
0 0 1 0
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
navigate rover1 waypoint2 waypoint5
0
1
0 0 1 4
1
end_operator
begin_operator
navigate rover1 waypoint2 waypoint6
0
1
0 0 1 5
1
end_operator
begin_operator
navigate rover1 waypoint2 waypoint7
0
1
0 0 1 6
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
navigate rover1 waypoint3 waypoint6
0
1
0 0 2 5
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
navigate rover1 waypoint5 waypoint2
0
1
0 0 4 1
1
end_operator
begin_operator
navigate rover1 waypoint6 waypoint2
0
1
0 0 5 1
1
end_operator
begin_operator
navigate rover1 waypoint6 waypoint3
0
1
0 0 5 2
1
end_operator
begin_operator
navigate rover1 waypoint7 waypoint2
0
1
0 0 6 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective3 camera1 colour
1
0 0
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective3 camera1 high_res
1
0 0
2
0 8 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective3 camera2 high_res
1
0 0
2
0 1 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective3 camera2 low_res
1
0 0
2
0 1 0 1
0 4 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective4 camera1 colour
1
0 0
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective4 camera1 high_res
1
0 0
2
0 8 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective4 camera2 high_res
1
0 0
2
0 1 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective4 camera2 low_res
1
0 0
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective1 camera1 colour
1
0 1
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective1 camera1 high_res
1
0 1
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective1 camera2 high_res
1
0 1
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective1 camera2 low_res
1
0 1
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective3 camera1 colour
1
0 1
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective3 camera1 high_res
1
0 1
2
0 8 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective3 camera2 high_res
1
0 1
2
0 1 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective3 camera2 low_res
1
0 1
2
0 1 0 1
0 4 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective4 camera1 colour
1
0 1
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective4 camera1 high_res
1
0 1
2
0 8 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective4 camera2 high_res
1
0 1
2
0 1 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective4 camera2 low_res
1
0 1
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective1 camera1 colour
1
0 2
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective1 camera1 high_res
1
0 2
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective1 camera2 high_res
1
0 2
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective1 camera2 low_res
1
0 2
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera1 colour
1
0 2
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera1 high_res
1
0 2
2
0 8 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera2 high_res
1
0 2
2
0 1 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera2 low_res
1
0 2
2
0 1 0 1
0 4 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective4 camera1 colour
1
0 2
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective4 camera1 high_res
1
0 2
2
0 8 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective4 camera2 high_res
1
0 2
2
0 1 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective4 camera2 low_res
1
0 2
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective5 camera1 colour
1
0 2
2
0 8 0 1
0 9 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective5 camera1 high_res
1
0 2
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective5 camera2 high_res
1
0 2
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective5 camera2 low_res
1
0 2
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective1 camera1 colour
1
0 3
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective1 camera1 high_res
1
0 3
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective1 camera2 high_res
1
0 3
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective1 camera2 low_res
1
0 3
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective3 camera1 colour
1
0 3
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective3 camera1 high_res
1
0 3
2
0 8 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective3 camera2 high_res
1
0 3
2
0 1 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective3 camera2 low_res
1
0 3
2
0 1 0 1
0 4 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective4 camera1 colour
1
0 3
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective4 camera1 high_res
1
0 3
2
0 8 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective4 camera2 high_res
1
0 3
2
0 1 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective4 camera2 low_res
1
0 3
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective1 camera1 colour
1
0 4
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective1 camera1 high_res
1
0 4
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective1 camera2 high_res
1
0 4
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective1 camera2 low_res
1
0 4
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective2 camera1 colour
1
0 4
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective2 camera1 high_res
1
0 4
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective2 camera2 high_res
1
0 4
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective2 camera2 low_res
1
0 4
2
0 1 0 1
0 6 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective3 camera1 colour
1
0 4
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective3 camera1 high_res
1
0 4
2
0 8 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective3 camera2 high_res
1
0 4
2
0 1 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective3 camera2 low_res
1
0 4
2
0 1 0 1
0 4 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective1 camera1 colour
1
0 5
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective1 camera1 high_res
1
0 5
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective1 camera2 high_res
1
0 5
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective1 camera2 low_res
1
0 5
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective2 camera1 colour
1
0 5
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective2 camera1 high_res
1
0 5
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective2 camera2 high_res
1
0 5
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective2 camera2 low_res
1
0 5
2
0 1 0 1
0 6 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective3 camera1 colour
1
0 5
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective3 camera1 high_res
1
0 5
2
0 8 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective3 camera2 high_res
1
0 5
2
0 1 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective3 camera2 low_res
1
0 5
2
0 1 0 1
0 4 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective2 camera1 colour
1
0 6
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective2 camera1 high_res
1
0 6
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective2 camera2 high_res
1
0 6
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective2 camera2 low_res
1
0 6
2
0 1 0 1
0 6 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective3 camera1 colour
1
0 6
1
0 8 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective3 camera1 high_res
1
0 6
2
0 8 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective3 camera2 high_res
1
0 6
2
0 1 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective3 camera2 low_res
1
0 6
2
0 1 0 1
0 4 -1 0
1
end_operator
0
