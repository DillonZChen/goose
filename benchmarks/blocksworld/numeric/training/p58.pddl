(define (problem p58-problem)
 (:domain p58-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 15) (= (capacity c2) 0) (= (capacity c3) 5) (= (capacity c4) 5) (base b11) (in b11 c1) (on b11 c1) (on b1 b11) (in b1 c1) (clear b1) (base b7) (in b7 c2) (on b7 c2) (on b4 b7) (in b4 c2) (on b6 b4) (in b6 c2) (on b16 b6) (in b16 c2) (on b14 b16) (in b14 c2) (on b8 b14) (in b8 c2) (on b15 b8) (in b15 c2) (on b10 b15) (in b10 c2) (on b13 b10) (in b13 c2) (on b17 b13) (in b17 c2) (on b5 b17) (in b5 c2) (on b12 b5) (in b12 c2) (on b2 b12) (in b2 c2) (on b9 b2) (in b9 c2) (on b3 b9) (in b3 c2) (clear b3) (clear c3) (clear c4))
 (:goal (and (on b1 c1) (on b13 b1) (on b10 b13) (on b3 b10) (on b9 b3) (on b11 b9) (on b16 b11) (on b4 b16) (on b17 b4) (on b8 b17) (on b6 b8) (on b14 b6) (on b2 b14) (on b12 b2) (on b5 b12) (on b15 b5) (on b7 b15) (clear b7)))
)
