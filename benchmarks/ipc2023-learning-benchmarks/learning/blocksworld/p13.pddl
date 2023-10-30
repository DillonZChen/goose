;; base case
;;
(define (problem blocksworld-13)
 (:domain blocksworld)
 (:objects  b1 b2 b3 b4 - object)
 (:init 
    (arm-empty)
    (clear b1)
    (on b1 b2)
    (on b2 b3)
    (on b3 b4)
    (on-table b4)
)
 (:goal (and
    (clear b4)
    (on b4 b2)
    (on b2 b3)
    (on b3 b1)
    (on-table b1)
)))