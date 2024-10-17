(define (problem p26-problem)
 (:domain p26-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 7) (= (capacity c2) 2) (= (capacity c3) 1) (= (capacity c4) 0) (= (capacity c5) 2) (base b1) (in b1 c1) (on b1 c1) (clear b1) (base b2) (in b2 c2) (on b2 c2) (clear b2) (base b4) (in b4 c3) (on b4 c3) (clear b4) (base b8) (in b8 c4) (on b8 c4) (on b5 b8) (in b5 c4) (on b7 b5) (in b7 c4) (on b3 b7) (in b3 c4) (on b6 b3) (in b6 c4) (clear b6) (clear c5))
 (:goal (and (on b3 c1) (on b1 b3) (on b8 b1) (on b2 b8) (on b4 b2) (on b7 b4) (on b6 b7) (on b5 b6) (clear b5)))
)
