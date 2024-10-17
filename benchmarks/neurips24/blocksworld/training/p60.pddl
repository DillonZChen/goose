(define (problem p60-problem)
 (:domain p60-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 4) (= (capacity c2) 1) (= (capacity c3) 11) (= (capacity c4) 11) (base b1) (in b1 c1) (on b1 c1) (on b3 b1) (in b3 c1) (on b16 b3) (in b16 c1) (on b17 b16) (in b17 c1) (on b12 b17) (in b12 c1) (on b15 b12) (in b15 c1) (on b8 b15) (in b8 c1) (clear b8) (base b5) (in b5 c2) (on b5 c2) (on b7 b5) (in b7 c2) (on b4 b7) (in b4 c2) (on b18 b4) (in b18 c2) (on b13 b18) (in b13 c2) (on b6 b13) (in b6 c2) (on b11 b6) (in b11 c2) (on b9 b11) (in b9 c2) (on b14 b9) (in b14 c2) (on b2 b14) (in b2 c2) (on b10 b2) (in b10 c2) (clear b10) (clear c3) (clear c4))
 (:goal (and (on b17 c1) (on b11 b17) (on b7 b11) (on b3 b7) (on b4 b3) (on b13 b4) (clear b13) (on b16 c2) (on b10 b16) (on b2 b10) (on b15 b2) (on b5 b15) (on b18 b5) (on b14 b18) (on b6 b14) (on b1 b6) (on b12 b1) (on b8 b12) (on b9 b8) (clear b9)))
)
