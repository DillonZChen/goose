(define (problem p47-problem)
 (:domain p47-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 - block
   c1 c2 c3 c4 c5 c6 c7 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 2) (= (capacity c2) 1) (= (capacity c3) 0) (= (capacity c4) 4) (= (capacity c5) 7) (= (capacity c6) 4) (= (capacity c7) 3) (base b2) (in b2 c1) (on b2 c1) (on b6 b2) (in b6 c1) (clear b6) (base b14) (in b14 c2) (on b14 c2) (on b10 b14) (in b10 c2) (on b9 b10) (in b9 c2) (clear b9) (base b11) (in b11 c3) (on b11 c3) (on b5 b11) (in b5 c3) (on b12 b5) (in b12 c3) (on b7 b12) (in b7 c3) (on b3 b7) (in b3 c3) (on b1 b3) (in b1 c3) (on b13 b1) (in b13 c3) (on b8 b13) (in b8 c3) (on b4 b8) (in b4 c3) (clear b4) (clear c4) (clear c5) (clear c6) (clear c7))
 (:goal (and (on b12 c1) (clear b12) (on b6 c2) (clear b6) (on b14 c3) (on b11 b14) (clear b11) (on b1 c4) (on b2 b1) (on b13 b2) (clear b13) (on b8 c5) (on b3 b8) (on b10 b3) (on b7 b10) (on b4 b7) (on b5 b4) (on b9 b5) (clear b9)))
)
