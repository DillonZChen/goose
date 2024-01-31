;; blocks=27, out_folder=training/easy, instance_id=93, seed=120

(define (problem blocksworld-93)
 (:domain blocksworld)
 (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 - object)
 (:init 
    (arm-empty)
    (clear b18)
    (on b18 b4)
    (on b4 b1)
    (on b1 b27)
    (on b27 b16)
    (on b16 b23)
    (on b23 b10)
    (on-table b10)
    (clear b5)
    (on b5 b13)
    (on-table b13)
    (clear b2)
    (on b2 b20)
    (on b20 b24)
    (on b24 b14)
    (on b14 b6)
    (on b6 b12)
    (on b12 b9)
    (on b9 b26)
    (on b26 b25)
    (on b25 b11)
    (on-table b11)
    (clear b3)
    (on-table b3)
    (clear b15)
    (on b15 b22)
    (on b22 b19)
    (on b19 b21)
    (on b21 b7)
    (on b7 b8)
    (on b8 b17)
    (on-table b17))
 (:goal  (and 
    (clear b10)
    (on b10 b21)
    (on b21 b8)
    (on b8 b7)
    (on b7 b3)
    (on-table b3)
    (clear b2)
    (on b2 b5)
    (on b5 b4)
    (on b4 b24)
    (on b24 b1)
    (on b1 b6)
    (on b6 b15)
    (on b15 b17)
    (on-table b17)
    (clear b20)
    (on b20 b23)
    (on b23 b18)
    (on b18 b27)
    (on b27 b25)
    (on b25 b22)
    (on-table b22)
    (clear b26)
    (on b26 b12)
    (on b12 b9)
    (on b9 b16)
    (on b16 b11)
    (on b11 b19)
    (on b19 b14)
    (on b14 b13)
    (on-table b13))))