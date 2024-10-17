(define (problem p51-problem)
 (:domain p51-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 7) (= (capacity c2) 7) (= (capacity c3) 0) (= (capacity c4) 8) (base b14) (in b14 c1) (on b14 c1) (clear b14) (base b3) (in b3 c2) (on b3 c2) (on b10 b3) (in b10 c2) (on b8 b10) (in b8 c2) (clear b8) (base b4) (in b4 c3) (on b4 c3) (on b2 b4) (in b2 c3) (on b5 b2) (in b5 c3) (on b11 b5) (in b11 c3) (on b12 b11) (in b12 c3) (on b7 b12) (in b7 c3) (on b1 b7) (in b1 c3) (on b6 b1) (in b6 c3) (on b13 b6) (in b13 c3) (on b15 b13) (in b15 c3) (on b9 b15) (in b9 c3) (clear b9) (clear c4))
 (:goal (and (on b7 c1) (on b10 b7) (on b12 b10) (on b1 b12) (on b14 b1) (clear b14) (on b4 c2) (on b13 b4) (on b2 b13) (on b15 b2) (on b3 b15) (on b8 b3) (on b5 b8) (on b11 b5) (on b9 b11) (on b6 b9) (clear b6)))
)
