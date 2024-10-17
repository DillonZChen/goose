(define (problem satellite_06-problem)
 (:domain satellite_06-domain)
 (:objects
   sat1 - satellite
   dir1 dir2 - direction
   ins1 ins2 - instrument
   mod1 - mode
 )
 (:init (pointing sat1 dir1) (power_avail sat1) (calibration_target ins1 dir2) (calibration_target ins2 dir1) (on_board ins1 sat1) (on_board ins2 sat1) (supports ins2 mod1) (supports ins1 mod1) (= (data_capacity sat1) 7) (= (fuel sat1) 7) (= (data dir1 mod1) 2) (= (data dir2 mod1) 3) (= (slew_time dir1 dir1) 3) (= (slew_time dir1 dir2) 1) (= (slew_time dir2 dir1) 3) (= (slew_time dir2 dir2) 2))
 (:goal (and (pointing sat1 dir2) (have_image dir2 mod1) (have_image dir1 mod1)))
)
