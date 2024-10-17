(define (problem satellite_04-problem)
 (:domain satellite_04-domain)
 (:objects
   sat1 - satellite
   dir1 dir2 - direction
   ins1 - instrument
   mod1 mod2 - mode
 )
 (:init (pointing sat1 dir2) (power_avail sat1) (calibration_target ins1 dir1) (on_board ins1 sat1) (supports ins1 mod1) (supports ins1 mod2) (= (data_capacity sat1) 7) (= (fuel sat1) 7) (= (data dir1 mod1) 3) (= (data dir1 mod2) 3) (= (data dir2 mod1) 1) (= (data dir2 mod2) 2) (= (slew_time dir1 dir1) 1) (= (slew_time dir1 dir2) 3) (= (slew_time dir2 dir1) 1) (= (slew_time dir2 dir2) 3))
 (:goal (and (pointing sat1 dir2) (have_image dir2 mod1) (have_image dir2 mod2)))
)
