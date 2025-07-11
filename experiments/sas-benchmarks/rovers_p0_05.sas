begin_version
3
end_version
begin_metric
0
end_metric
8
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
Atom calibrated(camera1, rover1)
NegatedAtom calibrated(camera1, rover1)
end_variable
begin_variable
var2
-1
2
Atom have_image(rover1, objective1, colour)
NegatedAtom have_image(rover1, objective1, colour)
end_variable
begin_variable
var3
-1
2
Atom communicated_image_data(objective1, colour)
NegatedAtom communicated_image_data(objective1, colour)
end_variable
begin_variable
var4
-1
2
Atom at_soil_sample(waypoint1)
Atom have_soil_analysis(rover1, waypoint1)
end_variable
begin_variable
var5
-1
2
Atom at_soil_sample(waypoint2)
Atom have_soil_analysis(rover1, waypoint2)
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
0
begin_state
2
1
1
1
0
0
0
1
end_state
begin_goal
2
3 0
7 0
end_goal
41
begin_operator
calibrate rover1 camera1 objective1 waypoint1
1
0 0
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective1 waypoint3
1
0 2
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective1 waypoint4
1
0 3
1
0 1 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 colour waypoint1 waypoint3
2
0 0
2 0
1
0 3 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 colour waypoint2 waypoint3
2
0 1
2 0
1
0 3 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 colour waypoint4 waypoint3
2
0 3
2 0
1
0 3 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint2 waypoint1 waypoint3
2
0 0
5 1
1
0 7 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint2 waypoint2 waypoint3
2
0 1
5 1
1
0 7 -1 0
1
end_operator
begin_operator
communicate_soil_data rover1 general waypoint2 waypoint4 waypoint3
2
0 3
5 1
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
navigate rover1 waypoint1 waypoint2
0
1
0 0 0 1
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
sample_soil rover1 rover1store waypoint1
1
0 0
2
0 4 0 1
0 6 0 1
1
end_operator
begin_operator
sample_soil rover1 rover1store waypoint2
1
0 1
2
0 5 0 1
0 6 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective1 camera1 colour
1
0 0
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective1 camera1 high_res
1
0 0
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective1 camera1 low_res
1
0 0
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective2 camera1 colour
1
0 0
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective2 camera1 high_res
1
0 0
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective2 camera1 low_res
1
0 0
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective2 camera1 colour
1
0 1
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective2 camera1 high_res
1
0 1
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective2 camera1 low_res
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
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective1 camera1 high_res
1
0 2
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective1 camera1 low_res
1
0 2
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective2 camera1 colour
1
0 2
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective2 camera1 high_res
1
0 2
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective2 camera1 low_res
1
0 2
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective1 camera1 colour
1
0 3
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective1 camera1 high_res
1
0 3
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective1 camera1 low_res
1
0 3
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective2 camera1 colour
1
0 3
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective2 camera1 high_res
1
0 3
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective2 camera1 low_res
1
0 3
1
0 1 0 1
1
end_operator
0
