(define (problem p1_07)
 (:domain spanner)
 (:objects
   shed location1 location2 location3 location4 location5 location6 location7 location8 location9 location10 location11 location12 location13 location14 location15 location16 location17 location18 location19 location20 location21 - location
   bob - man
 )
 (:init (link shed location1) (link location1 location2) (link location2 location3) (link location3 location4) (link location4 location5) (link location5 location6) (link location6 location7) (link location7 location8) (link location8 location9) (link location9 location10) (link location10 location11) (link location11 location12) (link location12 location13) (link location13 location14) (link location14 location15) (link location15 location16) (link location16 location17) (link location17 location18) (link location18 location19) (link location19 location20) (link location20 location21) (= (spanners_at location1) 2) (= (spanners_at location2) 1) (= (spanners_at location3) 3) (= (spanners_at location4) 3) (= (spanners_at location5) 1) (= (spanners_at location6) 3) (= (spanners_at location7) 2) (= (spanners_at location8) 1) (= (spanners_at location9) 3) (= (spanners_at location10) 2) (= (spanners_at location11) 3) (= (spanners_at location12) 1) (= (spanners_at location13) 5) (= (spanners_at location14) 1) (= (spanners_at location15) 1) (= (spanners_at location16) 2) (= (spanners_at location17) 1) (= (spanners_at location18) 0) (= (spanners_at location19) 2) (= (spanners_at location20) 1) (= (spanners_at location21) 4) (= (spanners_at shed) 0) (= (spanners_at gate) 0) (at bob shed) (link location21 gate)  (= (carrying bob) 0) (= (loose) 22))
 (:goal (and (= (loose) 0)))
)
