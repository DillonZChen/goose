(define (problem p0_17-problem)
 (:domain p0_17-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 6) (= (capacity c3) 15) (= (capacity c4) 6) (base b16) (in b16 c1) (on b16 c1) (on b15 b16) (in b15 c1) (on b18 b15) (in b18 c1) (on b3 b18) (in b3 c1) (on b7 b3) (in b7 c1) (on b5 b7) (in b5 c1) (on b1 b5) (in b1 c1) (on b11 b1) (in b11 c1) (on b14 b11) (in b14 c1) (on b6 b14) (in b6 c1) (on b9 b6) (in b9 c1) (on b17 b9) (in b17 c1) (on b4 b17) (in b4 c1) (on b8 b4) (in b8 c1) (on b10 b8) (in b10 c1) (on b12 b10) (in b12 c1) (on b2 b12) (in b2 c1) (on b13 b2) (in b13 c1) (clear b13) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b3 c1) (clear b3) (on b11 c2) (on b1 b11) (clear b1) (on b7 c3) (on b15 b7) (on b4 b15) (on b8 b4) (on b14 b8) (on b9 b14) (on b5 b9) (on b10 b5) (on b13 b10) (on b17 b13) (on b6 b17) (on b16 b6) (on b12 b16) (on b18 b12) (on b2 b18) (clear b2)))
)
