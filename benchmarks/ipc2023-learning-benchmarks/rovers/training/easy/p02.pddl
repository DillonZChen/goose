;; base cases
;;
(define (problem rover-02)
 (:domain rover)
 (:objects
    general - lander
    colour high_res low_res - mode
    rover1 - rover
    rover1store - store
    waypoint1 waypoint2 - waypoint
    camera1 - camera
    objective1 - objective)
 (:init
    (at_lander general waypoint1)
    (at rover1 waypoint2)
    (equipped_for_soil_analysis rover1)
    (equipped_for_imaging rover1)
    (empty rover1store)
    (store_of rover1store rover1)
    (at_soil_sample waypoint1)
    (visible waypoint1 waypoint2)
    (visible waypoint2 waypoint1)
    (can_traverse rover1 waypoint1 waypoint2)
    (can_traverse rover1 waypoint2 waypoint1)
    (calibration_target camera1 objective1)
    (on_board camera1 rover1)
    (supports camera1 high_res)
    (supports camera1 low_res)
    (supports camera1 colour)
    (visible_from objective1 waypoint2))
 (:goal  (and
    (communicated_soil_data waypoint1)
    (communicated_image_data objective1 high_res)
    (communicated_image_data objective1 low_res)
    (communicated_image_data objective1 colour))))
