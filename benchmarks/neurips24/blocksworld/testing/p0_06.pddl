(define (problem p0_06-problem)
 (:domain p0_06-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 8) (= (capacity c2) 0) (= (capacity c3) 3) (= (capacity c4) 2) (base b6) (in b6 c1) (on b6 c1) (clear b6) (base b3) (in b3 c2) (on b3 c2) (on b2 b3) (in b2 c2) (on b4 b2) (in b4 c2) (on b7 b4) (in b7 c2) (on b1 b7) (in b1 c2) (on b5 b1) (in b5 c2) (on b8 b5) (in b8 c2) (on b9 b8) (in b9 c2) (clear b9) (clear c3) (clear c4))
 (:goal (and (on b7 c1) (on b8 b7) (on b1 b8) (on b4 b1) (on b5 b4) (on b2 b5) (on b6 b2) (on b3 b6) (on b9 b3) (clear b9)))
)
