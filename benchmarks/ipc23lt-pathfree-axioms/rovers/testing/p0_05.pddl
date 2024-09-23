;; rovers=1, waypoints=4, cameras=1, objectives=2, out_folder=testing/easy, instance_id=5, seed=1011

(define (problem rover-05)
 (:domain rover)
 (:objects 
    general - lander
    colour high_res low_res - mode
    rover1 - rover
    rover1store - store
    waypoint1 waypoint2 waypoint3 waypoint4 - waypoint
    camera1 - camera
    objective1 objective2 - objective)
 (:init 
    (at_lander general waypoint3)
    (at rover1 waypoint3)
    (equipped_for_soil_analysis rover1)
    (equipped_for_rock_analysis rover1)
    (equipped_for_imaging rover1)
    (empty rover1store)
    (store_of rover1store rover1)
    (at_soil_sample waypoint1)
    (at_soil_sample waypoint2)
    (visible waypoint2 waypoint4)
    (visible waypoint1 waypoint2)
    (visible waypoint2 waypoint1)
    (visible waypoint4 waypoint2)
    (visible waypoint2 waypoint3)
    (visible waypoint3 waypoint2)
    (visible waypoint1 waypoint3)
    (visible waypoint3 waypoint1)
    (visible waypoint1 waypoint4)
    (visible waypoint4 waypoint1)
    (visible waypoint3 waypoint4)
    (visible waypoint4 waypoint3)
    (can_traverse rover1 waypoint2 waypoint4)
    (can_traverse rover1 waypoint1 waypoint2)
    (can_traverse rover1 waypoint2 waypoint1)
    (can_traverse rover1 waypoint4 waypoint2)
    (can_traverse rover1 waypoint2 waypoint3)
    (can_traverse rover1 waypoint3 waypoint2)
    (can_traverse rover1 waypoint1 waypoint4)
    (can_traverse rover1 waypoint4 waypoint1)
    (calibration_target camera1 objective1)
    (on_board camera1 rover1)
    (supports camera1 colour)
    (supports camera1 high_res)
    (supports camera1 low_res)
    (visible_from objective1 waypoint4)
    (visible_from objective1 waypoint1)
    (visible_from objective1 waypoint3)
    (visible_from objective2 waypoint3)
    (visible_from objective2 waypoint1)
    (visible_from objective2 waypoint4)
    (visible_from objective2 waypoint2))
 (:goal  (and 
    
    (communicated_soil_data waypoint2)
    (communicated_image_data objective1 colour))))
