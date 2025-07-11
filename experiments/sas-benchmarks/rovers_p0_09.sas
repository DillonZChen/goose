begin_version
3
end_version
begin_metric
0
end_metric
7
begin_variable
var0
-1
5
Atom at(rover1, waypoint1)
Atom at(rover1, waypoint2)
Atom at(rover1, waypoint3)
Atom at(rover1, waypoint4)
Atom at(rover1, waypoint5)
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
Atom have_image(rover1, objective1, low_res)
NegatedAtom have_image(rover1, objective1, low_res)
end_variable
begin_variable
var3
-1
2
Atom communicated_image_data(objective1, low_res)
NegatedAtom communicated_image_data(objective1, low_res)
end_variable
begin_variable
var4
-1
2
Atom calibrated(camera1, rover1)
NegatedAtom calibrated(camera1, rover1)
end_variable
begin_variable
var5
-1
2
Atom have_image(rover1, objective1, high_res)
NegatedAtom have_image(rover1, objective1, high_res)
end_variable
begin_variable
var6
-1
2
Atom communicated_image_data(objective1, high_res)
NegatedAtom communicated_image_data(objective1, high_res)
end_variable
0
begin_state
2
1
1
1
1
1
1
end_state
begin_goal
2
3 0
6 0
end_goal
80
begin_operator
calibrate rover1 camera1 objective2 waypoint1
1
0 0
1
0 4 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective2 waypoint2
1
0 1
1
0 4 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective2 waypoint3
1
0 2
1
0 4 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective2 waypoint4
1
0 3
1
0 4 -1 0
1
end_operator
begin_operator
calibrate rover1 camera1 objective2 waypoint5
1
0 4
1
0 4 -1 0
1
end_operator
begin_operator
calibrate rover1 camera2 objective1 waypoint1
1
0 0
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover1 camera2 objective1 waypoint2
1
0 1
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover1 camera2 objective1 waypoint3
1
0 2
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover1 camera2 objective1 waypoint4
1
0 3
1
0 1 -1 0
1
end_operator
begin_operator
calibrate rover1 camera2 objective1 waypoint5
1
0 4
1
0 1 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 high_res waypoint2 waypoint1
2
0 1
5 0
1
0 6 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 high_res waypoint3 waypoint1
2
0 2
5 0
1
0 6 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 high_res waypoint4 waypoint1
2
0 3
5 0
1
0 6 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 high_res waypoint5 waypoint1
2
0 4
5 0
1
0 6 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 low_res waypoint2 waypoint1
2
0 1
2 0
1
0 3 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 low_res waypoint3 waypoint1
2
0 2
2 0
1
0 3 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 low_res waypoint4 waypoint1
2
0 3
2 0
1
0 3 -1 0
1
end_operator
begin_operator
communicate_image_data rover1 general objective1 low_res waypoint5 waypoint1
2
0 4
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
navigate rover1 waypoint1 waypoint3
0
1
0 0 0 2
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
navigate rover1 waypoint3 waypoint1
0
1
0 0 2 0
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
navigate rover1 waypoint3 waypoint5
0
1
0 0 2 4
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
navigate rover1 waypoint5 waypoint1
0
1
0 0 4 0
1
end_operator
begin_operator
navigate rover1 waypoint5 waypoint3
0
1
0 0 4 2
1
end_operator
begin_operator
take_image rover1 waypoint1 objective1 camera1 colour
1
0 0
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective1 camera1 high_res
1
0 0
2
0 4 0 1
0 5 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective1 camera2 colour
1
0 0
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective1 camera2 low_res
1
0 0
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint1 objective2 camera1 colour
1
0 0
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective2 camera1 high_res
1
0 0
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective2 camera2 colour
1
0 0
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint1 objective2 camera2 low_res
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
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective1 camera1 high_res
1
0 1
2
0 4 0 1
0 5 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective1 camera2 colour
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
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint2 objective2 camera1 colour
1
0 1
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective2 camera1 high_res
1
0 1
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective2 camera2 colour
1
0 1
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint2 objective2 camera2 low_res
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
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective1 camera1 high_res
1
0 2
2
0 4 0 1
0 5 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective1 camera2 colour
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
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint3 objective2 camera1 colour
1
0 2
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective2 camera1 high_res
1
0 2
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective2 camera2 colour
1
0 2
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective2 camera2 low_res
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
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera1 high_res
1
0 2
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera2 colour
1
0 2
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint3 objective3 camera2 low_res
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
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective1 camera1 high_res
1
0 3
2
0 4 0 1
0 5 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective1 camera2 colour
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
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint4 objective2 camera1 colour
1
0 3
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective2 camera1 high_res
1
0 3
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint4 objective2 camera2 colour
1
0 3
1
0 1 0 1
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
take_image rover1 waypoint5 objective1 camera1 colour
1
0 4
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective1 camera1 high_res
1
0 4
2
0 4 0 1
0 5 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective1 camera2 colour
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
2
0 1 0 1
0 2 -1 0
1
end_operator
begin_operator
take_image rover1 waypoint5 objective2 camera1 colour
1
0 4
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective2 camera1 high_res
1
0 4
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective2 camera2 colour
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
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective3 camera1 colour
1
0 4
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective3 camera1 high_res
1
0 4
1
0 4 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective3 camera2 colour
1
0 4
1
0 1 0 1
1
end_operator
begin_operator
take_image rover1 waypoint5 objective3 camera2 low_res
1
0 4
1
0 1 0 1
1
end_operator
0
