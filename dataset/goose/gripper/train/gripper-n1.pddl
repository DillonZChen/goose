


(define (problem gripper-1)
(:domain gripper-strips)
(:objects  rooma roomb left right ball1 )
(:init
(room rooma)
(room roomb)
(gripper left)
(gripper right)
(ball ball1)
(free left)
(free right)
(at ball1 rooma)
(at-robby rooma)
)
(:goal
(and
(at ball1 roomb)
)
)
)


