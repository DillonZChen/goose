(define (problem p0_08-problem)
 (:domain p0_08-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 5) (= (capacity c2) 0) (= (capacity c3) 5) (= (capacity c4) 5) (base b9) (in b9 c1) (on b9 c1) (clear b9) (base b1) (in b1 c2) (on b1 c2) (on b4 b1) (in b4 c2) (on b3 b4) (in b3 c2) (on b10 b3) (in b10 c2) (on b8 b10) (in b8 c2) (on b5 b8) (in b5 c2) (on b7 b5) (in b7 c2) (on b6 b7) (in b6 c2) (on b2 b6) (in b2 c2) (clear b2) (clear c3) (clear c4))
 (:goal (and (on b8 c1) (on b1 b8) (on b6 b1) (clear b6) (on b4 c2) (on b3 b4) (on b10 b3) (on b5 b10) (on b2 b5) (on b9 b2) (on b7 b9) (clear b7)))
)
