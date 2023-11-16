;; vehicles=3, packages=3, locations=7, max_capacity=2, out_folder=training/easy, instance_id=18, seed=59

(define (problem transport-18)
 (:domain transport)
 (:objects 
    v1 v2 v3 - vehicle
    p1 p2 p3 - package
    l1 l2 l3 l4 l5 l6 l7 - location
    c0 c1 c2 - size
    )
 (:init (capacity v1 c1)
    (capacity v2 c1)
    (capacity v3 c2)
    (capacity-predecessor c0 c1)
    (capacity-predecessor c1 c2)
    (at p1 l1)
    (at p2 l2)
    (at p3 l7)
    (at v1 l3)
    (at v2 l6)
    (at v3 l1)
    (road l2 l4)
    (road l1 l2)
    (road l2 l1)
    (road l2 l7)
    (road l3 l7)
    (road l6 l1)
    (road l4 l2)
    (road l7 l3)
    (road l7 l2)
    (road l5 l3)
    (road l1 l6)
    (road l3 l5)
    (road l1 l7)
    (road l7 l1)
    (road l1 l5)
    (road l5 l1)
    (road l2 l6)
    (road l6 l2)
    )
 (:goal  (and 
    (at p1 l6)
    (at p2 l5)
    (at p3 l6))))
