;; base case
;;
(define (problem satellite-03)
 (:domain satellite)
 (:objects 
    sat1 - satellite
    ins1 - instrument
    mod1 mod2 - mode
    dir1 dir2 - direction
    )
 (:init 
    (pointing sat1 dir1)
    (power_avail sat1)
    (calibration_target ins1 dir1)
    (on_board ins1 sat1)
    (supports ins1 mod1)
    (supports ins1 mod2))
 (:goal  (and
    (pointing sat1 dir1)
    (have_image dir2 mod1)
    (have_image dir2 mod2)
 )))
