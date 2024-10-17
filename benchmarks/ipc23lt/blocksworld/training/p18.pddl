;; blocks=5, out_folder=training/easy, instance_id=18, seed=45

(define (problem blocksworld-18)
 (:domain blocksworld)
 (:objects b1 b2 b3 b4 b5 - object)
 (:init 
    (arm-empty)
    (clear b1)
    (on b1 b5)
    (on b5 b2)
    (on b2 b4)
    (on-table b4)
    (clear b3)
    (on-table b3))
 (:goal  (and 
    (clear b2)
    (on b2 b3)
    (on b3 b5)
    (on b5 b4)
    (on-table b4)
    (clear b1)
    (on-table b1))))
