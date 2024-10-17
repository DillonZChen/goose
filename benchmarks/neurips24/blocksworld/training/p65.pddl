(define (problem p65-problem)
 (:domain p65-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 2) (= (capacity c2) 0) (= (capacity c3) 16) (= (capacity c4) 10) (base b14) (in b14 c1) (on b14 c1) (on b17 b14) (in b17 c1) (on b9 b17) (in b9 c1) (on b4 b9) (in b4 c1) (on b10 b4) (in b10 c1) (on b18 b10) (in b18 c1) (on b8 b18) (in b8 c1) (on b3 b8) (in b3 c1) (clear b3) (base b2) (in b2 c2) (on b2 c2) (on b15 b2) (in b15 c2) (on b6 b15) (in b6 c2) (on b1 b6) (in b1 c2) (on b13 b1) (in b13 c2) (on b7 b13) (in b7 c2) (on b11 b7) (in b11 c2) (on b5 b11) (in b5 c2) (on b12 b5) (in b12 c2) (on b19 b12) (in b19 c2) (on b16 b19) (in b16 c2) (clear b16) (clear c3) (clear c4))
 (:goal (and (on b1 c1) (clear b1) (on b5 c2) (on b16 b5) (clear b16) (on b13 c3) (on b11 b13) (on b14 b11) (on b19 b14) (on b18 b19) (on b12 b18) (on b2 b12) (on b9 b2) (on b17 b9) (on b3 b17) (on b4 b3) (on b10 b4) (on b7 b10) (on b15 b7) (on b8 b15) (on b6 b8) (clear b6)))
)
