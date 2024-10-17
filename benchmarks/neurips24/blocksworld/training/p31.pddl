(define (problem p31-problem)
 (:domain p31-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 5) (= (capacity c3) 4) (= (capacity c4) 4) (base b4) (in b4 c1) (on b4 c1) (on b9 b4) (in b9 c1) (on b6 b9) (in b6 c1) (on b7 b6) (in b7 c1) (on b2 b7) (in b2 c1) (on b1 b2) (in b1 c1) (on b8 b1) (in b8 c1) (on b5 b8) (in b5 c1) (on b3 b5) (in b3 c1) (clear b3) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b3 c1) (on b5 b3) (clear b5) (on b2 c2) (on b1 b2) (on b4 b1) (clear b4) (on b9 c3) (on b7 b9) (on b8 b7) (on b6 b8) (clear b6)))
)
