(define (problem p46-problem)
 (:domain p46-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 6) (= (capacity c2) 6) (= (capacity c3) 1) (= (capacity c4) 8) (base b13) (in b13 c1) (on b13 c1) (on b6 b13) (in b6 c1) (on b7 b6) (in b7 c1) (clear b7) (base b14) (in b14 c2) (on b14 c2) (on b1 b14) (in b1 c2) (on b4 b1) (in b4 c2) (clear b4) (base b5) (in b5 c3) (on b5 c3) (on b2 b5) (in b2 c3) (on b9 b2) (in b9 c3) (on b8 b9) (in b8 c3) (on b10 b8) (in b10 c3) (on b3 b10) (in b3 c3) (on b11 b3) (in b11 c3) (on b12 b11) (in b12 c3) (clear b12) (clear c4))
 (:goal (and (on b8 c1) (clear b8) (on b14 c2) (on b12 b14) (on b13 b12) (on b7 b13) (clear b7) (on b3 c3) (on b1 b3) (on b5 b1) (on b10 b5) (on b11 b10) (on b4 b11) (on b2 b4) (on b9 b2) (on b6 b9) (clear b6)))
)
