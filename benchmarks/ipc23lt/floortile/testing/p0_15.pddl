;; rows=4, columns=5, robots=2, out_folder=testing/easy, instance_id=15, seed=1021

(define (problem floortile-15)
 (:domain floortile)
 (:objects 
    tile_0_1
    tile_0_2
    tile_0_3
    tile_0_4
    tile_0_5
    tile_1_1
    tile_1_2
    tile_1_3
    tile_1_4
    tile_1_5
    tile_2_1
    tile_2_2
    tile_2_3
    tile_2_4
    tile_2_5
    tile_3_1
    tile_3_2
    tile_3_3
    tile_3_4
    tile_3_5
    tile_4_1
    tile_4_2
    tile_4_3
    tile_4_4
    tile_4_5 - tile
    robot1
    robot2 - robot
    white black - color
)
 (:init 
    (robot-at robot1 tile_2_4)
    (robot-has robot1 black)
    (robot-at robot2 tile_4_1)
    (robot-has robot2 black)
    (available-color white)
    (available-color black)
    (clear tile_0_1)
    (clear tile_0_2)
    (clear tile_0_3)
    (clear tile_0_4)
    (clear tile_0_5)
    (clear tile_1_1)
    (clear tile_1_2)
    (clear tile_1_3)
    (clear tile_1_4)
    (clear tile_1_5)
    (clear tile_2_1)
    (clear tile_2_2)
    (clear tile_2_3)
    (clear tile_2_5)
    (clear tile_3_1)
    (clear tile_3_2)
    (clear tile_3_3)
    (clear tile_3_4)
    (clear tile_3_5)
    (clear tile_4_2)
    (clear tile_4_3)
    (clear tile_4_4)
    (clear tile_4_5)
    (up tile_1_1 tile_0_1 )
    (up tile_1_2 tile_0_2 )
    (up tile_1_3 tile_0_3 )
    (up tile_1_4 tile_0_4 )
    (up tile_1_5 tile_0_5 )
    (up tile_2_1 tile_1_1 )
    (up tile_2_2 tile_1_2 )
    (up tile_2_3 tile_1_3 )
    (up tile_2_4 tile_1_4 )
    (up tile_2_5 tile_1_5 )
    (up tile_3_1 tile_2_1 )
    (up tile_3_2 tile_2_2 )
    (up tile_3_3 tile_2_3 )
    (up tile_3_4 tile_2_4 )
    (up tile_3_5 tile_2_5 )
    (up tile_4_1 tile_3_1 )
    (up tile_4_2 tile_3_2 )
    (up tile_4_3 tile_3_3 )
    (up tile_4_4 tile_3_4 )
    (up tile_4_5 tile_3_5 )
    (down tile_0_1 tile_1_1 )
    (down tile_0_2 tile_1_2 )
    (down tile_0_3 tile_1_3 )
    (down tile_0_4 tile_1_4 )
    (down tile_0_5 tile_1_5 )
    (down tile_1_1 tile_2_1 )
    (down tile_1_2 tile_2_2 )
    (down tile_1_3 tile_2_3 )
    (down tile_1_4 tile_2_4 )
    (down tile_1_5 tile_2_5 )
    (down tile_2_1 tile_3_1 )
    (down tile_2_2 tile_3_2 )
    (down tile_2_3 tile_3_3 )
    (down tile_2_4 tile_3_4 )
    (down tile_2_5 tile_3_5 )
    (down tile_3_1 tile_4_1 )
    (down tile_3_2 tile_4_2 )
    (down tile_3_3 tile_4_3 )
    (down tile_3_4 tile_4_4 )
    (down tile_3_5 tile_4_5 )
    (left tile_0_1 tile_0_2 )
    (left tile_0_2 tile_0_3 )
    (left tile_0_3 tile_0_4 )
    (left tile_0_4 tile_0_5 )
    (left tile_1_1 tile_1_2 )
    (left tile_1_2 tile_1_3 )
    (left tile_1_3 tile_1_4 )
    (left tile_1_4 tile_1_5 )
    (left tile_2_1 tile_2_2 )
    (left tile_2_2 tile_2_3 )
    (left tile_2_3 tile_2_4 )
    (left tile_2_4 tile_2_5 )
    (left tile_3_1 tile_3_2 )
    (left tile_3_2 tile_3_3 )
    (left tile_3_3 tile_3_4 )
    (left tile_3_4 tile_3_5 )
    (left tile_4_1 tile_4_2 )
    (left tile_4_2 tile_4_3 )
    (left tile_4_3 tile_4_4 )
    (left tile_4_4 tile_4_5 )
    (right tile_0_2 tile_0_1 )
    (right tile_0_3 tile_0_2 )
    (right tile_0_4 tile_0_3 )
    (right tile_0_5 tile_0_4 )
    (right tile_1_2 tile_1_1 )
    (right tile_1_3 tile_1_2 )
    (right tile_1_4 tile_1_3 )
    (right tile_1_5 tile_1_4 )
    (right tile_2_2 tile_2_1 )
    (right tile_2_3 tile_2_2 )
    (right tile_2_4 tile_2_3 )
    (right tile_2_5 tile_2_4 )
    (right tile_3_2 tile_3_1 )
    (right tile_3_3 tile_3_2 )
    (right tile_3_4 tile_3_3 )
    (right tile_3_5 tile_3_4 )
    (right tile_4_2 tile_4_1 )
    (right tile_4_3 tile_4_2 )
    (right tile_4_4 tile_4_3 )
    (right tile_4_5 tile_4_4 ))
 (:goal  (and 
    (painted tile_1_1 white)
    (painted tile_1_2 black)
    (painted tile_1_3 white)
    (painted tile_1_4 black)
    (painted tile_1_5 white)
    (painted tile_2_1 black)
    (painted tile_2_2 white)
    (painted tile_2_3 black)
    (painted tile_2_4 white)
    (painted tile_2_5 black)
    (painted tile_3_1 white)
    (painted tile_3_2 black)
    (painted tile_3_3 white)
    (painted tile_3_4 black)
    (painted tile_3_5 white)
    (painted tile_4_1 black)
    (painted tile_4_2 white)
    (painted tile_4_3 black)
    (painted tile_4_4 white)
    (painted tile_4_5 black))))
