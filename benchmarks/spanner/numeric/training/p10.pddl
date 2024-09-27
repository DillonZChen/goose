(define (problem spanner-problem)
 (:domain spanner-domain)
 (:objects
   shed location1 location2 - location
   bob - man
 )
 (:init (link shed location1) (link location1 location2) (= (spanners_at location1) 1) (= (spanners_at location2) 1) (= (spanners_at shed) 0) (= (spanners_at gate) 0) (at bob shed) (link location2 gate)  (= (carrying bob) 0) (= (loose) 2))
 (:goal (and (= (loose) 0)))
)
