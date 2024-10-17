(define (problem p19-problem)
 (:domain p19-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 3) (= (capacity c2) 0) (= (capacity c3) 3) (= (capacity c4) 3) (base b1) (in b1 c1) (on b1 c1) (clear b1) (base b4) (in b4 c2) (on b4 c2) (on b6 b4) (in b6 c2) (on b3 b6) (in b3 c2) (on b5 b3) (in b5 c2) (on b2 b5) (in b2 c2) (clear b2) (clear c3) (clear c4))
 (:goal (and (on b2 c1) (clear b2) (on b1 c2) (on b6 b1) (on b3 b6) (on b4 b3) (on b5 b4) (clear b5)))
)
