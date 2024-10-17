(define (problem p76-problem)
 (:domain p76-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 7) (= (capacity c3) 7) (= (capacity c4) 12) (= (capacity c5) 7) (base b15) (in b15 c1) (on b15 c1) (on b7 b15) (in b7 c1) (on b3 b7) (in b3 c1) (on b4 b3) (in b4 c1) (on b13 b4) (in b13 c1) (on b22 b13) (in b22 c1) (on b11 b22) (in b11 c1) (on b14 b11) (in b14 c1) (on b16 b14) (in b16 c1) (on b2 b16) (in b2 c1) (on b8 b2) (in b8 c1) (on b18 b8) (in b18 c1) (on b20 b18) (in b20 c1) (on b19 b20) (in b19 c1) (on b1 b19) (in b1 c1) (on b12 b1) (in b12 c1) (on b17 b12) (in b17 c1) (on b21 b17) (in b21 c1) (on b6 b21) (in b6 c1) (on b10 b6) (in b10 c1) (on b9 b10) (in b9 c1) (on b5 b9) (in b5 c1) (clear b5) (clear c2) (clear c3) (clear c4) (clear c5))
 (:goal (and (on b17 c1) (on b7 b17) (clear b7) (on b22 c2) (on b10 b22) (on b2 b10) (clear b2) (on b1 c3) (on b15 b1) (on b19 b15) (on b12 b19) (on b4 b12) (clear b4) (on b16 c4) (on b5 b16) (on b3 b5) (on b21 b3) (on b9 b21) (on b20 b9) (on b18 b20) (on b8 b18) (on b6 b8) (on b11 b6) (on b14 b11) (on b13 b14) (clear b13)))
)
