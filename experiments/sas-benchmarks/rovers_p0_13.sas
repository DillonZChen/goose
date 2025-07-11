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
Atom have_image(rover2, objective2, low_res)
NegatedAtom have_image(rover2, objective2, low_res)
end_variable
begin_variable
var3
-1
2
Atom communicated_image_data(objective2, low_res)
NegatedAtom communicated_image_data(objective2, low_res)
end_variable
begin_variable
var4
-1
2
Atom calibrated(camera1, rover2)
NegatedAtom calibrated(camera1, rover2)
end_variable
begin_variable
var5
-1
2
Atom have_image(rover2, objective5, high_res)
NegatedAtom have_image(rover2, objective5, high_res)
end_variable
begin_variable
var6
-1
2
Atom communicated_image_data(objective5, high_res)
NegatedAtom communicated_image_data(objective5, high_res)
end_variable
begin_variable
var7
-1
2
Atom have_image(rover2, objective5, colour)
NegatedAtom have_image(rover2, objective5, colour)
end_variable
begin_variable
var8
-1
2
Atom communicated_image_data(objective5, colour)
NegatedAtom communicated_image_data(objective5, colour)
end_variable
begin_variable
var9
-1
2
Atom have_image(rover2, objective4, high_res)
NegatedAtom have_image(rover2, objective4, high_res)
end_variable
begin_variable
var10
-1
2
Atom communicated_image_data(objective4, high_res)
NegatedAtom communicated_image_data(objective4, high_res)
end_variable
begin_variable
var11
-1
2
Atom have_image(rover2, objective4, colour)
NegatedAtom have_image(rover2, objective4, colour)
end_variable
begin_variable
var12
-1
2
Atom communicated_image_data(objective4, colour)
NegatedAtom communicated_image_data(objective4, colour)
end_variable
begin_variable
var13
-1
2
Atom have_image(rover2, objective3, high_res)
NegatedAtom have_image(rover2, objective3, high_res)
end_variable
begin_variable
var14
-1
2
Atom communicated_image_data(objective3, high_res)
NegatedAtom communicated_image_data(objective3, high_res)
end_variable
begin_variable
var15
-1
2
Atom have_image(rover2, objective3, colour)
NegatedAtom have_image(rover2, objective3, colour)
end_variable
begin_variable
var16
-1
2
Atom communicated_image_data(objective3, colour)
NegatedAtom communicated_image_data(objective3, colour)
end_variable
begin_variable
var17
-1
2
Atom have_image(rover2, objective2, colour)
NegatedAtom have_image(rover2, objective2, colour)
end_variable
begin_variable
var18
-1
2
Atom communicated_image_data(objective2, colour)
NegatedAtom communicated_image_data(objective2, colour)
end_variable
begin_variable
var19
-1
2
Atom have_image(rover2, objective1, high_res)
NegatedAtom have_image(rover2, objective1, high_res)
end_variable
begin_variable
var20
-1
2
Atom communicated_image_data(objective1, high_res)
NegatedAtom communicated_image_data(objective1, high_res)
end_variable
begin_variable
var21
-1
2
Atom have_image(rover2, objective1, colour)
NegatedAtom have_image(rover2, objective1, colour)
end_variable
begin_variable
var22
-1
2
Atom communicated_image_data(objective1, colour)
NegatedAtom communicated_image_data(objective1, colour)
end_variable
begin_variable
var23
-1
2
Atom at_rock_sample(waypoint4)
Atom have_rock_analysis(rover2, waypoint4)
end_variable
begin_variable
var24
-1
2
Atom at_rock_sample(waypoint6)
Atom have_rock_analysis(rover2, waypoint6)
end_variable
begin_variable
var25
-1
2
Atom at_soil_sample(waypoint3)
Atom have_soil_analysis(rover2, waypoint3)
end_variable
begin_variable
var26
-1
2
Atom at_soil_sample(waypoint4)
Atom have_soil_analysis(rover2, waypoint4)
end_variable
begin_variable
var27
-1
2
Atom empty(rover2store)
Atom full(rover2store)
end_variable
begin_variable
var28
-1
2
Atom communicated_rock_data(waypoint6)
NegatedAtom communicated_rock_data(waypoint6)
end_variable
0
begin_state
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
1
1
1
1
1
1
1
1
1
0
0
0
0
0
1
end_state
begin_goal
11
3 0
6 0
8 0
10 0
12 0
14 0
16 0
18 0
20 0
22 0
28 0
end_goal
122
begin_operator
calibrate rover2 camera1 objective2 waypoint1
1
0 0
1
0 4 -1 0
1
end_operator
begin_operator
calibrate rover2 camera1 objective2 waypoint2
1
0 1
1
0 4 -1 0
1
end_operator
begin_operator
calibrate rover2 camera1 objective2 waypoint3
1
0 2
1
0 4 -1 0
1
end_operator
begin_operator
calibrate rover2 camera1 objective2 waypoint4
1
0 3
1
0 4 -1 0
1
end_operator
begin_operator
calibrate rover2 camera1 objective2 waypoint5
1
0 4
1
0 4 -1 0
1
end_operator
begin_operator
calibrate rover2 camera1 objective2 waypoint6
1
0 5
1
0 4 -1 0
1
end_operator
begin_operator
calibrate rover2 camera2 objective2 waypoint1
1
0 0
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover2 camera2 objective2 waypoint2
1
0 1
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover2 camera2 objective2 waypoint3
1
0 2
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover2 camera2 objective2 waypoint4
1
0 3
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover2 camera2 objective2 waypoint5
1
0 4
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover2 camera2 objective2 waypoint6
1
0 5
1
0 1 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective1 colour waypoint3 waypoint1
2
0 2
21 0
1
0 22 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective1 colour waypoint4 waypoint1
2
0 3
21 0
1
0 22 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective1 colour waypoint5 waypoint1
2
0 4
21 0
1
0 22 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective1 colour waypoint6 waypoint1
2
0 5
21 0
1
0 22 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective1 high_res waypoint3 waypoint1
2
0 2
19 0
1
0 20 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective1 high_res waypoint4 waypoint1
2
0 3
19 0
1
0 20 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective1 high_res waypoint5 waypoint1
2
0 4
19 0
1
0 20 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective1 high_res waypoint6 waypoint1
2
0 5
19 0
1
0 20 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective2 colour waypoint3 waypoint1
2
0 2
17 0
1
0 18 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective2 colour waypoint4 waypoint1
2
0 3
17 0
1
0 18 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective2 colour waypoint5 waypoint1
2
0 4
17 0
1
0 18 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective2 colour waypoint6 waypoint1
2
0 5
17 0
1
0 18 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective2 low_res waypoint3 waypoint1
2
0 2
2 0
1
0 3 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective2 low_res waypoint4 waypoint1
2
0 3
2 0
1
0 3 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective2 low_res waypoint5 waypoint1
2
0 4
2 0
1
0 3 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective2 low_res waypoint6 waypoint1
2
0 5
2 0
1
0 3 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective3 colour waypoint3 waypoint1
2
0 2
15 0
1
0 16 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective3 colour waypoint4 waypoint1
2
0 3
15 0
1
0 16 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective3 colour waypoint5 waypoint1
2
0 4
15 0
1
0 16 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective3 colour waypoint6 waypoint1
2
0 5
15 0
1
0 16 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective3 high_res waypoint3 waypoint1
2
0 2
13 0
1
0 14 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective3 high_res waypoint4 waypoint1
2
0 3
13 0
1
0 14 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective3 high_res waypoint5 waypoint1
2
0 4
13 0
1
0 14 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective3 high_res waypoint6 waypoint1
2
0 5
13 0
1
0 14 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective4 colour waypoint3 waypoint1
2
0 2
11 0
1
0 12 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective4 colour waypoint4 waypoint1
2
0 3
11 0
1
0 12 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective4 colour waypoint5 waypoint1
2
0 4
11 0
1
0 12 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective4 colour waypoint6 waypoint1
2
0 5
11 0
1
0 12 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective4 high_res waypoint3 waypoint1
2
0 2
9 0
1
0 10 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective4 high_res waypoint4 waypoint1
2
0 3
9 0
1
0 10 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective4 high_res waypoint5 waypoint1
2
0 4
9 0
1
0 10 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective4 high_res waypoint6 waypoint1
2
0 5
9 0
1
0 10 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective5 colour waypoint3 waypoint1
2
0 2
7 0
1
0 8 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective5 colour waypoint4 waypoint1
2
0 3
7 0
1
0 8 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective5 colour waypoint5 waypoint1
2
0 4
7 0
1
0 8 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective5 colour waypoint6 waypoint1
2
0 5
7 0
1
0 8 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective5 high_res waypoint3 waypoint1
2
0 2
5 0
1
0 6 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective5 high_res waypoint4 waypoint1
2
0 3
5 0
1
0 6 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective5 high_res waypoint5 waypoint1
2
0 4
5 0
1
0 6 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective5 high_res waypoint6 waypoint1
2
0 5
5 0
1
0 6 -1 0
1
end_operator
begin_operator
communicate_rock_data rover2 general waypoint6 waypoint3 waypoint1
2
0 2
24 1
1
0 28 -1 0
1
end_operator
begin_operator
communicate_rock_data rover2 general waypoint6 waypoint4 waypoint1
2
0 3
24 1
1
0 28 -1 0
1
end_operator
begin_operator
communicate_rock_data rover2 general waypoint6 waypoint5 waypoint1
2
0 4
24 1
1
0 28 -1 0
1
end_operator
begin_operator
communicate_rock_data rover2 general waypoint6 waypoint6 waypoint1
2
0 5
24 1
1
0 28 -1 0
1
end_operator
begin_operator
drop rover2 rover2store
0
1
0 27 1 0
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
navigate rover2 waypoint1 waypoint6
0
1
0 0 0 5
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
navigate rover2 waypoint3 waypoint2
0
1
0 0 2 1
1
end_operator
begin_operator
navigate rover2 waypoint3 waypoint5
0
1
0 0 2 4
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
navigate rover2 waypoint5 waypoint1
0
1
0 0 4 0
1
end_operator
begin_operator
navigate rover2 waypoint5 waypoint3
0
1
0 0 4 2
1
end_operator
begin_operator
navigate rover2 waypoint6 waypoint1
0
1
0 0 5 0
1
end_operator
begin_operator
sample_rock rover2 rover2store waypoint4
1
0 3
2
0 23 0 1
0 27 0 1
1
end_operator
begin_operator
sample_rock rover2 rover2store waypoint6
1
0 5
2
0 24 0 1
0 27 0 1
1
end_operator
begin_operator
sample_soil rover2 rover2store waypoint3
1
0 2
2
0 25 0 1
0 27 0 1
1
end_operator
begin_operator
sample_soil rover2 rover2store waypoint4
1
0 3
2
0 26 0 1
0 27 0 1
1
end_operator
begin_operator
take_image rover2 waypoint1 objective2 camera1 colour
1
0 0
2
0 4 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint1 objective2 camera1 high_res
1
0 0
1
0 4 0 1
1
end_operator
begin_operator
take_image rover2 waypoint1 objective2 camera2 low_res
1
0 0
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint1 objective3 camera1 colour
1
0 0
2
0 4 0 1
0 15 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint1 objective3 camera1 high_res
1
0 0
2
0 4 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint1 objective3 camera2 low_res
1
0 0
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint2 objective1 camera1 colour
1
0 1
2
0 4 0 1
0 21 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint2 objective1 camera1 high_res
1
0 1
2
0 4 0 1
0 19 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint2 objective1 camera2 low_res
1
0 1
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint2 objective2 camera1 colour
1
0 1
2
0 4 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint2 objective2 camera1 high_res
1
0 1
1
0 4 0 1
1
end_operator
begin_operator
take_image rover2 waypoint2 objective2 camera2 low_res
1
0 1
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint2 objective3 camera1 colour
1
0 1
2
0 4 0 1
0 15 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint2 objective3 camera1 high_res
1
0 1
2
0 4 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint2 objective3 camera2 low_res
1
0 1
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint2 objective4 camera1 colour
1
0 1
2
0 4 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint2 objective4 camera1 high_res
1
0 1
2
0 4 0 1
0 9 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint2 objective4 camera2 low_res
1
0 1
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint3 objective1 camera1 colour
1
0 2
2
0 4 0 1
0 21 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint3 objective1 camera1 high_res
1
0 2
2
0 4 0 1
0 19 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint3 objective1 camera2 low_res
1
0 2
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint3 objective2 camera1 colour
1
0 2
2
0 4 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint3 objective2 camera1 high_res
1
0 2
1
0 4 0 1
1
end_operator
begin_operator
take_image rover2 waypoint3 objective2 camera2 low_res
1
0 2
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint3 objective4 camera1 colour
1
0 2
2
0 4 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint3 objective4 camera1 high_res
1
0 2
2
0 4 0 1
0 9 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint3 objective4 camera2 low_res
1
0 2
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint4 objective2 camera1 colour
1
0 3
2
0 4 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint4 objective2 camera1 high_res
1
0 3
1
0 4 0 1
1
end_operator
begin_operator
take_image rover2 waypoint4 objective2 camera2 low_res
1
0 3
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint4 objective4 camera1 colour
1
0 3
2
0 4 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint4 objective4 camera1 high_res
1
0 3
2
0 4 0 1
0 9 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint4 objective4 camera2 low_res
1
0 3
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint5 objective2 camera1 colour
1
0 4
2
0 4 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint5 objective2 camera1 high_res
1
0 4
1
0 4 0 1
1
end_operator
begin_operator
take_image rover2 waypoint5 objective2 camera2 low_res
1
0 4
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint5 objective5 camera1 colour
1
0 4
2
0 4 0 1
0 7 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint5 objective5 camera1 high_res
1
0 4
2
0 4 0 1
0 5 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint5 objective5 camera2 low_res
1
0 4
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective1 camera1 colour
1
0 5
2
0 4 0 1
0 21 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint6 objective1 camera1 high_res
1
0 5
2
0 4 0 1
0 19 -1 0
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
2
0 4 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint6 objective2 camera1 high_res
1
0 5
1
0 4 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective2 camera2 low_res
1
0 5
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint6 objective3 camera1 colour
1
0 5
2
0 4 0 1
0 15 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint6 objective3 camera1 high_res
1
0 5
2
0 4 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint6 objective3 camera2 low_res
1
0 5
1
0 1 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective4 camera1 colour
1
0 5
2
0 4 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint6 objective4 camera1 high_res
1
0 5
2
0 4 0 1
0 9 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint6 objective4 camera2 low_res
1
0 5
1
0 1 0 1
1
end_operator
0
