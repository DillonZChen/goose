;; base case
;;
(define (problem blocksworld-04)
 (:domain blocksworld)
 (:objects  b1 b2 - object)
 (:init 
    (arm-empty)
    (clear b2)
    (on b2 b1)
    (on-table b1)
)
 (:goal (and
    (clear b2)
    (on-table b2)
    (clear b1)
    (on-table b1)
)))