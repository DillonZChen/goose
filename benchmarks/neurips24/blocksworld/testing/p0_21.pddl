(define (problem p0_21-problem)
 (:domain p0_21-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 - block
   c1 c2 c3 c4 c5 c6 c7 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 4) (= (capacity c2) 0) (= (capacity c3) 0) (= (capacity c4) 6) (= (capacity c5) 9) (= (capacity c6) 6) (= (capacity c7) 6) (base b20) (in b20 c1) (on b20 c1) (on b17 b20) (in b17 c1) (clear b17) (base b1) (in b1 c2) (on b1 c2) (on b4 b1) (in b4 c2) (on b3 b4) (in b3 c2) (on b18 b3) (in b18 c2) (on b16 b18) (in b16 c2) (on b8 b16) (in b8 c2) (on b21 b8) (in b21 c2) (clear b21) (base b10) (in b10 c3) (on b10 c3) (on b13 b10) (in b13 c3) (on b12 b13) (in b12 c3) (on b7 b12) (in b7 c3) (on b9 b7) (in b9 c3) (on b15 b9) (in b15 c3) (on b14 b15) (in b14 c3) (on b6 b14) (in b6 c3) (on b5 b6) (in b5 c3) (on b11 b5) (in b11 c3) (on b19 b11) (in b19 c3) (on b2 b19) (in b2 c3) (clear b2) (clear c4) (clear c5) (clear c6) (clear c7))
 (:goal (and (on b18 c1) (on b17 b18) (clear b17) (on b2 c2) (on b4 b2) (clear b4) (on b14 c3) (on b15 b14) (on b19 b15) (clear b19) (on b6 c4) (on b10 b6) (on b7 b10) (on b20 b7) (on b8 b20) (clear b8) (on b5 c5) (on b21 b5) (on b13 b21) (on b3 b13) (on b11 b3) (on b16 b11) (on b12 b16) (on b9 b12) (on b1 b9) (clear b1)))
)
