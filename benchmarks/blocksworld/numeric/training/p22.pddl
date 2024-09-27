(define (problem p22-problem)
 (:domain p22-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 3) (= (capacity c2) 3) (= (capacity c3) 0) (= (capacity c4) 4) (base b6) (in b6 c1) (on b6 c1) (clear b6) (base b1) (in b1 c2) (on b1 c2) (on b3 b1) (in b3 c2) (clear b3) (base b4) (in b4 c3) (on b4 c3) (on b7 b4) (in b7 c3) (on b2 b7) (in b2 c3) (on b5 b2) (in b5 c3) (clear b5) (clear c4))
 (:goal (and (on b6 c1) (on b1 b6) (clear b1) (on b5 c2) (on b2 b5) (on b7 b2) (on b3 b7) (on b4 b3) (clear b4)))
)
