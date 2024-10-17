(define (problem p43-problem)
 (:domain p43-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 6) (= (capacity c2) 0) (= (capacity c3) 7) (= (capacity c4) 6) (base b2) (in b2 c1) (on b2 c1) (clear b2) (base b5) (in b5 c2) (on b5 c2) (on b8 b5) (in b8 c2) (on b11 b8) (in b11 c2) (on b3 b11) (in b3 c2) (on b6 b3) (in b6 c2) (on b1 b6) (in b1 c2) (on b13 b1) (in b13 c2) (on b12 b13) (in b12 c2) (on b10 b12) (in b10 c2) (on b9 b10) (in b9 c2) (on b4 b9) (in b4 c2) (on b7 b4) (in b7 c2) (clear b7) (clear c3) (clear c4))
 (:goal (and (on b11 c1) (on b4 b11) (on b9 b4) (clear b9) (on b7 c2) (on b12 b7) (on b13 b12) (on b2 b13) (on b10 b2) (clear b10) (on b8 c3) (on b6 b8) (on b5 b6) (on b3 b5) (on b1 b3) (clear b1)))
)
