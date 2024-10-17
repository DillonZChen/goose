(define (problem p30-problem)
 (:domain p30-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 5) (= (capacity c2) 0) (= (capacity c3) 4) (= (capacity c4) 4) (base b1) (in b1 c1) (on b1 c1) (on b6 b1) (in b6 c1) (on b5 b6) (in b5 c1) (on b7 b5) (in b7 c1) (clear b7) (base b9) (in b9 c2) (on b9 c2) (on b2 b9) (in b2 c2) (on b3 b2) (in b3 c2) (on b4 b3) (in b4 c2) (on b8 b4) (in b8 c2) (clear b8) (clear c3) (clear c4))
 (:goal (and (on b6 c1) (on b7 b6) (on b4 b7) (on b1 b4) (on b5 b1) (on b8 b5) (on b3 b8) (on b2 b3) (on b9 b2) (clear b9)))
)
