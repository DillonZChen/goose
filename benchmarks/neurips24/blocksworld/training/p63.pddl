(define (problem p63-problem)
 (:domain p63-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 10) (= (capacity c2) 4) (= (capacity c3) 3) (= (capacity c4) 11) (base b5) (in b5 c1) (on b5 c1) (on b15 b5) (in b15 c1) (clear b15) (base b12) (in b12 c2) (on b12 c2) (on b18 b12) (in b18 c2) (on b19 b18) (in b19 c2) (on b1 b19) (in b1 c2) (on b2 b1) (in b2 c2) (on b9 b2) (in b9 c2) (on b4 b9) (in b4 c2) (on b14 b4) (in b14 c2) (clear b14) (base b7) (in b7 c3) (on b7 c3) (on b3 b7) (in b3 c3) (on b16 b3) (in b16 c3) (on b10 b16) (in b10 c3) (on b11 b10) (in b11 c3) (on b6 b11) (in b6 c3) (on b13 b6) (in b13 c3) (on b8 b13) (in b8 c3) (on b17 b8) (in b17 c3) (clear b17) (clear c4))
 (:goal (and (on b15 c1) (on b2 b15) (on b4 b2) (on b14 b4) (on b6 b14) (on b5 b6) (on b12 b5) (on b16 b12) (on b10 b16) (clear b10) (on b1 c2) (on b19 b1) (on b3 b19) (on b8 b3) (on b11 b8) (on b17 b11) (on b13 b17) (on b18 b13) (on b9 b18) (on b7 b9) (clear b7)))
)
