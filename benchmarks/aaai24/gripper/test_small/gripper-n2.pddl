


(define (problem gripper-2)
(:domain gripper-strips)
(:objects  rooma roomb left right ball1 ball2 )
(:init
(room rooma)
(room roomb)
(gripper left)
(gripper right)
(ball ball1)
(ball ball2)
(free left)
(free right)
(at ball1 rooma)
(at ball2 rooma)
(at-robby rooma)
)
(:goal
(and
(at ball1 roomb)
(at ball2 roomb)
)
)
)


