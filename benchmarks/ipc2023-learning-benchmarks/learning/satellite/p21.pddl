;; satellites=4, instruments=5, modes=1, directions=4, out_folder=training/easy, instance_id=21, seed=54

(define (problem satellite-21)
 (:domain satellite)
 (:objects 
    sat1 sat2 sat3 sat4 - satellite
    ins1 ins2 ins3 ins4 ins5 - instrument
    mod1 - mode
    dir1 dir2 dir3 dir4 - direction
    )
 (:init 
    (pointing sat1 dir2)
    (pointing sat2 dir4)
    (pointing sat3 dir3)
    (pointing sat4 dir4)
    (power_avail sat1)
    (power_avail sat2)
    (power_avail sat3)
    (power_avail sat4)
    (calibration_target ins1 dir4)
    (calibration_target ins2 dir4)
    (calibration_target ins3 dir2)
    (calibration_target ins4 dir4)
    (calibration_target ins5 dir3)
    (on_board ins1 sat2)
    (on_board ins2 sat3)
    (on_board ins3 sat1)
    (on_board ins4 sat4)
    (on_board ins5 sat1)
    (supports ins2 mod1)
    (supports ins1 mod1)
    (supports ins3 mod1)
    (supports ins5 mod1)
    (supports ins4 mod1))
 (:goal  (and 
   (have_image dir3 mod1)
   (have_image dir4 mod1)
   (have_image dir1 mod1))))
