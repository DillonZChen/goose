(define (problem p68-problem)
 (:domain p68-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 7) (= (capacity c3) 16) (= (capacity c4) 7) (base b18) (in b18 c1) (on b18 c1) (on b17 b18) (in b17 c1) (on b20 b17) (in b20 c1) (on b5 b20) (in b5 c1) (on b16 b5) (in b16 c1) (on b1 b16) (in b1 c1) (on b3 b1) (in b3 c1) (on b13 b3) (in b13 c1) (on b15 b13) (in b15 c1) (on b10 b15) (in b10 c1) (on b2 b10) (in b2 c1) (on b19 b2) (in b19 c1) (on b4 b19) (in b4 c1) (on b12 b4) (in b12 c1) (on b9 b12) (in b9 c1) (on b6 b9) (in b6 c1) (on b11 b6) (in b11 c1) (on b14 b11) (in b14 c1) (on b7 b14) (in b7 c1) (on b8 b7) (in b8 c1) (clear b8) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b8 c1) (clear b8) (on b19 c2) (on b3 b19) (on b7 b3) (clear b7) (on b12 c3) (on b16 b12) (on b10 b16) (on b1 b10) (on b17 b1) (on b6 b17) (on b20 b6) (on b9 b20) (on b13 b9) (on b18 b13) (on b15 b18) (on b5 b15) (on b11 b5) (on b2 b11) (on b4 b2) (on b14 b4) (clear b14)))
)
