;; grid_size=7, boxes=1, out_folder=training/easy, instance_id=1, seed=42
;;
;;   #  #  #  #  #  #  #
;;   #  .  + R+  +  .  #
;;   #  .  + 1+  +  .  #
;;   #  .  .  #  +  .  #
;;   #  .  .  .  +  +  #
;;   #  .  .  G  +  +  #
;;   #  #  #  #  #  #  #
;;

(define (problem sokoban-06)
 (:domain sokoban)
 (:objects
    loc_1_1 loc_1_2 loc_1_3 loc_1_4 loc_1_5 loc_1_6 loc_1_7
    loc_2_1 loc_2_2 loc_2_3 loc_2_4 loc_2_5 loc_2_6 loc_2_7
    loc_3_1 loc_3_2 loc_3_3 loc_3_4 loc_3_5 loc_3_6 loc_3_7
    loc_4_1 loc_4_2 loc_4_3 loc_4_4 loc_4_5 loc_4_6 loc_4_7
    loc_5_1 loc_5_2 loc_5_3 loc_5_4 loc_5_5 loc_5_6 loc_5_7
    loc_6_1 loc_6_2 loc_6_3 loc_6_4 loc_6_5 loc_6_6 loc_6_7
    loc_7_1 loc_7_2 loc_7_3 loc_7_4 loc_7_5 loc_7_6 loc_7_7 - location
    box1 - box
    )
 (:init

    (at-robot loc_2_4)
    (at box1 loc_3_4)

    ;; clear locations
    (clear loc_2_2) (clear loc_2_3) (clear loc_2_4) (clear loc_2_5) (clear loc_2_6)
    (clear loc_3_2) (clear loc_3_3)                 (clear loc_3_5) (clear loc_3_6)
    (clear loc_4_2) (clear loc_4_3)                 (clear loc_4_5) (clear loc_4_6)
    (clear loc_5_2) (clear loc_5_3) (clear loc_5_4) (clear loc_5_5) (clear loc_5_6)
    (clear loc_6_2) (clear loc_6_3) (clear loc_6_4) (clear loc_6_5) (clear loc_6_6)

    ;; right/left adjacencies
    (adjacent loc_2_2 loc_2_3 right)
    (adjacent loc_2_3 loc_2_4 right)
    (adjacent loc_2_4 loc_2_5 right)
    (adjacent loc_2_5 loc_2_6 right)

    (adjacent loc_3_2 loc_3_3 right)
    (adjacent loc_3_3 loc_3_4 right)
    (adjacent loc_3_4 loc_3_5 right)
    (adjacent loc_3_5 loc_3_6 right)

    (adjacent loc_4_2 loc_4_3 right)
    (adjacent loc_4_5 loc_4_6 right)

    (adjacent loc_5_2 loc_5_3 right)
    (adjacent loc_5_3 loc_5_4 right)
    (adjacent loc_5_4 loc_5_5 right)
    (adjacent loc_5_5 loc_5_6 right)

    (adjacent loc_6_2 loc_6_3 right)
    (adjacent loc_6_3 loc_6_4 right)
    (adjacent loc_6_4 loc_6_5 right)
    (adjacent loc_6_5 loc_6_6 right)

    (adjacent loc_2_3 loc_2_2 left)
    (adjacent loc_2_4 loc_2_3 left)
    (adjacent loc_2_5 loc_2_4 left)
    (adjacent loc_2_6 loc_2_5 left)

    (adjacent loc_3_3 loc_3_2 left)
    (adjacent loc_3_4 loc_3_3 left)
    (adjacent loc_3_5 loc_3_4 left)
    (adjacent loc_3_6 loc_3_5 left)

    (adjacent loc_4_3 loc_4_2 left)
    (adjacent loc_4_6 loc_4_5 left)

    (adjacent loc_5_3 loc_5_2 left)
    (adjacent loc_5_4 loc_5_3 left)
    (adjacent loc_5_5 loc_5_4 left)
    (adjacent loc_5_6 loc_5_5 left)

    (adjacent loc_6_3 loc_6_2 left)
    (adjacent loc_6_4 loc_6_3 left)
    (adjacent loc_6_5 loc_6_4 left)
    (adjacent loc_6_6 loc_6_5 left)

    ;; up/down adjacencies
    (adjacent loc_2_2 loc_3_2 down)
    (adjacent loc_3_2 loc_4_2 down)
    (adjacent loc_4_2 loc_5_2 down)
    (adjacent loc_5_2 loc_6_2 down)

    (adjacent loc_2_3 loc_3_3 down)
    (adjacent loc_3_3 loc_4_3 down)
    (adjacent loc_4_3 loc_5_3 down)
    (adjacent loc_5_3 loc_6_3 down)

    (adjacent loc_2_4 loc_3_4 down)
    (adjacent loc_5_4 loc_6_4 down)

    (adjacent loc_2_5 loc_3_5 down)
    (adjacent loc_3_5 loc_4_5 down)
    (adjacent loc_4_5 loc_5_5 down)
    (adjacent loc_5_5 loc_6_5 down)

    (adjacent loc_2_6 loc_3_6 down)
    (adjacent loc_3_6 loc_4_6 down)
    (adjacent loc_4_6 loc_5_6 down)
    (adjacent loc_5_6 loc_6_6 down)

    (adjacent loc_3_2 loc_2_2 up)
    (adjacent loc_4_2 loc_3_2 up)
    (adjacent loc_5_2 loc_4_2 up)
    (adjacent loc_6_2 loc_5_2 up)

    (adjacent loc_3_3 loc_2_3 up)
    (adjacent loc_4_3 loc_3_3 up)
    (adjacent loc_5_3 loc_4_3 up)
    (adjacent loc_6_3 loc_5_3 up)

    (adjacent loc_3_4 loc_2_4 up)
    (adjacent loc_6_4 loc_5_4 up)

    (adjacent loc_3_5 loc_2_5 up)
    (adjacent loc_4_5 loc_3_5 up)
    (adjacent loc_5_5 loc_4_5 up)
    (adjacent loc_6_5 loc_5_5 up)

    (adjacent loc_3_6 loc_2_6 up)
    (adjacent loc_4_6 loc_3_6 up)
    (adjacent loc_5_6 loc_4_6 up)
    (adjacent loc_6_6 loc_5_6 up)
 )
 (:goal  (and
    (at box1 loc_6_4))))
