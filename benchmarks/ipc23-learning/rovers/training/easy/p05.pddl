;; base cases
;;
(define (problem rover-01)
 (:domain rover)
 (:objects
    general - lander
    colour high_res low_res - mode
    rover1 - rover
    rover1store - store
    waypoint1 waypoint2 - waypoint
    camera1 camera2 - camera
    objective1 objective2 - objective)
 (:init
    (at_lander general waypoint2)
    (at rover1 waypoint2)
    (equipped_for_imaging rover1)
    (empty rover1store)
    (store_of rover1store rover1)
    (visible waypoint1 waypoint2)
    (visible waypoint2 waypoint1)
    (can_traverse rover1 waypoint1 waypoint2)
    (can_traverse rover1 waypoint2 waypoint1)
    (calibration_target camera1 objective2)
    (calibration_target camera2 objective1)
    (on_board camera1 rover1)
    (on_board camera2 rover1)
    (supports camera1 high_res)
    (supports camera1 colour)
    (supports camera2 low_res)
    (visible_from objective2 waypoint2)
    (visible_from objective1 waypoint1))
 (:goal  (and
    (communicated_image_data objective2 high_res)
    (communicated_image_data objective2 low_res)
    (communicated_image_data objective1 colour))))
