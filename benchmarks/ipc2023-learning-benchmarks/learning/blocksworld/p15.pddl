;; blocks=5, out_folder=training/easy, instance_id=15, seed=42

(define (problem blocksworld-15)
 (:domain blocksworld)
 (:objects b1 b2 b3 b4 b5 - object)
 (:init 
    (arm-empty)
    (clear b4)
    (on b4 b2)
    (on b2 b3)
    (on b3 b5)
    (on b5 b1)
    (on-table b1))
 (:goal  (and 
    (clear b4)
    (on-table b4)
    (clear b2)
    (on-table b2)
    (clear b3)
    (on b3 b1)
    (on b1 b5)
    (on-table b5))))
