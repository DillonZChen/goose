(define (problem p71-problem)
 (:domain p71-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 - block
   c1 c2 c3 c4 c5 c6 c7 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 3) (= (capacity c2) 0) (= (capacity c3) 5) (= (capacity c4) 6) (= (capacity c5) 7) (= (capacity c6) 5) (= (capacity c7) 5) (base b7) (in b7 c1) (on b7 c1) (on b10 b7) (in b10 c1) (on b17 b10) (in b17 c1) (clear b17) (base b12) (in b12 c2) (on b12 c2) (on b19 b12) (in b19 c2) (on b2 b19) (in b2 c2) (on b21 b2) (in b21 c2) (on b14 b21) (in b14 c2) (on b20 b14) (in b20 c2) (on b8 b20) (in b8 c2) (on b5 b8) (in b5 c2) (on b18 b5) (in b18 c2) (on b11 b18) (in b11 c2) (on b9 b11) (in b9 c2) (on b16 b9) (in b16 c2) (on b15 b16) (in b15 c2) (on b4 b15) (in b4 c2) (on b13 b4) (in b13 c2) (on b1 b13) (in b1 c2) (on b3 b1) (in b3 c2) (on b6 b3) (in b6 c2) (clear b6) (clear c3) (clear c4) (clear c5) (clear c6) (clear c7))
 (:goal (and (on b7 c1) (on b10 b7) (clear b10) (on b3 c2) (on b5 b3) (on b9 b5) (clear b9) (on b18 c3) (on b4 b18) (on b17 b4) (clear b17) (on b6 c4) (on b13 b6) (on b21 b13) (on b14 b21) (on b16 b14) (on b8 b16) (clear b8) (on b2 c5) (on b12 b2) (on b1 b12) (on b20 b1) (on b15 b20) (on b19 b15) (on b11 b19) (clear b11)))
)
