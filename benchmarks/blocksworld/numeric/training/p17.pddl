(define (problem p17-problem)
 (:domain p17-domain)
 (:objects
   b1 b2 b3 b4 b5 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 3) (= (capacity c2) 0) (= (capacity c3) 2) (= (capacity c4) 2) (base b4) (in b4 c1) (on b4 c1) (on b1 b4) (in b1 c1) (clear b1) (base b5) (in b5 c2) (on b5 c2) (on b2 b5) (in b2 c2) (on b3 b2) (in b3 c2) (clear b3) (clear c3) (clear c4))
 (:goal (and (on b1 c1) (on b5 b1) (on b4 b5) (on b3 b4) (on b2 b3) (clear b2)))
)
