;; base case
;;
(define (problem floortile-05)
 (:domain floortile)
 (:objects
    tile_0_1
    tile_1_1
    tile_2_1 - tile
    robot1 - robot
    white black - color
)

(:init
    (robot-at robot1 tile_0_1)
    (robot-has robot1 black)
    (available-color white)
    (available-color black)
    (clear tile_2_1)
    (clear tile_1_1)
    (up tile_2_1 tile_1_1 )
    (up tile_1_1 tile_0_1 )
    (down tile_1_1 tile_2_1 )
    (down tile_0_1 tile_1_1 )
)

(:goal  (and
    (painted tile_1_1 white)
    (painted tile_2_1 black)
)))
