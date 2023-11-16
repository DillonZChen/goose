;; blocks=5, out_folder=training/easy, instance_id=16, seed=43

(define (problem blocksworld-16)
 (:domain blocksworld)
 (:objects b1 b2 b3 b4 b5 - object)
 (:init 
    (arm-empty)
    (clear b2)
    (on b2 b5)
    (on b5 b4)
    (on b4 b3)
    (on b3 b1)
    (on-table b1))
 (:goal  (and 
    (clear b2)
    (on b2 b1)
    (on b1 b3)
    (on b3 b4)
    (on b4 b5)
    (on-table b5))))
