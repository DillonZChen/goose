;; base case
;;
(define (problem blocksworld-14)
 (:domain blocksworld)
 (:objects  b1 b2 b3 b4 - object)
 (:init 
    (arm-empty)
    (clear b4)
    (on b4 b2)
    (on b2 b3)
    (on b3 b1)
    (on-table b1)
)
 (:goal (and
    (clear b1)
    (on b1 b2)
    (on b2 b3)
    (on b3 b4)
    (on-table b4)
)))