;; base case
;;
(define (problem floortile-01)
 (:domain floortile)
 (:objects
    tile_0_1
    tile_1_1 - tile
    robot1 - robot
    white black - color
)

(:init
    (robot-at robot1 tile_1_1)
    (robot-has robot1 black)
    (available-color white)
    (available-color black)
    (clear tile_0_1)
    (up tile_1_1 tile_0_1 )
    (down tile_0_1 tile_1_1 )
)

(:goal  (and
    (painted tile_1_1 white)
)))
