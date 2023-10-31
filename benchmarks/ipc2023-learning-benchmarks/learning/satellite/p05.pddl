;; base case
;;
(define (problem satellite-05)
 (:domain satellite)
 (:objects 
    sat1 - satellite
    ins1 ins2 - instrument
    mod1 - mode
    dir1 dir2 - direction
 )
 (:init 
    (pointing sat1 dir2)
    (power_avail sat1)
    (calibration_target ins1 dir2)
    (calibration_target ins2 dir1)
    (on_board ins1 sat1)
    (on_board ins2 sat1)
    (supports ins2 mod1)
    (supports ins1 mod1))
 (:goal  (and
    (pointing sat1 dir1)
    (have_image dir2 mod1)
    (have_image dir1 mod1)
 )))
