;; base case
;;
(define (problem blocksworld-03)
 (:domain blocksworld)
 (:objects  b1 b2 - object)
 (:init 
    (arm-empty)
    (clear b1)
    (on b1 b2)
    (on-table b2)
)
 (:goal (and
    (clear b2)
    (on-table b2)
    (clear b1)
    (on-table b1)
)))