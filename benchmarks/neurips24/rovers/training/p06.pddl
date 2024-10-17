(define (problem rover_06-problem)
 (:domain rover_06-domain)
 (:objects
   rover1 rover2 - rover
   waypoint1 waypoint2 waypoint3 - waypoint
   rover1store rover2store - store
   camera1 camera2 camera3 - camera
   colour high_res low_res - mode
   general - lander
   objective1 objective2 objective3 - objective
 )
 (:init (at_lander general waypoint1) (at rover1 waypoint2) (at rover2 waypoint2) (equipped_for_rock_analysis rover1) (equipped_for_soil_analysis rover1) (equipped_for_imaging rover1) (equipped_for_rock_analysis rover2) (equipped_for_soil_analysis rover2) (equipped_for_imaging rover2) (empty rover1store) (store_of rover1store rover1) (empty rover2store) (store_of rover2store rover2) (at_rock_sample waypoint1) (at_rock_sample waypoint2) (at_rock_sample waypoint3) (at_soil_sample waypoint1) (at_soil_sample waypoint2) (at_soil_sample waypoint3) (visible waypoint1 waypoint2) (visible waypoint2 waypoint1) (visible waypoint1 waypoint3) (visible waypoint3 waypoint1) (visible waypoint3 waypoint2) (visible waypoint2 waypoint3) (can_traverse rover1 waypoint1 waypoint3) (can_traverse rover1 waypoint3 waypoint1) (can_traverse rover1 waypoint1 waypoint2) (can_traverse rover1 waypoint2 waypoint1) (can_traverse rover2 waypoint3 waypoint2) (can_traverse rover2 waypoint2 waypoint3) (calibration_target camera1 objective3) (calibration_target camera2 objective2) (calibration_target camera3 objective3) (on_board camera1 rover2) (on_board camera2 rover1) (on_board camera3 rover2) (supports camera1 low_res) (supports camera2 colour) (supports camera3 high_res) (visible_from objective1 waypoint1) (visible_from objective2 waypoint1) (visible_from objective3 waypoint3) (= (recharges) 0) (= (energy rover1) 16) (= (energy rover2) 16) (in_sun waypoint2) (in_sun waypoint3))
 (:goal (and (communicated_rock_data waypoint1) (communicated_rock_data waypoint2) (communicated_rock_data waypoint3) (communicated_soil_data waypoint1) (communicated_soil_data waypoint2) (communicated_soil_data waypoint3) (communicated_image_data objective3 high_res) (communicated_image_data objective3 low_res) (communicated_image_data objective3 colour) (communicated_image_data objective2 colour) (communicated_image_data objective1 colour)))
)
