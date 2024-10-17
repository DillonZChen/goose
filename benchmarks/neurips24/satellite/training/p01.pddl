(define (problem satellite_01-problem)
 (:domain satellite_01-domain)
 (:objects
   sat1 - satellite
   dir1 dir2 - direction
   ins1 - instrument
   mod1 - mode
 )
 (:init (pointing sat1 dir1) (power_avail sat1) (calibration_target ins1 dir1) (on_board ins1 sat1) (supports ins1 mod1) (= (data_capacity sat1) 7) (= (fuel sat1) 7) (= (data dir1 mod1) 2) (= (data dir2 mod1) 2) (= (slew_time dir1 dir1) 1) (= (slew_time dir1 dir2) 2) (= (slew_time dir2 dir1) 3) (= (slew_time dir2 dir2) 2))
 (:goal (and (have_image dir2 mod1)))
)
