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
Atom have_image(rover1, objective5, high_res)
NegatedAtom have_image(rover1, objective5, high_res)
end_variable
begin_variable
var3
-1
2
Atom communicated_image_data(objective5, high_res)
NegatedAtom communicated_image_data(objective5, high_res)
end_variable
begin_variable
var4
-1
2
Atom have_image(rover1, objective4, high_res)
NegatedAtom have_image(rover1, objective4, high_res)
end_variable
begin_variable
var5
-1
2
Atom communicated_image_data(objective4, high_res)
NegatedAtom communicated_image_data(objective4, high_res)
end_variable
begin_variable
var6
-1
2
Atom have_image(rover1, objective3, high_res)
NegatedAtom have_image(rover1, objective3, high_res)
end_variable
begin_variable
var7
-1
2
Atom communicated_image_data(objective3, high_res)
NegatedAtom communicated_image_data(objective3, high_res)
end_variable
begin_variable
var8
-1
2
Atom have_image(rover1, objective2, high_res)
NegatedAtom have_image(rover1, objective2, high_res)
end_variable
begin_variable
var9
-1
2
Atom communicated_image_data(objective2, high_res)
NegatedAtom communicated_image_data(objective2, high_res)
end_variable
begin_variable
var10
-1
2
Atom have_image(rover1, objective1, high_res)
NegatedAtom have_image(rover1, objective1, high_res)
end_variable
begin_variable
var11
-1
2
Atom communicated_image_data(objective1, high_res)
NegatedAtom communicated_image_data(objective1, high_res)
end_variable
begin_variable
var12
-1
2
Atom calibrated(camera1, rover1)
NegatedAtom calibrated(camera1, rover1)
end_variable
begin_variable
var13
-1
2
Atom have_image(rover1, objective5, low_res)
NegatedAtom have_image(rover1, objective5, low_res)
end_variable
begin_variable
var14
-1
2
Atom communicated_image_data(objective5, low_res)
NegatedAtom communicated_image_data(objective5, low_res)
end_variable
begin_variable
var15
-1
2
Atom have_image(rover1, objective4, low_res)
NegatedAtom have_image(rover1, objective4, low_res)
end_variable
begin_variable
var16
-1
2
Atom communicated_image_data(objective4, low_res)
NegatedAtom communicated_image_data(objective4, low_res)
end_variable
begin_variable
var17
-1
2
Atom have_image(rover1, objective4, colour)
NegatedAtom have_image(rover1, objective4, colour)
end_variable
begin_variable
var18
-1
2
Atom communicated_image_data(objective4, colour)
NegatedAtom communicated_image_data(objective4, colour)
end_variable
begin_variable
var19
-1
2
Atom have_image(rover1, objective3, low_res)
NegatedAtom have_image(rover1, objective3, low_res)
end_variable
begin_variable
var20
-1
2
Atom communicated_image_data(objective3, low_res)
NegatedAtom communicated_image_data(objective3, low_res)
end_variable
begin_variable
var21
-1
2
Atom have_image(rover1, objective3, colour)
NegatedAtom have_image(rover1, objective3, colour)
end_variable
begin_variable
var22
-1
2
Atom communicated_image_data(objective3, colour)
NegatedAtom communicated_image_data(objective3, colour)
end_variable
begin_variable
var23
-1
2
Atom have_image(rover1, objective2, colour)
NegatedAtom have_image(rover1, objective2, colour)
end_variable
begin_variable
var24
-1
2
Atom communicated_image_data(objective2, colour)
NegatedAtom communicated_image_data(objective2, colour)
end_variable
begin_variable
var25
-1
2
Atom have_image(rover1, objective1, low_res)
NegatedAtom have_image(rover1, objective1, low_res)
end_variable
begin_variable
var26
-1
2
Atom communicated_image_data(objective1, low_res)
NegatedAtom communicated_image_data(objective1, low_res)
end_variable
begin_variable
var27
-1
2
Atom have_image(rover1, objective1, colour)
NegatedAtom have_image(rover1, objective1, colour)
end_variable
begin_variable
var28
-1
2
Atom communicated_image_data(objective1, colour)
NegatedAtom communicated_image_data(objective1, colour)
end_variable
0
begin_state
4
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
1
1
1
1
1
end_state
begin_goal
13
3 0
5 0
7 0
9 0
11 0
14 0
16 0
18 0
20 0
22 0
24 0
26 0
28 0
end_goal
182
begin_operator
calibrate rover1 camera1 objective1 waypoint1
1
0 0
1
0 12 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective1 waypoint2
1
0 1
1
0 12 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective1 waypoint3
1
0 2
1
0 12 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective1 waypoint5
1
0 4
1
0 12 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective1 waypoint6
1
0 5
1
0 12 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective1 waypoint7
1
0 6
1
0 12 -1 0
1
end_operator
begin_operator
calibrate rover1 camera2 objective5 waypoint1
1
0 0
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover1 camera2 objective5 waypoint2
1
0 1
1
0 1 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 colour waypoint1 waypoint5
2
0 0
27 0
1
0 28 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 colour waypoint4 waypoint5
2
0 3
27 0
1
0 28 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 colour waypoint6 waypoint5
2
0 5
27 0
1
0 28 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 colour waypoint7 waypoint5
2
0 6
27 0
1
0 28 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 high_res waypoint1 waypoint5
2
0 0
10 0
1
0 11 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 high_res waypoint4 waypoint5
2
0 3
10 0
1
0 11 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 high_res waypoint6 waypoint5
2
0 5
10 0
1
0 11 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 high_res waypoint7 waypoint5
2
0 6
10 0
1
0 11 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 low_res waypoint1 waypoint5
2
0 0
25 0
1
0 26 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 low_res waypoint4 waypoint5
2
0 3
25 0
1
0 26 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 low_res waypoint6 waypoint5
2
0 5
25 0
1
0 26 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 low_res waypoint7 waypoint5
2
0 6
25 0
1
0 26 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective2 colour waypoint1 waypoint5
2
0 0
23 0
1
0 24 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective2 colour waypoint4 waypoint5
2
0 3
23 0
1
0 24 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective2 colour waypoint6 waypoint5
2
0 5
23 0
1
0 24 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective2 colour waypoint7 waypoint5
2
0 6
23 0
1
0 24 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective2 high_res waypoint1 waypoint5
2
0 0
8 0
1
0 9 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective2 high_res waypoint4 waypoint5
2
0 3
8 0
1
0 9 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective2 high_res waypoint6 waypoint5
2
0 5
8 0
1
0 9 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective2 high_res waypoint7 waypoint5
2
0 6
8 0
1
0 9 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 colour waypoint1 waypoint5
2
0 0
21 0
1
0 22 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 colour waypoint4 waypoint5
2
0 3
21 0
1
0 22 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 colour waypoint6 waypoint5
2
0 5
21 0
1
0 22 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 colour waypoint7 waypoint5
2
0 6
21 0
1
0 22 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 high_res waypoint1 waypoint5
2
0 0
6 0
1
0 7 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 high_res waypoint4 waypoint5
2
0 3
6 0
1
0 7 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 high_res waypoint6 waypoint5
2
0 5
6 0
1
0 7 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 high_res waypoint7 waypoint5
2
0 6
6 0
1
0 7 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 low_res waypoint1 waypoint5
2
0 0
19 0
1
0 20 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 low_res waypoint4 waypoint5
2
0 3
19 0
1
0 20 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 low_res waypoint6 waypoint5
2
0 5
19 0
1
0 20 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 low_res waypoint7 waypoint5
2
0 6
19 0
1
0 20 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 colour waypoint1 waypoint5
2
0 0
17 0
1
0 18 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 colour waypoint4 waypoint5
2
0 3
17 0
1
0 18 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 colour waypoint6 waypoint5
2
0 5
17 0
1
0 18 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 colour waypoint7 waypoint5
2
0 6
17 0
1
0 18 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 high_res waypoint1 waypoint5
2
0 0
4 0
1
0 5 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 high_res waypoint4 waypoint5
2
0 3
4 0
1
0 5 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 high_res waypoint6 waypoint5
2
0 5
4 0
1
0 5 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 high_res waypoint7 waypoint5
2
0 6
4 0
1
0 5 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 low_res waypoint1 waypoint5
2
0 0
15 0
1
0 16 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 low_res waypoint4 waypoint5
2
0 3
15 0
1
0 16 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 low_res waypoint6 waypoint5
2
0 5
15 0
1
0 16 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 low_res waypoint7 waypoint5
2
0 6
15 0
1
0 16 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective5 high_res waypoint1 waypoint5
2
0 0
2 0
1
0 3 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective5 high_res waypoint4 waypoint5
2
0 3
2 0
1
0 3 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective5 high_res waypoint6 waypoint5
2
0 5
2 0
1
0 3 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective5 high_res waypoint7 waypoint5
2
0 6
2 0
1
0 3 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective5 low_res waypoint1 waypoint5
2
0 0
13 0
1
0 14 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective5 low_res waypoint4 waypoint5
2
0 3
13 0
1
0 14 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective5 low_res waypoint6 waypoint5
2
0 5
13 0
1
0 14 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective5 low_res waypoint7 waypoint5
2
0 6
13 0
1
0 14 -1 0
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
navigate rover1 waypoint1 waypoint5
0
1
0 0 0 4
1
end_operator
begin_operator
navigate rover1 waypoint1 waypoint7
0
1
0 0 0 6
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
navigate rover1 waypoint4 waypoint3
0
1
0 0 3 2
1
end_operator
begin_operator
navigate rover1 waypoint5 waypoint1
0
1
0 0 4 0
1
end_operator
begin_operator
navigate rover1 waypoint5 waypoint6
0
1
0 0 4 5
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
navigate rover1 waypoint6 waypoint5
0
1
0 0 5 4
1
end_operator
begin_operator
navigate rover1 waypoint7 waypoint1
0
1
0 0 6 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective1 camera1 colour
1
0 0
2
0 12 0 1
0 27 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective1 camera1 low_res
1
0 0
2
0 12 0 1
0 25 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective1 camera2 colour
1
0 0
2
0 1 0 1
0 27 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective1 camera2 high_res
1
0 0
2
0 1 0 1
0 10 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective1 camera2 low_res
1
0 0
2
0 1 0 1
0 25 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective3 camera1 colour
1
0 0
2
0 12 0 1
0 21 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective3 camera1 low_res
1
0 0
2
0 12 0 1
0 19 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective3 camera2 colour
1
0 0
2
0 1 0 1
0 21 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective3 camera2 high_res
1
0 0
2
0 1 0 1
0 6 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective3 camera2 low_res
1
0 0
2
0 1 0 1
0 19 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective4 camera1 colour
1
0 0
2
0 12 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective4 camera1 low_res
1
0 0
2
0 12 0 1
0 15 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective4 camera2 colour
1
0 0
2
0 1 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective4 camera2 high_res
1
0 0
2
0 1 0 1
0 4 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective4 camera2 low_res
1
0 0
2
0 1 0 1
0 15 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective5 camera1 colour
1
0 0
1
0 12 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective5 camera1 low_res
1
0 0
2
0 12 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective5 camera2 colour
1
0 0
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective5 camera2 high_res
1
0 0
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective5 camera2 low_res
1
0 0
2
0 1 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective1 camera1 colour
1
0 1
2
0 12 0 1
0 27 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective1 camera1 low_res
1
0 1
2
0 12 0 1
0 25 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective1 camera2 colour
1
0 1
2
0 1 0 1
0 27 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective1 camera2 high_res
1
0 1
2
0 1 0 1
0 10 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective1 camera2 low_res
1
0 1
2
0 1 0 1
0 25 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective3 camera1 colour
1
0 1
2
0 12 0 1
0 21 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective3 camera1 low_res
1
0 1
2
0 12 0 1
0 19 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective3 camera2 colour
1
0 1
2
0 1 0 1
0 21 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective3 camera2 high_res
1
0 1
2
0 1 0 1
0 6 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective3 camera2 low_res
1
0 1
2
0 1 0 1
0 19 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective4 camera1 colour
1
0 1
2
0 12 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective4 camera1 low_res
1
0 1
2
0 12 0 1
0 15 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective4 camera2 colour
1
0 1
2
0 1 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective4 camera2 high_res
1
0 1
2
0 1 0 1
0 4 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective4 camera2 low_res
1
0 1
2
0 1 0 1
0 15 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective5 camera1 colour
1
0 1
1
0 12 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective5 camera1 low_res
1
0 1
2
0 12 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective5 camera2 colour
1
0 1
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective5 camera2 high_res
1
0 1
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective5 camera2 low_res
1
0 1
2
0 1 0 1
0 13 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective1 camera1 colour
1
0 2
2
0 12 0 1
0 27 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective1 camera1 low_res
1
0 2
2
0 12 0 1
0 25 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective1 camera2 colour
1
0 2
2
0 1 0 1
0 27 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective1 camera2 high_res
1
0 2
2
0 1 0 1
0 10 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective1 camera2 low_res
1
0 2
2
0 1 0 1
0 25 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera1 colour
1
0 2
2
0 12 0 1
0 21 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera1 low_res
1
0 2
2
0 12 0 1
0 19 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera2 colour
1
0 2
2
0 1 0 1
0 21 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera2 high_res
1
0 2
2
0 1 0 1
0 6 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera2 low_res
1
0 2
2
0 1 0 1
0 19 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective4 camera1 colour
1
0 2
2
0 12 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective4 camera1 low_res
1
0 2
2
0 12 0 1
0 15 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective4 camera2 colour
1
0 2
2
0 1 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective4 camera2 high_res
1
0 2
2
0 1 0 1
0 4 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective4 camera2 low_res
1
0 2
2
0 1 0 1
0 15 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective2 camera1 colour
1
0 3
2
0 12 0 1
0 23 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective2 camera1 low_res
1
0 3
1
0 12 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective2 camera2 colour
1
0 3
2
0 1 0 1
0 23 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective2 camera2 high_res
1
0 3
2
0 1 0 1
0 8 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective2 camera2 low_res
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
2
0 12 0 1
0 21 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective3 camera1 low_res
1
0 3
2
0 12 0 1
0 19 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective3 camera2 colour
1
0 3
2
0 1 0 1
0 21 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective3 camera2 high_res
1
0 3
2
0 1 0 1
0 6 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective3 camera2 low_res
1
0 3
2
0 1 0 1
0 19 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective4 camera1 colour
1
0 3
2
0 12 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective4 camera1 low_res
1
0 3
2
0 12 0 1
0 15 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective4 camera2 colour
1
0 3
2
0 1 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective4 camera2 high_res
1
0 3
2
0 1 0 1
0 4 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective4 camera2 low_res
1
0 3
2
0 1 0 1
0 15 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective1 camera1 colour
1
0 4
2
0 12 0 1
0 27 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective1 camera1 low_res
1
0 4
2
0 12 0 1
0 25 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective1 camera2 colour
1
0 4
2
0 1 0 1
0 27 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective1 camera2 high_res
1
0 4
2
0 1 0 1
0 10 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective1 camera2 low_res
1
0 4
2
0 1 0 1
0 25 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective2 camera1 colour
1
0 4
2
0 12 0 1
0 23 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective2 camera1 low_res
1
0 4
1
0 12 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective2 camera2 colour
1
0 4
2
0 1 0 1
0 23 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective2 camera2 high_res
1
0 4
2
0 1 0 1
0 8 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective2 camera2 low_res
1
0 4
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective4 camera1 colour
1
0 4
2
0 12 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective4 camera1 low_res
1
0 4
2
0 12 0 1
0 15 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective4 camera2 colour
1
0 4
2
0 1 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective4 camera2 high_res
1
0 4
2
0 1 0 1
0 4 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective4 camera2 low_res
1
0 4
2
0 1 0 1
0 15 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective1 camera1 colour
1
0 5
2
0 12 0 1
0 27 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective1 camera1 low_res
1
0 5
2
0 12 0 1
0 25 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective1 camera2 colour
1
0 5
2
0 1 0 1
0 27 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective1 camera2 high_res
1
0 5
2
0 1 0 1
0 10 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective1 camera2 low_res
1
0 5
2
0 1 0 1
0 25 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective3 camera1 colour
1
0 5
2
0 12 0 1
0 21 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective3 camera1 low_res
1
0 5
2
0 12 0 1
0 19 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective3 camera2 colour
1
0 5
2
0 1 0 1
0 21 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective3 camera2 high_res
1
0 5
2
0 1 0 1
0 6 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective3 camera2 low_res
1
0 5
2
0 1 0 1
0 19 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective4 camera1 colour
1
0 5
2
0 12 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective4 camera1 low_res
1
0 5
2
0 12 0 1
0 15 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective4 camera2 colour
1
0 5
2
0 1 0 1
0 17 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective4 camera2 high_res
1
0 5
2
0 1 0 1
0 4 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective4 camera2 low_res
1
0 5
2
0 1 0 1
0 15 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective1 camera1 colour
1
0 6
2
0 12 0 1
0 27 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective1 camera1 low_res
1
0 6
2
0 12 0 1
0 25 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective1 camera2 colour
1
0 6
2
0 1 0 1
0 27 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective1 camera2 high_res
1
0 6
2
0 1 0 1
0 10 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective1 camera2 low_res
1
0 6
2
0 1 0 1
0 25 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective3 camera1 colour
1
0 6
2
0 12 0 1
0 21 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective3 camera1 low_res
1
0 6
2
0 12 0 1
0 19 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective3 camera2 colour
1
0 6
2
0 1 0 1
0 21 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective3 camera2 high_res
1
0 6
2
0 1 0 1
0 6 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective3 camera2 low_res
1
0 6
2
0 1 0 1
0 19 -1 0
1
end_operator
0
