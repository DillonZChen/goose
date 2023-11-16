;; satellites=4, instruments=7, modes=1, directions=5, out_folder=training/easy, instance_id=30, seed=63

(define (problem satellite-30)
 (:domain satellite)
 (:objects 
    sat1 sat2 sat3 sat4 - satellite
    ins1 ins2 ins3 ins4 ins5 ins6 ins7 - instrument
    mod1 - mode
    dir1 dir2 dir3 dir4 dir5 - direction
    )
 (:init 
    (pointing sat1 dir4)
    (pointing sat2 dir4)
    (pointing sat3 dir3)
    (pointing sat4 dir3)
    (power_avail sat1)
    (power_avail sat2)
    (power_avail sat3)
    (power_avail sat4)
    (calibration_target ins1 dir4)
    (calibration_target ins2 dir1)
    (calibration_target ins3 dir4)
    (calibration_target ins4 dir1)
    (calibration_target ins5 dir1)
    (calibration_target ins6 dir2)
    (calibration_target ins7 dir3)
    (on_board ins1 sat4)
    (on_board ins2 sat2)
    (on_board ins3 sat3)
    (on_board ins4 sat1)
    (on_board ins5 sat1)
    (on_board ins6 sat2)
    (on_board ins7 sat2)
    (supports ins6 mod1)
    (supports ins5 mod1)
    (supports ins4 mod1)
    (supports ins1 mod1)
    (supports ins7 mod1)
    (supports ins2 mod1)
    (supports ins3 mod1))
 (:goal  (and (pointing sat1 dir5)
   (have_image dir5 mod1))))
