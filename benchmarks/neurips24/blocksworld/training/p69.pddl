(define (problem p69-problem)
 (:domain p69-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 7) (= (capacity c2) 9) (= (capacity c3) 5) (= (capacity c4) 0) (= (capacity c5) 9) (base b9) (in b9 c1) (on b9 c1) (on b16 b9) (in b16 c1) (clear b16) (base b4) (in b4 c2) (on b4 c2) (on b14 b4) (in b14 c2) (clear b14) (base b12) (in b12 c3) (on b12 c3) (on b11 b12) (in b11 c3) (on b13 b11) (in b13 c3) (on b7 b13) (in b7 c3) (clear b7) (base b6) (in b6 c4) (on b6 c4) (on b18 b6) (in b18 c4) (on b1 b18) (in b1 c4) (on b20 b1) (in b20 c4) (on b3 b20) (in b3 c4) (on b17 b3) (in b17 c4) (on b15 b17) (in b15 c4) (on b19 b15) (in b19 c4) (on b10 b19) (in b10 c4) (on b8 b10) (in b8 c4) (on b5 b8) (in b5 c4) (on b2 b5) (in b2 c4) (clear b2) (clear c5))
 (:goal (and (on b7 c1) (on b12 b7) (on b3 b12) (on b5 b3) (on b4 b5) (on b9 b4) (on b14 b9) (on b13 b14) (on b17 b13) (clear b17) (on b10 c2) (on b19 b10) (on b11 b19) (on b16 b11) (on b20 b16) (on b8 b20) (on b6 b8) (on b1 b6) (on b2 b1) (on b18 b2) (on b15 b18) (clear b15)))
)
