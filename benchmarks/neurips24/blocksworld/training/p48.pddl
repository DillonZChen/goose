(define (problem p48-problem)
 (:domain p48-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 3) (= (capacity c2) 0) (= (capacity c3) 6) (= (capacity c4) 6) (= (capacity c5) 6) (base b8) (in b8 c1) (on b8 c1) (on b10 b8) (in b10 c1) (on b7 b10) (in b7 c1) (on b14 b7) (in b14 c1) (clear b14) (base b1) (in b1 c2) (on b1 c2) (on b9 b1) (in b9 c2) (on b11 b9) (in b11 c2) (on b6 b11) (in b6 c2) (on b12 b6) (in b12 c2) (on b3 b12) (in b3 c2) (on b13 b3) (in b13 c2) (on b5 b13) (in b5 c2) (on b2 b5) (in b2 c2) (on b4 b2) (in b4 c2) (clear b4) (clear c3) (clear c4) (clear c5))
 (:goal (and (on b5 c1) (on b2 b5) (clear b2) (on b6 c2) (on b1 b6) (on b7 b1) (clear b7) (on b12 c3) (on b10 b12) (on b13 b10) (on b9 b13) (clear b9) (on b11 c4) (on b8 b11) (on b14 b8) (on b4 b14) (on b3 b4) (clear b3)))
)
