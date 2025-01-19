;; blocks=5, out_folder=testing/easy, instance_id=2, seed=1008

(define (problem blocksworld-02)
 (:domain blocksworld)
 (:objects b1 b2 b3 b4 b5 - object)
 (:init 
    (arm-empty)
    (clear b3)
    (on b3 b1)
    (on-table b1)
    (clear b2)
    (on b2 b4)
    (on b4 b5)
    (on-table b5))
 (:goal  (and 
    (clear b2)
    (on b2 b5)
    (on b5 b4)
    (on b4 b3)
    (on b3 b1)
    (on-table b1))))
