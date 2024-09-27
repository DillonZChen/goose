;; blocks=5, out_folder=testing/easy, instance_id=1, seed=1007

(define (problem blocksworld-01)
 (:domain blocksworld)
 (:objects b1 b2 b3 b4 b5 - object)
 (:init 
    (arm-empty)
    (clear b3)
    (on b3 b5)
    (on b5 b4)
    (on-table b4)
    (clear b2)
    (on b2 b1)
    (on-table b1))
 (:goal  (and 
    (clear b4)
    (on b4 b3)
    (on-table b3)
    (clear b2)
    (on-table b2)
    (clear b1)
    (on b1 b5)
    (on-table b5))))
