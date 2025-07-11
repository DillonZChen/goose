begin_version
3
end_version
begin_metric
0
end_metric
28
begin_variable
var0
-1
7
Atom at(rover3, waypoint1)
Atom at(rover3, waypoint2)
Atom at(rover3, waypoint3)
Atom at(rover3, waypoint4)
Atom at(rover3, waypoint5)
Atom at(rover3, waypoint6)
Atom at(rover3, waypoint7)
end_variable
begin_variable
var1
-1
7
Atom at(rover2, waypoint1)
Atom at(rover2, waypoint2)
Atom at(rover2, waypoint3)
Atom at(rover2, waypoint4)
Atom at(rover2, waypoint5)
Atom at(rover2, waypoint6)
Atom at(rover2, waypoint7)
end_variable
begin_variable
var2
-1
2
Atom calibrated(camera3, rover2)
NegatedAtom calibrated(camera3, rover2)
end_variable
begin_variable
var3
-1
2
Atom have_image(rover2, objective2, high_res)
NegatedAtom have_image(rover2, objective2, high_res)
end_variable
begin_variable
var4
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
var5
-1
2
Atom calibrated(camera2, rover1)
NegatedAtom calibrated(camera2, rover1)
end_variable
begin_variable
var6
-1
2
Atom have_image(rover1, objective3, colour)
NegatedAtom have_image(rover1, objective3, colour)
end_variable
begin_variable
var7
-1
2
Atom communicated_image_data(objective3, colour)
NegatedAtom communicated_image_data(objective3, colour)
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
Atom calibrated(camera1, rover1)
NegatedAtom calibrated(camera1, rover1)
end_variable
begin_variable
var11
-1
2
Atom have_image(rover1, objective4, low_res)
NegatedAtom have_image(rover1, objective4, low_res)
end_variable
begin_variable
var12
-1
2
Atom communicated_image_data(objective4, low_res)
NegatedAtom communicated_image_data(objective4, low_res)
end_variable
begin_variable
var13
-1
4
Atom at_rock_sample(waypoint1)
Atom have_rock_analysis(rover1, waypoint1)
Atom have_rock_analysis(rover2, waypoint1)
Atom have_rock_analysis(rover3, waypoint1)
end_variable
begin_variable
var14
-1
4
Atom at_rock_sample(waypoint7)
Atom have_rock_analysis(rover1, waypoint7)
Atom have_rock_analysis(rover2, waypoint7)
Atom have_rock_analysis(rover3, waypoint7)
end_variable
begin_variable
var15
-1
4
Atom at_soil_sample(waypoint1)
Atom have_soil_analysis(rover1, waypoint1)
Atom have_soil_analysis(rover2, waypoint1)
Atom have_soil_analysis(rover3, waypoint1)
end_variable
begin_variable
var16
-1
4
Atom at_soil_sample(waypoint4)
Atom have_soil_analysis(rover1, waypoint4)
Atom have_soil_analysis(rover2, waypoint4)
Atom have_soil_analysis(rover3, waypoint4)
end_variable
begin_variable
var17
-1
4
Atom at_soil_sample(waypoint5)
Atom have_soil_analysis(rover1, waypoint5)
Atom have_soil_analysis(rover2, waypoint5)
Atom have_soil_analysis(rover3, waypoint5)
end_variable
begin_variable
var18
-1
2
Atom empty(rover1store)
Atom full(rover1store)
end_variable
begin_variable
var19
-1
2
Atom empty(rover2store)
Atom full(rover2store)
end_variable
begin_variable
var20
-1
4
Atom at_soil_sample(waypoint6)
Atom have_soil_analysis(rover1, waypoint6)
Atom have_soil_analysis(rover2, waypoint6)
Atom have_soil_analysis(rover3, waypoint6)
end_variable
begin_variable
var21
-1
4
Atom at_soil_sample(waypoint7)
Atom have_soil_analysis(rover1, waypoint7)
Atom have_soil_analysis(rover2, waypoint7)
Atom have_soil_analysis(rover3, waypoint7)
end_variable
begin_variable
var22
-1
2
Atom empty(rover3store)
Atom full(rover3store)
end_variable
begin_variable
var23
-1
2
Atom communicated_soil_data(waypoint6)
NegatedAtom communicated_soil_data(waypoint6)
end_variable
begin_variable
var24
-1
2
Atom communicated_soil_data(waypoint5)
NegatedAtom communicated_soil_data(waypoint5)
end_variable
begin_variable
var25
-1
2
Atom communicated_soil_data(waypoint4)
NegatedAtom communicated_soil_data(waypoint4)
end_variable
begin_variable
var26
-1
2
Atom communicated_soil_data(waypoint1)
NegatedAtom communicated_soil_data(waypoint1)
end_variable
begin_variable
var27
-1
2
Atom communicated_rock_data(waypoint1)
NegatedAtom communicated_rock_data(waypoint1)
end_variable
0
begin_state
2
4
1
1
3
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
end_state
begin_goal
8
7 0
9 0
12 0
23 0
24 0
25 0
26 0
27 0
end_goal
251
begin_operator
calibrate rover1 camera1 objective2 waypoint1
1
4 0
1
0 10 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective2 waypoint3
1
4 2
1
0 10 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective2 waypoint4
1
4 3
1
0 10 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective2 waypoint6
1
4 5
1
0 10 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective2 waypoint7
1
4 6
1
0 10 -1 0
1
end_operator
begin_operator
calibrate rover1 camera2 objective2 waypoint1
1
4 0
1
0 5 -1 0
1
end_operator
begin_operator
calibrate rover1 camera2 objective2 waypoint3
1
4 2
1
0 5 -1 0
1
end_operator
begin_operator
calibrate rover1 camera2 objective2 waypoint4
1
4 3
1
0 5 -1 0
1
end_operator
begin_operator
calibrate rover1 camera2 objective2 waypoint6
1
4 5
1
0 5 -1 0
1
end_operator
begin_operator
calibrate rover1 camera2 objective2 waypoint7
1
4 6
1
0 5 -1 0
1
end_operator
begin_operator
calibrate rover2 camera3 objective1 waypoint5
1
1 4
1
0 2 -1 0
1
end_operator
begin_operator
calibrate rover2 camera3 objective1 waypoint7
1
1 6
1
0 2 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective2 high_res waypoint3 waypoint2
2
4 2
8 0
1
0 9 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective2 high_res waypoint4 waypoint2
2
4 3
8 0
1
0 9 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective2 high_res waypoint5 waypoint2
2
4 4
8 0
1
0 9 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 colour waypoint3 waypoint2
2
4 2
6 0
1
0 7 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 colour waypoint4 waypoint2
2
4 3
6 0
1
0 7 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective3 colour waypoint5 waypoint2
2
4 4
6 0
1
0 7 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 low_res waypoint3 waypoint2
2
4 2
11 0
1
0 12 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 low_res waypoint4 waypoint2
2
4 3
11 0
1
0 12 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective4 low_res waypoint5 waypoint2
2
4 4
11 0
1
0 12 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective2 high_res waypoint3 waypoint2
2
1 2
3 0
1
0 9 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective2 high_res waypoint4 waypoint2
2
1 3
3 0
1
0 9 -1 0
1
end_operator
begin_operator
communicate_image_data rover2 general objective2 high_res waypoint5 waypoint2
2
1 4
3 0
1
0 9 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint1 waypoint3 waypoint2
2
4 2
13 1
1
0 27 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint1 waypoint4 waypoint2
2
4 3
13 1
1
0 27 -1 0
1
end_operator
begin_operator
communicate_rock_data rover1 general waypoint1 waypoint5 waypoint2
2
4 4
13 1
1
0 27 -1 0
1
end_operator
begin_operator
communicate_rock_data rover2 general waypoint1 waypoint3 waypoint2
2
1 2
13 2
1
0 27 -1 0
1
end_operator
begin_operator
communicate_rock_data rover2 general waypoint1 waypoint4 waypoint2
2
1 3
13 2
1
0 27 -1 0
1
end_operator
begin_operator
communicate_rock_data rover2 general waypoint1 waypoint5 waypoint2
2
1 4
13 2
1
0 27 -1 0
1
end_operator
begin_operator
communicate_rock_data rover3 general waypoint1 waypoint3 waypoint2
2
0 2
13 3
1
0 27 -1 0
1
end_operator
begin_operator
communicate_rock_data rover3 general waypoint1 waypoint4 waypoint2
2
0 3
13 3
1
0 27 -1 0
1
end_operator
begin_operator
communicate_rock_data rover3 general waypoint1 waypoint5 waypoint2
2
0 4
13 3
1
0 27 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint1 waypoint3 waypoint2
2
4 2
15 1
1
0 26 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint1 waypoint4 waypoint2
2
4 3
15 1
1
0 26 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint1 waypoint5 waypoint2
2
4 4
15 1
1
0 26 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint4 waypoint3 waypoint2
2
4 2
16 1
1
0 25 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint4 waypoint4 waypoint2
2
4 3
16 1
1
0 25 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint4 waypoint5 waypoint2
2
4 4
16 1
1
0 25 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint5 waypoint3 waypoint2
2
4 2
17 1
1
0 24 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint5 waypoint4 waypoint2
2
4 3
17 1
1
0 24 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint5 waypoint5 waypoint2
2
4 4
17 1
1
0 24 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint6 waypoint3 waypoint2
2
4 2
20 1
1
0 23 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint6 waypoint4 waypoint2
2
4 3
20 1
1
0 23 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint6 waypoint5 waypoint2
2
4 4
20 1
1
0 23 -1 0
1
end_operator
begin_operator
communicate_soil_data rover2 general waypoint1 waypoint3 waypoint2
2
1 2
15 2
1
0 26 -1 0
1
end_operator
begin_operator
communicate_soil_data rover2 general waypoint1 waypoint4 waypoint2
2
1 3
15 2
1
0 26 -1 0
1
end_operator
begin_operator
communicate_soil_data rover2 general waypoint1 waypoint5 waypoint2
2
1 4
15 2
1
0 26 -1 0
1
end_operator
begin_operator
communicate_soil_data rover2 general waypoint4 waypoint3 waypoint2
2
1 2
16 2
1
0 25 -1 0
1
end_operator
begin_operator
communicate_soil_data rover2 general waypoint4 waypoint4 waypoint2
2
1 3
16 2
1
0 25 -1 0
1
end_operator
begin_operator
communicate_soil_data rover2 general waypoint4 waypoint5 waypoint2
2
1 4
16 2
1
0 25 -1 0
1
end_operator
begin_operator
communicate_soil_data rover2 general waypoint5 waypoint3 waypoint2
2
1 2
17 2
1
0 24 -1 0
1
end_operator
begin_operator
communicate_soil_data rover2 general waypoint5 waypoint4 waypoint2
2
1 3
17 2
1
0 24 -1 0
1
end_operator
begin_operator
communicate_soil_data rover2 general waypoint5 waypoint5 waypoint2
2
1 4
17 2
1
0 24 -1 0
1
end_operator
begin_operator
communicate_soil_data rover2 general waypoint6 waypoint3 waypoint2
2
1 2
20 2
1
0 23 -1 0
1
end_operator
begin_operator
communicate_soil_data rover2 general waypoint6 waypoint4 waypoint2
2
1 3
20 2
1
0 23 -1 0
1
end_operator
begin_operator
communicate_soil_data rover2 general waypoint6 waypoint5 waypoint2
2
1 4
20 2
1
0 23 -1 0
1
end_operator
begin_operator
communicate_soil_data rover3 general waypoint1 waypoint3 waypoint2
2
0 2
15 3
1
0 26 -1 0
1
end_operator
begin_operator
communicate_soil_data rover3 general waypoint1 waypoint4 waypoint2
2
0 3
15 3
1
0 26 -1 0
1
end_operator
begin_operator
communicate_soil_data rover3 general waypoint1 waypoint5 waypoint2
2
0 4
15 3
1
0 26 -1 0
1
end_operator
begin_operator
communicate_soil_data rover3 general waypoint4 waypoint3 waypoint2
2
0 2
16 3
1
0 25 -1 0
1
end_operator
begin_operator
communicate_soil_data rover3 general waypoint4 waypoint4 waypoint2
2
0 3
16 3
1
0 25 -1 0
1
end_operator
begin_operator
communicate_soil_data rover3 general waypoint4 waypoint5 waypoint2
2
0 4
16 3
1
0 25 -1 0
1
end_operator
begin_operator
communicate_soil_data rover3 general waypoint5 waypoint3 waypoint2
2
0 2
17 3
1
0 24 -1 0
1
end_operator
begin_operator
communicate_soil_data rover3 general waypoint5 waypoint4 waypoint2
2
0 3
17 3
1
0 24 -1 0
1
end_operator
begin_operator
communicate_soil_data rover3 general waypoint5 waypoint5 waypoint2
2
0 4
17 3
1
0 24 -1 0
1
end_operator
begin_operator
communicate_soil_data rover3 general waypoint6 waypoint3 waypoint2
2
0 2
20 3
1
0 23 -1 0
1
end_operator
begin_operator
communicate_soil_data rover3 general waypoint6 waypoint4 waypoint2
2
0 3
20 3
1
0 23 -1 0
1
end_operator
begin_operator
communicate_soil_data rover3 general waypoint6 waypoint5 waypoint2
2
0 4
20 3
1
0 23 -1 0
1
end_operator
begin_operator
drop rover1 rover1store
0
1
0 18 1 0
1
end_operator
begin_operator
drop rover2 rover2store
0
1
0 19 1 0
1
end_operator
begin_operator
drop rover3 rover3store
0
1
0 22 1 0
1
end_operator
begin_operator
navigate rover1 waypoint1 waypoint3
0
1
0 4 0 2
1
end_operator
begin_operator
navigate rover1 waypoint1 waypoint4
0
1
0 4 0 3
1
end_operator
begin_operator
navigate rover1 waypoint2 waypoint4
0
1
0 4 1 3
1
end_operator
begin_operator
navigate rover1 waypoint2 waypoint5
0
1
0 4 1 4
1
end_operator
begin_operator
navigate rover1 waypoint3 waypoint1
0
1
0 4 2 0
1
end_operator
begin_operator
navigate rover1 waypoint4 waypoint1
0
1
0 4 3 0
1
end_operator
begin_operator
navigate rover1 waypoint4 waypoint2
0
1
0 4 3 1
1
end_operator
begin_operator
navigate rover1 waypoint4 waypoint6
0
1
0 4 3 5
1
end_operator
begin_operator
navigate rover1 waypoint5 waypoint2
0
1
0 4 4 1
1
end_operator
begin_operator
navigate rover1 waypoint5 waypoint7
0
1
0 4 4 6
1
end_operator
begin_operator
navigate rover1 waypoint6 waypoint4
0
1
0 4 5 3
1
end_operator
begin_operator
navigate rover1 waypoint7 waypoint5
0
1
0 4 6 4
1
end_operator
begin_operator
navigate rover2 waypoint1 waypoint3
0
1
0 1 0 2
1
end_operator
begin_operator
navigate rover2 waypoint1 waypoint4
0
1
0 1 0 3
1
end_operator
begin_operator
navigate rover2 waypoint2 waypoint4
0
1
0 1 1 3
1
end_operator
begin_operator
navigate rover2 waypoint2 waypoint5
0
1
0 1 1 4
1
end_operator
begin_operator
navigate rover2 waypoint3 waypoint1
0
1
0 1 2 0
1
end_operator
begin_operator
navigate rover2 waypoint4 waypoint1
0
1
0 1 3 0
1
end_operator
begin_operator
navigate rover2 waypoint4 waypoint2
0
1
0 1 3 1
1
end_operator
begin_operator
navigate rover2 waypoint4 waypoint6
0
1
0 1 3 5
1
end_operator
begin_operator
navigate rover2 waypoint5 waypoint2
0
1
0 1 4 1
1
end_operator
begin_operator
navigate rover2 waypoint5 waypoint7
0
1
0 1 4 6
1
end_operator
begin_operator
navigate rover2 waypoint6 waypoint4
0
1
0 1 5 3
1
end_operator
begin_operator
navigate rover2 waypoint7 waypoint5
0
1
0 1 6 4
1
end_operator
begin_operator
navigate rover3 waypoint1 waypoint3
0
1
0 0 0 2
1
end_operator
begin_operator
navigate rover3 waypoint1 waypoint4
0
1
0 0 0 3
1
end_operator
begin_operator
navigate rover3 waypoint2 waypoint3
0
1
0 0 1 2
1
end_operator
begin_operator
navigate rover3 waypoint2 waypoint4
0
1
0 0 1 3
1
end_operator
begin_operator
navigate rover3 waypoint2 waypoint5
0
1
0 0 1 4
1
end_operator
begin_operator
navigate rover3 waypoint3 waypoint1
0
1
0 0 2 0
1
end_operator
begin_operator
navigate rover3 waypoint3 waypoint2
0
1
0 0 2 1
1
end_operator
begin_operator
navigate rover3 waypoint4 waypoint1
0
1
0 0 3 0
1
end_operator
begin_operator
navigate rover3 waypoint4 waypoint2
0
1
0 0 3 1
1
end_operator
begin_operator
navigate rover3 waypoint4 waypoint6
0
1
0 0 3 5
1
end_operator
begin_operator
navigate rover3 waypoint5 waypoint2
0
1
0 0 4 1
1
end_operator
begin_operator
navigate rover3 waypoint5 waypoint7
0
1
0 0 4 6
1
end_operator
begin_operator
navigate rover3 waypoint6 waypoint4
0
1
0 0 5 3
1
end_operator
begin_operator
navigate rover3 waypoint7 waypoint5
0
1
0 0 6 4
1
end_operator
begin_operator
sample_rock rover1 rover1store waypoint1
1
4 0
2
0 13 0 1
0 18 0 1
1
end_operator
begin_operator
sample_rock rover1 rover1store waypoint7
1
4 6
2
0 14 0 1
0 18 0 1
1
end_operator
begin_operator
sample_rock rover2 rover2store waypoint1
1
1 0
2
0 13 0 2
0 19 0 1
1
end_operator
begin_operator
sample_rock rover2 rover2store waypoint7
1
1 6
2
0 14 0 2
0 19 0 1
1
end_operator
begin_operator
sample_rock rover3 rover3store waypoint1
1
0 0
2
0 13 0 3
0 22 0 1
1
end_operator
begin_operator
sample_rock rover3 rover3store waypoint7
1
0 6
2
0 14 0 3
0 22 0 1
1
end_operator
begin_operator
sample_soil rover1 rover1store waypoint1
1
4 0
2
0 15 0 1
0 18 0 1
1
end_operator
begin_operator
sample_soil rover1 rover1store waypoint4
1
4 3
2
0 16 0 1
0 18 0 1
1
end_operator
begin_operator
sample_soil rover1 rover1store waypoint5
1
4 4
2
0 17 0 1
0 18 0 1
1
end_operator
begin_operator
sample_soil rover1 rover1store waypoint6
1
4 5
2
0 20 0 1
0 18 0 1
1
end_operator
begin_operator
sample_soil rover1 rover1store waypoint7
1
4 6
2
0 21 0 1
0 18 0 1
1
end_operator
begin_operator
sample_soil rover2 rover2store waypoint1
1
1 0
2
0 15 0 2
0 19 0 1
1
end_operator
begin_operator
sample_soil rover2 rover2store waypoint4
1
1 3
2
0 16 0 2
0 19 0 1
1
end_operator
begin_operator
sample_soil rover2 rover2store waypoint5
1
1 4
2
0 17 0 2
0 19 0 1
1
end_operator
begin_operator
sample_soil rover2 rover2store waypoint6
1
1 5
2
0 20 0 2
0 19 0 1
1
end_operator
begin_operator
sample_soil rover2 rover2store waypoint7
1
1 6
2
0 21 0 2
0 19 0 1
1
end_operator
begin_operator
sample_soil rover3 rover3store waypoint1
1
0 0
2
0 15 0 3
0 22 0 1
1
end_operator
begin_operator
sample_soil rover3 rover3store waypoint4
1
0 3
2
0 16 0 3
0 22 0 1
1
end_operator
begin_operator
sample_soil rover3 rover3store waypoint5
1
0 4
2
0 17 0 3
0 22 0 1
1
end_operator
begin_operator
sample_soil rover3 rover3store waypoint6
1
0 5
2
0 20 0 3
0 22 0 1
1
end_operator
begin_operator
sample_soil rover3 rover3store waypoint7
1
0 6
2
0 21 0 3
0 22 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective2 camera1 low_res
1
4 0
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective2 camera2 colour
1
4 0
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective2 camera2 high_res
1
4 0
2
0 5 0 1
0 8 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective2 camera2 low_res
1
4 0
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective3 camera1 low_res
1
4 0
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective3 camera2 colour
1
4 0
2
0 5 0 1
0 6 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective3 camera2 high_res
1
4 0
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective3 camera2 low_res
1
4 0
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective5 camera1 low_res
1
4 0
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective5 camera2 colour
1
4 0
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective5 camera2 high_res
1
4 0
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective5 camera2 low_res
1
4 0
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective3 camera1 low_res
1
4 1
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective3 camera2 colour
1
4 1
2
0 5 0 1
0 6 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective3 camera2 high_res
1
4 1
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective3 camera2 low_res
1
4 1
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective4 camera1 low_res
1
4 1
2
0 10 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective4 camera2 colour
1
4 1
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective4 camera2 high_res
1
4 1
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective4 camera2 low_res
1
4 1
2
0 5 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective5 camera1 low_res
1
4 1
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective5 camera2 colour
1
4 1
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective5 camera2 high_res
1
4 1
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective5 camera2 low_res
1
4 1
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective2 camera1 low_res
1
4 2
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective2 camera2 colour
1
4 2
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective2 camera2 high_res
1
4 2
2
0 5 0 1
0 8 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective2 camera2 low_res
1
4 2
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera1 low_res
1
4 2
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera2 colour
1
4 2
2
0 5 0 1
0 6 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera2 high_res
1
4 2
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera2 low_res
1
4 2
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective4 camera1 low_res
1
4 2
2
0 10 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective4 camera2 colour
1
4 2
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective4 camera2 high_res
1
4 2
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective4 camera2 low_res
1
4 2
2
0 5 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective2 camera1 low_res
1
4 3
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective2 camera2 colour
1
4 3
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective2 camera2 high_res
1
4 3
2
0 5 0 1
0 8 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective2 camera2 low_res
1
4 3
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective3 camera1 low_res
1
4 3
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective3 camera2 colour
1
4 3
2
0 5 0 1
0 6 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective3 camera2 high_res
1
4 3
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective3 camera2 low_res
1
4 3
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective4 camera1 low_res
1
4 3
2
0 10 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective4 camera2 colour
1
4 3
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective4 camera2 high_res
1
4 3
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective4 camera2 low_res
1
4 3
2
0 5 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective6 camera1 low_res
1
4 3
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective6 camera2 colour
1
4 3
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective6 camera2 high_res
1
4 3
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective6 camera2 low_res
1
4 3
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective1 camera1 low_res
1
4 4
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective1 camera2 colour
1
4 4
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective1 camera2 high_res
1
4 4
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective1 camera2 low_res
1
4 4
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective3 camera1 low_res
1
4 4
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective3 camera2 colour
1
4 4
2
0 5 0 1
0 6 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective3 camera2 high_res
1
4 4
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective3 camera2 low_res
1
4 4
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective6 camera1 low_res
1
4 4
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective6 camera2 colour
1
4 4
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective6 camera2 high_res
1
4 4
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective6 camera2 low_res
1
4 4
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective2 camera1 low_res
1
4 5
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective2 camera2 colour
1
4 5
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective2 camera2 high_res
1
4 5
2
0 5 0 1
0 8 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective2 camera2 low_res
1
4 5
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective4 camera1 low_res
1
4 5
2
0 10 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective4 camera2 colour
1
4 5
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective4 camera2 high_res
1
4 5
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective4 camera2 low_res
1
4 5
2
0 5 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint6 objective5 camera1 low_res
1
4 5
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective5 camera2 colour
1
4 5
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective5 camera2 high_res
1
4 5
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint6 objective5 camera2 low_res
1
4 5
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective1 camera1 low_res
1
4 6
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective1 camera2 colour
1
4 6
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective1 camera2 high_res
1
4 6
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective1 camera2 low_res
1
4 6
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective2 camera1 low_res
1
4 6
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective2 camera2 colour
1
4 6
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective2 camera2 high_res
1
4 6
2
0 5 0 1
0 8 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective2 camera2 low_res
1
4 6
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective3 camera1 low_res
1
4 6
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective3 camera2 colour
1
4 6
2
0 5 0 1
0 6 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective3 camera2 high_res
1
4 6
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective3 camera2 low_res
1
4 6
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective4 camera1 low_res
1
4 6
2
0 10 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective4 camera2 colour
1
4 6
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective4 camera2 high_res
1
4 6
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective4 camera2 low_res
1
4 6
2
0 5 0 1
0 11 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint7 objective6 camera1 low_res
1
4 6
1
0 10 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective6 camera2 colour
1
4 6
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective6 camera2 high_res
1
4 6
1
0 5 0 1
1
end_operator
begin_operator
take_image rover1 waypoint7 objective6 camera2 low_res
1
4 6
1
0 5 0 1
1
end_operator
begin_operator
take_image rover2 waypoint1 objective2 camera3 high_res
1
1 0
2
0 2 0 1
0 3 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint1 objective3 camera3 high_res
1
1 0
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint1 objective5 camera3 high_res
1
1 0
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint2 objective3 camera3 high_res
1
1 1
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint2 objective4 camera3 high_res
1
1 1
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint2 objective5 camera3 high_res
1
1 1
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint3 objective2 camera3 high_res
1
1 2
2
0 2 0 1
0 3 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint3 objective3 camera3 high_res
1
1 2
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint3 objective4 camera3 high_res
1
1 2
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint4 objective2 camera3 high_res
1
1 3
2
0 2 0 1
0 3 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint4 objective3 camera3 high_res
1
1 3
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint4 objective4 camera3 high_res
1
1 3
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint4 objective6 camera3 high_res
1
1 3
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint5 objective1 camera3 high_res
1
1 4
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint5 objective3 camera3 high_res
1
1 4
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint5 objective6 camera3 high_res
1
1 4
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective2 camera3 high_res
1
1 5
2
0 2 0 1
0 3 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint6 objective4 camera3 high_res
1
1 5
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint6 objective5 camera3 high_res
1
1 5
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint7 objective1 camera3 high_res
1
1 6
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint7 objective2 camera3 high_res
1
1 6
2
0 2 0 1
0 3 -1 0
1
end_operator
begin_operator
take_image rover2 waypoint7 objective3 camera3 high_res
1
1 6
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint7 objective4 camera3 high_res
1
1 6
1
0 2 0 1
1
end_operator
begin_operator
take_image rover2 waypoint7 objective6 camera3 high_res
1
1 6
1
0 2 0 1
1
end_operator
0
